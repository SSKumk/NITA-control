//
// Created by serge on 22.07.2021.
//

#include "Trajectories.h"
#include "Optimization program/Flow.h"

#include <iostream>

using namespace std;

BestTrajectory::BestTrajectory(string _P0, const Time &_t0, const Velocity &_v0, const Time &_tf) :
        t0{_t0}, v0{_v0}, tf{_tf} {
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
    makePoint();
    traj.push_back(TrajectoryPoint(curPointID, false, false));
    estimateTrajectory();
    revertPoint();
  } else {
    // Have to go further

    // The loop over schemes starting at this point
    for (int schemeID : flow.starts[curPointID]) {
      const Scheme &scheme = flow.schemes[schemeID];
      bool useHA = false;
      map<int, int>::iterator HA = flow.HAs.end();

      // Taking into account the additional interaction
      // for entering the scheme
      if (scheme.needInteract) {
        Ctr += 1;
      }

      // Loop over HOLDING_AREA at the start point
      do {
        // Process the scheme
        switch (scheme.type) {
          // Linear segment - just add points to the trajectory
          case LINEAR:
            makePoint();

            if (useHA) {
              Ctr += 1;
            }

            for (int i = 0; i < scheme.lin.size()-1; i++) {
              traj.push_back(TrajectoryPoint(scheme.lin[i], i == 0 && useHA, false));
            }
            dfs(scheme.lin.back());
            revertPoint();
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

              if (useHA) {
                Ctr += 1;
              }

              // Учитываем взаимодействия
              // Для веера взаимодействия нет: заход на веер без взаимодействия,
              // а сход на следующую схему с конца дуги ожидания будет учтен при обработке той схемы
              if (scheme.type == TROMBONE) {
                Ctr += 1;
              }

              for (int i = 0; i < scheme.stFrom.size()-1; i++) {
                traj.push_back(TrajectoryPoint(scheme.stFrom[i], i == 0 && useHA, false));
              }

              dfs(scheme.stFrom.back());
              revertPoint();
            }

            // Для тромбона обрабатываем траекторию, которая проходит напрямую мимо дуги ожидания
            if (scheme.type == TROMBONE) {
              makePoint();

              if (useHA) {
                Ctr += 1;
              }

              traj.push_back(TrajectoryPoint(scheme.stFrom.front(), useHA, false));
              traj.push_back(TrajectoryPoint(scheme.stFrom[scheme.stFrom.size() - 2], false, false));

              dfs(scheme.stFrom.back());
              revertPoint();
            }

            // Для веера обрабатываем траекторию, которая проходит напрямую мимо дуги ожидания
            if (scheme.type == FAN) {
              makePoint();

              if (useHA) {
                Ctr += 1;
              }

              // Учитываем взаимодествия
              Ctr += 1;

              traj.push_back(TrajectoryPoint(scheme.stFrom.front(), useHA, false));

              dfs(scheme.stTo.front());
              revertPoint();
            }

            // Для участка спрямления и тромбона генерируем траекторию,
            // проходящую по всей схеме без спрямления
            if (scheme.type == STRAIGHTENING || scheme.type == TROMBONE) {
              makePoint();

              // Считаем взаимодействия
              if (useHA) {
                Ctr += 1;
              }
              if (scheme.type == TROMBONE) {
                Ctr += 1;
              }

              for (int i = 0; i < scheme.stFrom.size() - 1; i++) {
                traj.push_back(TrajectoryPoint(scheme.stFrom[i], i == 0 && useHA, false));
              }

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

                if (useHA) {
                  Ctr += 1;
                }

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

      // Reverting the additional interaction
      // for entering the scheme
      if (scheme.needInteract) {
        Ctr -= 1;
      }

      // End of the loop over schemes starting at the point
    }
  }
}

void BestTrajectory::printTraj(double _Ctr, const std::vector<TrajectoryPoint> _tr) {
  cout << "Ctr = " << _Ctr << ";";
  for (TrajectoryPoint p : _tr) {
    cout << " " << flow.checkPoints[p.cpID].name;
    if (p.HA && p.SE) {
      cout << "(HA,SE)";
    } else if (p.HA) {
      cout << "(HA)";
    } else if (p.SE) {
      cout << "(SE)";
    }
  }
}


void BestTrajectory::PrintCurrentTrajectory() {
  printTraj(Ctr, traj);
}


void BestTrajectory::PrintBestTrajectory() {
  printOptTraj(bestCtr, bestTraj);
}
