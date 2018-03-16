#include "handler.hpp"

handler::handler(){
	confParams = params();
}

handler::handler(string filename) {
	confParams = params(filename);
}

handler::handler(int nodes_, int attr, double p, int repetitions_, int iterations_){

	confParams = params(nodes_, attr, p);

	confParams.repetitions = repetitions_;
	confParams.iterations = iterations_;
}

void handler::reset_vectors(){
	confParams.clear_saving_parameters();
}

void handler::work(){

	reset_vectors();

	//clock_t t_all = clock(), t = clock();

		std::cout << std::fixed;
	    	std::cout << std::setprecision(3);

		srand((unsigned)time(NULL));


		//freopen("out.txt","w",stdout);
		TRnd r(time(NULL), rand());
		//cout << "reps  "<< confParams.repetitions;
		//for(int j = 0; j < confParams.repetitions; ++j){
		//cout << confParams.p_step;
		for (double p = confParams.p_min; p <= confParams.p_max; p += confParams.p_step){
			create_file(confParams, p);
			for (int N = confParams.N_min; N <= confParams.N_max; N+= confParams.N_step){
				int printEvery = confParams.print_every;
//				for (int i = 0; i < confParams.printEvery.size(); ++i){
//					if (N <= confParams.printEvery[i].first){
//						printEvery = confParams.printEvery[i].second;
//						break;
//					}
//				}
//				if (printEvery == -1)
//					printEvery = confParams.printEveryLast;

				for (int attr = confParams.attr_min; attr <= confParams.attr_max; attr += confParams.attr_step)
				{
					std::cout << "N: " << N << " d: " << attr << " p: " << p << std::endl;
					//HeiderGraph G(N, attr, "complete", "attrRandom");
					//HeiderGraph G(N, attr, "complete", "attrChoice");
					HeiderGraph G(N, attr, r, TStr(confParams.graphType.c_str()), TStr(confParams.dynamicsType.c_str()));
					G.setParams(confParams);
					Stat statInfo;

					long long iter = 0;
					int largestGroupSize = 0;
					double bPart = 0;
					/* iter, largestGroupSize � out parameters */
					//cout << "reps  "<< confParams.repetitions;
					for (int i = 0; i < confParams.repetitions; ++i){
						if (confParams.INST_KEEP)
							std::cout << "INSTANCE:  " << i << std::endl;

						G.AntalDynamics(confParams.iterations, p, iter, largestGroupSize, bPart, printEvery, i);
						statInfo.AddIterationsVal(iter);
						statInfo.AddLargestGroupSizeVal(largestGroupSize);

						if (iter < confParams.iterations){
//							confParams.hb.push_back(1);
							statInfo.hb += 1;
						}

						if (largestGroupSize == N){
							statInfo.heaven += 1;
//							confParams.heaven.push_back(1);
						}
						statInfo.neg_links.push_back((N-largestGroupSize)*largestGroupSize);
						//confParams.Ns.push_back(N);
						G.RandomInit();
						//system("pause");
					}
					doubles combined = doubles();
					combined.push_back(N);
					combined.push_back(attr);
					combined.push_back(p);
					combined.push_back(log10(confParams.iterations));
					combined.push_back(confParams.repetitions);
					combined.push_back(statInfo.hb);
					combined.push_back(statInfo.heaven);
					combined.push_back(statInfo.Mean(statInfo.neg_links)/((double)N*(N-1)/2));
					combined.push_back(statInfo.GetMeanLargestGroupSize());
					combined.push_back(statInfo.GetSigmaLargestGroupSize());

					combined.push_back(statInfo.GetMeanIterations());
					combined.push_back(statInfo.GetSigmaIterations());
					//combined.push_back(N);
					confParams.combined.push_back(combined);


					std::cout << "Mean iterations to converge: " << statInfo.GetMeanIterations() << " std = " << statInfo.GetSigmaIterations() << std::endl;
					std::cout << "Mean largest group size: " << statInfo.GetMeanLargestGroupSize() << " std = " << statInfo.GetSigmaLargestGroupSize() << std::endl;
					std::cout << std::endl;
				}
			}
		} // p





		//przepisywanie wynikow do pliku
		if(confParams.combined.flag ){
			//save(DISTANCE);
			save(confParams.combined);
		}



//		if(params.pajek_structure2.flag){
//			//save(PAJEK_STRUCTURE);
//			save(confParams.pajek_structure2);
//		}
		reset_vectors();
		//hi.clear();
		//hdr.clear();
		//hdc.clear();

//		delete net;
//		t_all = clock() - t_all;
//#ifndef FOUT
//		cout << "elapsed s: " << ((float)t_all)/CLOCKS_PER_SEC << endl;
//		t_all = clock();
//		cout << j+1 << " ";
//#else
//		fout << "elapsed s: " << ((float)t_all)/CLOCKS_PER_SEC << endl;
//		t_all = clock();
//		fout << j+1 << " ";
//#endif
//	}
}
	
