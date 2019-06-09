#include <cstring>
#include <cstdio>
#include <iostream>
using namespace std;

#ifndef _DATE
#define _DATE

int days[13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

bool leapYear(int y) {
	return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

class Date {
private:
	int y, m, d;
public:
	Date(int _y = 0, int _m = 0, int _d = 0) : y(_y), m(_m), d(_d) {}
	Date operator++ () {
		if (d < days[m]) ++d;
		else {
			d = 1;
			if (m < 12) ++m;
			else m = 1, ++y;
		}
		return *this;
	}
	int operator - (const Date & other) const {
		return d > other.d ? d - other.d : other.d - d;
	}
	bool operator == (const Date & other) const {
		return (y == other.y && m == other.m && d == other.d);
	}
	bool operator != (const Date & other) {
		return (y != other.y || m != other.m || d != other.d);
	}
	bool operator < (const Date & other) const {
		if (y < other.y) return true;
		if (y > other.y) return false;
		if (m < other.m) return true;
		if (m > other.m) return false;
		if (d < other.d) return true;
		if (d > other.d) return false;
		return false;
	}
	bool operator > (const Date & other) const {
		if (y > other.y) return true;
		if (y < other.y) return false;
		if (m > other.m) return true;
		if (m < other.m) return false;
		if (d > other.d) return true;
		if (d < other.d) return false;
		return false;
	}
	bool operator <= (const Date & other) {
		return this->operator>(other);
	}
	bool operator >= (const Date & other) {
		return this->operator<(other);
	}
	friend istream& operator>> (istream & in, Date & obj) {
		char tmp;
		in >> obj.y >> tmp >> obj.m >> tmp >> obj.d;
		return in;
	}
	friend ostream& operator<< (ostream & os, const Date & obj) {
		os << obj.y << '-';
		if (obj.m < 10) os << '0';
		os << obj.m << '-';
		if (obj.d < 10) os << '0';
		os << obj.d;
		return os;
	}
};

#endif