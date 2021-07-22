#include <Optimization program/Build/Build.h>
#include "Optimization program/Flow.h"

using namespace std;

int main()
{
    string path_to_CPointsFile = "../Optimization program/Source information/Points/Flow-Test-Points.txt";
    string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Flow-Test-Schemes.txt";
    
    build_Flow(path_to_CPointsFile, path_to_SchemesFile, 0);
    print_schemes_flow();

    return 0;
}
