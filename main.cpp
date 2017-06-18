//CLI for the calculator in C++

#include <string>
#include <iostream>
#include "cal.h"

int main(int argc, char* argv[]) {
	std::string expr, temp;
	cal::calc_exp calculator(expr);
	std::cout << "Usage: ? evaluate # quit" << std::endl;
	while (true) {
		std::getline(std::cin, temp);
		if (temp[0] == '#')
			break;
		else if (temp[0] == '?') {
			calculator.set_exp(expr);
			std::cout << calculator.calc() << std::endl;
			expr = "";
		} else
			expr += temp;
	}
	return 0;
}