void handler::save(OPTIONS opt){
//	if(opt == COMBINED){
//		//confParams.each_Kin = true;
//		unsigned int n = confParams.combined.size();
//		if(n != 0){
//			//unsigned int n = d_each_Kin.size();
//			ofstream data_file((confParams.combined.name + confParams.add_on).c_str(), ofstream::app);
//			for(unsigned int j = 0; j < n; ++j){
//				data_file << confParams.combined[j] << confParams.delimiter;
//			}
//			data_file << endl;
//			data_file.close();
//		//	d_each_Kin.clear();
//			confParams.combined.clear();
//		}
//	}


	if(opt == PAJEK_STRUCTURE){
		//params.pajek_structure = true;
		unsigned int n = confParams.pajek_structure2.size();
		if(n != 0){
			for(unsigned int i = 0; i < n; ++i){
				string nr = "i" + std::string(size_t(1), '0' + i);
				ofstream data_file((confParams.pajek_structure2.name + nr + confParams.pajek_add_on).c_str(), ofstream::app);
				data_file << confParams.pajek_structure2[i];
				data_file.close();
			}
			confParams.pajek_structure2.clear();
		}
	}
//	if(opt == HIST_CC){
//		params.hist_cc = true;
//		if(d_hist_cc.size() != 0){
//			int n = d_hist_cc.size();
//			ofstream cc_hist_file((params.f_hist_cc + confParams.add_on).c_str(), ofstream::app);
//
//			for(int i = 0; i < n; ++i){
//				for(unsigned int k = 0; k < d_hist_cc[i].size(); ++k){
//					cc_hist_file << d_hist_cc[i][k] << confParams.delimiter;
//				}
//				cc_hist_file << endl;
//				d_hist_cc[i].clear();
//			}
//			cc_hist_file.close();
//			//d_hist_Kin.clear();
//		}
//	}
}

template<typename T>
void handler::save(saving_parameter<T>& par){
	save(par, bool2type<std::numeric_limits<T>::is_specialized>());
}

template <>
void handler::save(saving_parameter<string>& par){
	unsigned int n = par.size();
	if(n != 0){
		for(unsigned int i = 0; i < n; ++i){
			string nr = "i" + std::string(size_t(1), '0' + i);
			ofstream data_file((par.name + nr + confParams.pajek_add_on).c_str(), ofstream::app);
			data_file << par[i];
			data_file.close();
		}
		par.clear();
	}
}

template <typename T>
void handler::save(saving_parameter<T>& par, bool2type<true>){
	unsigned int n = par.size();
	if(n != 0){

		ofstream data_file((par.name + confParams.add_on).c_str(), ofstream::app);

		//if (numeric_limits<T>::is_specialized){
			for(unsigned int j = 0; j < n; ++j){
				data_file << par[j] << confParams.delimiter;



			}
		/*}
		else{
			for(unsigned int j = 0; j < n; ++j){
				unsigned int m= par[j].size();
				for(unsigned int k = 0; k < m; ++k){
					data_file << par[j][k] << params.delimiter;
				}
				data_file << endl;
			}

		}*/
		data_file << endl;
		data_file.close();
		//	d_iters.clear();
		par.clear();
	}
}

template <typename T>
void handler::save(saving_parameter<T>& par, bool2type<false>){
	unsigned int n = par.size();
	if(n != 0){

		ofstream data_file((par.name + confParams.add_on).c_str(), ofstream::app);

		/*if (numeric_limits<T>::is_specialized){
			for(unsigned int j = 0; j < n; ++j){
				data_file << par[j] << confParams.delimiter;



			}
		}
		else{*/
			for(unsigned int j = 0; j < n; ++j){
				unsigned int m= par[j].size();
				for(unsigned int k = 0; k < m; ++k){
					data_file << par[j][k] << confParams.delimiter;
				}
				data_file << endl;
			}

		//}
		//data_file << endl;
		data_file.close();
		//	d_iters.clear();
		par.clear();
	}
}



/*
template<typename T> void handler::save(saving_parameter par){
	unsigned int n = par.size();
	if(n != 0){

				ofstream data_file((confParams.sim_distr.name + confParams.add_on).c_str(), ofstream::app);
				for(unsigned int j = 0; j < n; ++j){
					unsigned int m = confParams.sim_distr[j].size();
					for(unsigned int k = 0; k < m; ++k){
						data_file << confParams.sim_distr[j][k] << confParams.delimiter;
					}
					data_file << endl;
				}
				data_file.close();
				//d_distr_sim.clear();
				confParams.sim_distr.clear();
			}
}*/

void handler::set_save_name(string f, OPTIONS opt){
//	if(opt == DIST_CLOS)
//		confParams.dist_clos2.name = f;
////	else if(opt == EACH_INFO)
////		f_each_info = f;
//	else if(opt == DIST_RAND)
//		confParams.dist_rand2.name = f;
//	else if(opt == ITERS)
//		confParams.iters2.name = f;
//	else if(opt == PAJEK_STRUCTURE)
//		confParams.pajek_structure2.name = f;
//	else if(opt == HIST_CC)
//		confParams.f_hist_cc = f;
}

/*
void handler::prepare_network_structure(hsystem* hs, int nr){
	int node_code;
	ints out_codes;

	nodes_it it = hs->get_all().begin(), end = hs->get_all().end(),
		itw, endw;
	for(; it != end; ++it){
		node_code = (*it)->get_code() + 1; //1 is added not to have node number 0
		
		nodes output = (*it)->get_input();

				itw = output.begin(); 
				endw = output.end();
				for(; itw != endw; ++itw){
					out_codes.push_back(get_node_code(itw) + 1);//1 is added not to have node number 0
				}

				data[save_moment][node_code] = out_codes;
				out_codes.clear();
			}
		}
}
*/

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
	outFileName += "_" + ToString(confParams.iterations) + "_" + dynType + "_" + ToString(rndInstanceNumber) + ".txt";
	//const wchar_t * wname = outFileName.c_str();

	//const char *ofn = outFileName.c_str();
	//out.open(ofn, std::ios::out);

	//if (!out){
	//	std::cout << "Error of file creation" << std::endl;
	//}

	if (confParams.TO_FILE){
		//std::cout << "OutFileName: " << outFileName.c_str() << std::endl;
		freopen(outFileName.c_str(), "w", stdout);
		//std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
		//std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	}
}

