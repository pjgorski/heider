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
	if (type == "complete")
		G = TSnap::GenFull<PNEANet>(N);
	else{
		cout << "Type " << type.CStr() << " of HeiderGraph is not supported" << endl;
		exit(UNSUPPORTED_GRAPH_TYPE);
	}

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
	cout << "Node " << i << " attributes: ";
	for (int j = 0; j < AttrLen; j++) {
		cout << NIdAttrValue[j].CStr() << " ";
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

				++triads;
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
	return balancedCount / ((double) triads / 3);
}

double HeiderGraph::GetImbalancedPart()
{
	if (wasModified){
		CalcCaseCounts();
		wasModified = false;
	}
	return imbalancedCount / ((double) triads / 3);
}

void HeiderGraph::GetCaseCounts( std::vector<int>& res )
{
	CalcCaseCounts();
	res = caseCounts;
}

HeiderGraph::~HeiderGraph(void)
{
}
