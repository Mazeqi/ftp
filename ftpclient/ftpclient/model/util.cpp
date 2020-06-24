#define _CRT_SECURE_NO_WARNINGS
#include"../Head/util.h"
#include<cstdio>
#include<string>
#include<iostream>
#include<vector>
#include<io.h>

Util::Util() {

}

vector<string> Util::splitString(const string& str, const string& pattern) {

    vector<string> res;

    if (str == "") {
        return res;
    }

    //在字符串末尾也加入分隔符，方便截取最后一段

    string strs = str + pattern;
    size_t pos = strs.find(pattern);

    while (pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);

        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(pattern);
    }

    return res;

}
vector<string> Util::dirFile(string path)
{
    vector<string> dirVec;

    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;

    // \\* 代表要遍历所有的类型,如改成\\*.jpg表示遍历jpg类型文件
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
        return dirVec;
    }
    do
    {
        char buf[1024];
        strcpy(buf, fileInfo.name);


        //cout << buf << endl;
        string fileName = string(buf);

        //cout << fileName;
   

        //判断文件的属性是文件夹还是文件
        if (fileInfo.attrib & _A_SUBDIR) {
           // fileName += "  [folder]";
            continue;
        }
        else {
            fileName += "  [file]";
        }
        dirVec.push_back(fileName);
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return dirVec;
}

