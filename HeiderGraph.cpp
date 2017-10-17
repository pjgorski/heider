#include "StdAfx.h"
#include "HeiderGraph.h"

#include <string>
#include <iostream>

using namespace std;

HeiderGraph::HeiderGraph(void)
{
}

/* type = ["complete"] */
HeiderGraph::HeiderGraph( int N, int d, TStr type )
{
	this->N = N;
	this->d = d;
	this->type = type;
	TRnd r(time(NULL), rand());
	rnd = r;
	if (type == "complete")
		G = TSnap::GenFull<PNEANet>(N);
	else{
		cout << "Type " << type.CStr() << " of HeiderGraph is not supported" << endl;
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

	// cout << "caseNum: " << caseNum << " res: " << res << endl;
	return res;
}

void HeiderGraph::ChangeMinusToPlus( int& node1, int& node2, int& node3 )
{
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}

	int ij_rel = GetWeight(node1, node2);
	int jk_rel = GetWeight(node2, node3);
	int ik_rel = GetWeight(node1, node3);

	//cout << "ij_rel: " << ij_rel << " jk_rel: " << jk_rel << " ik_rel: " << ik_rel << endl;

	TIntV ind;
	if (ij_rel < 0)
		ind.Add(1);
	if (jk_rel < 0)
		ind.Add(2);
	if (ik_rel < 0)
		ind.Add(3);

	if (ind.Len() == 0){
		cout << "ChangeMinus in + + + triad" << endl;
		system("pause");
		exit(ERR_TRANSFORM);
	}

	ind.Shuffle(rnd);
	int var = ind[0];
	if (var == 1){
		ChangeMinusToPlus(node1, node2);
		//cout << "var = 1" << endl;
	}
	else if (var == 2){
		ChangeMinusToPlus(node2, node3);
		//cout << "var = 2" << endl;
	}
	else if (var == 3){
		ChangeMinusToPlus(node1, node3);
		//cout << "var = 3" << endl;
	}
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}
}

void HeiderGraph::ChangeMinusToPlus( int& node1, int& node2 )
{
	TIntV diffAttrIndV;
	GetDiffAttrV(node1, node2, diffAttrIndV);
	int minusesCount = diffAttrIndV.Len(),
		plusesCount = d-minusesCount;
	//cout << "Minuses count: " << minusesCount << " Pluses count: " << plusesCount << endl;
	int plusesToAdd = ceil(d / 2.0) - plusesCount;
	//cout << "Pluses to add: " << plusesToAdd << endl;
	diffAttrIndV.Shuffle(rnd);
	for (int i = 0; i < plusesToAdd; ++i){
		int attrInd = diffAttrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
	}
	wasModified = true;
}

void HeiderGraph::ChangePlusToMinus( int& node1, int& node2, int& node3 )
{
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}

	int ij_rel = GetWeight(node1, node2);
	int jk_rel = GetWeight(node2, node3);
	int ik_rel = GetWeight(node1, node3);

	//cout << "ij_rel: " << ij_rel << " jk_rel: " << jk_rel << " ik_rel: " << ik_rel << endl;

	TIntV ind;
	if (ij_rel > 0)
		ind.Add(1);
	if (jk_rel > 0)
		ind.Add(2);
	if (ik_rel > 0)
		ind.Add(3);

	if (ind.Len() == 0){
		cout << "ChangeMinus in + + + triad" << endl;
		system("pause");
		exit(ERR_TRANSFORM);
	}

	ind.Shuffle(rnd);
	int var = ind[0];
	if (var == 1){
		ChangePlusToMinus(node1, node2);
		//cout << "var = 1" << endl;
	}
	else if (var == 2){
		ChangePlusToMinus(node2, node3);
		//cout << "var = 2" << endl;
	}
	else if (var == 3){
		ChangePlusToMinus(node1, node3);
		//cout << "var = 3" << endl;
	}
	if (LOUD){
		PrintNodeAttrs(node1);
		PrintNodeAttrs(node2);
		PrintNodeAttrs(node3);
	}
}

