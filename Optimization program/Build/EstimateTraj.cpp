//
// Created by serge on 23.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"

#include "gurobi_c++.h"

#include <iostream>

using namespace std;

void BestTrajectory::estimateTrajectory() {
  PrintCurrentTrajectory();
  cout << endl;
}
