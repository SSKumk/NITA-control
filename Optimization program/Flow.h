//
// Created by Антон on 07.07.2021.
//

#ifndef TIMES_AND_PLANES_OPTIMIZATION_2021_07_FLOW_H
#define TIMES_AND_PLANES_OPTIMIZATION_2021_07_FLOW_H

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <Optimization program/Fields of Zone/Fields of Flow.h>

class CheckPoint;

struct Flow
{
    std::vector<CheckPoint> checkPoints{};
    std::map<std::string, int> cpName2cpID{};
    
    std::vector<Scheme> schemes{};
    std::map<int, int> HAs{};
    std::map<int, std::vector<int>> starts{};
    
    int final_point{-1};
    
    static Flow &get_instance();
private:
    Flow() = default;
};

extern Flow flow;

void print_schemes_flow();

//void calc_best_trajectory(std::vector<std::map<int, int>> &trjs, const int current_vertex, const int vertex_to_reach);
//bool isBetter(const std::map<int, int> &trj, int start_vertex, int end_vertex, double t0, double v0, double tEnd);

#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_FLOW_H
