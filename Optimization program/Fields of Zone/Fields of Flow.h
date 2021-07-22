//
// Created by Антон on 07.07.2021.
//

#ifndef TIMES_AND_PLANES_OPTIMIZATION_2021_07_FIELDS_OF_FLOW_H
#define TIMES_AND_PLANES_OPTIMIZATION_2021_07_FIELDS_OF_FLOW_H

#include <string>
#include <vector>
#include <set>
#include "Optimization program/Measure units/Measure units.h"
#include "cmath"

struct CheckPoint
{
    int ID{-1};
    std::string name{};
    Coordinate x{0};
    Coordinate y{0};
    Coordinate z{0};
    Velocity V_min{0};
    Velocity V_max{0};
    bool landing_flag = false;
    
    std::set<int> schemesID{};
};

Coordinate distance(const CheckPoint &a, const CheckPoint &b);

struct Scheme
{
    int ID{-1};
    int startP{-1};
    std::vector<int> endPs{};
    
    std::string type{""}; //Тип схемы: HA - Зона Ожидания, L - линейная, F - веер, T - полутромбон, S - спрямление
    
    std::vector<int> lin{};//Линейная схема
    
    std::vector<int> stFrom{}; // st - straightening
    std::vector<int> stTo{}; // Схема со спрямлением
    
    Time t_min{0};// Для ЗО
    Time t_max{0};
};

#endif //TIMES_AND_PLANES_OPTIMIZATION_2021_07_FIELDS_OF_FLOW_H
