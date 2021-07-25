//
// Created by Антон on 13.07.2021.
//
#include "Fields of Flow.h"

using namespace std;

Coordinate distance(const CheckPoint &a, const CheckPoint &b)
{
    return Coordinate(
            sqrt(pow(b.x.m() - a.x.m(), 2) +
                 pow(b.y.m() - a.y.m(), 2) +
                 pow(b.z.m() - a.z.m(), 2)));
}

ostream& operator<<(std::ostream& os, const SchemeType& t) {
  switch (t) {
    case LINEAR:
      os << "L";
      break;
    case HOLDING_AREA:
      os << "HA";
      break;
    case STRAIGHTENING:
      os << "S";
      break;
    case FAN:
      os << "F";
      break;
    case TROMBONE:
      os << "T";
      break;
    case NONE:
      os << "NONE";
      break;
  }
  return os;
}
