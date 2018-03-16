#pragma once

#include "base.h"
//#include "Constants.h"
#include <vector>
#include <iostream>

using  namespace std;

class Stat
{
public:
	std::vector<long long> iterations;
	std::vector<long long> largestGroupSizes;
	std::vector<int> neg_links;
	int hb, heaven;
	std::vector<int> mean_attr;
	std::vector<int> mean_attr_beg;
	std::vector<int> mean_attr_end;
	template <typename T>
	double Mean(std::vector<T>& vec);
	double Sigma(std::vector<long long>& vec);


	Stat(void) {hb = 0; heaven = 0;};
	void AddIterationsVal(int val) {iterations.push_back(val);}
	void AddLargestGroupSizeVal(int val) {largestGroupSizes.push_back(val);}
	void Clear() {iterations.clear();largestGroupSizes.clear();}
	double GetMeanIterations() {return Mean(iterations);}
	double GetSigmaIterations() {return Sigma(iterations);}
	double GetMeanLargestGroupSize() {return Mean(largestGroupSizes);};
	double GetSigmaLargestGroupSize(){return Sigma(largestGroupSizes);}
	~Stat(void);
};

template<typename T>
double Stat::Mean( vector<T>& vec )
{
	if (vec.size() == 0){
		cout << "Error! Calculating mean of a vector with zero count" << endl;
		return -1;
		//exit(ZERO_VECTOR_MEAN);
	}

	double mean = 0;
	for (unsigned int i = 0; i < vec.size(); ++i)
		mean += vec[i];

	mean /= vec.size();
	return mean;
}
