#include "../Head/Util.h"
#include<cstdio>
#include<string>
#include<iostream>
#include<vector>


Util::Util() {

}

vector<string> Util:: splitString(const string& str, const string& pattern) {

    vector<string> res;

    if (str == "") {
        return res;
    }
       
    //���ַ���ĩβҲ����ָ����������ȡ���һ��

    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);

        //ȥ���ѷָ���ַ���,��ʣ�µ��ַ����н��зָ�
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }

    return res;

}
