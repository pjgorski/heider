#include "StdAfx.h"
#include "HeiderTheor.h"

#include <string>
#include <iostream>

using namespace std;


int get_weight(vector<int> node1, vector<int>node2){
	int weight = 0;
	for (int i = 0; i < node1.size(); ++i){
		int val1 = node1[i] == 1? 1 : -1;
		int val2 = node2[i] == 1? 1 : -1;
		weight += val1 * val2;
	}
	return (weight > 0) ? 1 : -1;
}

bool is_balanced(int ij_rel, int jk_rel, int ik_rel, int& case_num){
	if (ij_rel < 0 && jk_rel < 0 && ik_rel < 0)
		case_num = 1;
	if (ij_rel > 0 && jk_rel > 0 && ik_rel < 0)
		case_num = 2;
	if (ij_rel > 0 && jk_rel < 0 && ik_rel > 0)
		case_num = 3;
	if (ij_rel < 0 && jk_rel > 0 && ik_rel > 0)
		case_num = 4;

	bool balanced = false;

	if (LOUD)
		cout << ij_rel << " " << jk_rel << " " << ik_rel << endl;

	if (ij_rel > 0  && jk_rel > 0  && ik_rel > 0){
		balanced = true;
		case_num = 5;
	}
	else if (ij_rel < 0 && jk_rel < 0 && ik_rel > 0){
		balanced = true;
		case_num = 6;
	}
	else if (ij_rel < 0 && jk_rel > 0 && ik_rel < 0){
		balanced = true;
		case_num = 7;
	}
	else if (ij_rel > 0 && jk_rel < 0 && ik_rel < 0){
		balanced = true;
		case_num = 8;
	}

	if (LOUD)
		cout << balanced << endl;
	//system("pause");
	return balanced;
}

unsigned long long int factorial(unsigned long long int n)
{

	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

unsigned long long int binomial(unsigned long long int n, unsigned long long int k){
	if (k < 0 || n < 0)
		cout << "Wrong parameters in binomial(): n = " << n << " k = " << k << endl;
	unsigned long long int res = 1;
	//cout << "fact(" << n << ") = " << fn << endl;
	//cout << "Binomial1 (" << n << " " << k << "): " << factorial(n) / (factorial(k) * factorial(n-k)) << endl;
	return factorial(n) / (factorial(k) * factorial(n-k));
	//if (k >= n/2)
	//	k = n-k;
	//for (unsigned long long int i = 1; i <= k; ++i)
	//{
	//	res *= ((n-i+1)/(long double)i);
	//}
	////cout << "Binomial2 (" << n << " " << k << "): " << res << endl;
	////system("pause");
	//return res;
}

const int MINUS_MINUS_LOUD = false;

/* number of minus-minus-minus combinatons divided by pow(2,d) - assumed to be used to calculate part of imbalanced*/
unsigned long long get_minus_minus_count(int d, bool isPlusPlus){
	unsigned long long int count = 0;
	int k_min = 0, k_max = floor(d/2.0);
	if (isPlusPlus)
		k_min = ceil(d/2.0), k_max = d;
	for (int k = k_min; k <= k_max; ++k)
		//for (int k = 0; k <= floor(d/2.0); ++k) 
	{
		unsigned long long int sum_similar = 0, sum_different = 0, sum_different_l = 0;
		//sum_similar += pow(2.0, k);
		sum_similar = binomial(d, k);

		if (MINUS_MINUS_LOUD) cout << "k = " << k <<  //" sum_similar = " << sum_similar << endl;
			" l = 0.." <<  k/2.0 << endl;

		for (int l = 0; l < k/2.0; ++l)
		{
			//if (LOUD) cout << "l: " << l << " binomial(k,l) = " << binomial(k,l) << endl; 
			int i_min = ceil(d/2.0) - (k-l), i_max = d-k-i_min;
			if (isPlusPlus){
				//i_max = floor(d/2.0)-l;
				//i_min = d-k-i_max;
				//if (i_max > d-k){
				//cout << "i_max - (d-k)" << i_max - (d-k) << endl;
				//system("pause");
				if (i_min < 0){
					i_max = d-k;
					i_min = 0;
				}
			}

			if (MINUS_MINUS_LOUD) cout << "i_min: " << i_min << " i_max: " << i_max << endl;
			unsigned long long int third_node_variants = 0;
			for (int i = i_min; i <= i_max; ++i){
				//if (LOUD) cout << "d-k: " << d-k << " i: " << i << endl;
				third_node_variants += binomial(d-k,i)  ;
				//if (LOUD) cout << "sum_different for i: " << binomial(d-k,i)  << endl;
			}
			third_node_variants *= binomial(k, l);
			sum_different_l += third_node_variants;

			//cout << "l = " << l << " binomial(k,l) = " << binomial(k,l) << " pow(2, d-k): " << pow(2.0, d-k) << " " << " binomial(d-k, ceil((d-k)/2.0)): " << binomial(d-k, ceil((d-k)/2.0)) <<  endl;
			//cout << "l = " << l << " sum_different: " << binomial(k,l) * pow(2.0, d-k) * binomial(d-k, ceil((d-k)/2.0)) * 2 << endl;;
		}
		//cout << "sum_different after l-binomial: " << sum_different << endl;
		sum_different = sum_different_l;
		//cout << "sum_different after power: " << sum_different << endl;
		// cout << "d-k: " << d-k << " ceil(d-k/2.0): "<< ceil(d-k/2.0) << endl;
		//cout << "sum_different after d-k-binomial: " << sum_different << endl;
		count += sum_similar * sum_different;
		if (LOUD) cout << "k = " << k << " sum_similar: " << sum_similar << " sum_different: " << sum_different << " count: " << count << " added: " << sum_different*sum_similar << endl;
	}

	return count;
}

void check_triad_distr(int attr_count){
	int balanced_count = 0,
		imbalanced_count = 0;
	int max_attr_val = pow(2.0, attr_count);

	vector<int> case_counts(BALANCE_CASE_COUNT);
	for (int i = 0; i < max_attr_val; ++i){
		for (int j = 0; j < max_attr_val; ++j){
			for (int k = 0; k < max_attr_val; ++k){
				vector<int> node1, node2, node3;
				for (int l = 0; l < attr_count; ++l){
					int val1 = (i >> l) & 1,
						val2 = (j >> l) & 1,
						val3 = (k >> l) & 1;
					node1.push_back(val1);
					node2.push_back(val2);
					node3.push_back(val3);
				}
				int ij = get_weight(node1, node2),
					jk = get_weight(node2, node3),
					ik = get_weight(node1, node3);
				int case_num;
				bool balanced = is_balanced(ij, jk, ik, case_num);//, true);
				//if (case_num == 1){
				//	cout << node1[0] << " " << node1[1] << " " << node1[2] << endl;
				//	cout << node2[0] << " " << node2[1] << " " << node2[2] << endl;
				//	cout << node3[0] << " " << node3[1] << " " << node3[2] << endl << endl;
				//}

				++case_counts[case_num-1];

				if (balanced)
					++balanced_count;
				else
					++imbalanced_count;
			}
		}
	}
	cout << "Balanced count: " << balanced_count << " imbalanced count: " << imbalanced_count << endl;
	cout << "Balanced part: " << (float)balanced_count/(pow(max_attr_val,3.0)) << " Imbalanced part: " << (float)imbalanced_count/((pow(max_attr_val,3.0))) << endl;
	cout << "Case counts: " ;
	for (int i = 0; i < case_counts.size(); ++i){
		cout << case_counts[i] << " ";
	}
	cout << endl;

}