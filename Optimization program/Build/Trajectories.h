//
// Created by serge on 22.07.2021.
//

#ifndef TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H
#define TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H

#include <vector>
#include <string>

struct TrajectoryPoint {
  int cpID{-1};
  bool HA{false};
  bool SE{false};
};

class BestTrajectory {
public:
  std::vector<TrajectoryPoint> traj;
  double t0;
  double v0;
  double tf;

  BestTrajectory(std::string _P0, double _t0, double _v0, double _tf);

private:
  std::vector<TrajectoryPoint> curTraj;
  void dfs(int curPointID);
};



#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H
