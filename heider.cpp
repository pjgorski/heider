// heider.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "HeiderGraph.h"
#include "HeiderTheor.h"
#include "Constants.h"
//#include "mmnet.h"
//#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>


using namespace std;

void get_avg_balance_parts(const int N, const int d, const int I, float&  avg_balanced, float& avg_imbalanced){
	vector<int>avg_case_count(BALANCE_CASE_COUNT);

	for (int num = 0; num < I; ++num){
		HeiderGraph G(N, d, "complete");
		vector<int> caseCounts;
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
			cout << "Balanced part: " << balanced_part << " Imbalanced part: " << imbalanced_part << endl;
		}
	} // iterations

	cout << "attrCount: " << d << " N: " << N << " ";
	cout << "Avg_balanced: " << avg_balanced / I << "  avg_imbalanced: " << avg_imbalanced / I << endl;
	if (LOUD){
		cout << "Avg cases: ";
		for (int i = 0; i < avg_case_count.size(); ++i){
			cout << avg_case_count[i]/I << " ";
		}
		cout << endl;
	}
}

void balance_test(vector<int> NVals, vector<int> attrVals, int I){
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
			cout << "Cannot calculate (overflow), d = " << d << endl;
		else{
			cout << "Balanced  count: " << balanced_count << " imbalanced count: " << imbalanced_count << endl;
			cout << "Total: " << (balanced_count*4 + imbalanced_count*4) * pow((double)2,d) << " pow(2,d) = " << pow(2.0,3* d) << endl;
			unsigned long long int baseline = pow((double)2,3*d)/2; 
			unsigned long long diff_balanced = balanced_count*4 * pow((double)2,d) - baseline,
				diff_imbalanced = baseline-imbalanced_count*4 * pow((double)2,d);
			cout << "Baseline: " << baseline << " diff_balanced = " << diff_balanced <<
				" diff_imbalanced = " << diff_imbalanced << endl;
			unsigned long long int to_add_imbalanced = diff_imbalanced-diff_balanced;
			cout << "To add imbalanced: " << to_add_imbalanced << endl;

			cout << "d: "<< d << " Part of imbalanced triangles (theoretical): " <<  imbalanced_count * pow(2.0, 2*(1-d)) << 
				" Part of balanced triangles (theoretical): " <<  balanced_count * pow(2.0, 2*(1-d))  << 
				" Total: " << (balanced_count + imbalanced_count) * pow(2.0, 2*(1-d)) << endl;
		}
	}
	system("pause");
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*const int attr_count = 5;
	check_triad_distr(attr_count);
	system("pause");*/
	
	//theor_balance_test(3, 21);


	cout << fixed;
    cout << setprecision(3);
	
	std::ofstream out("out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

	//freopen("out.txt","w",stdout);

	srand ((unsigned)time(NULL));
	//TNEANet Graph = TNEANet::TNEANet();
	const int I = 10;
	const int N_min = 20, N_max = 40, N_step = 10;
	const int attr_min = 3, attr_max = 11, attr_step = 2;
	//const int N_min = 10, N_max = 15, N_step = 5;
	//const int attr_min = 3, attr_max = 5, attr_step = 2;

	vector<int> NVals, attrVals;
	for (int i = N_min; i <= N_max; i+=N_step)
		NVals.push_back(i);
	for (int i = attr_min; i <= attr_max; i+=attr_step)
		attrVals.push_back(i);

	balance_test(NVals, attrVals, I);

	
	//int NodeId = 0;
	//for (TNEANet::TAIntI NI = G->BegNAIntI(v[0]);
	//NI < G->EndNAIntI(v[0]); NI++) {
	//// Check if defaults are now 0.
	//if (NI.GetDat()() != 0) {
	//	printf("Attribute: %s, Node: %i, Val: %i\n", v[0].CStr(), NodeId, NI.GetDat()());
	//	NodeId++;
	//}
 // } 
	//out.close();
	system("pause");
	return 0;
}

