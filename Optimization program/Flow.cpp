//
// Created by Антон on 07.07.2021.
//
#include <iostream>
#include "Flow.h"
#include "gurobi_c++.h"


using namespace std;

Flow &Flow::get_instance()
{
    // The only instance of the class is created at the first call get_instance()
    // and will be destroyed only when the program exits
    static Flow instance;
    return instance;
}

Flow flow = Flow::get_instance();

void print_schemes_flow()
{
    cout << "ID \t" << "Type \t" << "Start\t" << "End" << endl << endl;
    for (const auto &scheme : flow.schemes)
    {
        cout << scheme.ID
             << "\t"
             << scheme.type
             << "\t"
             << flow.checkPoints.at(scheme.startP).name
             << "\t"
             << flow.checkPoints.at(scheme.endP).name
             << endl;
    }
}
//struct Point;
//bool isIn(const std::map<int, int> &m, const int key, int &HA_ID);
//void print_(vector<Point> &grb_points);
//
//void calc_best_trajectory(std::vector<std::map<int, int>> &trjs, const int current_vertex, const int vertex_to_reach)
//{
//    static std::map<int, int> trj{};
//    if (current_vertex == vertex_to_reach)
//    {
//        //estimate trj
//        trjs.push_back(trj);
//
////        if (isBetter(trj, 0, 0, 0, 0, 0))
////            flow.best_trj = trj;
//    }
//    else
//    {
//        for (const auto son : flow.graph.at(current_vertex).sons)
//        {
//            trj.insert({current_vertex, son});
//            calc_best_trajectory(trjs, son, vertex_to_reach);
//            trj.erase(current_vertex);
//        }
//    }
//}
//
//void i2s(int n, int l, string &s)
//{
//    stringstream ss;
//    ss << setw(l) << setfill('0') << n;
//    s = ss.str();
//}
//
//GRBLinExpr obj_func{0};
//
//class Point
//{
//public:
//    int ID;
//    GRBVar t;
//    GRBVar tpr; //Время прихода в точку начала ЗО
//    GRBVar v;
//    GRBVar S;
//    GRBVar nu;
//    GRBVar chi;
//
//
//private:
//    static void process_linear_segment(GRBModel &model,
//                                       Point &curr,
//                                       const Point &pred,
//                                       double S_edge,
//                                       const string &numS)
//    {
//
//        curr.t = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + numS);
//        curr.v = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "v" + numS);
//        curr.S = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "S" + numS);
//        curr.chi = model.addVar(0, 1, 0, GRB_BINARY, "chi" + numS);
//
//        obj_func += curr.chi;
//
//        double v_min{flow.checkPoints.at(curr.ID).V_min.meters_per_second};
//        double v_max{flow.checkPoints.at(curr.ID).V_max.meters_per_second};
//        model.addConstr(v_min <= curr.v);
//        model.addConstr(v_max >= curr.v);
//
//        model.addConstr(curr.S == S_edge);
//
//        model.addQConstr((1 - curr.chi)*pred.v + curr.chi*v_min, GRB_LESS_EQUAL, curr.v);
//        model.addQConstr((1 - curr.chi)*pred.v + curr.chi*v_max, GRB_GREATER_EQUAL, curr.v);
//        model.addQConstr(2*curr.S, GRB_EQUAL, (curr.v + pred.v)*(curr.t - pred.t));
//    }
//
//public:
//    static Point create_LPoint(GRBModel &model, int ID, const Point &pred, double S_edge)
//    {
//        Point p;
//        p.ID = ID;
//
//        string numS;
//        i2s(ID, 2, numS);
//        process_linear_segment(model, p, pred, S_edge, numS);
//        return p;
//    }
//
//    static Point create_HAPoint(GRBModel &model, int ID, const Point &pred, const HoldingArea &ha)
//    {
//        Point p;
//        p.ID = ID;
//
//        string numS;
//        i2s(ID, 2, numS);
//        numS += "HA";
//
//        double S_edge = distance(flow.checkPoints.at(pred.ID), flow.checkPoints.at(p.ID)).m();
//        process_linear_segment(model, p, pred, S_edge, numS);
//
//        p.tpr = p.t;
//
//        p.t = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "t" + numS);
//
//
//        model.addConstr(p.tpr + ha.t_min.s() <= p.t);
//        model.addConstr(p.tpr + ha.t_max.s() >= p.t);
//
//
//        return p;
//    };
//};
//
//bool isBetter(const std::map<int, int> &trj, int start_vertex, int end_vertex, double t0, double v0, double tEnd)
//{
//    GRBEnv *env = new GRBEnv();
//    GRBModel model = GRBModel(*env);
//
//    Point grb_start_point;
//    grb_start_point.t = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "t00");
//    grb_start_point.v = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "v00");
//
//    model.addConstr(grb_start_point.t == t0); //s
//    model.addConstr(grb_start_point.v == v0); //m_s
//
//    obj_func = GRBLinExpr(0);
//    int cInter{0};
//    vector<Point> grb_points{grb_start_point};
//    int predIDv{start_vertex};
//    int currIDv;
//    do //ЗО не может быть первой точкой
//    {
//        currIDv = trj.at(predIDv);
//        int &currIDcp = flow.graph.at(currIDv).cpID;
//        //Обработка ЗО
//        int HA_ID{-1};
//        if (isIn(flow.vex2HA, currIDv, HA_ID))
//        {
//            grb_points.push_back(Point::create_HAPoint(model,
//                                                       currIDcp,
//                                                       grb_points.back(),
//                                                       flow.holdingAreas.at(HA_ID)));
//            predIDv = currIDv;
//            cInter++; //Взаимодействие на уход на ЗО
//        }
//        else //Линейный участок
//        {
//            CheckPoint pred = flow.checkPoints.at(flow.graph.at(predIDv).cpID);
//            CheckPoint current = flow.checkPoints.at(currIDcp);
//            grb_points.push_back(Point::create_LPoint(model,
//                                                      currIDcp,
//                                                      grb_points.back(),
//                                                      distance(pred, current).m()));//m
//            predIDv = currIDv;
//        }
//    }
//    while (currIDv != end_vertex);
//
//    model.addConstr(grb_points.back().t == tEnd);
//
//    obj_func += cInter;
//    model.setObjective(obj_func, GRB_MINIMIZE);
//
//    model.set(GRB_IntParam_Threads, 6);
//    model.set(GRB_IntParam_NonConvex, 2);
//    model.optimize();
//
//
//    print_(grb_points);
//
//
//    return true;
//}
//
//bool isIn(const std::map<int, int> &m, const int key, int &HA_ID)
//{
//    auto it = m.find(key);
//    if (it != m.end())
//    {
//        HA_ID = it->second;
//        return true;
//    }
//    return false;
//}
//void print_(vector<Point> &grb_points)
//{
//    for (int n = 0; n < grb_points.size(); n++)
//    {
//        cout << grb_points.at(n).t.get(GRB_StringAttr_VarName)
//             << ":\t"
//             << grb_points.at(n).t.get(GRB_DoubleAttr_X)
//             << "\t\t"
//             << grb_points.at(n).v.get(GRB_StringAttr_VarName)
//             << ":\t"
//             << grb_points.at(n).v.get(GRB_DoubleAttr_X)
//             << "\t\t";
//        try
//        {
//            cout << grb_points.at(n).chi.get(GRB_StringAttr_VarName)
//                 << "\t\t";
//            if (grb_points.at(n).chi.get(GRB_DoubleAttr_X) == 1)
//                cout << 1 << "\t\t";
//            else
//                cout << 0 << "\t\t";
//        }
//        catch (GRBException &ex)
//        {
//            cout << "chi"
//                 << ":\t\t"
//                 << "---"
//                 << "\t\t";
//        }
//        try
//        {
//            cout << grb_points.at(n).nu.get(GRB_StringAttr_VarName)
//                 << ":\t"
//                 << grb_points.at(n).nu.get(GRB_DoubleAttr_X)
//                 << "\t";
//        }
//        catch (GRBException &ex)
//        {
//            cout << "nu"
//                 << ":\t"
//                 << "---"
//                 << "\t";
//        }
//        cout << endl;
//    }
//}
