#include "app_defenitions.h"
using namespace std;

WNDCLASS WndClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Proc)
{
	WNDCLASS WC = { 0 };
	WC.hIcon = Icon;
	WC.hCursor = Cursor;
	WC.hInstance = hInst;
	WC.lpszClassName = Name;
	WC.hbrBackground = BGColor;
	WC.lpfnWndProc = Proc;
	return WC;
}

bool line(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	return LineTo(hdc, x2, y2);
}

void WndAddMenu(HWND hWnd)
{
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();
	HMENU SubActionMenu = CreateMenu();

	AppendMenu(SubActionMenu, MF_STRING, OnMenuSaveClicked, L"Save Scene");
	AppendMenu(SubActionMenu, MF_STRING, OnMenuImportClicked, L"Import Scene");

	AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubActionMenu, L"Action");
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, OnExitClicked, L"Exit");

	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Menu");

	SetMenu(hWnd, RootMenu);
}

void WndAddWidgets(HWND hWnd)
{
	CreateWindowA("button", "Start", WS_VISIBLE | WS_CHILD, 810, 535, 170, 30, hWnd, (HMENU)OnStartClicked, NULL, NULL);
	CreateWindowA("button", "Reset", WS_VISIBLE | WS_CHILD, 810, 575, 170, 30, hWnd, (HMENU)OnResetClicked, NULL, NULL);
	CreateWindowA("button", "Add/change start point", WS_VISIBLE | WS_CHILD, 810, 5, 170, 30, hWnd, (HMENU)OnAddStartClicked, NULL, NULL);
	CreateWindowA("button", "Add/change finish point", WS_VISIBLE | WS_CHILD, 810, 45, 170, 30, hWnd, (HMENU)OnAddFinishClicked, NULL, NULL);
	CreateWindowA("button", "Add OBS object", WS_VISIBLE | WS_CHILD, 810, 85, 170, 30, hWnd, (HMENU)OnAddOBSClicked, NULL, NULL);
	CreateWindowA("button", "Delete OBS object", WS_VISIBLE | WS_CHILD, 810, 125, 170, 30, hWnd, (HMENU)OnDeleteOBSClicked, NULL, NULL);
	
}

void SaveConfiguration(LPCSTR way)
{
	ofstream file;
	char n = ' ';
	file.open(way, ios_base::binary);
	if (file.is_open()) {
		file.write((char*)&start.x, sizeof(int));
		file.write((char*)&n, sizeof(char));
		file.write((char*)&start.y, sizeof(int));
		file.write((char*)&n, sizeof(char));
		file.write((char*)&finish.x, sizeof(int));
		file.write((char*)&n, sizeof(char));
		file.write((char*)&finish.y, sizeof(int));
		for (int i = 0; i < AppObs.size(); ++i)
		{
			file.write((char*)&n, sizeof(char));
			file.write((char*)&AppObs[i].first.x, sizeof(int));
			file.write((char*)&n, sizeof(char));
			file.write((char*)&AppObs[i].first.y, sizeof(int));
			file.write((char*)&n, sizeof(char));
			file.write((char*)&AppObs[i].second, sizeof(int));
		}
	}
	file.close();
}

void LoadConfiguration(LPCSTR way)
{
	char n;
	ifstream file;
	file.open(way, ios_base::binary);
	Point p(N, N);
	float r;
	if (file.is_open()) {
		file.read((char*)&start.x, sizeof(int));
		file.read((char*)&n, sizeof(char));
		file.read((char*)&start.y, sizeof(int));
		file.read((char*)&n, sizeof(char));
		file.read((char*)&finish.x, sizeof(int));
		file.read((char*)&n, sizeof(char));
		file.read((char*)&finish.y, sizeof(int));
		while (file) {
			file.read((char*)&n, sizeof(char));
			file.read((char*)&p.x, sizeof(int));
			file.read((char*)&n, sizeof(char));
			file.read((char*)&p.y, sizeof(int));
			file.read((char*)&n, sizeof(char));
			file.read((char*)&r, sizeof(int));
			AppObs.push_back(make_pair(p, r));
		}
	}
	if (AppObs.size() > 1) {
		if (AppObs[AppObs.size() - 1].first.x == AppObs[AppObs.size() - 2].first.x and
			AppObs[AppObs.size() - 1].first.y == AppObs[AppObs.size() - 2].first.y and
			AppObs[AppObs.size() - 1].second == AppObs[AppObs.size() - 2].second) {
			AppObs.erase(AppObs.end() - 1);
		}
	}
	file.close();
}

