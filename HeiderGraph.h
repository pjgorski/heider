#pragma once

//#include "windows.h"
//#include <base.h>
//#include <gbase.h>
//#include "attr.h"
//#include "graph.h"
//#include "network.h"
//#include "ggen.h"
//#include "triad.h"
#include "params.hpp"

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include "stdafx.h"

//#include <cstdint>
#include <x86intrin.h>

//inline uint64_t nthset(uint64_t x, unsigned n) {
//    return _pdep_u64(1ULL << n, x);
//}

inline unsigned builtinPopcount(unsigned bb)
{
    return __builtin_popcountll(bb);
}


typedef TVec<TIntV > TIntVecV;

class HeiderGraph
{
	PNEANet G; // graph
	TRnd rnd; //??
	int N;
	int E;
	int d;
	params confParams;
	TStr graphType;
	TStr changeSignType;
	std::vector<TStr> attrNames;
	std::vector<int> caseCounts;
	std::map<int,int> personType;
	int GetWeight(int node1, int node2);
	/* calculate number of different balance cases */
	void CalcCaseCounts();
	bool IsBalancedNode(int node1, int node2, int node3, int& case_num);
	bool IsBalanced(int ij_rel, int jk_rel, int ik_rel, int& case_num);
	void GetRandomTriad(int &node1, int& node2, int& node3);
	/* get random triad using neighbors of node1 */ 
	void GetRandomTriadForNode(const int &node1, int& node2, int& node3);
	/* triad type is a number of unfriendly links */
	int GetTriadType(int& node1, int& node2, int& node3);
	void ChangeSign(int& node1, int& node2, int& node3, bool isPlusToMinus);
	void ChangeSign(int& node1, int& node2, bool isPlusToMinus, std::string type = "attrRandom");
	void ChangeSign(int& node1, int& node2, bool isPlusToMinus, int first, std::string type);
	void ChangeSign (int& node1, int& node2, bool isPlusToMinus, int first);
	void ChangeSignAttrChoice(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrRandom(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrRandomCount(int& node1, int& node2, bool isPlusToMinus);
	void ChangeSignAttrMax(int& node1, int& node2, bool isPlusToMinus);
	TIntV GetDiffAttrV(int& node1, int& node2, TIntV& diffAttrIndV);
	TIntV GetSimAttrV(int& node1, int& node2, TIntV& simAttrIndV);
	int GetNbrRelations(int node);
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

	TIntVecV attr; //first index is attribute than node.


public:
	HeiderGraph(void);
	HeiderGraph(int N, int d,TRnd r,  TStr graphType, TStr changeSignType);
	/* types = [attrChoice, attrRandom, attrMax, attrRandomCount] */
	void AntalDynamics(int maxIterCount, double p, int& iter, int&largestGroupSize, double bPart, int printEvery, int idRun = 0 );

	void setParams(params& p){confParams = p;}
	void RandomInit();
	void PrintNodeAttrs(int i);
	void PrintTriadsInfo();
	double GetBalancedPart();
	double GetImbalancedPart();
	void GetCaseCounts(std::vector<int>& res);
	~HeiderGraph(void);
};

