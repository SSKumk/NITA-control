//
// Created by Антон on 07.07.2021.
//

#include "Optimization program/Tools/Tools.h"
#include "Read.h"
#include "Optimization program/Flow.h"
#include <regex>
#include "string_view"
#include "iostream"
#include <fstream>
#include "Optimization program/Errors/Errors.h"
#include <string>
#include <Optimization program/Tools/Tools.h>

using namespace std;


void read_checkPoints(string_view path) {
//  flow.checkPoints.resize(count_number_of_line(path) - 1);
  flow.checkPoints.clear();

  ifstream file{};
  openFile(path, file);

  string line{};
  int lineNum{0};

  // Skipping initial empty lines and comments
  do {
    lineNum++;
    getline(file, line);
  } while (!file.eof() && (line.empty() || line[0] == '#'));

  if (file.eof()) {
    cerr << "The file " << path << " contains empty lines and/or comments only!" << endl;
    exit(FILE_NOT_ALLOWED_FORMAT);
  }

  vector<string> mus = splitStringBySeparator(line, ' ');

  if (mus.size() != 5) {
    cerr << "Wrong number of measure units in the " << lineNum << "th lineof the file " << path << endl;
    exit(FILE_NOT_ALLOWED_FORMAT);
  }

  string x_mu{mus[0]}, y_mu{mus[1]}, z_mu{mus[2]}, Vmin_mu{mus[3]}, Vmax_mu{mus[4]};

  /*
   * Пояснение к регулярному выражению:
   * Всего 7 групп захвата, согласно формату:
   *
   * NameP x y z v1 v2 land_flag
   *
   * 1) - имя точки [NameP], начинается с буквы
   * 2,3,4) - координаты x,y,z
   * 5,6) - скорости vmin, vmax
   * 7) - флаг посадочной полосы - LAND или пусто или 0
   */

  cmatch res{};
  regex regular(
          R"(([a-z,A-Z]\w*)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([0-9]*\.?[0-9]+)\s+([0-9]*\.?[0-9]+)\s*(LAND|0)?\s*)");

  int count_land_flag{0};
  while (getline(file, line)) {
    lineNum++;

    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    if (!regex_match(line.c_str(), res, regular)) {
      cerr << "Warning! Line '" << lineNum << "' in " << path << " doesn't match the input format of a point." << endl;
      exit(FILE_NOT_ALLOWED_FORMAT);
    }

    if (flow.cpName2cpID.find(res[1]) != flow.cpName2cpID.end()) {
      cerr << "'" << res[1] << "' point occurs multiply in " << path << endl;
      exit(DUPLICATE_NAME);
    }

    double x_value, y_value, z_value, v_min, v_max;
    x_value = stod(string(res[2]));
    y_value = stod(string(res[3]));
    z_value = stod(string(res[4]));
    v_min = stod(string(res[5]));
    v_max = stod(string(res[6]));

    CheckPoint checkPoint;
    checkPoint.name = res[1];
    checkPoint.ID = flow.checkPoints.size();
    checkPoint.x = {x_value, x_mu};
    checkPoint.y = {y_value, y_mu};
    checkPoint.z = {z_value, z_mu};
    checkPoint.V_min = {v_min, Vmin_mu};
    checkPoint.V_max = {v_max, Vmax_mu};

    if (res[7] == "LAND") {
      checkPoint.landing_flag = true;
      count_land_flag++;

      flow.final_point = checkPoint.ID;
    }
    flow.checkPoints.push_back(checkPoint);
    flow.cpName2cpID.insert({checkPoint.name, checkPoint.ID});
  }
  file.close();

  if (count_land_flag != 1) {
    cerr << "Exactly one point should bemarked by the 'LAND' flag in " << path << endl;
    exit(FILE_NOT_ALLOWED_FORMAT);
  }
}

void fill_scheme_field(sub_match<const char *> str, vector<int> &field) {
  std::stringstream ss({string(str)});
  string token{};
  while (ss >> token) {
    try {
      field.push_back(flow.cpName2cpID.at(token));
    }
    catch (const std::out_of_range &ex) {
      throw std::runtime_error(token);
    }
  }
}

//Пройтись по всем точкам схемы и запонить, что эти точки относятся к этой схеме
void cpScheme(Scheme &scheme) {
  if (scheme.type == LINEAR) {
    for (const int pointID : scheme.lin) {
      flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
    }
  } else if (scheme.type == HOLDING_AREA) {
    flow.checkPoints.at(scheme.startP).schemesID.insert(scheme.ID);
  } else {
    for (const auto pointID : scheme.stFrom) {
      flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
    }
    for (const auto pointID : scheme.stTo) {
      flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
    }
  }
}

