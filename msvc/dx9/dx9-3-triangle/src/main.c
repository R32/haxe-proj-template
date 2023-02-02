#include "stdafx.h"
#include "d3d9_utility.h"


#define WIDTH 640
#define HEIGHT 480
IDirect3DDevice9 *device = NULL;
IDirect3DVertexBuffer9 *triangle = NULL;

struct vertex {
	float x, y, z;
};

bool setup()
{
	device->lpVtbl->CreateVertexBuffer(
		device,
		3 * sizeof(struct vertex), // size in bytes
		D3DUSAGE_WRITEONLY,        // flags
		D3DFVF_XYZ,                // vertex format
		D3DPOOL_MANAGED,           // managed memory pool
		&triangle,                 // eturn create vertex buffer
		0                          // not used - set to 0
	);

	// fill the buffers with the triangle data.
	struct vertex *vertices;
	triangle->lpVtbl->Lock(triangle, 0, 0, &vertices, 0);
	vertices[0] = (struct vertex){ -1.0f, 0.0f, 2.0f };
	vertices[1] = (struct vertex){  0.0f, 1.0f, 2.0f };
	vertices[2] = (struct vertex){  1.0f, 0.0f, 2.0f };
	triangle->lpVtbl->Unlock(triangle);

	//  Set the projection matrix.
	//D3DXMatrixPerspectiveFovLH(&out, fovy, aspect, zn, zf)
	//D3DXMatrixPerspectiveFovLH(&out, D3DX_PI * 0.5f, (float)Width / (float)Height, 1.0f, 1000.0f)
	//
	//      xScale     0          0               0
	//	0        yScale       0               0
	//	0          0       zf / (zf - zn)     1
	//	0          0  -zn * zf / (zf - zn)    0
	// where:
	// yScale = cot(fovY / 2)
	// xScale = yScale / aspect ratio
	const float y = 1.0f; // cot(45d)
	const float x = y / ((float)WIDTH / (float)HEIGHT);
	const float z = 1000.0f / (1000.0f - 1.0f);
	D3DMATRIX proj = {
		._11 = x   , ._12 = 0.0f, ._13 = 0.0f,      ._14 = 0.0f,
		._21 = 0.0f, ._22 = y   , ._23 = 0.0f,      ._24 = 0.0f,
		._31 = 0.0f, ._32 = 0.0f, ._33 = z   ,      ._34 = 1.0f,
		._41 = 0.0f, ._42 = 0.0f, ._43 = -1.0f * z, ._44 = 0.0f,
	};
	device->lpVtbl->SetTransform(device, D3DTS_PROJECTION, &proj);
	device->lpVtbl->SetRenderState(device, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}

void cleanup()
{
	triangle->lpVtbl->Release(triangle);
}

bool display(float delta)
{
	if (!device)
		return false;
	device->lpVtbl->Clear(device, 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffdddddd, 1.0f, 0);
	device->lpVtbl->BeginScene(device);
	
	device->lpVtbl->SetStreamSource(device, 0, triangle, 0, sizeof(struct vertex));
	device->lpVtbl->SetFVF(device, D3DFVF_XYZ);

	device->lpVtbl->DrawPrimitive(device, D3DPT_TRIANGLELIST, 0 , 1);

	device->lpVtbl->EndScene(device);
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
	if (!d3d9_init3d(hInstance, L"chapter 1 d3d9 init", WIDTH, HEIGHT, true, D3DDEVTYPE_HAL, &device, winproc)) {
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
