#include "stdafx.h"
#include <string>
#include "handler.hpp"

using namespace std;

int main(int argc, char* argv[]) {

	if(argc == 2){
		//cout << "fdfd " <<
		handler h;
		h = handler(argv[1]);
		h.work();
	}
	else{
		handler h;
		h = handler();

		h.save(handler::COMBINED);
		//h.save(handler::PAJEK_STRUCTURE);

		h.add_n("test");
		h.work();
	}
	//*/
	return 0;

}


