/*
 * params.cpp
 *
 *  Created on: Feb 13, 2018
 *      Author: pgorski
 */

#include "params.hpp"

params::params(){
	initialize(" ");
}

params::params(string filename){
	initialize(filename);
}

params::params(int N_, int A_, double p_){
	initialize(" ");

	N = N_min = N_max = N_;
	A = attr_min = attr_max = A_;
	p = p_min = p_max = p_;

	update_file_info_names();
}

params::~params(){
}

void params::initialize(std::string filename){

//-----------------NETWORK STRUCTURE DATA--------------
	structure=""; //filename with initial network structure

	N = 3;
	N_min = N_max = 3;
	N_step = 1;

	A = attr_min = attr_max = 3;
	attr_step = 1;

	graphType = "complete";

//----------------MODEL PARAMETERS
	p = p_min = 0;
	p_max = 1;
	p_step = 0.1;
	dynamicsType = "attrRandom";

//---------------SIMULATION PARAMETERS-------------
	iterations = 100000;
	repetitions = 1;

	//iteration management
	inform_after = 10000; //after how many iterations should the user be informed about the progress
	save_from = 100; //start saving hist information after save_hist_from iterations
	save_every = 10000; //after how many iterations should the data be saved
	print_every = 100;


//--------------SAVING PARAMETERS------------------
	delimiter = ' ';
	add_on = ".txt"; //this will be added to the end of the file
	pajek_add_on = ".net";

	//file info
	string n = "modheider_";
	stringstream h;
	h << "_n_" << N << "_" << "A_" << A << "_" << "p_" << p;

	string h2 = h.str();

	mean_attr.name = (n + "mean_attr" + h2);
	combined.name = (n + "alldata" + h2);

	reps.flag = true;
	combined.flag = true;

	pajek_structure2.name = (n + "pajek_" + h2);

	//-----------------FLAGS-------------------
	TO_FILE = false;
	ITER_KEEP = false;
	INST_KEEP = false;
	FINAL_STATE_KEEP = false;

	if(filename != " ")
		read_from_file(filename);
}

