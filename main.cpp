#include "Optimization program/Build/Build.h"
#include "Optimization program/Build/Trajectories.h"
#include "Optimization program/Flow.h"

using namespace std;

int main() {
/*
  string path_to_CPointsFile = "../Optimization program/Source information/Points/Flow-Test-Points.txt";
  string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Flow-Test-Schemes.txt";
*/

  string path_to_CPointsFile = "../Optimization program/Source information/Points/Test02.txt";
  string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Test02.txt";


  build_Flow(path_to_CPointsFile, path_to_SchemesFile, 0);
  print_schemes_flow();

  BestTrajectory bt("P0", Time(0.0, "s"), Velocity(400.0, "km_h"), Time(13000.0, "s"));

  return 0;
}
