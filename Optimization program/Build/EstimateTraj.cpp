//
// Created by serge on 23.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"
#include "Optimization program/Tools/Tools.h"
#include "Optimization program/Fields of Zone/Fields of Flow.h"

#include "gurobi_c++.h"

#include <iostream>

using namespace std;

struct AddVars {
  GRBVar tIn;
  GRBVar tOut;
  GRBVar v;
  GRBVar S;
  GRBVar chi;
};

GRBEnv *env = new GRBEnv();

void BestTrajectory::estimateTrajectory() {
  PrintCurrentTrajectory();
  cout << endl;

  vector<AddVars> vars(traj.size());
  GRBModel model(*env);

  string ind;
  int i = 0;
  while (i < traj.size()) {
    i2s(i, 2, ind);
    ind = "_" + ind;
    CheckPoint &curPoint = flow.checkPoints[traj[i].cpID];

    // For all point except to the first one and to point of straightening
    // impose the velocity constraints
    if (i > 0 && !traj[i].SE) {
      vars[i].v = model.addVar(curPoint.V_min.m_s(), curPoint.V_max.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
      vars[i].chi = model.addVar(0, 1, 1, GRB_BINARY, "chi" + ind);

      model.addConstr(vars[i - 1].v - GRB_INFINITY * vars[i].chi - vars[i].v <= 0, "lower velocity change" + ind);
      model.addConstr(vars[i - 1].v + GRB_INFINITY * vars[i].chi - vars[i].v >= 0, "upper velocity change" + ind);
    }

    // Just a segment of motion
    if (!traj[i].HA && !traj[i].SE) {
      // The initial point
      if (i == 0) {
        vars[i].tOut = model.addVar(t0.s(), t0.s(), 0, GRB_CONTINUOUS, "t" + ind);
        vars[i].v = model.addVar(v0.m_s(), v0.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
      } else {
        vars[i].tOut = model.addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + ind);

        double dist = distance(flow.checkPoints[traj[i - 1].cpID], curPoint).m();
        vars[i].S = model.addVar(dist, dist, 0, GRB_CONTINUOUS, "S" + ind);

        model.addQConstr(2 * vars[i].S == (vars[i].tOut - vars[i - 1].tOut) *
                                  (vars[i].v + vars[i - 1].v), "S" + ind + " constr");
      }

      i++;
    }
      // Point with holding area
    else if (traj[i].HA && !traj[i].SE) {
      if (i == 0) {
        vars[i].tIn = model.addVar(t0.s(), t0.s(), 0, GRB_CONTINUOUS, "t'" + ind);
        vars[i].v = model.addVar(v0.m_s(), v0.m_s(), 0, GRB_CONTINUOUS, "v" + ind);
      } else {
        vars[i].tIn = model.addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t'" + ind);

        double dist = distance(flow.checkPoints[traj[i - 1].cpID], curPoint).m();
        vars[i].S = model.addVar(dist, dist, 0, GRB_CONTINUOUS, "S" + ind);

        model.addQConstr(2 * vars[i].S == (vars[i].tIn - vars[i - 1].tOut) *
          (vars[i].v + vars[i - 1].v), "S" + ind + "_constr");
      }

      vars[i].tOut = model.addVar(-GRB_INFINITY, +GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + ind);

      Scheme &ha = flow.schemes[flow.HAs[curPoint.ID]];
      model.addConstr(vars[i].tOut + ha.t_min.s() <= vars[i].tOut, "t" + ind + " lower HA constraint");
      model.addConstr(vars[i].tOut + ha.t_max.s() >= vars[i].tOut, "t" + ind + " upper HA constraint");

      i++;
    }

    // Point of straightening
    else { // if (!traj[i].HA && traj[i].SE)

    }
  }
}


}
