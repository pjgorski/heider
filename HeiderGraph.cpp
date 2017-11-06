#include "StdAfx.h"
#include "HeiderGraph.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <ctime> // time_t
#include <cstdio>

using namespace std;

HeiderGraph::HeiderGraph(void)
{
}

/* type = ["complete"] */
HeiderGraph::HeiderGraph( int N, int d, TStr graphType, TStr changeSignType )
{
	this->N = N;
	this->d = d;
	this->graphType = graphType;
	this->changeSignType = changeSignType;
	TRnd r(time(NULL), rand());
	rnd = r;
	if (graphType == "complete")
		G = TSnap::GenFull<PNEANet>(N);
	else{
		cout << "Type " << graphType.CStr() << " of HeiderGraph is not supported" << endl;
		exit(UNSUPPORTED_GRAPH_TYPE);
	}

	this->E = G->GetEdges();
	for (int i = 0; i < d; ++i){
		string s = "attr" + to_string((long long)i);
		attrNames.push_back(TStr(s.c_str()));
		G->AddIntAttrN(attrNames[i],0);
	}
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
		system("pause");
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
}

void HeiderGraph::GetRandomTriadForNode( const int &node, int& nbr1, int& nbr2 )
{
	TIntV nbrs;
	TNEANet::TNodeI nodeI = G->GetNI(node);
	int nbrCount = nodeI.GetOutDeg();
	if (nbrCount < 2){
		cout << "Attempt to get triad for a node with degree " << nbrCount << endl;
		system("pause");
		exit(NON_TRIAD);
	}
	nbr1 = node;
	nbr2 = node;
	/* get indexes of two random edges of node */
	while (nbr1 == node || nbr2 == node || nbr1 == nbr2){
		if (nbr1 == node)
			nbr1 = rnd.GetUniDevInt(0, nbrCount);
		if (nbr2 == node || nbr2 == nbr1)
			nbr2 = rnd.GetUniDevInt(0, nbrCount);
	}
	/* get neighbors by indexes */
	nbr1 = nodeI.GetOutNId(nbr1);
	nbr2 = nodeI.GetOutNId(nbr2);

}

