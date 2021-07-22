//
// Created by Антон on 09.07.2021.
//

#include "Build.h"
#include "Optimization program/Flow.h"
#include "Optimization program/Read/Read.h"

using namespace std;

void build_Flow(string_view path_PointsFile, string_view path_SchemesFile, int start_point) {
  read_checkPoints(path_PointsFile);
  read_schemes(path_SchemesFile);
}

