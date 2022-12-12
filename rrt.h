#pragma once
#include "graph.h"
#include <vector>
#include <iostream>
using namespace std;

vector<Point> RRT(int N, Graph& g, const Point& Qinit, const Point& Qgoal, const vector<pair<Point, float>>& v);
bool intersection_pc(pair<Point, float> c, const Point& p);