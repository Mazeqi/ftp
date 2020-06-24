#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef  UTIL_HEAD
#define  UTIL_HEAD

#include<vector>
#include<string>
using namespace std;

class Util {

public:

	Util();

	vector<string> splitString(const string &str, const string &pattern);

	vector<string> dirFile(string path);

};
#endif // ! 