void read_schemes(string_view path) {
  flow.schemes.clear();
  ifstream file{};
  openFile(path, file);

  /*
   * Всего три регулярных выражения:
   *
   * Для линейной схемы
   *  Одна группа захвата
   *  L0 L1 ...
   *
   *  1) Точки линейной схемы [L0 L1 ...]
   *
   * Для схем типа спрямление
   *  Три группы захвата
   *  [type]StrFrom(F0 F1 ...) StrTo(T0 T1 ...)
   *
   *  1) Тип спрямления [type]: F(an) - веер, T(rombone) - полутромбон, S(traighten) - спрямление
   *  2) Точки откуда можно спрямляться [F0 F1 ...]
   *  3) Точки на которые можно спрямляться [T0 T1 ...]
   *
   * Для Зон Ожидания (ЗО)
   *  Пять групп захвата
   *  Point: time_minMU time_maxMU
   *
   *  1) Точка захода на ЗО [Point]
   *  2) Минимальное время ожидания на ЗО [time_min]
   *  3) Единицы измерения [MU] s - секунды, min - минуты, h - часы
   *  4) Максимальное время ожидания на ЗО [time_max]
   *  5) Единицы измерения [MU] s - секунды, min - минуты, h - часы
   */

  int lineNum{0};
  string line{};
  cmatch res{};
  regex regLin(R"((\+?)\s*([\w]+\s+[\w\s]+))");
  regex regStr(R"((\+?)\s*\[(F|T|S)\]\s*StrFrom\(([\w\s]+)\)\s*StrTo\(([\w\s]+)\))");
  regex regHA(R"((\w+)\s*:\s*([0-9]*\.?[0-9]+)(s|min|h)\s*([0-9]*\.?[0-9]+)(s|min|h)\s*)");
  while (getline(file, line)) {
    lineNum++;

    if (line.empty() || line[0] == '#') {
      continue;
    }

    Scheme scheme;
    scheme.ID = flow.schemes.size();

    try {
      //Обработка Линейной схемы
      if (regex_match(line.c_str(), res, regLin)) {
        scheme.type = LINEAR;
        fill_scheme_field(res[2], scheme.lin);
        scheme.startP = scheme.lin.front();
        scheme.endPs.push_back(scheme.lin.back());
      }
      //Обработка схемы ЗО
      else if (regex_match(line.c_str(), res, regHA)) {
        try {
          scheme.startP = flow.cpName2cpID.at(res[1]);
          scheme.endPs.push_back(flow.cpName2cpID.at(res[1]));
        }
        catch (const std::out_of_range &ex) {
          throw std::runtime_error(res[1]);
        }
        scheme.type = HOLDING_AREA;
        flow.HAs.insert({scheme.startP, scheme.ID});
        string t_minMU = string(res[3]);
        double t_min = stod(string(res[2]));
        scheme.t_min = {t_min, t_minMU};
        string t_maxMU = string(res[5]);
        double t_max = stod(string(res[4]));
        scheme.t_max = {t_max, t_maxMU};
      }
      //Обработка схемы типа спрямления
      else if (regex_match(line.c_str(), res, regStr))
      {
        if (res[2] == "S") {
          scheme.type = STRAIGHTENING;
        } else if (res[2] == "F") {
          scheme.type = FAN;
        } else if (res[2] == "T") {
          scheme.type = TROMBONE;
        }

        fill_scheme_field(res[3], scheme.stFrom);
        fill_scheme_field(res[4], scheme.stTo);
        scheme.startP = scheme.stFrom.front();
        copy(scheme.stTo.begin(), scheme.stTo.end(), back_inserter(scheme.endPs));

        if (find(scheme.stTo.begin(), scheme.stTo.end(), scheme.stFrom.back()) == scheme.stTo.end()) {
          scheme.endPs.push_back(scheme.stFrom.back());
        }
      }
      else {
        cerr << "Warning! Line '" << lineNum << "' in " << path << " doesn't match any scheme format" << endl;
        exit(FILE_NOT_ALLOWED_FORMAT);
      }

      // Обработка флага дополнительного взаимодействия
      // и занесение схемы в списки ее начальной точки
      if (scheme.type != HOLDING_AREA) {
        scheme.needInteract = res[1] == "+";

        flow.starts[scheme.startP].push_back(scheme.ID);
      }
      cpScheme(scheme);
      flow.schemes.push_back(scheme);
    }
    catch (const out_of_range &ex) //Ловим ошибку о не обнаружении точки среди точек из checkPoints
    {
      cerr << "Can't find '" << ex.what() << "' in line '" << lineNum
           << "' among points from Points file" << endl;
      exit(NO_OBJECT);
    }
  }
  file.close();
}

void openFile(const string_view &path, ifstream &openFile) {
  openFile.open(string(path));
  if (!openFile.is_open()) {
    cerr << "Error! File " << path << " does not exist!" << endl;
    exit(FILE_NOT_EXIST);
  }
}


