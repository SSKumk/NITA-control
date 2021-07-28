//
// Created by serge on 22.07.2021.
//
#include <sstream>
#include <iomanip>

#include "Tools.h"

using namespace std;

void i2s(int n, int l, string &s)
{
  stringstream ss;
  ss << setw(l) << setfill('0') << n;
  s = ss.str();
}

vector<string> splitStringBySeparator(const string &_str, char _delim) {
  vector<string> result;
  stringstream ss(_str);
  string tmp;
  while (std::getline(ss, tmp, _delim)) {
    if (!tmp.empty()) {
      result.push_back(tmp);
    }
  }

  return result;
}

