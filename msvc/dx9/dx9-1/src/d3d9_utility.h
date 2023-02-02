#pragma once
#include <d3d9.h>

bool d3d9_init3d(
	HINSTANCE hInstance,
	WCHAR *title,
	int width, int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9 **device,
	WNDPROC proc
);

int d3d9_mainloop( bool(*display)(float) );