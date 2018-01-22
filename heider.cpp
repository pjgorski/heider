// heider.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include "Constants.h"
#include "HeiderGraph.h"
#include "HeiderTheor.h"
#include "Stat.h"
//#include <comdef.h> // for conversion between wchar and char
//#include "mmnet.h"
//#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <vector>


typedef long long _Longlong;


//using namespace std;

void get_avg_balance_parts(const int N, const int d, const int I, float&  avg_balanced, float& avg_imbalanced){
	std::vector<int>avg_case_count(BALANCE_CASE_COUNT);

	for (int num = 0; num < I; ++num){
		HeiderGraph G(N, d, "complete", "attrChoice");
		std::vector<int> caseCounts;
		G.GetCaseCounts(caseCounts);
		float balanced_part = G.GetBalancedPart(),
			imbalanced_part = G.GetImbalancedPart();

		for (int i = 0; i < BALANCE_CASE_COUNT; ++i)
		{
			avg_case_count[i] += caseCounts[i];
		}

		avg_balanced += balanced_part;
		avg_imbalanced += imbalanced_part;
		if (LOUD){
			std::cout << "Balanced part: " << balanced_part << " Imbalanced part: " << imbalanced_part << std::endl;
		}
	} // iterations

	std::cout << "attrCount: " << d << " N: " << N << " ";
	std::cout << "Avg_balanced: " << avg_balanced / I << "  avg_imbalanced: " << avg_imbalanced / I << std::endl;
	if (LOUD){
		std::cout << "Avg cases: ";
		for (int i = 0; i < avg_case_count.size(); ++i){
			std::cout << avg_case_count[i]/I << " ";
		}
		std::cout << std::endl;
	}
}

void balance_test(std::vector<int> NVals, std::vector<int> attrVals, int I){
	for (int attrInd = 0; attrInd < attrVals.size(); ++attrInd){
		for (int NInd = 0; NInd < NVals.size(); ++NInd){
			int attrCount = attrVals[attrInd],
				N = NVals[NInd];
			float balanced_part = 0, imbalanced_part = 0;
			get_avg_balance_parts(N, attrCount, I, balanced_part, imbalanced_part);
		}
	}
}

void theor_balance_test(int dmin, int dmax){
	for (int d = dmin; d < dmax; d+=2)
	{
		unsigned long long int imbalanced_count = get_minus_minus_count(d);
		unsigned long long int balanced_count = get_minus_minus_count(d, true);
		//cout << "Number of minus-minus-minus cases: " << get_minus_minus_count(d) << endl;
		if (d > 31)
			std::cout << "Cannot calculate (overflow), d = " << d << std::endl;
		else{
			std::cout << "Balanced  count: " << balanced_count << " imbalanced count: " << imbalanced_count << std::endl;
			std::cout << "Total: " << (balanced_count*4 + imbalanced_count*4) * pow((double)2,d) << " pow(2,d) = " << pow(2.0,3* d) << std::endl;
			unsigned long long int baseline = pow((double)2,3*d)/2; 
			unsigned long long diff_balanced = balanced_count*4 * pow((double)2,d) - baseline,
				diff_imbalanced = baseline-imbalanced_count*4 * pow((double)2,d);
			std::cout << "Baseline: " << baseline << " diff_balanced = " << diff_balanced <<
				" diff_imbalanced = " << diff_imbalanced << std::endl;
			unsigned long long int to_add_imbalanced = diff_imbalanced-diff_balanced;
			std::cout << "To add imbalanced: " << to_add_imbalanced << std::endl;

			std::cout << "d: "<< d << " Part of imbalanced triangles (theoretical): " <<  imbalanced_count * pow(2.0, 2*(1-d)) << 
				" Part of balanced triangles (theoretical): " <<  balanced_count * pow(2.0, 2*(1-d))  << 
				" Total: " << (balanced_count + imbalanced_count) * pow(2.0, 2*(1-d)) << std::endl;
		}
	}
	system("pause");
}

std::ofstream out;

