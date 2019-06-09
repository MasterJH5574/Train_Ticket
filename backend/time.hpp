#include <iostream>
#include "tools.hpp"

#ifndef _TIME
#define _TIME
using namespace std;

class time {
	friend istream& operator >> (istream& is, time& obj);
	friend ostream& operator << (ostream& os, time& obj);
private:
	char data[5];
public:
	time() {}
	~time() {}
};

istream& operator >> (istream& is, time& obj) {
	is >> obj.data;
}

ostream& operator << (ostream& os, time& obj) {
	os << obj.data;
}

#endif
