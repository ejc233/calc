#include<string>
#pragma once

namespace cal {

class calc_exp {
public:
	calc_exp(std::string e);
	int set_exp(std::string e);
	std::string get_exp();
	double calc();
private:
	std::string expr;
	double result;
	bool done;
};

}