void create_file(const params& confParams, const double p){
	long long int rndInstanceNumber = rand()%1000 + 5;

	_Longlong N_min = (_Longlong)confParams.N_min, N_max = (_Longlong)confParams.N_max,
					attr_min = (_Longlong)confParams.attr_min, attr_max = (_Longlong)confParams.attr_max;
	long double p_ld = (long double)p;

	std::string outFileName = ToString(N_min) + "_" + ToString(N_max) + "_" + ToString(attr_min) + "_"
		+ ToString(attr_max) + "_" + ToString(p_ld);
	

	//std::wstring outFileName = std::to_wstring(N_min) + L"_" + std::to_wstring(N_max) + L"_" 
	//	+ std::to_wstring(attr_min) + L"_"	+ std::to_wstring(attr_max) + L"_" + std::to_wstring(p_ld);
	if (p == 1 || p == 0){
		//outFileName += L".0";
		outFileName += ".0";
	}
	std::string dynType(confParams.dynamicsType.begin(), confParams.dynamicsType.end());

	//outFileName += L"_"  + std::to_string((long long)confParams.I) + L"_" + dynType + L"_" + std::to_wstring(rndInstanceNumber) + L".txt";
	outFileName += "_" + ToString(confParams.I) + "_" + dynType + "_" + ToString(rndInstanceNumber) + ".txt";
	//const wchar_t * wname = outFileName.c_str();
	
	//const char *ofn = outFileName.c_str();
	//out.open(ofn, std::ios::out);

	//if (!out){
	//	std::cout << "Error of file creation" << std::endl;
	//}

	if (TO_FILE){
		//std::cout << "OutFileName: " << outFileName.c_str() << std::endl;
		freopen(outFileName.c_str(), "w", stdout);
		//std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
		//std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	}
}

int main(int argc, char* argv[])
{
	/*const int attr_count = 5;
	check_triad_distr(attr_count);
	system("pause");*/
	params confParams;
	//theor_balance_test(3, 21);
	read_conf(confParams);

	std::cout << std::fixed;
    	std::cout << std::setprecision(3);

	srand((unsigned)time(NULL));
	

	//freopen("out.txt","w",stdout);

	

	for (double p = 0; p <= 1; p += confParams.p){
		create_file(confParams, p);
		for (int N = confParams.N_min; N <= confParams.N_max; N+= confParams.N_step){
			int printEvery = -1;
			for (int i = 0; i < confParams.printEvery.size(); ++i){
				if (N <= confParams.printEvery[i].first){
					printEvery = confParams.printEvery[i].second;
					break;
				}
			}
			if (printEvery == -1)
				printEvery = confParams.printEveryLast;

			for (int attr = confParams.attr_min; attr <= confParams.attr_max; attr += confParams.attr_step)
			{
				std::cout << "N: " << N << " d: " << attr << std::endl;
				//HeiderGraph G(N, attr, "complete", "attrRandom");
				//HeiderGraph G(N, attr, "complete", "attrChoice");
				HeiderGraph G(N, attr, TStr(confParams.graphType.c_str()), TStr(confParams.dynamicsType.c_str()));

				Stat statInfo;
										
				for (int i = 0; i < confParams.I; ++i){
					if (INST_KEEP)
						std::cout << "INSTANCE:  " << i << std::endl;
					int iter = 0, largestGroupSize = 0;
					double bPart = 0;
					/* iter, largestGroupSize � out parameters */
					G.AntalDynamics(confParams.maxIter, p, iter, largestGroupSize, bPart, printEvery, i);
					statInfo.AddIterationsVal(iter);
					statInfo.AddLargestGroupSizeVal(largestGroupSize);
					G.RandomInit();
					//system("pause");
				}
				std::cout << "Mean iterations to converge: " << statInfo.GetMeanIterations() << " std = " << statInfo.GetSigmaIterations() << std::endl;
				std::cout << "Mean largest group size: " << statInfo.GetMeanLargestGroupSize() << " std = " << statInfo.GetSigmaLargestGroupSize() << std::endl;
				std::cout << std::endl;
			}
		}
		out.close();
	} // p



	/* BALANCE TEST */
	//const int I = 30;
	//const int N_min = 20, N_max = 40, N_step = 10;
	//const int attr_min = 3, attr_max = 11, attr_step = 2;
	////const int N_min = 10, N_max = 15, N_step = 5;
	////const int attr_min = 3, attr_max = 5, attr_step = 2;

	//vector<int> NVals, attrVals;
	//for (int i = N_min; i <= N_max; i+=N_step)
	//	NVals.push_back(i);
	//for (int i = attr_min; i <= attr_max; i+=attr_step)
	//	attrVals.push_back(i);

	//balance_test(NVals, attrVals, I);
	system("pause");
	//out.close();
	return 0;
}

