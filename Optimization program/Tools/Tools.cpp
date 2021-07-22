//
// Created by serge on 22.07.2021.
//
#include <sstream>

#include "Tools.h"

using namespace std;

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

