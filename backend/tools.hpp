#include <iostream>
using namespace std;

#ifndef _TOOLS
#define _TOOLS

inline int read_int(istream &in) {
	char ch;
	do {
		in >> noskipws >> ch;
		if (ch == ':' || ch == '-' || ch == '\n' || ch == ' ') return 0;
	} while (ch < '0' || ch > '9');
	int ans = ch - '0';

	do {
		in >> noskipws >> ch;
		if (ch >= '0' && ch <= '9')
			ans = ans * 10 + (int)(ch - '0');
		else return ans;
	} while (1);
}

#endif