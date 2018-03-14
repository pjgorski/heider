#include "stdafx.h"
#include "HeiderGraph.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <ctime> // time_t
#include <cstdio>

using namespace std;

HeiderGraph::HeiderGraph(void)
{
	N = 0;
	E = 0;
	d = 0;
	balancedCount = 0;
	imbalancedCount = 0;
	positiveWeightsCount = 0;
	negativeWeightsCount = 0;
	triads = 0;
	wasModified = false;
}

/* type = ["complete"] */
HeiderGraph::HeiderGraph( int N, int d, TRnd r, TStr graphType, TStr changeSignType )
{
	this->N = N;
	this->d = d;
	std::string s = graphType.CStr();
	//s.erase(s.end()-1, s.end());
	this->graphType = graphType;//s.c_str();
	//cout << graphType << endl;
	std::string dyn = changeSignType.CStr();
	//dyn.erase(dyn.end()-1, dyn.end());
	this->changeSignType = changeSignType;//dyn.c_str();

	rnd = r;
	
	if (this->graphType == "complete"){
		G = TSnap::GenFull<PNEANet>(N);
	}
	else{
		std::cout << "Type " << s.c_str() << " of HeiderGraph is not supported" << std::endl;
		exit(UNSUPPORTED_GRAPH_TYPE);
	}

	this->E = G->GetEdges();
//	for (int i = 0; i < d; ++i){
//	//int i = 0;
//
//		std::string s2 = "attr" + ToString((long long)i);
//		attrNames.push_back(TStr(s2.c_str()));
//		G->AddIntAttrN(attrNames[i],0);
//	}
	for (int i = 0; i < BALANCE_CASE_COUNT; ++i)
		caseCounts.push_back(0);
	triads = TSnap::GetTriads(G);
	RandomInit();
	CalcCaseCounts();
	wasModified = false;
}

void HeiderGraph::GetRandomTriad(int &node1, int& node2, int& node3){
	if (triads == 0){
		cout << "Graph does not contain triads" << endl;
		//system("pause");
		exit(ZERO_TRIADS);
	}

	bool triadFound = false;

	while (!triadFound){
		TNEANet::TEdgeI rndEI = G->GetRndEI(rnd);
		int rndSrc = rndEI.GetSrcNId(), rndDest = rndEI.GetDstNId();
		TIntV NbrV;
		//cout << "Random edge: (" << rndSrc << ", " << rndDest << ")" << endl;
		TSnap::GetCmnNbrs(G, rndSrc, rndDest, NbrV);
		if (NbrV.Len() != 0){
			int nbrInd = rnd.GetUniDevInt(0, NbrV.Len()-1);
			node1 = rndSrc;
			node2 = rndDest;
			node3 = NbrV[nbrInd];
			triadFound = true;
		}
	}
//	TIntV t;
//	G->GetNIdV(t);
//
//	int pos1 = rnd.GetUniDevInt(0, t.Len() - 1),
//			pos2 = rnd.GetUniDevInt(0, t.Len() - 2),
//			pos3 = rnd.GetUniDevInt(0, t.Len() - 3);
//
//	node1 = int(t.GetVal(pos1));
//	t.Del(pos1);
//
//
//	node2 = int(t.GetVal(pos2));
//	t.Del(pos2);
//
//
//	node3 = int(t.GetVal(pos3));
	//t.Del(pos3);

//	//We assume complete graph, so above for now is unnecessary
	//TNEANet::TEdgeI rndEI = G->GetRndEI(rnd);
			//int rndSrc = rndEI.GetSrcNId(), rndDest = rndEI.GetDstNId();
			//TInt
//	node1 = G->GetRndNI(rnd).GetId();
//	node2 = G->GetRndNI(rnd).GetId();
//	node3 = G->GetRndNI(rnd).GetId();
}

