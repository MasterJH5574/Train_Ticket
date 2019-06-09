#include <iostream>
#include "tools.hpp"
#include <iomanip>
#include <cmath>

#ifndef _PRICE
#define _PRICE

class Price {
	friend istream &operator >> (istream &in, Price &obj);
	friend ostream &operator << (ostream &out, Price &obj);
private:
	// before dot, after dot
	double a;

public:
	Price() { a = 0; }
	Price &operator= (const Price &other) {
		if (this == &other) return *this;
		a = other.a;
		return *this;
	}
	Price& operator= (const double &other) {
		a = other;
		return *this;
	}
	Price &operator += (const Price &other) {
		a += other.a;
		return *this;
	}
	
	~Price() {}
};

istream &operator >> (istream &in, Price &obj) {
	in >> obj.a;
	return in;
}

ostream &operator << (ostream &out, Price &obj) {
	if (abs(obj.a) < 1e-3) {
		out << "0.0";
		return out;
	}
	out << fixed << setprecision(2) << obj.a;
	return out;
}


#endif