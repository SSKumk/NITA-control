#include "Optimization program/Build/Build.h"
#include "Optimization program/Build/Trajectories.h"
#include "Optimization program/Flow.h"

using namespace std;

int main() {
//  string path_to_CPointsFile = "../Optimization program/Source information/Points/Flow-Test-Points.txt";
//  string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Flow-Test-Schemes.txt";

  string path_to_CPointsFile = "../Optimization program/Source information/Points/Koltsovo.txt";
  string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Koltsovo.txt";

//  string path_to_CPointsFile = "../Optimization program/Source information/Points/Test07.txt";
//  string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Test07.txt";


  build_Flow(path_to_CPointsFile, path_to_SchemesFile, 0);
  print_schemes_flow();

  BestTrajectory bt("P0", Time(0.0, "s"), Velocity(400.0, "km_h"), Time(3000.0, "s"));

  cout << "-----------------------------------" << endl;
  cout << "The best trajectory:" << endl;
  bt.PrintBestTrajectory();

  return 0;
}
