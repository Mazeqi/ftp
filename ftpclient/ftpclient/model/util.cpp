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
vector<string> Util::dirFile(string path)
{
    vector<string> dirVec;

    long hFile = 0;
    struct _finddata_t fileInfo;
    string pathName, exdName;

    // \\* ����Ҫ�������е�����,��ĳ�\\*.jpg��ʾ����jpg�����ļ�
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
   

        //�ж��ļ����������ļ��л����ļ�
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

