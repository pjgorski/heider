#pragma once

#include "base.h"
#include "Constants.h"
#include <vector>

class Stat
{
	std::vector<int> iterations;
	double mean_iterations;
	double sigma_iterations;
public:
	Stat(void) {mean_iterations = 0; sigma_iterations = 0;};
	void AddIterationsVal(int val) {iterations.push_back(val);}
	void Clear() {iterations.clear();}
	double GetMeanIterations();
	double GetSigmaIterations();
	~Stat(void);
};

