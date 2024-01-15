#include "stdafx.h"
#include "resource.h"

static int LOGID = 0;
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
// global config
struct dwmconfig {
	WCHAR title[32];
	MARGINS margins;
	BOOL is_enable;
} dwmcfg = { 0 };

#define CLASS_NAME         L"dwm-basic"
#define LEFT_EXTENDWIDTH   (dwmcfg.margins.cxLeftWidth)
#define RIGHT_EXTENDWIDTH  (dwmcfg.margins.cxRightWidth)
#define TOP_EXTENDWIDTH    (dwmcfg.margins.cyTopHeight)
#define BOTTOM_EXTENDWIDTH (dwmcfg.margins.cyBottomHeight)
#define RECT_WIDTH(rect)   ((rect)->right - (rect)->left)
#define RECT_HEIGHT(rect)  ((rect)->bottom - (rect)->top)

//
#define BLKBORDER          2

static inline void dwm_clientrect(RECT *rect)
{
	rect->left += (LEFT_EXTENDWIDTH - BLKBORDER);
	rect->top += (TOP_EXTENDWIDTH - BLKBORDER);
	rect->right -= (RIGHT_EXTENDWIDTH - BLKBORDER);
	rect->bottom -= (BOTTOM_EXTENDWIDTH - BLKBORDER);
}

static void dwm_remargins(HWND hwnd)
{
	RECT window;
	GetWindowRect(hwnd, &window);
	RECT pt = window;
	ScreenToClient(hwnd, (POINT *)&pt);       // left, top
	ScreenToClient(hwnd, (POINT *)&pt.right); // right, bottom
	// client coordinates
	RECT client;
	GetClientRect(hwnd, &client);
	// margins
	LEFT_EXTENDWIDTH = client.left - pt.left + BLKBORDER; // 2px
	TOP_EXTENDWIDTH = client.top - pt.top + (10 + BLKBORDER);
	RIGHT_EXTENDWIDTH = pt.right - client.right + BLKBORDER;
	BOTTOM_EXTENDWIDTH = pt.bottom - client.bottom + BLKBORDER;
	// Inform application of the frame change. => WM_NCCALCSIZE => WM_ACTIVATE => (SetWindowPos End)
	SetWindowPos(hwnd, NULL,
		window.left, window.top, RECT_WIDTH(&window), RECT_HEIGHT(&window),
		SWP_FRAMECHANGED
	);
}

static void dwm_ncpaint(HWND hwnd, HDC hdc, RECT *out, BOOL active)
{
	RECT client;
	GetClientRect(hwnd, &client);
	HTHEME theme = OpenThemeData(NULL, L"CompositedWindow::Window");
	if (!theme)
		theme = OpenThemeData(NULL, L"Window");
	if (!theme)
		return;
	HDC memory = CreateCompatibleDC(hdc);
	if (!memory)
		goto Cleanup;
	int cx = RECT_WIDTH(&client);
	int cy = RECT_HEIGHT(&client);
	// Define the BITMAPINFO structure used to draw text.
	// Note that biHeight is negative. This is done because
	// DrawThemeTextEx() needs the bitmap to be in top-to-bottom order.
	BITMAPINFO info = { 0 };
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = cx;
	info.bmiHeader.biHeight = -cx;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;
	HBITMAP bitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, NULL, NULL, 0);
	if (!bitmap)
		goto Cleanup;

	HBITMAP bitmapold = (HBITMAP)SelectObject(memory, bitmap);
	// draw
	if (!dwmcfg.is_enable) {
		// TODO : min/max/close button
		size_t color = active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION;
		FillRect(memory, &client, (HBRUSH)(color + 1));
	}
	// update client rect
	dwm_clientrect(&client);
	if (out)
		*out = client;
	RECT header = {
		.top = 0,
		.right = client.right,
		.bottom = client.top,
		.left = client.left,
	};
	// Setup the theme drawing options.
	DTTOPTS dopts = { sizeof(DTTOPTS) };
	dopts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
	dopts.iGlowSize = 10;
	// Select a font.
	LOGFONT logfont;
	HFONT fontold = NULL;
	if (SUCCEEDED(GetThemeSysFont(theme, TMT_CAPTIONFONT, &logfont))) {
		HFONT font = CreateFontIndirect(&logfont);
		fontold = (HFONT)SelectObject(memory, font);
	}
	// Draw the title.
	DrawThemeTextEx(theme,
		memory,
		0, 0,
		dwmcfg.title,
		-1,
		DT_CENTER | DT_WORD_ELLIPSIS | DT_VCENTER | DT_SINGLELINE,
		&header,
		&dopts
	);
	// Blit text to the frame.
	ExcludeClipRect(hdc, client.left, client.top, client.right, client.bottom);
	BitBlt(hdc, 0, 0, cx, cy, memory, 0, 0, SRCCOPY);
	SelectObject(memory, bitmapold);
	if (fontold) {
		SelectObject(memory, fontold);
	}
	DeleteObject(bitmap);
