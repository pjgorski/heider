#include "stdafx.h"
#include "Stat.h"
#include "params.hpp"


using namespace std;

double Stat::Mean( vector<int>& vec )
{
	if (vec.size() == 0){
		cout << "Error! Calculating mean of a vector with zero count" << endl;
		return -1;
		//exit(ZERO_VECTOR_MEAN);
	}

	double mean = 0;
	for (int i = 0; i < vec.size(); ++i)
		mean += vec[i];

	mean /= vec.size();
	return mean;
}

double Stat::Sigma( vector<int>& vec )
{
	double mean = Mean(vec);
	double sigma = 0;
	for (int i = 0; i < vec.size(); ++i)
		sigma += pow(vec[i]-mean, 2.0);
	sigma /= (vec.size()-1);
	sigma = sqrt(sigma);
	return sigma;
}



Stat::~Stat(void)
{
}
