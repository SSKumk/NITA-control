//
// Created by serge on 22.07.2021.
//

#ifndef TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H
#define TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H

#include <vector>
#include <string>
#include <limits>

#include "Optimization program/Measure units/Measure units.h"

#include "gurobi_c++.h"

struct TrajectoryPoint {
  int cpID;
  bool HA;
  bool SE;

  double tIn{std::numeric_limits<double>::min()};
  double tOut{std::numeric_limits<double>::min()};
  double S{std::numeric_limits<double>::min()};
  double v{std::numeric_limits<double>::min()};
  int chi{std::numeric_limits<int>::min()};

  TrajectoryPoint()  : cpID{-1}, HA{false}, SE{false} {}
  TrajectoryPoint(int _cpID, bool _HA, bool _SE) : cpID{_cpID}, HA{_HA}, SE{_SE} {}
};

struct AddVars {
  GRBVar tIn;
  GRBVar tOut;
  GRBVar v;
  GRBVar S;
  GRBVar chi;
};

class BestTrajectory {
public:
  std::vector<TrajectoryPoint> bestTraj{};
  double bestValue{1e38};
  double bestCtr{1e38};

  Time t0;
  Velocity v0;
  Time tf;

  BestTrajectory(std::string _P0, const Time &_t0, const Velocity &_v0, const Time &_tf);

  void PrintCurrentTrajectory();

  void PrintBestTrajectory();

  double getCtr() { return Ctr; }

private:
  std::vector<TrajectoryPoint> traj{};

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

  // Internal procedure for trajectory output
  static void printTraj(double _Ctr, const std::vector<TrajectoryPoint> _tr);

  // Internal procedure for output of a trajectory with optimization information
  void printOptTraj(double res, const std::vector<TrajectoryPoint> &traj);

  // Variables for the estimation procedures
  std::vector<AddVars> vars;
  GRBModel *model;
  std::string ind;

  void ImposeVelocityContraints(int predInd, int curInd);
  void ProcessDirectSegment(int predInd, int curInd, double distMin, double distMax);
  void ProcessHoldingArea(int predInd, int curInd, double distMin, double distMax);
};


#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_TRAJECTORIES_H
