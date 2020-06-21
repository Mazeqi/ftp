#pragma once
#ifndef  UTIL_HEAD
#define  UTIL_HEAD

#include<vector>
#include<string>
using namespace std;

class Util {

public:

	Util();

	vector<string> splitString(const string &str, const string &pattern);

};
#endif // ! 
