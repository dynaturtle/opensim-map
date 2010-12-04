#include "mapRenderer.h"
#include "color.h"



MapRenderer::MapRenderer(
		HINSTANCE hInstance,
		HWND hWnd,
		float viewHeight,
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
		char *cachePath)
{
	InitD3D(hInstance, hWnd, Width, Height, Windowed);
	axis = new Axis(device);
	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
	viewZ = viewHeight;
	viewX = (minX + maxX) /2;
	viewY = (minY + maxY) /2;

	for(int i=0; i<volumeCount; i++)
	{
		if( volumeParams != NULL)
		{
			LLVolume *llvolume = new LLVolume(volumeParams[i], 1.0f, false, false);
			
			S32 numFaces = llvolume->getNumFaces();
			llvolume->destroy();
			for(int j=0; j<numFaces; j++)
			{
				LLVolumeFace llvolumeFace = llvolume->getVolumeFace(j);
				getTextureName(textureEntryLists[i][j], cachePath);
				prims.push_back(Prim(
					device,
					&llvolumeFace,
					positions[i],
					rotations[i],
					scales[i],
					getTextureName(textureEntryLists[i][j], cachePath)));
			}

		}
	}
}

char* MapRenderer::getTextureName(LLTextureEntry textureEntry, char *cachePath)
{
	char textureName[100];
	char jpgName[80];

	strcpy(textureName, cachePath);
	textureEntry.getID().toString(jpgName);
	strcat(jpgName, ".jpg");
	strcat(textureName, "\\");
	strcat(textureName, jpgName);
		
	return textureName;
}

MapRenderer::~MapRenderer()
{
	device->Release();

	delete axis;
}


bool MapRenderer::InitD3D(HINSTANCE hInstance, HWND hWnd, int width, int height,	 bool windowed)
{
	HRESULT hr = 0;

	IDirect3D9* d3d9 = NULL;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		logError("Create D3D failed");
		return 0;
	}

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hWnd; //hWnd
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,         // device type
		hWnd,               // window associated with device, hWnd
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    &device);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd, //hWnd
			vp,
			&d3dpp,
			&device);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 ObjectShape
			logError("create d3d device failed");
			return false;
		}
	}

	d3d9->Release();

	return true;
}


bool MapRenderer::render(float timeDelta)
{
	//正射投影
	D3DXMATRIX proj;
	D3DXMatrixOrthoRH(&proj, 
		maxX - minX, //width of the view volume
		maxY - minY, //height of the view volume
		1.0f, //z-near
		1000.f); //z-far
	//D3DXMatrixPerspectiveFovRH(
	//	&proj,
	//	D3DX_PI * 0.25,
	//	(float)Width/(float)Height,
	//	1.0f,
	//	1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	//
	//需要键盘控制镜头的话要在这里插入代码
	//
	if( ::GetAsyncKeyState('W') & 0x8000f )
		camera.walk(0.004f * timeDelta);

	if( ::GetAsyncKeyState('S') & 0x8000f )
		camera.walk(-0.004f * timeDelta);

	if( ::GetAsyncKeyState('A') & 0x8000f )
		camera.strafe(-0.004f * timeDelta);

	if( ::GetAsyncKeyState('D') & 0x8000f )
		camera.strafe(0.004f * timeDelta);

	if( ::GetAsyncKeyState('R') & 0x8000f )
		camera.fly(0.004f * timeDelta);

	if( ::GetAsyncKeyState('F') & 0x8000f )
		camera.fly(-0.004f * timeDelta);

	if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
		camera.pitch(0.0004f * timeDelta);

	if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
		camera.pitch(-0.0004f * timeDelta);

	if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
		camera.yaw(-0.0004f * timeDelta);
		
	if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
		camera.yaw(0.0004f * timeDelta);

	if( ::GetAsyncKeyState('N') & 0x8000f )
		camera.roll(0.0004f * timeDelta);

	if( ::GetAsyncKeyState('M') & 0x8000f )
		camera.roll(-0.0004f * timeDelta);

	 /*Update the view matrix representing the cameras 
     new position/orientation.*/
	/*D3DXMATRIX V;
	camera.getViewMatrix(&V);

	device->SetTransform(D3DTS_VIEW, &V);*/

	D3DXVECTOR3 position(viewX, viewY, viewZ);
	D3DXVECTOR3 target(viewX, viewY, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
	D3DXMatrixLookAtRH(&V, &position, &target, &up);

	device->SetTransform(D3DTS_VIEW, &V);


	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	device->BeginScene();

	DrawBasicScene();
	axis->draw(device);
	
	for(S32 i=0; i<prims.size(); i++)
	{
		bool res = prims[i].draw(device);
		if (res == false)
		{
			logError("mesh draw error");
			return false;
		}
	}

	device->EndScene();
		
	if(!ScreenShot(LPCWSTR("output.bmp")))
	{
		logError("screenShot Error");
		return false;
	}
	//present是否需要呢？
	device->Present(0, 0, 0, 0);
	return true;
}

int MapRenderer::DrawBasicScene()
{
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	device->SetRenderState(D3DRS_LIGHTING, false);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	return 0;
}

bool MapRenderer::ScreenShot (LPCWSTR filename)
{
	IDirect3DSurface9	*tmp = NULL;
	IDirect3DSurface9	*back = NULL;

	//生成固定颜色模式的离屏表面（Width和 Height为屏幕或窗口的宽高）
	if (FAILED (device->CreateOffscreenPlainSurface(Width, Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tmp, NULL)))
		return false;

	// 获得BackBuffer的D3D Surface
	if (FAILED (device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back)))
		return false;

	// Copy一下，，需要时转换颜色格式
	if (FAILED (D3DXLoadSurfaceFromSurface(tmp, NULL, NULL, back, NULL, NULL, D3DX_FILTER_NONE, 0)))
		return false;

	// 保存成BMP格式
	if (FAILED (D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, tmp, NULL, NULL)))
		return false;

	// 释放Surface，防止内存泄漏
	tmp->Release();
	back->Release();
	return true;
}

D3DLIGHT9 MapRenderer::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 MapRenderer::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 MapRenderer::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DMATERIAL9 MapRenderer::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}