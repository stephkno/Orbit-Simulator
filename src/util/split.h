#pragma once
#include <vector>
#include <string>

using namespace std;
class Split{
public:
  static vector<string> split(const string s, char c){
    vector<string> out = std::vector<string>();
     string::size_type i = 0;
     string::size_type j = s.find(c);

     while (j != string::npos) {
        out.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j == string::npos)
           out.push_back(s.substr(i, s.length()));
     }
     return out;
  }
};
