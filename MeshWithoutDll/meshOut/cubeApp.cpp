
#include "d3dUtility.h"
#include "camera.h"
#include "axis.h"
#include <string.h>
#include "sqlite3.h"

//
//Globals
//
int Width = 640;
int Height = 480;
bool Windowed = true;
Camera TheCamera(Camera::AIRCRAFT);
std::vector<Object> obj;
std::vector<LLProfileParams> profileParams;
std::vector<LLPathParams> pathParams;
std::vector<LLVector3> positions;
std::vector<LLQuaternion> rotations;
std::vector<LLVector3> scales;

IDirect3DTexture9* Tex = 0;
IDirect3DDevice9* Device = NULL;
Axis* axis = 0;
ID3DXFont* font;
RECT rect;

int getParams(void* data, int columns, char** columeValues, char** columeNames)
{
	//U8 p0 = atoi(columeValues[0]);
	//U16 P1 =atoi(columeValues[1]);
	//U16 P2 = atoi(columeValues[2]);	
	//U16 P3 = atoi(columeValues[3]);

	//U8 P4 = 	atoi(columeValues[4]);
	//U16 P5 = atoi(columeValues[5]);
	//U16 P6 = atoi(columeValues[6]);
	//U8 P7 = 	atoi(columeValues[7]);
	//U8 P8 = 	atoi(columeValues[8]);
	//U8 P9 = 	atoi(columeValues[9]);
	//U8 P10 = atoi(columeValues[10]);
	//U8 P11 = atoi(columeValues[11]);
	//U8 P12 = atoi(columeValues[12]);
	//U8 P13 = atoi(columeValues[13]);
	//U8 P14 = atoi(columeValues[14]);
	//U8 P15 = atoi(columeValues[15]);
	//U8 P16 = atoi(columeValues[16]);
	//U8 P17 = atoi(columeValues[17]);
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
	positions.push_back(pos);
	
	rotations.push_back(rot);
	return 0;
}

bool accessSqlite()
{
	sqlite3* pDB = NULL;
	std::string DBName = "E:\\GIS\\openSim_EXE\\opensim-0.7.0.1-bin\\bin\\OpenSim.db";
	int rc = sqlite3_open(DBName.c_str(), &pDB);

	std::string getParamsCmd = "SELECT ProfileCurve, ProfileBegin, ProfileEnd, ProfileHollow, PathCurve, PathBegin, PathEnd, PathScaleX, PathScaleY, PathShearX, PathShearY, PathTwist, PathTwistBegin, PathRadiusOffset, PathTaperX, PathTaperY, PathRevolutions, PathSkew, ScaleX, ScaleY, ScaleZ FROM primshapes";
	rc = sqlite3_exec(pDB, getParamsCmd.c_str(), getParams, 0, 0);

	std::string getPosCmd = "SELECT GroupPositionX, GroupPositionY, GroupPositionZ, RotationX, RotationY, RotationZ, RotationW FROM prims";
	rc = sqlite3_exec(pDB, getPosCmd.c_str(), getPosRot, 0, 0);
	
	sqlite3_close(pDB);
	return true;
}


