//
// Created by Антон on 07.07.2021.
//
#include <iostream>
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
  cout << "ID \t" << "Type \t" << "Start\t" << "End" << endl << endl;
  for (const auto &scheme : flow.schemes) {
    cout << scheme.ID
         << "\t"
         << scheme.type
         << "\t"
         << flow.checkPoints.at(scheme.startP).name
         << "\t";
    for (int ID : scheme.endPs) {
      cout << flow.checkPoints.at(ID).name << ' ';
    }
    cout << endl;
  }
}

