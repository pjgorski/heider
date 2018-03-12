/*
 * model_params.hpp
 *
 *  Created on: 25-11-2016
 *      Author: pgorski
 */

#ifndef MODEL_PARAMS_HPP_
#define MODEL_PARAMS_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

using namespace std;

class model_params{
public:
	enum MODELS{
		ER,
		BA,
		UNKNOWN
	};

	model_params(MODELS m): type(m) {
		if (m == ER){
			name = "ER";
		}
		else if (m == BA){
			name = "BA";
		}
		else name = "UNKNOWN";
	}

	string get_name() const {return name;}

	const MODELS type;
protected:
	string name;
};


typedef boost::shared_ptr<model_params> model_params_ptr;


#endif /* MODEL_PARAMS_HPP_ */
