/*
 * params.hpp
 *
 *  Created on: Feb 12, 2018
 *      Author: pgorski
 */

#ifndef PARAMS_HPP_
#define PARAMS_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <cstdlib>

#include "model_params.hpp"
#include "saving_parameter.hpp"

using namespace std;

typedef vector<int> ints;
typedef vector<double> doubles;
typedef vector<doubles> doubles2;
typedef vector<string> strings;
typedef map<string, doubles> map_str_dbl2;

class params {
public:
	params();
	params(string filename);
	params(int N, int A, double p);
	~params();

	void update_file_info_names();
	void clear_saving_parameters();

//important fields

//-----------------NETWORK STRUCTURE DATA--------------
	string structure; //filename with initial network structure and attributes

	int N; //nodes number
	int N_min, N_max, N_step; //parameters in the case many simulations should be run in one run
	int A; //number of attributes
	int attr_min, attr_max, attr_step; //parameters in the case many simulations should be run in one run

	std::string graphType; //type of graph

//----------------MODEL PARAMETERS
	double p; //p parameter
	double p_min, p_max, p_step; //parameters in the case many simulations should be run in one run
	std::string dynamicsType; //type of model dynamics

//---------------SIMULATION PARAMETERS-------------
	int iterations;
	int repetitions;

	//iteration management
	int inform_after; //after how many iterations should the user be informed about the progress
	int save_from; //start saving information after save_from iterations
	int save_every; //after how many iterations should the data be saved
	int print_every; //inform/check after number of iters

//--------------SAVING PARAMETERS------------------
	saving_parameter<int> Ns; // number of nodes
	saving_parameter<int> As; // number of attributes
	saving_parameter<double> ps; // values of p parameter used
	saving_parameter<int> hb; // achieved HB in simulation?
	saving_parameter<int> heaven; // achieved heaven in simulation?
	saving_parameter<int> neg_links; // number of negative links
	saving_parameter<int> larger_group_size; // size of larger group
	saving_parameter<int> iterations_needed; // number of needed iterations
	saving_parameter<int> iterations_max; // number of needed iterations
	saving_parameter<doubles> mean_attr; //norm of mean attribute
	saving_parameter<double> mean_attr_end; //end norm of mean attribute
	saving_parameter<double> mean_attr_beg; //beginning norm of mean attribute
	saving_parameter<int> reps; // number of repetitions

	saving_parameter<doubles> combined;

	saving_parameter<string> pajek_structure2;

	//--------------------------------------------FLAGS-----------------------
	bool TO_FILE;
	bool ITER_KEEP;
	bool INST_KEEP;
	bool FINAL_STATE_KEEP;

	char delimiter;

	string add_on; //this will be added to the end of the file
	string pajek_add_on;

protected:
	void initialize(std::string);
	void read_from_file(std::string filename);
};

const int BALANCE_CASE_COUNT = 8;
const bool LOUD = false;


const int UNSUPPORTED_GRAPH_TYPE = 2;
const int ZERO_TRIADS = 3;
const int NON_TRIAD = 4;
const int ERR_TRANSFORM = 5;
const int WRONG_CONF_FILE = 6;
const int ZERO_VECTOR_MEAN = 7;
const int WRONG_BASE_NODE_ID = 8;
const int UNSUPPORTED_CHANGE_SIGN_TYPE = 9;

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}


#endif /* PARAMS_HPP_ */
