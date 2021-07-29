//
// Created by serge on 23.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"
#include "Optimization program/Tools/Tools.h"
#include "Optimization program/Fields of Zone/Fields of Flow.h"

#include <iostream>

using namespace std;

GRBEnv *env = new GRBEnv();

void BestTrajectory::ImposeVelocityContraints(int predInd, int curInd) {
  vars[curInd].v = model->addVar(flow.checkPoints[traj[curInd].cpID].V_min.m_s(),
                                 flow.checkPoints[traj[curInd].cpID].V_max.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
  vars[curInd].chi = model->addVar(0, 1, 1, GRB_BINARY, "chi" + ind);

  model->addConstr(vars[predInd].v - GRB_INFINITY * vars[curInd].chi - vars[curInd].v <= 0,
                   "lower velocity change" + ind);
  model->addConstr(vars[predInd].v + GRB_INFINITY * vars[curInd].chi - vars[curInd].v >= 0,
                   "upper velocity change" + ind);
}


void BestTrajectory::ProcessDirectSegment(int predInd, int curInd, double distMin, double distMax) {
  if (curInd == 0) {
    vars[curInd].tOut = model->addVar(t0.s(), t0.s(), 0, GRB_CONTINUOUS, "t" + ind);
    vars[curInd].v = model->addVar(v0.m_s(), v0.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
  }
  else {
    vars[curInd].tOut = model->addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + ind);

    vars[curInd].S = model->addVar(distMin, distMax, 0, GRB_CONTINUOUS, "S" + ind);

    model->addQConstr(2 * vars[curInd].S == (vars[curInd].tOut - vars[predInd].tOut) *
                                            (vars[curInd].v + vars[predInd].v), "S" + ind + " constr");
  }
}

void BestTrajectory::ProcessHoldingArea(int predInd, int curInd, double distMin, double distMax) {
  if (curInd == 0) {
    vars[curInd].tIn = model->addVar(t0.s(), t0.s(), 0, GRB_CONTINUOUS, "t'" + ind);
    vars[curInd].v = model->addVar(v0.m_s(), v0.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
  }
  else {
    vars[curInd].tIn = model->addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t'" + ind);

    vars[curInd].S = model->addVar(distMin, distMax, 0, GRB_CONTINUOUS, "S" + ind);

    model->addQConstr(2 * vars[curInd].S == (vars[curInd].tIn - vars[predInd].tOut) *
                                            (vars[curInd].v + vars[predInd].v), "S" + ind + "_constr");
  }

  vars[curInd].tOut = model->addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + ind);

  Scheme &ha = flow.schemes[flow.HAs[flow.checkPoints[traj[curInd].cpID].ID]];
  model->addConstr(vars[curInd].tIn + ha.t_min.s() <= vars[curInd].tOut, "t" + ind + " lower HA constraint");
  model->addConstr(vars[curInd].tIn + ha.t_max.s() >= vars[curInd].tOut, "t" + ind + " upper HA constraint");
}


void BestTrajectory::estimateTrajectory() {
  cout << "Trajectory to be estimated: ";
  PrintCurrentTrajectory();
  cout << endl;

  vars.clear();
  vars.resize(traj.size());

  try {
    model = new GRBModel(*env);

    int i = 0;
    while (i < traj.size()) {
      i2s(i, 2, ind);
      ind = "_" + ind;

      // A segment without straightening
      if (!traj[i].SE) {
        double dist = GRB_INFINITY;
        if (i > 0) {
          dist = distance(flow.checkPoints[traj[i - 1].cpID], flow.checkPoints[traj[i].cpID]).m();
          ImposeVelocityContraints(i - 1, i);
        }

        // With holding area
        if (traj[i].HA) {
          ProcessHoldingArea(i - 1, i, dist, dist);
        }
          // Without holding area
        else {
          ProcessDirectSegment(i - 1, i, dist, dist);
        }
        i++;
      }
        // A segment with straightening
      else { // if (traj[i].SE)
        i2s(i + 1, 2, ind);
        ind = "_" + ind;

        double
          distMin = distance(flow.checkPoints[traj[i - 1].cpID], flow.checkPoints[traj[i + 1].cpID]).m(),
          distMax = distance(flow.checkPoints[traj[i - 1].cpID], flow.checkPoints[traj[i].cpID]).m() +
                    distance(flow.checkPoints[traj[i].cpID], flow.checkPoints[traj[i + 1].cpID]).m();

        // Adding velocity constraints
        ImposeVelocityContraints(i - 1, i + 1);

        // With holding area
        if (traj[i].HA) {
          ProcessHoldingArea(i - 1, i + 1, distMin, distMax);
        }
          // Without holding area
        else {
          ProcessDirectSegment(i - 1, i + 1, distMin, distMax);
        }

        i += 2;
      }
    }

    // Constraint for the time at the final point
    model->addConstr(vars.back().tOut == tf.s());

    model->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    model->set(GRB_IntParam_NonConvex, 2);
    model->set(GRB_DoubleAttr_ObjCon, Ctr);
    model->optimize();
  } catch (GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Exception during optimization" << endl;
  }

  if (model != NULL) {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
      double res = model->get(GRB_DoubleAttr_ObjVal);

      printOptTraj(res, traj, vars);

      if (res < bestValue) {
        bestValue = res;
        bestCtr = Ctr;
        bestTraj = traj;
        bestVars = vars;
      }
    }
    else {
      cout << "*** No optimal solution has been found!" << endl;
    }

    delete model;
    model = NULL;
  }
}


void BestTrajectory::printOptTraj(double res, const vector<TrajectoryPoint> &traj, const vector<AddVars> &vars) {
  cout << "Objective value = " << res << endl;
  for (int i = 0; i < traj.size(); i++) {
    if (!traj[i].SE) {
      if (i == 0) {
        cout << flow.checkPoints[traj[i].cpID].name << ": " <<
             "v = " << vars[i].v.get(GRB_DoubleAttr_X) << ", " <<
             "t = " << vars[i].tOut.get(GRB_DoubleAttr_X) << ", " << endl;
      }
      else {
        cout << flow.checkPoints[traj[i].cpID].name << ": " <<
             "chi = " << vars[i].chi.get(GRB_DoubleAttr_X) << ", " <<
             "v = " << vars[i].v.get(GRB_DoubleAttr_X) << ", " <<
             "t = " << vars[i].tOut.get(GRB_DoubleAttr_X) << ", " <<
             "S = " << vars[i].S.get(GRB_DoubleAttr_X) << endl;
      }
    }
  }
}

