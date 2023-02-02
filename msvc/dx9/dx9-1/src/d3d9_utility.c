#include "stdafx.h"

bool d3d9_init3d(
	HINSTANCE hInstance,
	WCHAR *title,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9 **device,
	WNDPROC proc
) {
	// Create the main application window.
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"Direct3D9App";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass() - FAILED", 0, 0);
		return false;
	}

	HWND hwnd = CreateWindow(
		L"Direct3D9App",  title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width, height,
		HWND_DESKTOP, NULL, hInstance, 0
	);
	if (!hwnd) {
		MessageBox(0, L"CreateWindow() - FAILED", 0, 0);
		return false;
	}
	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);

	// Step 1: Create the IDirect3D9 object.
	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9) {
		MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.
	D3DCAPS9 caps;
	d3d9->lpVtbl->GetDeviceCaps(d3d9, D3DADAPTER_DEFAULT, deviceType, &caps);
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	} else {
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	HRESULT hr = d3d9->lpVtbl->CreateDevice(d3d9,
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		device              // return created device
	);
	d3d9->lpVtbl->Release(d3d9);
	return SUCCEEDED(hr);
}

int d3d9_mainloop(bool (*display)(float))
{
	MSG msg = {0};
	static DWORD lasttime;
	if (lasttime == 0) {
		lasttime = timeGetTime();
	}
	while (true) {
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;
		DWORD curr = timeGetTime();
		display((curr - lasttime) * 0.001f);
		lasttime = curr;
	}
	return (int)msg.wParam;
}
