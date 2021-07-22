#include <Optimization program/Build/Build.h>
#include "iostream"
#include "Optimization program/Measure units/Measure units.h"
#include "Optimization program/Flow.h"
#include "Optimization program/Read/Read.h"
#include <regex>
using namespace std;

int main()
{
    
    string path_to_CPointsFile = "../Optimization program/Source information/Points/Test.txt";
    string path_to_SchemesFile = "../Optimization program/Source information/Schemes/Test.txt";
    
    build_Flow(path_to_CPointsFile, path_to_SchemesFile, 0);
    print_schemes_flow();
    
    
    
    
    
    
    
    flow;
    return 0;
}
