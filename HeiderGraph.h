#pragma once

#include "windows.h"
#include "base.h"
#include "gbase.h"
#include "attr.h"
#include "graph.h"
#include "network.h"
#include "ggen.h"
#include "triad.h"
#include "Constants.h"

#include <vector>



class HeiderGraph
{
	PNEANet G;
	TRnd rnd;
	int N;
	int E;
	int d;
	TStr graphType;
	TStr changeSignType;
	std::vector<TStr> attrNames;
	std::vector<int> caseCounts;
	int GetWeight(int node1, int node2);
	/* calculate number of different balance cases */
	void CalcCaseCounts();
	bool IsBalancedNode(int node1, int node2, int node3, int& case_num);
	bool IsBalanced(int ij_rel, int jk_rel, int ik_rel, int& case_num);
	void GetRandomTriad(int &node1, int& node2, int& node3);
	int GetTriadType(int& node1, int& node2, int& node3);
	void ChangeSign(int& node1, int& node2, int& node3, bool isPlusToMinus);
	void ChangeSign(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrChoice(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrRandom(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignTarget(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrRandomCount(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrMax(int& node1, int& node2, bool isPlusToMinus);
	void GetDiffAttrV(int& node1, int& node2, TIntV& diffAttrIndV);
	void GetSimAttrV(int& node1, int& node2, TIntV& simAttrIndV);
	void SafeAttrModification(int& node, int& attrInd);
	void GetStat(int& node, std::vector<int>& oldNbrWeights, std::vector<int>&oldNbrCaseCounts, int& oldNbrBalancedCount, int& oldNbrImbalancedCount,
		int& oldNbrPositiveWCount, int& oldNbrNegativeWCount);
	TStr GetStrTriadType(int triadType);
	int GetLargestGroupSize();
	void PrintWeightMatrix();
	void Mutate(double pm);
	int balancedCount;
	int imbalancedCount;
	int positiveWeightsCount;
	int negativeWeightsCount;
	int triads;
	/* if an attribute was modified and we did not recalculate caseCount, 
	balanced/imbalancedCount, positive/negativeWeightsCount, 
	wasModified is set to true, and after recalculation is set to false*/
	bool wasModified;
	void SaveFinalState(double p, int idRun);
public:
	HeiderGraph(void);
	HeiderGraph(int N, int d, TStr graphType, TStr changeSignType);
	/* types = [attrChoice, attrRandom] */
	void AntalDynamics(int maxIterCount, double p, int& iter, int&largestGroupSize, double bPart, int printEvery, int idRun = 0 );
	void RandomInit();
	void PrintNodeAttrs(int i);
	void PrintTriadsInfo();
	double GetBalancedPart();
	double GetImbalancedPart();
	void GetCaseCounts(std::vector<int>& res);
	~HeiderGraph(void);
};

