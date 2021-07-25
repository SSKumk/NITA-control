//
// Created by Антон on 07.07.2021.
//
#include <iostream>
#include <iomanip>

#include "Flow.h"

using namespace std;

Flow &Flow::get_instance() {
  // The only instance of the class is created at the first call get_instance()
  // and will be destroyed only when the program exits
  static Flow instance;
  return instance;
}

Flow flow = Flow::get_instance();

void print_schemes_flow() {
  cout
    << left << setw(8) << "ID"
    << left << setw(8) << "Type"
    << left << setw(8) << "Start"
    << left << setw(8) << "End"
    << endl;
  for (const auto &scheme : flow.schemes) {
    cout
      << left << setw(8) << scheme.ID
      << left << setw(8) << scheme.type
      << left << setw(8) << flow.checkPoints.at(scheme.startP).name;
    for (int ID : scheme.endPs) {
      cout << flow.checkPoints.at(ID).name << ' ';
    }
    cout << endl;
  }
}

