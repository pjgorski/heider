#include "StdAfx.h"
#include "Stat.h"




double Stat::GetMeanIterations()
{
	if (iterations.size() == 0)
		return -1;
	else{
		mean_iterations	 = 0;
		for (int i = 0; i < iterations.size(); ++i)
			mean_iterations += iterations[i];
	}
	mean_iterations /= iterations.size();
	return mean_iterations;
}

double Stat::GetSigmaIterations()
{
	GetMeanIterations();
	sigma_iterations = 0;
	for (int i = 0; i < iterations.size(); ++i)
		sigma_iterations += pow(iterations[i]-mean_iterations, 2.0);
	sigma_iterations /= (iterations.size()-1);
	sigma_iterations = sqrt(sigma_iterations);
	return sigma_iterations;
}

Stat::~Stat(void)
{
}
