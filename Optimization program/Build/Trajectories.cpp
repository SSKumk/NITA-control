//
// Created by serge on 22.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"

using namespace std;

BestTrajectory::BestTrajectory(string _P0, double _t0, double _v0, double _tf) :
  t0{_t0}, v0{_v0}, tf{_tf}
{
  traj.clear();

  curTraj.clear();
  dfs(flow.cpName2cpID[_P0]);
}

void BestTrajectory::makePoint() {
  Ctr_stack.push_back(Ctr);
  TrLen_stack.push_back(traj.size());
}

void BestTrajectory::revertPoint() {
  Ctr = Ctr_stack.back();
  Ctr_stack.pop_back();

  traj.resize(TrLen_stack.back());
  TrLen_stack.pop_back();
}


void BestTrajectory::dfs(int curPointID) {
  if (curPointID == flow.final_point) {
    // We have reached the final point - estimate it and correct the optimum if necessary
    estimateTrajectory();
  } else {
    // Have to go further

    // The loop over schemes starting at this point
    for (int schemeID : flow.starts[curPointID]) {
      makePoint();

      const Scheme &scheme = flow.schemes[schemeID];
      bool useHA = false;
      map<int, int>::iterator HA = flow.HAs.end();

      // Loop over HOLDING_AREA at the start point
      do {
        // Process the scheme
        switch (scheme.type) {
          // Linear segment - just add points to the trajectory
          case LINEAR:
            for (int i = 0; i < scheme.lin.size(); i++) {
              traj.push_back(TrajectoryPoint(scheme.lin[i], i == 0 && useHA, false));
            }
            dfs(scheme.lin.back());
            break;

          // Case of a straightening segment
          case STRAIGHTENING:
          case FAN:
          case TROMBONE:
            // Loop over point whereto to straighten
            for (int toInd : scheme.stTo) {
              // Loop over points wherefrom to straighten
              for (int fromNum = scheme.stFrom.size() - 1; fromNum >= 0; fromNum--) {
                makePoint();

                for (int i = 0; i <= fromNum; i++) {
                  traj.push_back(TrajectoryPoint(scheme.stFrom[i], i == 0 && useHA, )
                }

                revertPoint();
              }
            }
            break;
        }

        useHA = !useHA;
        if (useHA) {
          HA = flow.HAs.find(scheme.startP);
        }
      // End of the loop over HOLDING_AREA at the start point
      } while (useHA && HA != flow.HAs.end());

      revertPoint();

    // End of the loop over schemes starting at the point
    }
  }
}

