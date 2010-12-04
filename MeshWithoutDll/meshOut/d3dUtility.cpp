
#include "d3dUtility.h"



bool InitD3D(
			 HINSTANCE hInstance,
			 int width,
			 int height,
			 bool windowed,
			 D3DDEVTYPE deviceType,
			 IDirect3DDevice9** device)
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

	HWND hWnd = ::CreateWindow(
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

	//
	//Init D3D
	//
	HRESULT hr = 0;

	IDirect3D9* d3d9 = NULL;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		::MessageBox(0, "Direct3DCreate9 Failed", "Error", MB_OK);
		return 0;
	}

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

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
	d3dpp.hDeviceWindow              = hWnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hWnd,               // window associated with device
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    device);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			deviceType,
			hWnd,
			vp,
			&d3dpp,
			device);

		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 ObjectShape
			::MessageBox(0, "CreateDevice() - FAILED", "Error", MB_OK);
			return false;
		}
	}

	d3d9->Release();

	return true;
}



int EnterMsgLoop(
				 bool (*ptr_display)(float timeDelta))
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
			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}

bool DrawBasicScene(IDirect3DDevice9* Device)
{
	
	//
	// Pre-Render Setup
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//D3DXVECTOR3 dir(1.0f, 0.f, 0.f);
	//D3DXCOLOR col = WHITE;
	//D3DLIGHT9 light = InitDirectionalLight(&dir, &col);

	//Device->SetLight(0, &light);
	//Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
//	Device->SetRenderState(D3DRS_SPECULARENABLE, true);
/*	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);  
	Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT)*/;

	return true;
}


D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
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

D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
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

D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
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

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}
