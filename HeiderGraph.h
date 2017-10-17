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
	int N;
	int d;
	TStr type;
	std::vector<TStr> attrNames;
	std::vector<int> caseCounts;
	int GetWeight(int node1, int node2);
	/* calculate number of different balance cases */
	void CalcCaseCounts();
	bool IsBalanced(int ij_rel, int jk_rel, int ik_rel, int& case_num);
	int balancedCount;
	int imbalancedCount;
	int positiveWeightsCount;
	int negativeWeightsCount;
	int triads;
	/* if an attribute was modified and we did not recalculate caseCount, balanced/imbalancedCount, positive/negativeWeightsCount, 
	wasModified is set to true, and after recalculation is set to false*/
	bool wasModified;
public:
	HeiderGraph(void);
	HeiderGraph(int N, int d, TStr type);
	void RandomInit();
	void PrintNodeAttrs(int i);
	void PrintTriadsInfo();
	double GetBalancedPart();
	double GetImbalancedPart();
	void GetCaseCounts(std::vector<int>& res);
	~HeiderGraph(void);
};

