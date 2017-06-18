#include "cal.h"
#include <cmath>
#include <tuple>
#include <unordered_map>
#include <cctype>

using std::tuple;
using std::tie;
using std::make_tuple;

//Mathematical constants
const double MATH_PI = 4 * std::atan(1);
const double MATH_E = std::exp(1);

//memory
std::unordered_map<double, double> memory;

//constructor
cal::calc_exp::calc_exp(std::string s) :
		expr(s), result(0), done(false) {
}

//getter and setter for expr
int cal::calc_exp::set_exp(std::string s) {
	expr = s;
	done = false;
	return 0;
}

std::string cal::calc_exp::get_exp() {
	return expr;
}

//skip space, tab and \n
void next(char* &c) {
	while (isspace(*c))
		c++;
}

//Binary operators
tuple<double, char*> eval(tuple<double, char*>, int);

//Unary operators
tuple<double, char*> num(char* c) {
	next(c);
	double r = 0;
	switch (c[0]) {
	case '(':
		c++;
		tie(r, c) = eval(num(c), 0);
		if (c[0] == ')')
			c++;
		break;

		//Math constants and functions
		//pi
	case 'P':
		if (c[1] == 'i') {
			c += 2;
			r = MATH_PI;
		}
		break;
		//E means e
	case 'E':
		c++;
		r = MATH_E;
		break;
		//exp
	case 'e':
		if (c[1] == 'x' && c[2] == 'p') {
			c += 3;
			tie(r, c) = num(c);
			r = std::exp(r);
		}
		break;
		//sin
	case 's':
		if (c[1] == 'i' && c[2] == 'n') {
			c += 3;
			tie(r, c) = num(c);
			r = std::sin(r);
		}
		break;
		//log
	case 'l':
		if (c[1] == 'o' && c[2] == 'g') {
			c += 3;
			tie(r, c) = num(c);
			r = std::log(r);
		}
		break;
		//cos
	case 'c':
		if (c[1] == 'o' && c[2] == 's') {
			c += 3;
			tie(r, c) = num(c);
			r = std::cos(r);
		}
		break;
		//tan
	case 't':
		if (c[1] == 'a' && c[2] == 'n') {
			c += 3;
			tie(r, c) = num(c);
			r = std::tan(r);
		}
		break;
		//atan
	case 'a':
		if (c[1] == 't' && c[2] == 'a' && c[3] == 'n') {
			c += 4;
			tie(r, c) = num(c);
			r = std::atan(r);
		}

		//acos
		else if (c[1] == 'c' && c[1] == 'o' && c[2] == 's') {
			c += 4;
			tie(r, c) = num(c);
			r = std::acos(r);
		}
		//asin
		else if (c[1] == 's' && c[2] == 'i' && c[3] == 'n') {
			c += 4;
			tie(r, c) = num(c);
			r = std::asin(r);
		}
		break;
		//$() reads from memory
	case '$':
		c++;
		tie(r, c) = num(c);
		r = memory[r];
		break;

		//[] means loop. Evaluate the first number in [], if it is zero, end loop.
		//If not, evaluate the expression inside []. if it is zero, end loop.
	case '[':
		c++;
		{
			double r0;
			char* c0 = c;
			do {
				tie(r0, c) = num(c0);
				if (r0 == 0)
					break;
				tie(r0, c) = eval(make_tuple(r0, c), 0);
			} while (r0);
			//find the matching ]
			int count = 0;
			while (true) {
				if (c[0] == '[')
					count++;
				else if (c[0] == ']')
					count--;
				if (count < 0)
					break;
				c++;
			}
			c++;
			r = 0;
		}
		break;
		//negative sign
	case '-':
		c++;
		tie(r, c) = num(c);
		r *= -1;
		break;
	default:
		while (c[0] >= '0' && c[0] <= '9') {
			r = r * 10 + (c[0] - '0');
			c++;
		}
		double dec = 1;
		if (c[0] == '.') {
			c++;
			while (c[0] >= '0' && c[0] <= '9') {
				dec /= 10;
				r += dec * (c[0] - '0');
				c++;
			}
		}
	}
	next(c);
	return make_tuple(r, c);
}

//precedence of binary operators
inline int oplev(char c) {
	switch (c) {
	case '?':
		return 0;
	case ';':
		return 0;
	case ':':
		return 1;  //assignment operator: value:address
	case '>':
		return 2;
	case '<':
		return 2;
	case '+':
		return 3;
	case '-':
		return 3;
	case '*':
		return 4;
	case '/':
		return 4;
	case '%':
		return 4;
	case '^':
		return 5;
	default:
		return -1;
	}
}

//Parsing binary operators with recursive descent
tuple<double, char*> eval(tuple<double, char*> cur, int lev) {
	char* c;
	double r;
	tie(r, c) = cur;
	next(c);
	while (lev <= oplev(c[0])) {
		char op = c[0];
		c++;
		double rhs;
		tie(rhs, c) = num(c);
		while (oplev(c[0]) > oplev(op))
			tie(rhs, c) = eval(make_tuple(rhs, c), oplev(c[0]));
		switch (op) {
		case '+':
			r += rhs;
			break;
		case '-':
			r -= rhs;
			break;
		case '*':
			r *= rhs;
			break;
		case '/':
			r /= rhs;
			break;
		case '^':
			r = std::pow(r, rhs);
			break;
		case '%':
			r = ((long) r) % ((long) rhs);
			break;
		case '>':
			r = (r > rhs);
			break;
		case '<':
			r = (r < rhs);
			break;
		case ':':
			memory[rhs] = r;
			break;
		case ';':
			r = rhs;
			break;
		}
	}
	next(c);
	return make_tuple(r, c);
}

//check [] mismatch to prevent overflow
bool mismatch(char* ch) {
	int count = 0;
	while (ch[0] != 0) {
		if (ch[0] == '[')
			count++;
		else if (ch[0] == ']')
			count--;
		ch++;
	}
	return count > 0;
}

//main calculation function
double cal::calc_exp::calc() {
	//evaluate only when needed
	if (done)
		return result;
	done = true;
	if (mismatch((char*) expr.c_str()))
		return nan("");
	double result = std::get < 0 > (eval(num((char*) expr.c_str()), 0));
	return result;
}

