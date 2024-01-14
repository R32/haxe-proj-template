#include "stdafx.h"
#include "resource.h"

#define CLASS_NAME L"my-theme-base"

static LRESULT CALLBACK winproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CREATE:
	{
		HDC hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		ReleaseDC(hwnd, hdc);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		WCHAR *text = L"hello world!";
		HDC hdc = BeginPaint(hwnd, &ps);
		DrawText(hdc, text, (int)wcslen(text), &ps.rcPaint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
	}
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

static void register_class(HINSTANCE instance)
{
	WNDCLASS wc;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.lpfnWndProc = winproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE parent, LPSTR cmdline, int cmdshow)
{
	WCHAR title[16];
	setlocale(LC_CTYPE, "");
	LoadStringW(instance, IDWCS_TITLE, title, ARRAYSIZE(title));
	register_class(instance);
	HWND hwnd = CreateWindow(CLASS_NAME, title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		HWND_DESKTOP, NULL, instance, 0
	);
	ShowWindow(hwnd, cmdshow);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