//@not used
void HeiderGraph::GetRandomTriadForNode( const int &node, int& nbr1, int& nbr2 )
{
	TIntV nbrs;
	TNEANet::TNodeI nodeI = G->GetNI(node);
	int nbrCount = nodeI.GetOutDeg();
	if (nbrCount < 2){
		cout << "Attempt to get triad for a node with degree " << nbrCount << endl;
		//system("pause");
		exit(NON_TRIAD);
	}
	nbr1 = node;
	nbr2 = node;
	/* get indexes of two random edges of node */
	while (nbr1 == node || nbr2 == node || nbr1 == nbr2){
		if (nbr1 == node)
			nbr1 = rnd.GetUniDevInt(0, nbrCount-1);
		if (nbr2 == node || nbr2 == nbr1)
			nbr2 = rnd.GetUniDevInt(0, nbrCount-1);
	}
	/* get neighbors by indexes */
	nbr1 = nodeI.GetOutNId(nbr1);
	nbr2 = nodeI.GetOutNId(nbr2);

}

int HeiderGraph::GetTriadType( int& node1, int& node2, int& node3 )
{
	if (!G->IsEdge(node1, node2) || !G->IsEdge(node2, node3) || !G->IsEdge(node1, node3)) {
		cout << "Calculating balance for a non-triad" << endl;
		//system("pause");
		exit(NON_TRIAD);
	}

	int ij_rel = GetWeight(node1, node2);
	int jk_rel = GetWeight(node2, node3);
	int ik_rel = GetWeight(node1, node3);
	int caseNum = 0;
	IsBalanced(ij_rel, jk_rel, ik_rel, caseNum);
	int res = 0;
	/* res is a number of unfriendly links (according to Antal)*/
	if (caseNum == 1)
		res = 3;
	else if (caseNum == 2 || caseNum == 3 || caseNum == 4)
		res = 1;
	else if (caseNum == 5)
		res = 0;
	else if (caseNum == 6 || caseNum == 7 || caseNum == 8)
		res = 2;

	//cout << "caseNum: " << caseNum << " res: " << res << endl;
	return res;
}

void HeiderGraph::ChangeSign( int& node1, int& node2, int& node3, bool isPlusToMinus )
{
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}

	int ij_rel = GetWeight(node1, node2);
	int jk_rel = GetWeight(node2, node3);
	int ik_rel = GetWeight(node1, node3);

	TIntV ind;

	/* from minus to plus */
	if (!isPlusToMinus){
		if (ij_rel < 0)
			ind.Add(1);
		if (jk_rel < 0)
			ind.Add(2);
		if (ik_rel < 0)
			ind.Add(3);
	}
	else{
		if (ij_rel > 0)
			ind.Add(1);
		if (jk_rel > 0)
			ind.Add(2);
		if (ik_rel > 0)
			ind.Add(3);
	}

	ind.Shuffle(rnd);
	int var = ind[0];
	if (var == 1){
		ChangeSign(node1, node2, isPlusToMinus);
		//cout << "var = 1" << endl;
	}
	else if (var == 2){
		ChangeSign(node2, node3, isPlusToMinus);
		//cout << "var = 2" << endl;
	}
	else if (var == 3){
		ChangeSign(node1, node3, isPlusToMinus);
		//cout << "var = 3" << endl;
	}
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}
}



