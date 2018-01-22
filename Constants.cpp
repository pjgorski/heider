#include "stdafx.h"
#include "Constants.h"

bool read_flag(std::string flagName){
	std::ifstream in(file);
	if(!in.is_open()) {
		std::cout << "Failed to read config file" << std::endl;
		system("pause");
		exit(WRONG_CONF_FILE);
	}

	std::string s;

	while(!in.eof()) {
		std::getline(in,s);
		int desc_end = s.find_first_of(":");
		if (desc_end != s.length()-1 && desc_end != std::string::npos){
			std::string parName(s.begin(), s.begin() + desc_end);
			s.erase(s.begin(), s.begin() + desc_end + 2);
			if (parName == flagName){
				//std::cout << s.size() << " " << s << std::endl;
				s.erase(s.end()-1, s.end());
				return s == "true"? true : false;
			}
		}
	}

	if(!in.is_open()) {
		std::cout << "Failed to find flagValue of " << flagName << std::endl;
		system("pause");
		exit(WRONG_CONF_FILE);
	}
}

/* read configuration params from configuration file */
void read_conf(params& confParams){
	std::ifstream in(file);
	if(!in.is_open()) {
		std::cout << "Failed to read config file" << std::endl;
		system("pause");
		exit(WRONG_CONF_FILE);
	}

	std::string s;
	int par_index = 0;
	while(!in.eof()) {
		std::getline(in,s);
		int desc_end = s.find_first_of(":");
		std::string parName;
		if (desc_end != s.length()-1 && desc_end != std::string::npos){
			parName = s.substr(0, desc_end);
			s.erase(s.begin(), s.begin() + desc_end + 2);
		}

		switch(par_index){
		case 0: 
			confParams.I = std::atoi(s.c_str());
			break;
		case 1: 
			confParams.N_min = std::atoi(s.c_str());
			break;
		case 2:
			confParams.N_max = std::atoi(s.c_str());
			break;
		case 3: 
			confParams.N_step = std::atoi(s.c_str());
			break;
		case 4:
			confParams.attr_min = std::atoi(s.c_str());
			break;
		case 5:
			confParams.attr_max = std::atoi(s.c_str());
			break;
		case 6:
			confParams.attr_step = std::atoi(s.c_str());
			break;
		case 7:
			confParams.p = std::atof(s.c_str());
			break;
		case 8:
			confParams.maxIter = std::atoi(s.c_str());
			break;
		case 9:
			confParams.dynamicsType = s.c_str();
			break;
		case 10:
			confParams.graphType = s.c_str();
			break;
		case 11:
			{
				bool isEnd = false;
				while (!isEnd){
					std::getline(in,s);
					desc_end = s.find_first_of(" ");
					
					if (desc_end != std::string::npos){
						std::string first(s.begin(), s.begin()+desc_end);
						int val1 = std::atoi(first.c_str());
						std::string second(s.begin()+desc_end+1, s.end());
						int val2 = std::atoi(second.c_str());
						confParams.printEvery.push_back(std::make_pair(val1, val2));
					}
					else{
						confParams.printEveryLast = std::atoi(s.c_str());
						isEnd = true;
					}
					++par_index;
				}
				break;
			}
		default:
			break;
		}
		++par_index;
	}


	//std::cout << PROBABILITY_TO_FALL << std::endl;
	in.close();
}
