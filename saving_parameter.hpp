/*
 * SavingParameter.hpp
 *
 *  Created on: 05-01-2017
 *      Author: pgorski
 */

#ifndef SAVINGPARAMETER_HPP_
#define SAVINGPARAMETER_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>

using namespace std;

typedef vector<double> doubles;
typedef vector<int> ints;
typedef vector<doubles > doubles2;
typedef vector<ints > ints2;
typedef vector<std::string > strings;

//

template < typename T >
class saving_parameter{
public:
	saving_parameter() : name(""), flag(false){}
	saving_parameter(string name_): name(name_), flag(false) {}

	string get_filename() {return name;}
	//void save(char delimiter, string add_on);

	void push_back(T data) {data_container.push_back(data);}
	void clear() {data_container.clear(); }
	unsigned int size() const {return data_container.size();}
	T& operator[](unsigned int i){return data_container[i];}

	string name;
	bool flag;
	//string

	vector<T> data_container;

private:
	//const string name;
	//void save_impl(char delimiter, string,bool2type<std::numeric_limits<T>::is_specialized>);
	//void save_impl(char delimiter, string,bool2type<true>);bool2type<std::numeric_limits<T>::is_specialized>()
	//void save_impl(char delimiter, string,bool2type<false>);

};



#endif /* SAVINGPARAMETER_HPP_ */
