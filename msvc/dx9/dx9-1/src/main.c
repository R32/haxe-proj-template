#include "stdafx.h"

IDirect3DDevice9 *device = NULL;

bool setup()
{
	// Nothing to setup in this sample.
	return true;
}

void cleanup()
{
	// Nothing to cleanup in this sample.
}

bool display(float delta)
{
	if (!device)
		return false;
	device->lpVtbl->Clear(device, 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	device->lpVtbl->Present(device, 0, 0, 0, 0);
	return true;
}

static LRESULT CALLBACK winproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		break;
	case WM_CLOSE:
		/* FALL THRU */
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	setlocale(LC_CTYPE, "");
	if (!d3d9_init3d(hInstance, L"chapter 1 d3d9 init", 800, 600, true, D3DDEVTYPE_HAL, &device, winproc)) {
		MessageBox(0, L"InitD3D() - FAILED", 0, 0);
		return -1;
	}
	if (!setup()) {
		MessageBox(0, L"Setup() - FAILED", 0, 0);
		return -1;
	}
	d3d9_mainloop(display);
	cleanup();
	device->lpVtbl->Release(device);
	return 0;
}
