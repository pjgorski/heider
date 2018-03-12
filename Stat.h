#pragma once

#include "base.h"
//#include "Constants.h"
#include <vector>
#include <iostream>

using  namespace std;

class Stat
{
public:
	std::vector<int> iterations;
	std::vector<int> largestGroupSizes;
	std::vector<int> neg_links;
	int hb = 0, heaven = 0;
	std::vector<int> mean_attr;
	std::vector<int> mean_attr_beg;
	std::vector<int> mean_attr_end;
	double Mean(std::vector<int>& vec);
	double Sigma(std::vector<int>& vec);


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