Cleanup:
	SelectClipRgn(hdc, NULL);
	if (memory)
		DeleteDC(memory);
	if (theme)
		CloseThemeData(theme);
}

static LRESULT nc_hittest(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	// Get the point coordinates for the hit test.
	POINT mouse = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

	// Get the window rectangle.
	RECT rcwnd;
	GetWindowRect(hwnd, &rcwnd);

	// Determine if the hit test is for resizing. Default middle (1,1).
	USHORT row = 1;
	USHORT col = 1;
	BOOL onborder = FALSE;

	// Determine if the point is at the top or bottom of the window.
	if (mouse.y >= rcwnd.top && mouse.y < rcwnd.top + TOP_EXTENDWIDTH) {
		onborder = mouse.y < (rcwnd.top + 6);
		row = 0;
	} else if (mouse.y < rcwnd.bottom && mouse.y >= rcwnd.bottom - BOTTOM_EXTENDWIDTH) {
		row = 2;
	}

	// Determine if the point is at the left or right of the window.
	if (mouse.x >= rcwnd.left && mouse.x < rcwnd.left + LEFT_EXTENDWIDTH) {
		col = 0; // left side
	} else if (mouse.x < rcwnd.right && mouse.x >= rcwnd.right - RIGHT_EXTENDWIDTH) {
		col = 2; // right side
	}

	// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
	LRESULT hittests[3][3] =
	{
	    { HTTOPLEFT, onborder ? HTTOP : HTCAPTION, HTTOPRIGHT },
	    { HTLEFT, HTNOWHERE, HTRIGHT },
	    { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
	};
	return hittests[row][col];
}

static LRESULT CALLBACK proc_dwm(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, BOOL *callbasic)
{
	LRESULT ret = 0;
	BOOL done = DwmDefWindowProc(hwnd, msg, wparam, lparam, &ret);
	switch (msg) {
	case WM_NCCALCSIZE:
		if (wparam) {
			ret = 0;
			*callbasic = FALSE;
		}
		break;
	case WM_NCHITTEST:
		if (ret != HTNOWHERE)
			break;
		ret = nc_hittest(hwnd, wparam, lparam);
		if (ret != HTNOWHERE)
			*callbasic = FALSE;
		break;
	case WM_NCPAINT:
		if (!dwmcfg.is_enable)
			*callbasic = FALSE;
		break;
	case WM_NCACTIVATE:
		if (!dwmcfg.is_enable) {
			ret = 1;
			*callbasic = IsIconic(hwnd);
			if (wparam == TRUE) {
				InvalidateRect(hwnd, NULL, 1);
			} else if(wparam == FALSE) {
				HDC hdc = GetDC(hwnd);
				dwm_ncpaint(hwnd, hdc, NULL, 0);
				ReleaseDC(hwnd, hdc);
			}
		}
		break;
	default:
		*callbasic = !done;
		break;
	}
	return ret;
}
static LRESULT CALLBACK proc_basic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static int active;
	switch (msg) {
	case WM_CREATE:
		dwm_remargins(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		active = LOWORD(wparam);
		DwmExtendFrameIntoClientArea(hwnd, &dwmcfg.margins);
		// DwmExtendFrameIntoClientArea(hwnd, &((MARGINS){-1, -1, -1, -1}));
		break;
	case WM_PAINT:
	{
		RECT rect;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		dwm_ncpaint(hwnd, hdc, &rect, active);
		DrawText(hdc, L"hello world!", -1, &rect, DT_SINGLELINE);
		EndPaint(hwnd, &ps);
		trace("%d invalid rect - left : %d, top : %d, right : %d, bottom: %d\n", LOGID++, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
	}
		break;
	case WM_DWMCOMPOSITIONCHANGED:
		DwmIsCompositionEnabled(&dwmcfg.is_enable); // legicy
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	return 0;
}

static LRESULT CALLBACK proc_root(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	BOOL callbasic = 1;
	LRESULT ret = 0;
	ret = proc_dwm(hwnd, msg, wparam, lparam, &callbasic);
	if (callbasic) {
		ret = proc_basic(hwnd, msg, wparam, lparam);
	}
	return ret;
}

static void register_class(HINSTANCE instance)
{
	WNDCLASS wc;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.lpfnWndProc = proc_root;
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
	setlocale(LC_CTYPE, "");
	LoadStringW(NULL, IDWCS_TITLE, dwmcfg.title, ARRAYSIZE(dwmcfg.title));
	DwmIsCompositionEnabled(&dwmcfg.is_enable);
	register_class(instance);
	HWND hwnd = CreateWindow(CLASS_NAME, L"", WS_OVERLAPPEDWINDOW,
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