int HeiderGraph::GetTriadType( int& node1, int& node2, int& node3 )
{
	if (!G->IsEdge(node1, node2) || !G->IsEdge(node2, node3) || !G->IsEdge(node1, node3)) {
		cout << "Calculating balance for a non-triad" << endl;
		system("pause");
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
	/*PLEASE REMOVE THIS TERRIBLE ASSIGNMENT */
	changeSignType = type.c_str();
	if (changeSignType == "attrChoice")
		ChangeSignAttrChoice(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrRandom")
		ChangeSignAttrRandom(node1, node2, isPlusToMinus);
	else if (changeSignType == "target")
		ChangeSignTarget(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrRandomCount")
		ChangeSignAttrRandomCount(node1, node2, isPlusToMinus);
	else if (changeSignType == "attrMax")
		ChangeSignAttrMax(node1, node2, isPlusToMinus);
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
void HeiderGraph::ChangeSignAttrRandom (int& node1, int& node2, bool isPlusToMinus, int first){
	if (first != node1 && first != node2){
		cout << "Attempt to change attribute in incorrect node, node 1 =  " << node1 
			<< " node2 = " << node2 << " base node: " << first << endl;
		system("pause");
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
		system("pause");
		exit(UNSUPPORTED_CHANGE_SIGN_TYPE);
	}
}

void HeiderGraph::ChangeSignAttrRandom( int& node1, int& node2, bool isPlusToMinus )
{
	TIntV attrIndV;
	if (isPlusToMinus)
		GetSimAttrV(node1, node2, attrIndV);
	else
		GetDiffAttrV(node1, node2, attrIndV);

	attrIndV.Shuffle(rnd);
	int attrInd = attrIndV[0];
	int nodeToChange = rnd.GetUniDevInt(1,2);

	if (nodeToChange == 1){
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
	}
	else{
		int val = G->GetIntAttrIndDatN(node2, attrInd);
		G->AddIntAttrDatN(node2, val * (-1), attrNames[attrInd]);
	}

	wasModified = true;
}

bool pairCompare(const pair<int, int>& a, const pair<int, int>& b){
	return a.first < b.first;
}

void HeiderGraph::ChangeSignTarget( int& node1, int& node2, bool isPlusToMinus )
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
	signsToAdd = attrIndV.Len();
	attrIndV.Shuffle(rnd);


	vector<pair<int,int>> absWeights;
	for (int i = 0; i < N; i++){
		TIntV v1, v2;
		G->IntAttrValueNI(node1, v1);
		G->IntAttrValueNI(i, v2);
		int sum = 0;
		for (int j = 0; j < attrNames.size(); ++j)
			sum += v1[i] * v2[i];
		absWeights.push_back(make_pair(i,sum));
	}
	sort(absWeights.begin(), absWeights.end(), pairCompare);
	vector<int> weights_before, weights_after;
	int maxCount = N/10;
	for (int i = 0; i < maxCount; ++i){
		weights_before.push_back(absWeights[i].second);
	}

	int oldBalancedCount = balancedCount;
	for (int i = 0; i < signsToAdd; ++i){
		
		int attrInd = attrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		
		////system("pause");
	}

	for (int i = 0; i < maxCount; ++i){
		weights_after.push_back(absWeights[i].second);
	}

	int diff = 0;
	for (int i = 0; i < maxCount; ++i){
		diff += weights_after[i]-weights_before[i];
	}


	//CalcCaseCounts();
	//cout << "old: " << oldBalancedCount << " new: " << balancedCount << endl;
	if (diff < 0) {
		for (int i = 0; i < signsToAdd; ++i){

			int attrInd = attrIndV[i];
			int val = G->GetIntAttrIndDatN(node1, attrInd);
			G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
		}
	}

	/* check */
	wasModified = true;
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

void HeiderGraph::GetDiffAttrV( int& node1, int& node2, TIntV& diffAttrIndV )
{
	diffAttrIndV.Clr();
	TIntV attr1, attr2;
	G->IntAttrValueNI(node1, attr1);
	G->IntAttrValueNI(node2, attr2);

	for (int i = 0; i < attr1.Len(); ++i){
		if (attr1[i] != attr2[i])
			diffAttrIndV.Add(i);
	}

}

void HeiderGraph::GetSimAttrV( int& node1, int& node2, TIntV& simAttrIndV )
{
	simAttrIndV.Clr();
	TIntV attr1, attr2;
	G->IntAttrValueNI(node1, attr1);
	G->IntAttrValueNI(node2, attr2);

	for (int i = 0; i < attr1.Len(); ++i){
		if (attr1[i] == attr2[i])
			simAttrIndV.Add(i);
	}
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
	string outFileName = "network_" + to_string((long long)N) + "_" + to_string((long long)d) + "_" 
		+ to_string((long double)p) + "_" 
		+ to_string((long long)idRun) + "_" + changeSignType.CStr() + ".txt";
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
	if (ITER_KEEP)
		cout << "Iteration: 0 Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	int i = 0;
	int successful_attempts = 0, balanced_iter = 0;
	while (i <= maxIterCount && imbalancedCount > 0){
		++i;
		int node1, node2, node3;
		GetRandomTriad(node1, node2, node3);
		//Mutate(0.1);
		//if (LOUD)
		//	cout << "Triad: (" << node1 << ", " << node2 << ", " << node3 << ")" << endl;
		/*PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);*/
		int triadType = GetTriadType(node1, node2, node3);
		//cout << "Triad type: " << triadType << endl;
		int caseNum;
		if (IsBalancedNode(node1, node2, node3, caseNum)){
			++balanced_iter;
			
			if (i%printEvery == 0 && i!=0){
				CalcCaseCounts();
				if (ITER_KEEP)
					cout << "Iteration: " << i << " (balanced)" << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
				//system("pause");
			}
			//continue;
		}
		int newTriadType = -1;
		bool plusToMinus = false; 
		if (triadType == 1){
			double prob = rnd.GetUniDev();
			if (prob < p){
				if (triadType == 0)
					continue;
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
			if (ITER_KEEP)
				cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
		}
		//system("pause");
	}
	iter = i;
	bPart = GetBalancedPart();
	// cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	if (INST_KEEP)
		cout << "Iterations (balanced): " << balanced_iter << ", part of successful attempts for imbalanced: " << (double) successful_attempts / (i-balanced_iter) << endl;
	if (FINAL_STATE_KEEP)
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

/* p - percent of extroverts */
void HeiderGraph::SetIntrovertsExtroverts(double p){
	
	for (TNEANet::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) { 
		int id = NI.GetId();
		if (rnd.GetUniDev() >= p)
			personType[id] = -1;
		else
			personType[id] = 1;
	}
}

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

/* there are p \cdot N introvert nodes and (1-p) \cdot N extravert nodes 
   extraverts do not want to make relations with neighbors worse:
   that is, when extravert is in ++- triangle (imbalanced), it chooses
   only ++- => +++ transformation (not ++- => --+);
   and introvert don't mind if its individual relation will worsen
   after attempt of balancing triangle
   that is: 
   we choose a random node and a random triangle of this node
   if random triangle is balanced, do nothing
   if it is imbalanced:
   if it is of --- type, change relation with one of the neighbors to +
   if it is of ++- type and and node is extravert:
   if it has ++ relation to both nodes, do nothing
   if it has one minus relation, change it to plus
   if node is introvert:
   revert one of the relations
*/
void HeiderGraph::IntrovertExtrovertDynamics( int maxIterCount, double p, int& iter, int& largestGroupSize, double bPart, int printEvery, int idRun /*= 0*/ )
{
	SetIntrovertsExtroverts(p);
	clock_t begin,end;
	begin = clock();
	CalcCaseCounts();
	if (ITER_KEEP)
		cout << "Iteration: 0 Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	int i = 0;
	int successful_attempts = 0, balanced_iter = 0;
	while (i <= maxIterCount && imbalancedCount > 0){
		++i;

		/* get random node */
		int node = rnd.GetUniDevInt(0, N-1);
		TIntV nodeAttr;

		/* get random triangle comprising chosen node */
		int nbr1 = node, nbr2 = node;
		GetRandomTriadForNode(node, nbr1, nbr2);
		
		int sign1 = GetWeight(node, nbr1),
			sign2 = GetWeight(node, nbr2),
			sign3 = GetWeight(nbr1, nbr2);

		int caseNum = -1;
		//cout << personType[node] << endl;
		// if a person is an introvert
		if (IsBalanced(sign1, sign2, sign3, caseNum))
			continue;
		/* --- case */
		else if (caseNum == 0){
			int nbrToChange = rnd.GetUniDevInt(1);
			if (nbrToChange == 0)
				ChangeSignAttrRandom(node, nbr1, false, node);
			else
				ChangeSignAttrRandom(node, nbr2, false, node);
		}
		/* ++- case */
		else {
			/* if person is introvert */
			if (personType[node] == -1){
				int nbrToChange = rnd.GetUniDevInt(1);
				if (nbrToChange == 0)
					nbrToChange = nbr1;
				else 
					nbrToChange = nbr2;
				if (sign1 == -1)
					ChangeSignAttrRandom(node, nbrToChange, false, node);
				else
					ChangeSignAttrRandom(node, nbrToChange, true, node);
			}
			/* if a person is extravert */
			else{
				if (sign1 == 1 && sign2 == 1)
					continue;
				int nbrToChange = sign1 == -1? nbr1: nbr2;
				ChangeSignAttrRandom(node, nbrToChange, false, node);
			}
		}
		
		
		if (i%printEvery == 0 && i!=0){
			CalcCaseCounts();
			//if (i%10 == 0)
			//	system("pause");
			if (ITER_KEEP)
				cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
		}
		//system("pause");
	}
	iter = i;
	bPart = GetBalancedPart();
	// cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	if (INST_KEEP)
		cout << "Iterations (balanced): " << balanced_iter << ", part of successful attempts for imbalanced: " << (double) successful_attempts / (i-balanced_iter) << endl;
	if (FINAL_STATE_KEEP)
		SaveFinalState(p, idRun);
	end = clock();
	PrintTriadsInfo();
	//PrintWeightMatrix();
	largestGroupSize = GetLargestGroupSize();
	cout << "Largest group size: " << largestGroupSize << endl;
	if (largestGroupSize == N)
		cout << "Paradise achieved!" << endl;
	cout << "Time of execution: " << (end-begin)/(double)CLOCKS_PER_SEC << " p: " << p << endl;
}

void HeiderGraph::RandomInit()
{
	for (int i = 0; i < N; ++i){
		for (int j = 0; j < d; ++j){
			int val = rand()%2;
			if (val == 0)
				val = -1;
			G->AddIntAttrDatN(i, val, attrNames[j]);
		}
	}
	wasModified = true;
}

void HeiderGraph::PrintNodeAttrs( int i )
{
	TStrV NIdAttrValue;
	G->AttrValueNI(i, NIdAttrValue);
	TInt AttrLen = NIdAttrValue.Len();
	cout << "Node " << i << " attributes: " << "\t";
	for (int j = 0; j < AttrLen; j++) {
		cout << NIdAttrValue[j].CStr() << "\t";
	} 
	cout << endl;
}

/* returns +=1 */
int HeiderGraph::GetWeight( int node1, int node2 )
{
	TIntV attr1, attr2;
	G->IntAttrValueNI(node1, attr1);
	G->IntAttrValueNI(node2, attr2);
	if (attr1.Len() != attr2.Len())
		cout << "attr1.Len() != attr2.Len()" << endl;
	int weight = 0;
	for (int i = 0; i < attr1.Len(); ++i){
		weight += attr1[i] * attr2[i];
	}
	if (weight > 0)
		weight = 1;
	if (weight < 0)
		weight = -1;
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

	//if (LOUD)
	//	cout << ij_rel << " " << jk_rel << " " << ik_rel << endl;

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
		system("pause");
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
