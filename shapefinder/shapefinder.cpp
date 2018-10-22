// shapefinder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "shapefinder.h"

using namespace std;

map<string, string> args;

int main(int argc, char* argv[]) {
	load_arguements(argc, argv);

	detect_shape();

	cin.get();
	return 0;
}

void load_arguements(int argc, char* argv[]) {
	int cnt = (argc - 1) / 2;

	cout << "num_arguments: " << cnt << endl;
	
	for (int i = 0; i < cnt; i++)
	{
		args[argv[(2 * i) + 1]] = argv[(2 * i) + 2];
	}

	for (auto it = args.cbegin(); it != args.cend(); it++)
	{
		cout << "args[\"" << it->first << "\"] = \"" << it->second << "\"\n";
	}
}

void detect_shape() {


	cout << "detected a shape!" << endl;
}

