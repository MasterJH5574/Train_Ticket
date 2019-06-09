#pragma 4096
#include<iostream>
//#include "commands.hpp"
#include <functional>
using namespace std;
//int main() {
//	ifstream in("in.txt");
//	ofstream out("out.txt");
//	while (1) read_command(in, out);
//	return 0;
//}


auto g_Lambda = [](int i, int j)
{
	return i + j;
}; //匿名函数 此处有分号

int main()
{
	std::function<int(int, int)> f = g_Lambda;
	cout << f(2, 3);

	getchar();
	return 0;
}