void params::read_from_file(std::string filename){
	string line;
	ifstream file (filename.c_str());

	int i = 0;
	map_str_dbl2 params;

	if (file.is_open()){
		while ( getline (file,line) ){
			cout << line << endl;
			istringstream is(line);
			string sub, sub2;
			is >> sub;
			//names.push_back(sub);

			if(sub == "add_on"){
				sub.clear();
				is >> sub;

				add_on = sub + add_on;
				pajek_add_on = sub + pajek_add_on;
				++i;
				continue;
			}
			else if(sub == "structure"){
				sub.clear();
				is >> sub;

				structure = sub;
				++i;
				continue;
			}
			else if(sub == "Graph_type"){
				sub.clear();
				is >> sub;

				graphType = sub;
				++i;
				continue;
			}
			else if(sub == "Dynamics_type"){
				sub.clear();
				is >> sub;

				dynamicsType = sub;
				++i;
				continue;
			}

			doubles dat;
			while(is){
				sub2.clear();
				is >> sub2;
				dat.push_back(atof(sub2.c_str()));
			}

			params[sub] = dat;
			++i;
		}

		file.close();
  	}
	else cout << "Unable to open file";

	map_str_dbl2::iterator it, end = params.end();

	it = params.find("N");
	if(it == end){} //error?
	else {
		doubles v = it->second;
		if (v.back()==0){
			v.pop_back();
		}
		if(v.size() == 2){
			N_min = v[0];
			N_max = v[1];
			N_step = 1;
			N = N_min;
		}else if(v.size() == 3){
			N_min = v[0];
			N_max = v[1];
			N_step = v[2];
			N = N_min;
		}else if (v.size() == 1){
			N_min = N_max = N = v[0];
			N_step = 1;
		}
	}

	it = params.find("A");
	if(it == end){} //error?
	else {
		doubles v = it->second;
		if (v.back()==0){
			v.pop_back();
		}
		if(v.size() == 2){
			attr_min = v[0];
			attr_max = v[1];
			attr_step = 2;
			A = attr_min;
		}else if(v.size() == 3){
			attr_min = v[0];
			attr_max = v[1];
			attr_step = v[2];
			A = attr_min;
		}else if (v.size() == 1){
			attr_min = attr_max = A = v[0];
			attr_step = 2;
		}
	}

	it = params.find("p");
	if(it == end){} //error?
	else {
		doubles v = it->second;
		if (v.back()==0){
			v.pop_back();
		}
		//
		if(v.size() == 2){
			p_min = v[0];
			p_max = v[1];
			//if (p_max == 0)
			p_step = 0.1;
			p = p_min;
		}else if(v.size() == 3){
			p_min = v[0];
			p_max = v[1];
			p_step = v[2];
			p = p_min;
		}else if (v.size() == 1){
			p_min = p_max = p = v[0];
			p_step = 0.1;
		}
		//cout << v.size() << " ";
//		for (int i = 0; i < v.size(); ++i)
//			cout << v[i] << " " ;
	}
/*
	it = params.find("Q");
	if(it != end)
		Q = (int) (it->second).at(0);*/

	it = params.find("iterations");
	if(it != end)
		iterations = (long long) pow(10,(it->second).at(0));

	it = params.find("repetitions");
	if(it != end)
		repetitions = (int) (it->second).at(0);

	it = params.find("inform_after");
	if(it != end)
		inform_after = (int) (it->second).at(0);

	it = params.find("save_from");
	if(it != end)
		save_from = (int) (it->second).at(0);

	it = params.find("save_every");
	if(it != end)
		save_every = (int) (it->second).at(0);

	it = params.find("print_every");
	if(it != end)
		print_every = (int) (it->second).at(0);

	if (print_every == -1)
		print_every = N*(N-1)/2;


	it = params.find("mean_attr");
	if(it != end){
		mean_attr.flag = (bool) (it->second).at(0);
	}

	it = params.find("pajek_structure");
	if(it != end){
		pajek_structure2.flag = (bool) (it->second).at(0);
	}

	it = params.find("TO_FILE");
	if(it != end){
		TO_FILE = (bool) (it->second).at(0);
	}

	it = params.find("ITER_KEEP");
	if(it != end){
		ITER_KEEP= (bool) (it->second).at(0);
	}

	it = params.find("INST_KEEP");
	if(it != end){
		INST_KEEP= (bool) (it->second).at(0);
	}

	it = params.find("FINAL_STATE_KEEP");
	if(it != end){
		FINAL_STATE_KEEP= (bool) (it->second).at(0);
	}



	update_file_info_names();
}


void params::update_file_info_names(){
	//file info
	string n = "modheider_";
	stringstream h;
	h << "_n_" << N << "_" << "A_" << A << "_";

	string h2 = h.str();

	mean_attr.name = (n + "mean_attr" + h2);
	combined.name = (n + "alldata" + h2);

	pajek_structure2.name = (n + "pajek_" + h2);
}

void params::clear_saving_parameters(){
	Ns.clear();// the number of iterations that passed corresponding to data written in dist vectors
	As.clear();// save relative change hop distance between random users
	ps.clear(); // save relative change hop distance between best users
	hb.clear(); //save Kin after a number of  iterations
	heaven.clear(); //similarity matrix
	neg_links.clear(); //distribution of similarity
	larger_group_size.clear();
	iterations_needed.clear(); //distribution of mean sim of nearest neighbors
	mean_attr.clear(); //distribution of similarity between nn
	mean_attr_end.clear(); //number of follows
	mean_attr_beg.clear(); //number of retweets
	combined.clear();//number of unfollows
	iterations_max.clear(); //similarities during follow

	pajek_structure2.clear();
}