void SetOpenFileParams(HWND hWnd)
{
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = ".bin";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

bool check_intersection(const Point& p)
{
	for (int i = 0; i < AppObs.size(); ++i)
	{
		if (intersection_pc(AppObs[i], p)) {
			return false;
		}
	}
	return true;
}

BOOL CALLBACK AddStartPoint(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	float x, y;
	switch (msg) {
	case WM_INITDIALOG:
		return TRUE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {

		case IDOK: 
			x = GetDlgItemInt(hWnd, IDC_EDIT1, NULL, false);
			y = GetDlgItemInt(hWnd, IDC_EDIT2, NULL, false);

			if (x > 0 and y > 0 and x < 800 and y < 600) {
				start.x = x;
				start.y = y;
			}
			else {
				MessageBoxA(hWnd, "Invalid coordinate!", NULL, MB_OK);
				break;
			}

			if (!check_intersection(start)) {
 				MessageBoxA(hWnd, "Start point intersects with OBS!", NULL, MB_OK);
				start.x = N;
				start.y = N;
				break;
			}

			if (start == finish) {
				MessageBoxA(hWnd, "Invalid coordinate!", NULL, MB_OK);
				start.x = N;
				start.y = N;
				break;
			}
			
			EndDialog(hWnd, LOWORD(wp));
			return FALSE;
			break;

		case IDCANCEL:
			EndDialog(hWnd, LOWORD(wp));
			return TRUE;
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, LOWORD(wp));
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CALLBACK AddFinishPoint(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	float x, y;
	switch (msg) {
	case WM_INITDIALOG:
		return TRUE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK2:
			x = GetDlgItemInt(hWnd, IDC_EDIT3, NULL, false);
			y = GetDlgItemInt(hWnd, IDC_EDIT4, NULL, false);

			if (x > 0 and x < 800 and y > 0 and y < 600) {
				finish.x = x;
				finish.y = y;
			}
			else {
				MessageBoxA(hWnd, "Invalid coordinate!", NULL, MB_OK);
				break;
			}

			if (!check_intersection(finish)) {
				MessageBoxA(hWnd, "Finish point intersects with OBS!", NULL, MB_OK);
				finish.x = N;
				finish.y = N;
				break;
			}

			if (start == finish) {
				MessageBoxA(hWnd, "Invalid coordinate!", NULL, MB_OK);
				finish.x = N;
				finish.y = N;
				break;
			}

			EndDialog(hWnd, LOWORD(wp));
			return FALSE;
			break;

		case IDCANCEL2:
			EndDialog(hWnd, LOWORD(wp));
			return TRUE;
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, LOWORD(wp));
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CALLBACK AddObsObject(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	float x, y, r;
	Point p;
	switch (msg) {
	case WM_INITDIALOG:
		return TRUE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK3:
			x = GetDlgItemInt(hWnd, IDC_EDIT5, NULL, false);
			y = GetDlgItemInt(hWnd, IDC_EDIT6, NULL, false);
			r = GetDlgItemInt(hWnd, IDC_EDIT7, NULL, false);
			
			if (x > 0 and x < 800 and y > 0 and y < 600)
			{
				if (r > 0 and (2 * r < 600))
				{
					if (x - r > 0 and x + r < 800)
					{
						p.x = x;
					}
					else
					{
						if (x - r <= 0)
						{
							x = x - (x - r);
							p.x = x;
						}
						if (x + r >= 800)
						{
							x = x - (x + r) + 800;
							p.x = x;
						}
					}

					if (y - r > 0 and y + r < 600)
					{
						p.y = y;
					}
					else
					{
						if (y - r <= 0)
						{
							y = y - (y - r);
							p.y = y;
						}
						if (y + r >= 600)
						{
							y = y - (y + r) + 600;
							p.y = y;
						}
					}
					AppObs.push_back(make_pair(p, r));

					if (!check_intersection(start) or !check_intersection(finish)) {
						MessageBoxA(hWnd, "Object intersects start/finish!", NULL, MB_OK);
						AppObs.erase(AppObs.end() - 1);
						break;
					}
				}
				else
				{
					MessageBoxA(hWnd, "Invalid radius value!", NULL, MB_OK);
					break;
				}
			}
			else {
				MessageBoxA(hWnd, "Invalid coordinate!", NULL, MB_OK);
				break;
			}
			
			EndDialog(hWnd, LOWORD(wp));
			return FALSE;
			break;

		case IDCANCEL3:
			EndDialog(hWnd, LOWORD(wp));
			return TRUE;
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, LOWORD(wp));
		return TRUE;
		break;
	}
	return FALSE;
}

void DrawLocation()
{
	line(ps.hdc, 5, 5, 5, 605);
	line(ps.hdc, 5, 605, 805, 605);
	line(ps.hdc, 805, 605, 805, 5);
	line(ps.hdc, 805, 5, 5, 5);
}

void DrawOBS()
{
	if (AppObs.size() > 0) {
		hp = CreatePen(PS_ALTERNATE, 1, RGB(93, 167, 223));
		hBrush = CreateSolidBrush(RGB(93, 167, 223));
		SelectObject(ps.hdc, hp);
		SelectObject(ps.hdc, hBrush);
		for (int i = 0; i < AppObs.size(); ++i)
		{
			Ellipse(ps.hdc, AppObs[i].first.x - AppObs[i].second + 5,
				-(AppObs[i].first.y + AppObs[i].second) + 605,
				AppObs[i].first.x + AppObs[i].second + 5,
				-(AppObs[i].first.y - AppObs[i].second) + 605);
		}
		DeleteObject(hBrush);
		DeleteObject(hp);
	}
}

void DrawTree()
{
	if (g.points.size() > 0) {
		hp = CreatePen(PS_ALTERNATE, 1, RGB(0, 0, 0));
		SelectObject(ps.hdc, hp);
		for (int i = 0; i < g.points.size(); ++i)
		{
			auto u = g.get_adjacent_vertices(i);
			for (int j = 0; j < u.size(); ++j)
			{
				line(ps.hdc, g.points[i].x + 5, -g.points[i].y + 605, g.points[u[j]].x + 5, -g.points[u[j]].y + 605);
			}	
		}
		DeleteObject(hp);
	}
}

void DrawPath()
{
	if (path.size() != 0) {
		hp = CreatePen(PS_ALTERNATE, 2, RGB(255, 0, 0));
		SelectObject(ps.hdc, hp);
		for (int i = 0; i < path.size() - 1; ++i)
		{
			line(ps.hdc, 5 + path[i].x, -path[i].y + 605, 5 + path[i + 1].x, -path[i + 1].y + 605);
		}
		DeleteObject(hp);
	}
}

void DrawStart()
{
	if (start.x != N) {
		hp = CreatePen(PS_ALTERNATE, 1, RGB(255, 0, 0));
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(ps.hdc, hp);
		SelectObject(ps.hdc, hBrush);
		Ellipse(ps.hdc, 5 + start.x - 3, -(start.y + 3) + 605, start.x + 5 + 3, -(start.y - 3) + 605);
		DeleteObject(hp);
		DeleteObject(hBrush);
	}
}

void DrawFinish()
{
	if (finish.x != N) {
		hp = CreatePen(PS_ALTERNATE, 1, RGB(255, 0, 0));
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(ps.hdc, hp);
		SelectObject(ps.hdc, hBrush);
		Ellipse(ps.hdc, 5 + finish.x - 3, -(finish.y + 3) + 605, finish.x + 5 + 3, -(finish.y - 3) + 605);
		DeleteObject(hp);
		DeleteObject(hBrush);
	}
}

int GetSamplesNumber()
{
	ifstream file;
	int n = 1000;
	file.open("config.txt");
	if (file.is_open()) {
		file >> n;
	}
	return n;
}

LRESULT CALLBACK AppMainProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HINSTANCE hInst;
	
	switch (msg) {
	case WM_COMMAND:
		switch (wp) {
		case OnMenuSaveClicked:

			if (start.x == N and finish.x == N) {
				MessageBoxA(hWnd, "Not enought data!", NULL, MB_OK);
			}
			else {
				if (GetSaveFileNameA(&ofn)) {
					SaveConfiguration(filename);
				}
			}
			break;

		case OnMenuImportClicked:
			if (GetOpenFileNameA(&ofn)) {
				start.x = N;
				start.y = N;
				finish.x = N;
				finish.y = N;
				AppObs.clear();
				path.clear();
				g.destroy();
				LoadConfiguration(filename);
				InvalidateRect(hWnd, &OBS, TRUE);
			}
			break;

		case OnExitClicked:
			DeleteObject(hp);
			DeleteObject(hBrush);
			PostQuitMessage(0);
			break;

		case OnStartClicked:
			path.clear();
			g.destroy();
			if (start.x != N and finish.x != N)
			{
				path = RRT(GetSamplesNumber(), g, start, finish, AppObs);
				if (path.size() == 0) {
					MessageBoxA(hWnd, "Unsuccess!", NULL, MB_OK);
				}
			}
			else {
				MessageBoxA(hWnd, "Not enougth data!", NULL, MB_OK);
			}
			InvalidateRect(hWnd, &OBS, TRUE);
			break;

		case OnResetClicked:
			start.x = N;
			start.y = N;
			finish.x = N;
			finish.y = N;
			AppObs.clear();
			path.clear();
			g.destroy();
			
			InvalidateRect(hWnd, &OBS, TRUE);
			break;

		case OnAddStartClicked:
			path.clear();
			g.destroy();
			DialogBox(hInst, (LPCTSTR)IDD_ADDSTART, hWnd, (DLGPROC)AddStartPoint);
			InvalidateRect(hWnd, &OBS, TRUE);
			break;

		case OnAddFinishClicked:
			path.clear();
			g.destroy();
			DialogBox(hInst, (LPCTSTR)IDD_ADDFINISH, hWnd, (DLGPROC)AddFinishPoint);
			InvalidateRect(hWnd, &OBS, TRUE);
			break;

		case OnAddOBSClicked:
			path.clear();
			g.destroy();
			DialogBox(hInst, (LPCTSTR)IDD_ADDOBS, hWnd, (DLGPROC)AddObsObject);
			InvalidateRect(hWnd, &OBS, TRUE);
			break;

		case OnDeleteOBSClicked:
			if (!AppObs.empty()) {
				AppObs.erase(AppObs.end() - 1);
				path.clear();
				g.destroy();
				InvalidateRect(hWnd, &OBS, TRUE);
			}
			else {
				MessageBoxA(hWnd, "There is no OBS object!", NULL, MB_OK);
			}
			break;

		default:
			break;
		}
		break;
	
	case WM_CREATE:
		WndAddMenu(hWnd);
		WndAddWidgets(hWnd);
		SetOpenFileParams(hWnd);
		break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);

		DrawLocation();
		DrawOBS();
		DrawTree();
		DrawPath();
		DrawStart();
		DrawFinish();

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		DeleteObject(hp);
		DeleteObject(hBrush);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCMDline, _In_ int nShowCmd)
{
	WNDCLASS AppMainClass = WndClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)), L"WndClass", AppMainProc);

	if (!RegisterClassW(&AppMainClass)) {
		return -1;
	}

	MSG AppMainMsg = { 0 };

	CreateWindow(L"WndClass", L"RRT Graphic Modulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 460, 205, 1000, 670, NULL, NULL, NULL, NULL);
	while (GetMessage(&AppMainMsg, NULL, NULL, NULL))
	{
		TranslateMessage(&AppMainMsg);
		DispatchMessage(&AppMainMsg);
	}
	return 0;
}