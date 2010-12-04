#pragma comment(lib, "winmm.lib")
#include "mapRenderer.h"
#include "sqlite3.h"
#include <mmsystem.h>


std::vector<LLProfileParams> profileParams;
std::vector<LLPathParams> pathParams;
std::vector<LLVector3> worldPositions;
std::vector<LLVector3> positions;
std::vector<LLQuaternion> rotations;
std::vector<LLVector3> scales;
HWND hWnd;

int getParams(void* data, int columns, char** columeValues, char** columeNames)
{
	profileParams.push_back(LLProfileParams(
		(U8)atoi(columeValues[0]),
		(U16)atoi(columeValues[1]),
		(U16)atoi(columeValues[2]),	
		(U16)atoi(columeValues[3])));
	pathParams.push_back(LLPathParams(
		(U8)atoi(columeValues[4]),
		(U16)atoi(columeValues[5]),
		(U16)atoi(columeValues[6]),
		(U8)atoi(columeValues[7]),
		(U8)atoi(columeValues[8]),
		(U8)atoi(columeValues[9]),
		(U8)atoi(columeValues[10]),
		(U8)atoi(columeValues[11]),
		(U8)atoi(columeValues[12]),
		(U8)atoi(columeValues[13]),
		(U8)atoi(columeValues[14]),
		(U8)atoi(columeValues[15]),
		(U8)atoi(columeValues[16]),
		(U8)atoi(columeValues[17])));

	scales.push_back(LLVector3(atof(columeValues[18]), atof(columeValues[19]), atof(columeValues[20])));
		
	return 0;
}

int getPosRot(void* data, int columns, char** columeValues, char** columeNames)
{
	LLVector3 pos(atof(columeValues[0]), atof(columeValues[1]), atof(columeValues[2]));
	LLQuaternion rot(atof(columeValues[3]), atof(columeValues[4]), atof(columeValues[5]), atof(columeValues[6]));
	LLVector3 worldPos(atof(columeValues[7]), atof(columeValues[8]), atof(columeValues[9]));
	positions.push_back(pos);
	rotations.push_back(rot);
	worldPositions.push_back(worldPos);
	return 0;
}

bool accessSqlite()
{
	sqlite3* pDB = NULL;
	std::string DBName = "E:\\GIS\\openSim_EXE\\opensim-0.7.0.1-bin\\bin\\OpenSim.db";
	int rc = sqlite3_open(DBName.c_str(), &pDB);

	std::string getParamsCmd = "SELECT ProfileCurve, ProfileBegin, ProfileEnd, ProfileHollow, PathCurve, PathBegin, PathEnd, PathScaleX, PathScaleY, PathShearX, PathShearY, PathTwist, PathTwistBegin, PathRadiusOffset, PathTaperX, PathTaperY, PathRevolutions, PathSkew, ScaleX, ScaleY, ScaleZ FROM primshapes";
	rc = sqlite3_exec(pDB, getParamsCmd.c_str(), getParams, 0, 0);

	std::string getPosCmd = "SELECT GroupPositionX, GroupPositionY, GroupPositionZ, RotationX, RotationY, RotationZ, RotationW, PositionX, PositionY, PositionZ FROM prims";
	rc = sqlite3_exec(pDB, getPosCmd.c_str(), getPosRot, 0, 0);
	
	sqlite3_close(pDB);
	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Init(
			 HINSTANCE hInstance,
			 int width,
			 int height,
			 bool windowed)
{
	WNDCLASS wc;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "CubeClass";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszMenuName = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	if(!RegisterClass(&wc))
	{
		::MessageBox(0, "RegisterClass Failed", "Error", MB_OK);
		return 0;
	}

	hWnd = ::CreateWindow(
		"CubeClass",
		"3DCubeApp",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		width,
		height,
		0,
		0,
		hInstance,
		0);
	if(!hWnd)
	{
		::MessageBox(0, "CreateWindow Failed", "Error", MB_OK);
		return 0;
	}
	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);
}

int EnterMsgLoop(MapRenderer mr)
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();
	while(msg.message != WM_QUIT)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = currTime - lastTime;
			mr.render(timeDelta);

			lastTime = currTime;
			

		}
	}
	return msg.wParam;
}

int main(HINSTANCE hInstance,
				   HINSTANCE PrevInstance,
				   PSTR cmdLine,
				   int showCmd)
{
	accessSqlite();
	Init(hInstance, 640, 480, true);
//	MapRenderer mr(hInstance, hWnd, 27.f, 126, 128, 129, 130.5, profileParams, pathParams, worldPositions, positions, rotations, scales);
//	EnterMsgLoop(mr);
	return 0;
}