void HeiderGraph::ChangePlusToMinus( int& node1, int& node2 )
{
	TIntV simAttrIndV;
	GetSimAttrV(node1, node2, simAttrIndV);
	int plusesCount = simAttrIndV.Len(),
		minusesCount = d-plusesCount;
	//cout << "Minuses count: " << minusesCount << " Pluses count: " << plusesCount << endl;
	int minusesToAdd = ceil(d / 2.0) - minusesCount;
	//cout << "Minuses to add: " << minusesToAdd << endl;
	
	simAttrIndV.Shuffle(rnd);
	for (int i = 0; i < minusesToAdd; ++i){
		int attrInd = simAttrIndV[i];
		int val = G->GetIntAttrIndDatN(node1, attrInd);
		G->AddIntAttrDatN(node1, val * (-1), attrNames[attrInd]);
	}
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

void HeiderGraph::AntalDynamics( int maxIterCount, double p )
{
	cout << "Iteration: 0 Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	int i = 0;
	int successful_attempts = 0, balanced_iter = 0;
	while (i <= maxIterCount && imbalancedCount > 0){
		++i;
		int node1, node2, node3;
		GetRandomTriad(node1, node2, node3);
		if (LOUD)
			cout << "Triad: (" << node1 << ", " << node2 << ", " << node3 << ")" << endl;
		int triadType = GetTriadType(node1, node2, node3);
		if (IsBalanced(node1, node2, node3)){
			++balanced_iter;
			continue;
		}
		int newTriadType = -1;
		if (triadType == 1){
			double prob = rnd.GetUniDev();
			if (prob < p){
				if (LOUD)
					cout << "Triad type before changing - to +: " << triadType << " " << GetStrTriadType(triadType).CStr() << endl;
				ChangeMinusToPlus(node1, node2, node3);
				newTriadType = GetTriadType(node1, node2, node3);
				if (LOUD)
					cout << "Triad type after changing - to +: " << newTriadType << " "  <<  GetStrTriadType(newTriadType).CStr() << endl;
			}
			else {
				if (LOUD)
					cout << "Triad type before changing + to -:"  << triadType << " " <<  GetStrTriadType(triadType).CStr() << endl;
				ChangePlusToMinus(node1, node2, node3);
				newTriadType = GetTriadType(node1, node2, node3);
				if (LOUD)
					cout << "Triad type after changing + to -: " << newTriadType << " "  << GetStrTriadType(newTriadType).CStr() << endl;
			}
		}
		else if (triadType == 3){
			if (LOUD)
				cout << "Triad type before changing - to +: " << triadType << " "  <<  GetStrTriadType(triadType).CStr() << endl;
			ChangeMinusToPlus(node1, node2, node3);
			newTriadType = GetTriadType(node1, node2, node3);
			if (LOUD)
				cout << "Triad type after changing - to +: "  << newTriadType << " " << GetStrTriadType(newTriadType).CStr() << endl;
		}
		if (triadType != newTriadType)
			++successful_attempts;
		CalcCaseCounts();
		if (i%500 == 0 && i!=0)
			cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
		//system("pause");
	}
	cout << "Iteration: " << i << " Balanced part: " << GetBalancedPart() << " Imbalanced part: " << GetImbalancedPart() << endl;
	cout << "Iterations (balanced): " << balanced_iter << ", part of successful attempts for imbalanced: " << (double) successful_attempts / (i-balanced_iter) << endl;
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

	if (LOUD)
		cout << ij_rel << " " << jk_rel << " " << ik_rel << endl;

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

	if (LOUD)
		cout << balanced << endl;
	//system("pause");
	return balanced;
}

bool HeiderGraph::IsBalanced( int node1, int node2, int node3 )
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