bool Setup()
{
	axis = new Axis(Device);
	accessSqlite();
	
	//LLProfileParams profileParams = LLProfileParams(
	//	LL_PCODE_PROFILE_SQUARE, //Curve
	//	0.f, //Begin
	//	1.0f, //End
	//	0.0f); //Hollow
	//LLPathParams pathParams = LLPathParams(
	//	LL_PCODE_PATH_LINE, //curve 
	//	0.f, //begin
	//	1.0f, //end
	//	1.f, //scaleX
	//	1.f, //scaleY
	//	0.f, //shearX
	//	0.f, //shearY
	//	0.f, //twistEnd
	//	0.f, //twistBegin
	//	0.f, //radiusOffset
	//	0.f, //taperX
	//	0.f, //taperY
	//	1.f, //revolutions
	//	0.f);  //skew
	for(int i=0; i<profileParams.size(); i++)
	{
		LLVolumeParams volumeParams = LLVolumeParams(
			profileParams[ i ], 
			pathParams[ i ], 
			0, //LLUUID
			LL_SCULPT_TYPE_NONE);
		LLVolume* volume = new LLVolume(
			volumeParams, 
			4.f, //detail
			false, //generate_single_face
			FALSE); //is_unique

		S32 numFaces = volume->getNumFaces();
		for(S32 j=0; j<numFaces; j++)
		{
			LLVolumeFace volumeFace = volume->getVolumeFace(j); 
			obj.push_back( Object(Device, &volumeFace, positions[i], rotations[i], scales[i]));
		}
	}
	
	//创建纹理
	D3DXCreateTextureFromFile(
	Device,
	"testTex.jpg",
	&Tex);

	//HRESULT D3DXCreateFont(
	//	  Device,		//LPDIRECT3DDEVICE9 pDevice,
	//	  50,	//INT Height,
	//	  20,	 //UINT Width,
	//	  500,		//UINT Weight,
	//	  0,	//UINT MipLevels,
	//	  false,	//BOOL Italic,
	//	  DEFAULT_CHARSET,	 //DWORD CharSet,
	//	  0,	//DWORD OutputPrecision,
	//	  0,	//DWORD Quality,
	//	  0,	//DWORD PitchAndFamily,
	//	  "Arial",	//LPCTSTR pFacename,
	//	  &font	//LPD3DXFONT * ppFont
	//);
	//RECT rect = {-1, 0, Width, Height};

	/*
	Set projection matrix
	*/
	D3DXMATRIX proj;
	//正射投影
	//D3DXMatrixOrthoLH(&proj, 
	//	5, //width of the view volume
	//	5, //height of the view volume
	//	1.0f, //z-near
	//	1000.f); //z-far

	//透视投影
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25,
		(float)Width/(float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

//截图
void ScreenShot (char *filename)
{
	IDirect3DSurface9	*tmp = NULL;
	IDirect3DSurface9	*back = NULL;

	//生成固定颜色模式的离屏表面（Width和 Height为屏幕或窗口的宽高）
	Device->CreateOffscreenPlainSurface(Width, Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tmp, NULL);

	// 获得BackBuffer的D3D Surface
	Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back);

	// Copy一下，，需要时转换颜色格式
	D3DXLoadSurfaceFromSurface(tmp, NULL, NULL, back, NULL, NULL, D3DX_FILTER_NONE, 0);

	// 保存成BMP格式
	D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, tmp, NULL, NULL);

	// 释放Surface，防止内存泄漏
	tmp->Release();
	back->Release();
}


bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update: Update the camera.
		//

		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.walk(0.004f * timeDelta);

		if( ::GetAsyncKeyState('S') & 0x8000f )
			TheCamera.walk(-0.004f * timeDelta);

		if( ::GetAsyncKeyState('A') & 0x8000f )
			TheCamera.strafe(-0.004f * timeDelta);

		if( ::GetAsyncKeyState('D') & 0x8000f )
			TheCamera.strafe(0.004f * timeDelta);

		if( ::GetAsyncKeyState('R') & 0x8000f )
			TheCamera.fly(0.004f * timeDelta);

		if( ::GetAsyncKeyState('F') & 0x8000f )
			TheCamera.fly(-0.004f * timeDelta);

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.pitch(0.0004f * timeDelta);

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.pitch(-0.0004f * timeDelta);

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-0.0004f * timeDelta);
			
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(0.0004f * timeDelta);

		if( ::GetAsyncKeyState('N') & 0x8000f )
			TheCamera.roll(0.0004f * timeDelta);

		if( ::GetAsyncKeyState('M') & 0x8000f )
			TheCamera.roll(-0.0004f * timeDelta);

		 /*Update the view matrix representing the cameras 
         new position/orientation.*/
		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);

		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Render
		//

	/*	D3DXVECTOR3 position(127.0f, 129.0f, -31.0f);
		D3DXVECTOR3 target(127.0f, 129.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);*/

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		DrawBasicScene(Device);
		axis->draw(Device);
		
		for(S32 i=0; i<obj.size(); i++)
		{
			obj[i].draw(Device, &WHITE_MTRL, Tex);
		}

		Device->EndScene();
			
		ScreenShot("1.bmp");
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

void Cleanup()
{
	//Delete<ObjectShape*>(obj);
	Release<IDirect3DTexture9*>(Tex);
	Delete<Axis*>(axis);

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



int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE PrevInstance,
				   PSTR cmdLine,
				   int showCmd)
{
	if(!InitD3D(hInstance, Width, Height, Windowed, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D Failed", "Error", MB_OK);
		return 0;
	}
	if(!Setup())
	{
		::MessageBox(0, "Setup Failed", "Error", MB_OK);
		return 0;			
	}

	EnterMsgLoop( Display );

	Cleanup();
	
	Device->Release();

	return 0;
}