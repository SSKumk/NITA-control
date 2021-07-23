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

  TrajectoryPoint(int _cpID, bool _HA, bool _SE) : cpID{_cpID}, HA{_HA}, SE{_SE} {}
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
  void estimateTrajectory();

  // Value of the penalty for trajectory change
  double Ctr{0};

  // Stack for holding Ctr values for DFS
  std::vector<double> Ctr_stack{};

  // Stack for the lengths of trajectory
  std::vector<int> TrLen_stack{};

  // Procedures for making a restore point and restore to the last point
  void makePoint();
  void revertPoint();
};



#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H
