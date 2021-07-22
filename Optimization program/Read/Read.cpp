//
// Created by Антон on 07.07.2021.
//

#include "Read.h"
#include "Optimization program/Flow.h"
#include <regex>
#include "string_view"
#include "iostream"
#include <fstream>
#include "Optimization program/Errors/Errors.h"
#include <string>

using namespace std;


void read_checkPoints(string_view path)
{
    flow.checkPoints.resize(count_number_of_line(path) - 1);
    
    ifstream file{};
    openFile(path, file);
    
    string x_mu{}, y_mu{}, z_mu{}, Vmin_mu{}, Vmax_mu{};
    file >> x_mu >> y_mu >> z_mu >> Vmin_mu >> Vmax_mu; //Собрали информацию о единицах измерений
    
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
    
    string line{};
    cmatch res{};
    regex regular(
            R"(([a-z,A-Z]\w*)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([0-9]*\.?[0-9]+)\s+([0-9]*\.?[0-9]+)\s*(LAND|0)?\s*)");
    
    getline(file, line);// Пропустить первую строку с единицами измерений
    int count_land_flag{0};
    for (int i = 0; i < flow.checkPoints.size(); ++i)
    {
        getline(file, line);
        if (!regex_match(line.c_str(), res, regular))
        {
            cerr << "Warning! Line '" << i + 2 << "' in " << path << " doesn't follow the input format." << endl;
            exit(FILE_NOT_ALLOWED_FORMAT);
        }
        for (const auto &el : flow.checkPoints) //Проверка, чтобы две точки с одинаковым названием не встречались
        {
            if (el.name == res[1])
            {
                cerr << "Attention! '" << res[1] << "' point occurs twice in " << path << endl;
                exit(DUPLICATE_NAME);
            }
        }
        double x_value, y_value, z_value, v_min, v_max;
        x_value = stod(string(res[2]));
        y_value = stod(string(res[3]));
        z_value = stod(string(res[4]));
        v_min = stod(string(res[5]));
        v_max = stod(string(res[6]));
        
        auto &checkPoint = flow.checkPoints[i];
        checkPoint.name = res[1];
        checkPoint.ID = i;
        checkPoint.x = {x_value, x_mu};
        checkPoint.y = {y_value, y_mu};
        checkPoint.z = {z_value, z_mu};
        checkPoint.V_min = {v_min, Vmin_mu};
        checkPoint.V_max = {v_max, Vmax_mu};
        
        if (res[7] == "LAND")
        {
            checkPoint.landing_flag = true;
            count_land_flag++;
            
            flow.final_point = i;
        }
        flow.cpName2cpID.insert({res[1], i});
    }
    file.close();
    
    if (count_land_flag != 1)
    {
        cerr << "Check amount of 'LAND' flag! In " << path << endl;
        exit(FILE_NOT_ALLOWED_FORMAT);
    }
}

void fill_scheme_field(sub_match<const char *> str, vector<int> &field)
{
    std::stringstream ss({string(str)});
    string token{};
    while (ss >> token)
    {
        try
        {
            field.push_back(flow.cpName2cpID.at(token));
        }
        catch (const std::out_of_range &ex)
        {
            throw std::runtime_error(token);
        }
    }
}

//Пройтись по всем точкам схемы и запонить, что эти точки относятся к этой схеме
void cpScheme(Scheme &scheme)
{
    if (scheme.type == "L")
    {
        for (const auto pointID : scheme.lin)
        {
            flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
        }
    }
    if (scheme.type == "S")
    {
        for (const auto pointID : scheme.stFrom)
        {
            flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
        }
        for (const auto pointID : scheme.stTo)
        {
            flow.checkPoints.at(pointID).schemesID.insert(scheme.ID);
        }
    }
}

void read_schemes(string_view path)
{
    flow.schemes.resize(count_number_of_line(path));
    
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
    
    string line{};
    cmatch res{};
    regex regLin(R"(([\w]+\s+[\w\s]+))");
    regex regStr(R"(\[(F|T|S)\]StrFrom\(([\w\s]+)\)\s*StrTo\(([\w\s]+)\))");
    regex regHA(R"((\w+)\s*:\s*([0-9]*\.?[0-9]+)(s|min|h)\s*([0-9]*\.?[0-9]+)(s|min|h)\s*)");
    for (int i = 0; i < flow.schemes.size(); ++i)
    {
        auto &scheme = flow.schemes[i];
        scheme.ID = i;
        
        getline(file, line);
        try
        {
            if (regex_match(line.c_str(), res, regLin)) //Обработка Линейной схемы
            {
                scheme.type = "L";
                fill_scheme_field(res[1], scheme.lin);
                scheme.startP = scheme.lin.front();
                scheme.endP = scheme.lin.back();
                flow.starts[scheme.startP].push_back(scheme.ID);
                cpScheme(scheme);
            }
            else if (regex_match(line.c_str(), res, regHA)) //Обработка схемы ЗО
            {
                try
                {
                    scheme.startP = flow.cpName2cpID.at(res[1]);
                    scheme.endP = flow.cpName2cpID.at(res[1]);
                }
                catch (const std::out_of_range &ex)
                {
                    throw std::runtime_error(res[1]);
                }
                scheme.type = "HA";
                
                flow.HAs.insert({scheme.startP, scheme.ID});
                
                auto t_minMU = string(res[3]);
                auto t_min = stod(string(res[2]));
                scheme.t_min = {t_min, t_minMU};
                
                auto t_maxMU = string(res[5]);
                auto t_max = stod(string(res[4]));
                scheme.t_max = {t_max, t_maxMU};
            }
            else if (regex_match(line.c_str(), res, regStr)) //Обработка схемы типа спрямления
            {
                scheme.type = res[1];
                fill_scheme_field(res[2], scheme.stFrom);
                fill_scheme_field(res[3], scheme.stTo);
                scheme.startP = scheme.stFrom.front();
                scheme.endP = scheme.stFrom.back();
                cpScheme(scheme);
                flow.starts[scheme.startP].push_back(scheme.ID);
            }
            else
            {
                cerr << "Warning! Line '" << i + 1 << "' in " << path << " doesn't follow the input format." << endl;
                exit(FILE_NOT_ALLOWED_FORMAT);
            }
        }
        catch (const out_of_range &ex) //Ловим ошибку о не обнаружении точки среди точек из checkPoints
        {
            cerr << "Can't find '" << ex.what() << "' in line '" << i + 1
                 << "' among points from Points file" << endl;
            exit(NO_OBJECT);
        }
    }
    file.close();
}

void openFile(const string_view &path, ifstream &openFile)
{
    openFile.open(string(path));
    if (!openFile.is_open())
    {
        cerr << "Error! File " << path << " does not exist!" << endl;
        exit(FILE_NOT_EXIST);
    }
}

int count_number_of_line(string_view path)
{
    ifstream checkPointFile{};
    openFile(path, checkPointFile);
    
    int i{};
    string tmp{};
    while (getline(checkPointFile, tmp))
        i++;
    return i;
}

