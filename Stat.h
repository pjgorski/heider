#pragma once

#include "base.h"
#include "Constants.h"
#include <vector>

class Stat
{
	std::vector<int> iterations;
	std::vector<int> largestGroupSizes;
	double Mean(std::vector<int>& vec);
	double Sigma(std::vector<int>& vec);
public:
	Stat(void) {};
	void AddIterationsVal(int val) {iterations.push_back(val);}
	void AddLargestGroupSizeVal(int val) {largestGroupSizes.push_back(val);}
	void Clear() {iterations.clear();largestGroupSizes.clear();}
	double GetMeanIterations() {return Mean(iterations);}
	double GetSigmaIterations() {return Sigma(iterations);}
	double GetMeanLargestGroupSize() {return Mean(largestGroupSizes);};
	double GetSigmaLargestGroupSize(){return Sigma(largestGroupSizes);}
	~Stat(void);
};

