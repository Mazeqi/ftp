#pragma once
#include<vector>
#include<string>
using namespace std;

class Util {

public:

	Util();

	vector<string> splitString(const string& str, const string& pattern);

	vector<string> dirFile(string path);

};