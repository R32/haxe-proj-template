#include "stdafx.h"
#include "stdio.h"
#include "tf_uiless.h"

const WCHAR appClassName[] = L"tsf-test";

static int count_id = 0;
#define COUNT_ID()   (count_id++)

// Uses "%lc" for WCHAR and "%ls" for (WCHAR *)
void trace(CHAR *format, ...)
{
	//static FILE *file = NULL;
	//if (!file) fopen_s(&file, "log.txt", "wb");
	va_list rest;
	va_start(rest, format);
	CHAR buff[512];
	vsprintf_s(buff, sizeof(buff), format, rest);
	va_end(rest);
	OutputDebugStringA(buff);
	//fputs(buff, file);
}

struct tf_uiless uiless = { 0 };

static void ime_paint_composition(struct tf_uiless *uiless, HWND hwnd, HDC hdc)
{
	struct dt_composition *compst = &uiless->composition;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &compst->rect, gray);
	if (compst->flags & GCS_CURSORPOS) {
		WCHAR *buff = (WCHAR *)compst->buff;
		for (int i = compst->wcslen; i > compst->cursor; i--) {
			buff[i] = buff[i - 1];
		}
		if (compst->wcslen > compst->cursor) {
			compst->wcslen++;
			buff[compst->wcslen] = 0;
			buff[compst->cursor] = ',';
		}
		//trace("[%d] GCS_CURSORPOS - DELTASTART : %d, CURSORPOS : %d, len : 0x%x\n", COUNT_ID(), compst->cstart, compst->cursor, compst->wcslen);
	}
	if (compst->flags & GCS_RESULTSTR) {}
	if (compst->wcslen) {
		TextOut(hdc, compst->rect.left, compst->rect.top, (WCHAR *)compst->buff, compst->wcslen);
	}
}

static void ime_paint_candidate(struct tf_uiless *uiless, HWND hwnd, HDC hdc)
{
	struct dt_candidate *data = &uiless->candidate;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &data->rect, gray);

	ulflush_candidate(uiless, hwnd);

	if (!data->count)
		return;
	const int cy = uiless->csize.cy;
	const int top = data->rect.top;
	static WCHAR tmp[32];
	for (int i = data->pagestart, j = 0; i < data->pagestop; i++, j++) {
		const WCHAR *hanzi = data->buff + data->offset[j];
		const WCHAR *format = i != data->selection ? L"%d: %s" : L"%d: [%s]";
		const DWORD count = _snwprintf_s(tmp, sizeof(tmp), ARRAYSIZE(tmp), format, j + 1, hanzi);
		TextOut(hdc, data->rect.left, cy * j + top, tmp, count);
	}
	int i = 0;
	if (data->pagecurrent > 0) {
		tmp[i++] = '<';
		tmp[i++] = '<';
	}
	if (data->pagestop < data->count) {
		if (i)
			tmp[i++] = ' ';
		tmp[i++] = '>';
		tmp[i++] = '>';
	}
	TextOut(hdc, data->rect.left, top + (data->pagestop - data->pagestart) * cy, tmp, i);
}

static void ime_paint_result(struct tf_uiless *uiless, HWND hwnd, HDC hdc)
{
	struct dt_result *result = &uiless->result;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &result->rect, gray);
	if (result->offset) {
		TextOut(hdc, result->rect.left, result->rect.top, (WCHAR *)result->buff, result->offset);
	}
}

static void ime_paint(struct tf_uiless *uiless, HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	ime_paint_composition(uiless, hwnd, hdc);
	ime_paint_candidate(uiless, hwnd, hdc);
	ime_paint_result(uiless, hwnd, hdc);
	EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CREATE:
	{
		HDC hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextExtentPoint32(hdc, L"��", 1, &uiless.csize);
		ReleaseDC(hwnd, hdc);
		//tf_setup(hwnd);
		uiless_initialize(&uiless, hwnd);
		ullang_enable(&uiless);
	}
		break;
	case WM_DESTROY:
		uiless_release(&uiless);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		RECT rect;
		long cy = uiless.csize.cy;
		long cx = uiless.csize.cx;
		GetClientRect(hwnd, &rect);
		uiless.result.rect      = (RECT){0, cy * 0, rect.right, cy * (0 + 1) };
		uiless.composition.rect = (RECT){0, cy * 2, rect.right, cy * (2 + 1) };
		uiless.candidate.rect   = (RECT){0, cy * 5, cx * 10   , cy * (5 + 10)};
	}
		break;

	case WM_PAINT:
		ime_paint(&uiless, hwnd);
		break;
	case WM_IME_SETCONTEXT:
		lparam = 0;
		goto defwinproc;
		break;

	case WM_IME_STARTCOMPOSITION:
		// trace("[%d] WM_IME_STARTCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_IME_COMPOSITION:
		// trace("[%d] WM_IME_COMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		ulflush_composition(&uiless, hwnd, wparam, lparam);
		break;
	case WM_IME_ENDCOMPOSITION:
		// trace("[%d] WM_IME_ENDCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_CHAR:
	{
		struct dt_result *result = &uiless.result;
		if (wparam == VK_BACK) {
			if (result->offset)
				result->offset--;
		} else {
			if (result->offset >= (result->rect.right - result->rect.left) / uiless.csize.cx)
				result->offset = 0;
			result->buff[result->offset++] = (WCHAR)wparam;
		}
		InvalidateRect(hwnd, &result->rect, 0);
	}
		break;
	case WM_IME_NOTIFY:
		uiflush_candidate_immnotify(&uiless, hwnd, wparam, lparam);
		break;
	case WM_RBUTTONUP:
	{
		int r = ullang_englishkbl(&uiless, GetCurrentThreadId());
		trace("switch to english : %d\n", r == S_OK);
	}
		break;
	default:
		// trace("[%d] event : 0x%x\n", COUNT_ID(), msg);
		defwinproc:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

static void MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = appClassName;
	RegisterClass(&wc);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	setlocale(LC_CTYPE, ""); // Localization
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	MyRegisterClass(hInstance);
	HWND hwnd = CreateWindow(appClassName, L"TSF-IME ����", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 500, 390, HWND_DESKTOP, NULL, hInstance, 0);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CoUninitialize();
	return 0;
}
