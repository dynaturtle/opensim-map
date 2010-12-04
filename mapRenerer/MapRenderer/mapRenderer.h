#pragma once
#pragma warning (disable: 4018)

#include <vector>
#include <d3dx9.h>
#include "camera.h"
#include "axis.h"
#include "logError.h"
#include "../llprimitive/llprimitive.h"
#include "prim.h"
#include "../llcommon/llmemory.h"

int Width = 640;
int Height = 480;
bool Windowed = true;
Camera camera(Camera::AIRCRAFT);



extern "C" class __declspec(dllexport) MapRenderer
{
public:
	MapRenderer(
		HINSTANCE hInstance,
		HWND hWnd,
		const float ViewHeight, 
		float minX,
		float minY,
		float maxX,
		float maxY,
		LLVolumeParams *volumeParams,
		int volumeCount,
		LLVector3 *positions,
		LLQuaternion *rotations,
		LLVector3 *scales,
		LLTextureEntry **textureEntryLists,
		char *cachePath); 
	~MapRenderer();
	bool render(float);
	char* getTextureName(LLTextureEntry textureEntry, char *cachePath);
	bool InitD3D(HINSTANCE hInstance, 
		HWND hWnd, 
		const int width,
		const int height, 
		const bool windowed);
	int DrawBasicScene();
	bool ScreenShot (const LPCWSTR filename);
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);
	static D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

	void updateMin(LLVector3 );
	void updateMax(LLVector3 );

private:
	std::vector<Prim> prims;
	IDirect3DDevice9* device;

	Axis* axis;
	float minX, minY, maxX, maxY;
	float viewX, viewY, viewZ;
};



	//
	// Materials
	//
	const D3DMATERIAL9 WHITE_MTRL  = MapRenderer::InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL    = MapRenderer::InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL  = MapRenderer::InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL   = MapRenderer::InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = MapRenderer::InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);