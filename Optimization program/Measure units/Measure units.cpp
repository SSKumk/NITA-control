//
// Created by Антон on 07.07.2021.
//
#include "Measure units.h"
#include "cmath"

double Velocity::km_h() const
{
    return meters_per_second*m_s2km_h;
}
double Velocity::m_s() const
{
  return meters_per_second;
}

double Time::s() const
{
    return seconds;
}
