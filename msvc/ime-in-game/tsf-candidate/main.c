#include "stdafx.h"
#include "tf_uiless.h"

const WCHAR appClassName[] = L"tsf-test";

static int count_id = 0;
#define COUNT_ID()   (count_id++)

// Uses "%lc" for WCHAR and "%ls" for (WCHAR *)
void trace(CHAR *format, ...)
{
	va_list rest;
	va_start(rest, format);
	CHAR buff[512];
	vsprintf_s(buff, sizeof(buff), format, rest);
	va_end(rest);
	OutputDebugStringA(buff);
}

struct tf_uiless uiless = { 0 };
/*
static void ime_handle_composition(struct tsftag *imet, HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct composition_ui *compst = &imet->composition;
}

static void ime_handle_notify(struct tsftag *imet, HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DWORD hr = 1;
	switch (wparam) {
	case IMN_CHANGECANDIDATE:
		// Defer reading data until WM_PAINT, Because it's too frequent from IMN_CHANGECANDIDATE
		imet->candidate.query = (int)lparam; 
		// FALL THRU
	case IMN_CLOSECANDIDATE:
		InvalidateRect(hwnd, &imet->candidate.rect, 0);
		imet->candidate.bytes = 0; // reset
		break;
	default:
		break;
	}
}
*/
static void ime_paint_composition(struct tf_uiless *uiless, HWND hwnd, HDC hdc)
{
	struct dt_composition *compst = &uiless->composition;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &compst->rect, gray);
}

static void ime_paint_candidate(struct tf_uiless *uiless, HWND hwnd, HDC hdc)
{
	struct ITfCandidateListUIElement *itform = uiless->itfcandidate;
	struct dt_candidate *data = &uiless->candidate;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &data->rect, gray);
	
	ulflush_candidate(itform, data);

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
		GetTextExtentPoint32(hdc, L"∏ﬂ", 1, &uiless.csize);
		ReleaseDC(hwnd, hdc);
		//tf_setup(hwnd);
		uiless_initialize(&uiless, hwnd);
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
	/*
	case WM_IME_STARTCOMPOSITION:
		// trace("[%d] WM_IME_STARTCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_IME_COMPOSITION:
		// trace("[%d] WM_IME_COMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		//ime_handle_composition(&tsft, hwnd, wparam, lparam);
		break;	
	case WM_IME_ENDCOMPOSITION:
		// trace("[%d] WM_IME_ENDCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_CHAR:
	{
		struct result_tag *result = &tsft.result;
		if (wparam == VK_BACK) {
			if (result->offset)
				result->offset--;
		} else {
			if (result->offset >= (result->rect.right - result->rect.left) / tsft.csize.cx)
				result->offset = 0;
			result->buff[result->offset++] = (WCHAR)wparam;
		}		
		InvalidateRect(hwnd, &result->rect, 0);
	}
		break;
	case WM_IME_NOTIFY:
		switch (wparam) {
		case IMN_OPENCANDIDATE:
			trace("IMN_OPENCANDIDATE\n");
			break;
		case IMN_CHANGECANDIDATE:
			trace("IMN_CHANGECANDIDATE\n");
			break;
		case IMN_CLOSECANDIDATE:
			trace("IMN_CLOSECANDIDATE\n");
			break;
		}
		break;
	*/
	case WM_RBUTTONUP:
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
	HWND hwnd = CreateWindow(appClassName, L"TSF-IME ≤‚ ‘", WS_OVERLAPPEDWINDOW,
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
