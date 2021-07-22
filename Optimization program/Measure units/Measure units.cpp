//
// Created by Антон on 07.07.2021.
//
#include "Measure units.h"
#include "cmath"

//Distance operator+(const Distance &l_d, const Distance &r_d)
//    {
//        return {l_d.meter + r_d.meter};
//    }
//
//Distance operator-(const Distance &l_d, const Distance &r_d)
//    {
//        return {l_d.meter - r_d.meter};
//    }
//
//Distance operator*(const double &a, const Distance &r_d)
//    {
//        return {a*r_d.meter};
//    }
//
//Distance operator*(const Distance &l_d, const double &a)
//    {
//        return a*l_d;
//    }
//
//Distance operator/(const Distance &l_d, const Distance &r_d)
//    {
//        assert(r_d.meter != 0 && "Division by zero!");
//        return {l_d.meter/r_d.meter};
//    }
//
//Distance pow(Distance x, int a)
//    {
//        return {std::pow(x.meter, a)};
//    }
//
//Distance sqrt(Distance x)
//    {
//        return {std::sqrt(x.meter)};
//    }
//
//Velocity operator+(const Velocity &l_v, const Velocity &r_v)
//    {
//        return {l_v.meters_per_second + r_v.meters_per_second};
//    }
//
//Velocity operator-(const Velocity &l_v, const Velocity &r_v)
//    {
//        return {l_v.meters_per_second - r_v.meters_per_second};
//    }
//
//Velocity operator*(const double &a, const Velocity &r_v)
//    {
//        return {a*r_v.meters_per_second};
//    }
//
//Velocity operator*(const Velocity &l_v, const double &a)
//    {
//        return a*l_v;
//    }

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
