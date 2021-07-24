//
// Created by serge on 22.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"

using namespace std;

BestTrajectory::BestTrajectory(string _P0, double _t0, double _v0, double _tf) :
        t0{_t0}, v0{_v0}, tf{_tf} {
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
            // Запоминаем, является ли конечная точка сегмента спрямления точкой, на которую спрямляемся
            bool straightToFinalPoint = find(scheme.stTo.begin(), scheme.stTo.end(), scheme.stFrom.back()) != scheme.stTo.end();

            // Отдельно обрабатываем траекторию, идущую без спрямления на участке с внешним спрямлением
            if (!straightToFinalPoint) {
              makePoint();

              // Учитываем взаимодествия
              switch (scheme.type) {
                case FAN:
                  Ctr += 2;
                  break;
                case TROMBONE:
                  Ctr += 1;
                  break;
              }

              for (int i = 0; i < scheme.stFrom.size(); i++) {
                traj.push_back(TrajectoryPoint(scheme.stFrom[i], i == 0 && useHA, false));
              }

              dfs(scheme.stFrom.back());
              revertPoint();
            }

            // Для тромбона и веера обрабатываем траекторию, которая проходит напрямую мимо дуги ожидания
            if (scheme.type != STRAIGHTENING) {
              makePoint();

              // Учитываем взаимодествия
              if (scheme.type == FAN) {
                Ctr += 1;
              }

              traj.push_back(TrajectoryPoint(scheme.stFrom.front(), useHA, false));
              // Если финальная точка - это точка на которую спрямляемся,
              // то нужно занести в траекторию предпоследнюю точку
              if (straightToFinalPoint) {
                traj.push_back(TrajectoryPoint(scheme.stFrom[scheme.stFrom.size() - 2], false, false));
              }
              traj.push_back(TrajectoryPoint(scheme.stFrom.back(), false, false));

              dfs(scheme.stFrom.back());
              revertPoint();
            }

            // Перебираем траектории, проходящие со спрямлением
            // Loop over point whereto to straighten
            for (int toInd : scheme.stTo) {
              // Index of the last point, from which we can straighten
              int lastPointID;
              if (scheme.stFrom.back() == toInd) {
                lastPointID = scheme.stFrom.size() - 2;
              } else {
                lastPointID = scheme.stFrom.size() - 1;
              }

              // Loop over points wherefrom to straighten
              for (int fromNum = 1; fromNum <= lastPointID; fromNum++) {

                makePoint();

                // Считаем взаимодействия
                switch(scheme.type){
                  case STRAIGHTENING:
                    Ctr += 1;
                    break;
                  case FAN:
                    Ctr += 1;
                    break;
                  case TROMBONE:
                    Ctr += 2;
                    break;
                }

                // Формируем траекторию
                for (int i = 0; i <= fromNum; i++) {
                  traj.push_back(TrajectoryPoint(scheme.stFrom[i], i == 0 && useHA, i == fromNum));
                }
                traj.push_back(TrajectoryPoint(toInd, false, false));

                dfs(toInd);
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

      // End of the loop over schemes starting at the point
    }
  }
}

