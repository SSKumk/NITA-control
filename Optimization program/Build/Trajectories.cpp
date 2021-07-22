//
// Created by serge on 22.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"

#include "gurobi_c++.h"

using namespace std;

BestTrajectory::BestTrajectory(string _P0, double _t0, double _v0, double _tf) :
  t0{_t0}, v0{_v0}, tf{_tf}
{
  traj.clear();

  curTraj.clear();
  dfs(flow.cpName2cpID[_P0]);
}

void BestTrajectory::dfs(int curPointID) {

}

