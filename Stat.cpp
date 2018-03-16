#include "stdafx.h"
#include "Stat.h"
#include "params.hpp"


using namespace std;



double Stat::Sigma( vector<long long>& vec )
{
	double mean = Mean(vec);
	double sigma = 0;
	for (unsigned int i = 0; i < vec.size(); ++i)
		sigma += pow(vec[i]-mean, 2.0);
	sigma /= (vec.size()-1);
	sigma = sqrt(sigma);
	return sigma;
}



Stat::~Stat(void)
{
}
