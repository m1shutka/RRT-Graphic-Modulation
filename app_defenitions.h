#pragma once
#include <Windows.h>
#include <iostream>
#include "resource.h"
#include "rrt.h"
#include <string>
#include <fstream>
using namespace std;

const int N = 10000000;
vector<pair<Point, float>> AppObs;
Point start(N, N);
Point finish(N, N);
vector<Point> path;
Graph g;

//commands buttons
#define OnMenuSaveClicked   1
#define OnMenuImportClicked 2
#define OnExitClicked       4
#define OnStartClicked      5
#define OnResetClicked      6
#define OnAddStartClicked   7
#define OnAddFinishClicked  8
#define OnAddOBSClicked     9
#define OnDeleteOBSClicked  10

char filename[128];

OPENFILENAMEA ofn;
PAINTSTRUCT ps;
HDC hdc;
HBRUSH hBrush;
HPEN hp;
RECT OBS = { 5, 5, 805, 605 };

void WndAddMenu(HWND hWnd);
void WndAddWidgets(HWND hWnd);