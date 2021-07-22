//
// Created by Антон on 13.07.2021.
//
#include "Fields of Flow.h"


Coordinate distance(const CheckPoint &a, const CheckPoint &b)
{
    return Coordinate(
            sqrt(pow(b.x.m() - a.x.m(), 2) +
                 pow(b.y.m() - a.y.m(), 2) +
                 pow(b.z.m() - a.z.m(), 2)));
};