void HeiderGraph::ChangeSign( int& node1, int& node2, bool isPlusToMinus, string type)
{
	/*to be refactored */
	// changeSignType = type.c_str();
	/*  type parameter is not used */
	if (changeSignType == "attrChoice")
		ChangeSignAttrChoice(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrRandom")
		ChangeSignAttrRandom(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrRandomCount")
		ChangeSignAttrRandomCount(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrMax")
		ChangeSignAttrMax(node1, node2, isPlusToMinus);
}

void HeiderGraph::ChangeSign( int& node1, int& node2, bool isPlusToMinus, int first, std::string type  )
{
	if (first != node1 && first != node2){
		cout << "Attempt to change attribute in incorrect node, node 1 =  " << node1 
			<< " node2 = " << node2 << " base node: " << first << endl;
		//system("pause");
		exit(WRONG_BASE_NODE_ID);
	}
	int second = node2;

	if (first == node2)
		second = node1;

	/* if type = attrChoice */
	if (type.find("attrChoice") != string::npos){
		ChangeSignAttrChoice(first, second, isPlusToMinus);
	}
	else if (type.find("attrRandom") != string::npos){
		ChangeSignAttrRandom (first, second, isPlusToMinus);
	}
	else if (type.find("attrMax") != string::npos){
		ChangeSignAttrMax (first, second, isPlusToMinus);
	}
	else{
		cout << "Unsupported type of changing sign, dynamics: " << type 
			<< endl;
		//system("pause");
		exit(UNSUPPORTED_CHANGE_SIGN_TYPE);
	}
}

void HeiderGraph::ChangeSignAttrChoice( int& node1, int& node2, bool isPlusToMinus )
{
	TIntV attrIndV;
	int minusesCount = 0, plusesCount = 0;
	if (!isPlusToMinus){
		GetDiffAttrV(node1, node2, attrIndV);
		minusesCount = attrIndV.Len(),
			plusesCount = d-minusesCount;
	}
	else{
		GetSimAttrV(node1, node2, attrIndV);
		plusesCount =attrIndV.Len(),
			minusesCount = d-plusesCount;
	}

	int signsToAdd = 0;
	if (!isPlusToMinus)
		signsToAdd = ceil(d / 2.0) - plusesCount;
	else
		signsToAdd = ceil(d / 2.0) - minusesCount;
	if (d == 3)
		signsToAdd = 1;
	//cout << "Pluses to add: " << plusesToAdd << endl;
	attrIndV.Shuffle(rnd);
	for (int i = 0; i < signsToAdd; ++i){
		int attrInd = attrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		////system("pause");
	}
	
	/* check */
	wasModified = true;
}

/* change random attribute to increase of decrease similarity of attributes
   attribute is changed in first node
*/
void HeiderGraph::ChangeSign (int& node1, int& node2, bool isPlusToMinus, int first){
	if (first != node1 && first != node2){
		cout << "Attempt to change attribute in incorrect node, node 1 =  " << node1 
			<< " node2 = " << node2 << " base node: " << first << endl;
		//system("pause");
		exit(WRONG_BASE_NODE_ID);
	}
	int second = node2;

	if (first == node2)
		second = node1;

	string type = changeSignType.CStr();
	/* if type = attrChoice */
	if (type.find("AttrChoice") != string::npos){
		ChangeSignAttrChoice(first, second, isPlusToMinus);
	}
	else if (type.find("AttrRandom") != string::npos){
		ChangeSignAttrRandom (first, second, isPlusToMinus);
	}
	else if (type.find("AttrMax") != string::npos){
		ChangeSignAttrMax (first, second, isPlusToMinus);
	}
	else{
		cout << "Unsupported type of changing sign, dynamics: " << type 
			 << endl;
		//system("pause");
		exit(UNSUPPORTED_CHANGE_SIGN_TYPE);
	}
}

void HeiderGraph::ChangeSignAttrRandom( int& node1, int& node2, bool isPlusToMinus )
{
	TIntV attrIndV, and_xor;
	int bits;
	if (isPlusToMinus)
		bits = GetSimAttrV(node1, node2, and_xor);
	else
		bits = GetDiffAttrV(node1, node2, and_xor);

	int r = rnd.GetUniDevInt(0, bits-1);//attrIndV.Len()-1);

	//attrIndV.Shuffle(rnd);
	//int attrInd = attrIndV[0];
	int nodeToChange = rnd.GetUniDevInt(1,2);

	int node;
	if (nodeToChange == 1){
		//int val = G->GetIntAttrIndDatN(node1, attrInd);
		//G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		node = node1;
	}
	else{
		//int val = G->GetIntAttrIndDatN(node2, attrInd);
		//G->AddIntAttrDatN(node2, val * (-1), attrNames[attrInd]);
		node = node2;
	}

	//cout << "node1 and node2: " << node1 << " " << node2 << endl;

	int pos = 0;
	for (int i = 0, i2 = 0; i < and_xor.Len(); ++i){
		int val = and_xor[i], j = 0;

		while(val > 0 && pos <= r){
			if ((val&1) == 1){
				++pos;

				if(pos > r){
					//cout << " " << attr[i][node] << endl;
					int val2 = attr[i][node];
					val2 ^= (1UL << j); //toggling bit
					attr[i][node] = val2;
//					int val3 = G->GetIntAttrIndDatN(node, i2);
//					G->AddIntAttrDatN(node, val3 * (-1), attrNames[i2]);
					//cout << attr[i][node] << endl;
				}
			}
			val = val >> 1;
			++i2;
			++j;
		}
	}



	wasModified = true;
}

bool pairCompare(const pair<int, int>& a, const pair<int, int>& b){
	return a.first < b.first;
}

void HeiderGraph::ChangeSignAttrRandomCount( int& node1, int& node2, bool isPlusToMinus )
{
	TIntV attrIndV;
	int minusesCount = 0, plusesCount = 0;
	if (!isPlusToMinus){
		GetDiffAttrV(node1, node2, attrIndV);
		minusesCount = attrIndV.Len(),
			plusesCount = d-minusesCount;
	}
	else{
		GetSimAttrV(node1, node2, attrIndV);
		plusesCount =attrIndV.Len(),
			minusesCount = d-plusesCount;
	}

	int signsToAdd = 0;
	//if (!isPlusToMinus)
	//	signsToAdd = ceil(d / 2.0) - plusesCount;
	//else
	//	signsToAdd = ceil(d / 2.0) - minusesCount;
	//if (d == 3)
	//	signsToAdd = 1;
	signsToAdd = rnd.GetUniDevInt(1, attrIndV.Len());
	//cout << "Pluses to add: " << plusesToAdd << endl;
	attrIndV.Shuffle(rnd);
	for (int i = 0; i < signsToAdd; ++i){
		int attrInd = attrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		////system("pause");
	}

	/* check */
	wasModified = true;
}

void HeiderGraph::ChangeSignAttrMax( int& node1, int& node2, bool isPlusToMinus )
{
	TIntV attrIndV;
	int minusesCount = 0, plusesCount = 0;
	if (!isPlusToMinus){
		GetDiffAttrV(node1, node2, attrIndV);
		minusesCount = attrIndV.Len(),
			plusesCount = d-minusesCount;
	}
	else{
		GetSimAttrV(node1, node2, attrIndV);
		plusesCount =attrIndV.Len(),
			minusesCount = d-plusesCount;
	}

	int signsToAdd = 0;
	//if (!isPlusToMinus)
	//	signsToAdd = ceil(d / 2.0) - plusesCount;
	//else
	//	signsToAdd = ceil(d / 2.0) - minusesCount;
	//if (d == 3)
	//	signsToAdd = 1;
	signsToAdd = attrIndV.Len();
	//cout << "Pluses to add: " << plusesToAdd << endl;
	attrIndV.Shuffle(rnd);
	for (int i = 0; i < signsToAdd; ++i){
		int attrInd = attrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		////system("pause");
	}

	/* check */
	wasModified = true;
}

int HeiderGraph::GetDiffAttrV( int& node1, int& node2, TIntV& bit_xor )
{
//	diffAttrIndV.Clr();
//	TIntV attr1, attr2;
//	G->IntAttrValueNI(node1, attr1);
//	G->IntAttrValueNI(node2, attr2);
//
//	for (int i = 0; i < attr1.Len(); ++i){
//		if (attr1[i] != attr2[i])
//			diffAttrIndV.Add(i);
//	}

	//TIntV bit_xor;
	int dif_bits = 0;
	for (int i = 0; i < attr.Len(); ++i){
		bit_xor.Add((attr[i][node1] ^ attr[i][node2])&max_vals[i]);
		dif_bits += builtinPopcount(bit_xor[i]);
	}

	return dif_bits;

}

int HeiderGraph::GetSimAttrV( int& node1, int& node2, TIntV& bit_and )
{
//	simAttrIndV.Clr();
//	TIntV attr1, attr2;
//	G->IntAttrValueNI(node1, attr1);
//	G->IntAttrValueNI(node2, attr2);
//
//	for (int i = 0; i < attr1.Len(); ++i){
//		if (attr1[i] == attr2[i])
//			simAttrIndV.Add(i);
//	}
//
//	TIntV bit_and;
	int same_bits = 0;
	for (int i = 0; i < attr.Len(); ++i){
		bit_and.Add((~(attr[i][node1] ^ attr[i][node2])) & max_vals[i]);
		//same_bits += builtinPopcount(~((attr[i][node1] ^ attr[i][node2]));
		same_bits += builtinPopcount(bit_and[i]);
		//cout << bit_and[i] << " ";
	}


//	cout << endl << "test" << endl;
//	cout << "same bits: " << same_bits <<endl;
	//PrintNodeAttrs(node1);
	//PrintNodeAttrs(node2);
//	int r = rnd.GetUniDevInt(0, same_bits-1);
//	cout << "Picked to change bit nr: " << r << endl;
//	cout << "node1: " << nthset(node1, r) << endl;
//	cout << "node2: " << nthset(node2, r) << endl;

	return same_bits;
}


TStr HeiderGraph::GetStrTriadType( int triadType )
{
	TStr res;
	if (triadType == 0)
		res = "(+ + +)";
	if (triadType == 1)
		res = "(- + + / + + - / + - +)";
	if (triadType == 2)
		res = "(+ - - / - + - / - - +)";
	if (triadType == 3)
		res = "- - -";
	return res;
}

int HeiderGraph::GetLargestGroupSize()
{
	int plusesCount = 0;
	for (int i = 0; i < N; ++i)
		if (GetWeight(0,i) >= 1)
			++plusesCount;
	int largestGroupSize = 0;
	if (plusesCount > N-plusesCount)
		largestGroupSize = plusesCount;
	else largestGroupSize = N-plusesCount;
	
	return largestGroupSize;
}

void HeiderGraph::PrintWeightMatrix()
{
	for (int i = 0; i < N; ++i){
		for (int j = 0; j < N; j++){
			if (i == j)
				cout << 1 << " ";
			else cout << GetWeight(i,j) << " ";
		}
		cout << endl;
	}
}

//unused
void HeiderGraph::Mutate( double pm )
{
	double rval  = rnd.GetUniDev();
	if (rval < pm){
		int node = rnd.GetUniDevInt(0, N-1);
		int attrInd = rnd.GetUniDevInt(0, d-1);
		int val = G->GetIntAttrDatN(node, attrNames[attrInd]);
		G->AddIntAttrDatN(node, val * (-1), attrNames[attrInd]);
	}
}


void HeiderGraph::SaveFinalState( double p, int idRun )
{
	std::string outFileName = "network_" + ToString((long long)N) + "_" + ToString((long long)d) + "_" 
		+ ToString((long double)p) + "_" 
		+ ToString((long long)idRun) + "_" + changeSignType.CStr() + ".txt";
	std::ofstream out(outFileName.c_str());
	if (!out){
		cout << "Error of file creation" << endl;
	}

	for (int i = 0; i < N; ++i){
		TStrV NIdAttrValue;
		G->AttrValueNI(i, NIdAttrValue);
		TInt AttrLen = NIdAttrValue.Len();
		out << "Node " << i << " attributes: " << "\t";
		for (int j = 0; j < AttrLen; j++) {
			out << NIdAttrValue[j].CStr() << "\t";
		} 
		out << endl;
	}

	out.close();
}

void HeiderGraph::AntalDynamics( int maxIterCount, double p, int& iter, int& largestGroupSize, double bPart, int printEvery, int idRun )
{
	clock_t begin,end;
	begin = clock();
	CalcCaseCounts();
	if (confParams.ITER_KEEP)
		cout << "Iteration: 0 Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	int i = 0;
	int successful_attempts = 0, balanced_iter = 0;
	while (i <= maxIterCount && imbalancedCount > 0){
		++i;
		int node1, node2, node3;
		GetRandomTriad(node1, node2, node3);
		//if (LOUD)
		//	cout << "Triad: (" << node1 << ", " << node2 << ", " << node3 << ")" << endl;
		/*PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);*/
		int triadType = GetTriadType(node1, node2, node3);
		//cout << "Triad type: " << triadType << endl;
		//int caseNum;
		if (triadType%2==0){//IsBalancedNode(node1, node2, node3, caseNum)){
			++balanced_iter;
			
			if (i%printEvery == 0 && i!=0){
				CalcCaseCounts();
				if (confParams.ITER_KEEP)
					cout << "Iteration: " << i << " (balanced)" << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
				//system("pause");
			}
			continue;
		}
		int newTriadType = -1;
		bool plusToMinus = false; 
		if (triadType == 1){
			double prob = rnd.GetUniDev();
			if (prob < p){
				//if (triadType == 0)
				//	continue;
				if (LOUD)
					cout << "Triad type before changing - to +: " << triadType << " " << GetStrTriadType(triadType).CStr() << endl;
				ChangeSign(node1, node2, node3, plusToMinus);
				newTriadType = GetTriadType(node1, node2, node3);
				if (LOUD)
					cout << "Triad type after changing - to +: " << newTriadType << " "  <<  GetStrTriadType(newTriadType).CStr() << endl;
			}
			else {
				if (LOUD)
					cout << "Triad type before changing + to -:"  << triadType << " " <<  GetStrTriadType(triadType).CStr() << endl;
				plusToMinus = true;
				ChangeSign(node1, node2, node3, plusToMinus);
				newTriadType = GetTriadType(node1, node2, node3);
				if (LOUD)
					cout << "Triad type after changing + to -: " << newTriadType << " "  << GetStrTriadType(newTriadType).CStr() << endl;
			}
		}
		else if (triadType == 3){
			if (LOUD)
				cout << "Triad type before changing - to +: " << triadType << " "  <<  GetStrTriadType(triadType).CStr() << endl;
			ChangeSign(node1, node2, node3, plusToMinus);
			newTriadType = GetTriadType(node1, node2, node3);
			if (LOUD)
				cout << "Triad type after changing - to +: "  << newTriadType << " " << GetStrTriadType(newTriadType).CStr() << endl;
		}
		else {
			cout << "Errorneous" << endl;
		}
		if (triadType != newTriadType)
			++successful_attempts;
		
		if (i%printEvery == 0 && i!=0){
			CalcCaseCounts();
			if (confParams.ITER_KEEP)
				cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
		}
		//system("pause");
	}
	iter = i;
	bPart = GetBalancedPart();
	// cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	if (confParams.INST_KEEP)
		cout << "Iterations (balanced): " << balanced_iter << ", part of successful attempts for imbalanced: " << (double) successful_attempts / (i-balanced_iter) << endl;
	if (confParams.FINAL_STATE_KEEP)
		SaveFinalState(p, idRun);
	end = clock();
	PrintTriadsInfo();
	//PrintWeightMatrix();
	largestGroupSize = GetLargestGroupSize();
	cout << "Largest group size: " << largestGroupSize << endl;
	if (largestGroupSize == N)
		cout << "Paradise achieved!" << endl;
	cout << "Time of execution: " << (end-begin)/(double)CLOCKS_PER_SEC << endl;
}


//unised
int HeiderGraph::GetNbrRelations( int node )
{
	TNEANet::TNodeI nodeI = G->GetNI(node);
	int totalRelations = 0;
	for (int ei = 0; ei < nodeI.GetOutDeg(); ++ei){
		int nbr = nodeI.GetOutNId(ei);
		int relation = GetWeight(node, nbr);
		totalRelations += relation;
	}
	return totalRelations;
}

/**
 * Initialization of attributes
 */
void HeiderGraph::RandomInit()
{
	//number of values needed to store:
	int max_bits = 30, max_val = pow(2,max_bits)-1;
	int dv = ceil(double(d) / max_bits);
	TIntV bits;
	int b;
	for (b = d; b > 0; b-=max_bits){
		bits.Add(max_bits);
		max_vals.Add(max_val);
	}
	bits[dv-1] = b+max_bits;
	max_vals[dv-1] = pow(2,int(bits[dv-1]))-1;


	attr.Clr();

	for (int j = 0; j < dv; ++j){
		TIntV temp;
		for (int i = 0; i < N; ++i){
			int r = rnd.GetUniDevInt(0, pow(2, int(bits[j]))-1);
			temp.Add(r);
			//cout << r << " ";
		}
		attr.Add(temp);

	}

//	for (int j = 0; j < dv; ++j){
//		//cout << attr
//		for (int i = 0; i < N; ++i){
//			cout << attr[j][i] << " ";
//		}
//	}

//	//TIntVecV tempt;
//	for (int i = 0; i < N; ++i){
//		//TIntV temp;
//		for (int j = 0; j < d; ++j){
//			int val = 0;
//			double r = rnd.GetUniDev();
//			if (r >= 0.5)
//				val = -1;
//			else
//				val = 1;
//			val = ((attr[0][i] >> j) & 1)*2-1;
//
//			//cout << val;
//
//			G->AddIntAttrDatN(i, val, attrNames[j]);
//
//		}
//		//cout << endl;
//	}
//

	//PrintNodeAttrs(0);
//	PrintNodeAttrs(1);
//	PrintNodeAttrs(2);

	wasModified = true;
}

void HeiderGraph::PrintNodeAttrs( int i )
{
//	TStrV NIdAttrValue;
//	G->AttrValueNI(i, NIdAttrValue);
//	TInt AttrLen = NIdAttrValue.Len();
//	cout << "Node " << i << " attributes: " << "\t";
//	for (int j = 0; j < AttrLen; j++) {
//		cout << NIdAttrValue[j].CStr() << "\t";
//	}
//	cout << endl;

	for (int j = 0; j < attr.Len(); ++j){
	for (int k = d-1; k >= 0; --k) {
	   cout << ((attr[j][i] >> k) & 1);
	}
	}
	cout << endl;
}

/* returns +=1 */
int HeiderGraph::GetWeight( int node1, int node2 )
{
//	TIntV attr1, attr2;
//	G->IntAttrValueNI(node1, attr1);
//	G->IntAttrValueNI(node2, attr2);
//	if (attr1.Len() != attr2.Len())
//		cout << "attr1.Len() != attr2.Len()" << endl;
//	int weight = 0;
//	for (int i = 0; i < attr1.Len(); ++i){
//		weight += attr1[i] * attr2[i];
//	}
//	if (weight > 0)
//		weight = 1;
//	if (weight < 0)
//		weight = -1;

	int same_bits = 0;
	for (int i = 0; i < attr.Len(); ++i)
		same_bits += builtinPopcount((~(attr[i][node1] ^ attr[i][node2])) & max_vals[i]);

	int weight = same_bits > d/2. ? 1 : -1;


	//PrintNodeAttrs(node1);
	//PrintNodeAttrs(node2);
	//cout << "bits: " << same_bits << " weight: " << weight << endl;

	return weight;
}

bool HeiderGraph::IsBalanced(int ij_rel, int jk_rel, int ik_rel, int& case_num){
	if (ij_rel < 0 && jk_rel < 0 && ik_rel < 0)
		case_num = 1;
	if (ij_rel > 0 && jk_rel > 0 && ik_rel < 0)
		case_num = 2;
	if (ij_rel > 0 && jk_rel < 0 && ik_rel > 0)
		case_num = 3;
	if (ij_rel < 0 && jk_rel > 0 && ik_rel > 0)
		case_num = 4;

	bool balanced = false;

	if (ij_rel > 0  && jk_rel > 0  && ik_rel > 0){
		balanced = true;
		case_num = 5;
	}
	else if (ij_rel < 0 && jk_rel < 0 && ik_rel > 0){
		balanced = true;
		case_num = 6;
	}
	else if (ij_rel < 0 && jk_rel > 0 && ik_rel < 0){
		balanced = true;
		case_num = 7;
	}
	else if (ij_rel > 0 && jk_rel < 0 && ik_rel < 0){
		balanced = true;
		case_num = 8;
	}

	//if (LOUD)
	//	cout << balanced << endl;
	//system("pause");
	return balanced;
}

bool HeiderGraph::IsBalancedNode( int node1, int node2, int node3, int& caseNum )
{
	if (!G->IsEdge(node1, node2) || !G->IsEdge(node2, node3) || !G->IsEdge(node1, node3)) {
		cout << "Calculating balance for a non-triad" << endl;
		//system("pause");
		exit(NON_TRIAD);
	}
		
	int ij_rel = GetWeight(node1, node2);
	int jk_rel = GetWeight(node2, node3);
	int ik_rel = GetWeight(node1, node3);
	return IsBalanced(ij_rel, jk_rel, ik_rel, caseNum);
}

void HeiderGraph::CalcCaseCounts(){
	for (int i = 0; i < BALANCE_CASE_COUNT; ++i)
			caseCounts[i] = 0;

	balancedCount = 0, imbalancedCount = 0,
			positiveWeightsCount = 0, negativeWeightsCount = 0;

	for (int i = 0; i < N; ++i){
		for (int j = i+1; j < N; ++j){
			TIntV NbrV;
			TSnap::GetCmnNbrs(G, i, j, NbrV);
			for (int nbr = 0; nbr < NbrV.Len(); ++nbr){
				int k = NbrV[nbr];
				if (k < i || k < j)
					continue;

				//PrintNodeAttrs(i);
				//PrintNodeAttrs(i)
				//PrintNodeAttrs(i)
				int ij_rel = GetWeight(i, j);
				int jk_rel = GetWeight(j, k);
				int ik_rel = GetWeight(i, k);
				if (ij_rel > 0)
					++positiveWeightsCount;
				else
					++negativeWeightsCount;
				if (jk_rel > 0)
					++positiveWeightsCount;
				else
					++negativeWeightsCount;
				if (ik_rel > 0)
					++positiveWeightsCount;
				else
					++negativeWeightsCount;

				/*cout << "Weight(" << i << " ," << j << "): " << ij_rel << endl;
				cout << "Weight(" << j << " ," << k << "): " << jk_rel << endl;
				cout << "Weight(" << i << " ," << k << "): " << ik_rel << endl;*/
				int case_num = 0;
				bool balanced = IsBalanced(ij_rel, jk_rel, ik_rel, case_num);//, true);
				++caseCounts[case_num-1];
				if (balanced)
					++balancedCount;
				else
					++imbalancedCount;
					
				/*cout << "Balance: " << balanced << endl;
				system("pause");*/
			}
			//cout << i << " " << j << " nbrs: ";
			//for (int k = 0; k < NbrV.Len(); ++k)
			//	cout << NbrV[k] << " ";
			//cout << endl;
		}
	}
}

void HeiderGraph::PrintTriadsInfo()
{
	if (wasModified){
		CalcCaseCounts();
		wasModified = false;
	}
	cout << "Triads: " << triads << endl;
	cout << "Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	cout << "Positive weights part: " << (float)positiveWeightsCount/(triads * 3) <<
		" negative weights part: " << (float)negativeWeightsCount/(triads * 3) << endl;
	cout << "Case counts: " << endl;
	for (int i = 0; i < BALANCE_CASE_COUNT; ++i){
		cout << caseCounts[i] << " ";
	}
	
	cout << endl;
}


double HeiderGraph::GetBalancedPart()
{
	if (wasModified){
		CalcCaseCounts();
		wasModified = false;
	}
	return balancedCount / ((double) triads);
	//if (!wasModified)
	//	return balancedCount / (double) triads;
	//balancedCount = 0;
	//for (int i = 0; i < N; ++i){
	//	for (int i = j; i < N; ++j){
	//		if (i == j)
	//			continue;
	//		TIntV attr1, attr2;
	//		G->IntAttrValueNI(i, attr1);
	//		G->IntAttrValueNI(j, attr2);
	//		int weight = 0;
	//		for (int k = 0; k < this->d; k++)

	//}
	//return balancedCount / (double) triads;
}

double HeiderGraph::GetImbalancedPart()
{
	if (wasModified){
		CalcCaseCounts();
		wasModified = false;
	}
	return imbalancedCount / ((double) triads);
}

void HeiderGraph::GetCaseCounts( std::vector<int>& res )
{
	CalcCaseCounts();
	res = caseCounts;
}

HeiderGraph::~HeiderGraph(void)
{
}
