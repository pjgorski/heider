/*
TODO: powinien do kazdego pliku byc dodatkowo wygenerowany plik informaujacy o parametrach tej symulacji
*/

#ifndef HSYSTEM_HANDLER_HPP
#define HSYSTEM_HANDLER_HPP

#include <iostream>
#include <cstdio>
#include <fstream>
#include <ctime>
#include <sstream>
#include <string.h>
#include <vector>
#include <iomanip>
//#include <map>

#include "params.hpp"
//#include "network.hpp"
//#include "network_generator.hpp"
//#include "er_network_generator.hpp"
#include "saving_parameter.hpp"
#include "HeiderGraph.h"
#include "Stat.h"

typedef vector<double> doubles;
typedef vector<int> ints;
typedef vector<doubles > doubles2;
typedef vector<ints > ints2;
//typedef map<int, int> map_int;
//typedef map<int, ints> map_ints;
//typedef vector<map_ints> vec_map_ints;
typedef vector<std::string > strings;

template<bool> struct bool2type { };

typedef long long _Longlong;

using namespace std;

class handler {
public:
	enum OPTIONS{
		MEAN_ATTR,
		COMBINED,
		PAJEK_STRUCTURE,
	};

	handler();
	handler(string filename);
	handler(int nodes_, int attr, double p, int repetitions, int iterations);

	//void initialize();
	void reset_vectors();
	void work();

	void set_inform_after(int i) {confParams.inform_after = i;}
	void set_save_hist_from(int i) {confParams.save_from = i;}
	void set_save_every(int i) {confParams.save_every = i;}
	
	void save(OPTIONS);

	template<typename T> void save(saving_parameter<T>& par);
	template<typename T> void save(saving_parameter<T>& par, bool2type<true>);//bool2type<std::numeric_limits<T>::is_specialized>()
	template<typename T> void save(saving_parameter<T>& par, bool2type<false>);//bool2type<std::numeric_limits<T>::is_specialized>()

	void set_delimiter (char c) {confParams.delimiter = c;}
	void set_save_name(string, OPTIONS);
	void add_n(string n) {confParams.add_on = n + confParams.add_on; confParams.pajek_add_on = n + confParams.pajek_add_on;}



protected:

	params confParams;
};

template <>
void handler::save(saving_parameter<string>& par);

void create_file(const params& confParams, const double p);

template <typename T>
std::string ToString2(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

#endif
