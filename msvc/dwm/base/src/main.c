#include "stdafx.h"

static const WCHAR appClassName[] = L"dwm-sample-class";

#define LEFT_EXTENDWIDTH       8
#define RIGHT_EXTENDWIDTH      8
#define TOP_EXTENDWIDTH       36
#define BOTTOM_EXTENDWIDTH    30
#define szTitle               L"custom DWM sample"
#define RECT_WIDTH(rect)      ((rect)->right - (rect)->left)
#define RECT_HEIGHT(rect)     ((rect)->bottom - (rect)->top)

// Paint the title on the custom frame.
static void PaintCustomCaption(HWND hwnd, HDC hdc)
{
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	HTHEME hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
	if (!hTheme)
		return;
	HDC hdcPaint = CreateCompatibleDC(hdc);
	if (!hdcPaint)
		goto Cleanup;
	
	int cx = RECT_WIDTH(&rcClient);
	int cy = RECT_HEIGHT(&rcClient);
	// Define the BITMAPINFO structure used to draw text.
	// Note that biHeight is negative. This is done because
	// DrawThemeTextEx() needs the bitmap to be in top-to-bottom order.
	BITMAPINFO dib = { 0 };
	dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib.bmiHeader.biWidth = cx;
	dib.bmiHeader.biHeight = -cy;
	dib.bmiHeader.biPlanes = 1;
	dib.bmiHeader.biBitCount = 32;
	dib.bmiHeader.biCompression = BI_RGB;

	HBITMAP hbm = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
	if (!hbm)
		goto Cleanup;

	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);
	// Setup the theme drawing options.
	DTTOPTS DttOpts = { sizeof(DTTOPTS) };
	DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
	DttOpts.iGlowSize = 15;

	// Select a font.
	LOGFONT lgFont;
	HFONT hFontOld = NULL;
	if (SUCCEEDED(GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont))) {
		HFONT hFont = CreateFontIndirect(&lgFont);
		hFontOld = (HFONT)SelectObject(hdcPaint, hFont);
	}

	// Draw the title.
	RECT rcPaint = rcClient;
	rcPaint.top += 8;
	rcPaint.right -= 125;
	rcPaint.left += 8;
	rcPaint.bottom = 50;
	DrawThemeTextEx(hTheme,
		hdcPaint,
		0, 0,
		szTitle,
		-1,
		DT_LEFT | DT_WORD_ELLIPSIS,
		&rcPaint,
		&DttOpts
	);
	// Blit text to the frame.
	BitBlt(hdc, 0, 0, cx, cy, hdcPaint, 0, 0, SRCCOPY);
	SelectObject(hdcPaint, hbmOld);
	if (hFontOld) {
		SelectObject(hdcPaint, hFontOld);
	}
	DeleteObject(hbm);
Cleanup:
	if (hdcPaint)
		DeleteDC(hdcPaint);
	if (hTheme)
		CloseThemeData(hTheme);
}

static LRESULT HitTestNCA(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	// Get the point coordinates for the hit test.
	POINT mouse = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

	// Get the window rectangle.
	RECT rcWindow;
	GetWindowRect(hwnd, &rcWindow);

	// Get the frame rectangle, adjusted for the style without a caption.
	RECT rcFrame = { 0, 0, 0, 0 };
	AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, 0);

	// Determine if the hit test is for resizing. Default middle (1,1).
	USHORT row = 1;
	USHORT col = 1;
	BOOL fOnResizeBorder = FALSE;

	// Determine if the point is at the top or bottom of the window.
	if (mouse.y >= rcWindow.top && mouse.y < rcWindow.top + TOP_EXTENDWIDTH) {
		fOnResizeBorder = (mouse.y < (rcWindow.top - rcFrame.top));
		row = 0;
	} else if (mouse.y < rcWindow.bottom && mouse.y >= rcWindow.bottom - BOTTOM_EXTENDWIDTH) {
		row = 2;
	}

	// Determine if the point is at the left or right of the window.
	if (mouse.x >= rcWindow.left && mouse.x < rcWindow.left + LEFT_EXTENDWIDTH) {
		col = 0; // left side
	} else if (mouse.x < rcWindow.right && mouse.x >= rcWindow.right - RIGHT_EXTENDWIDTH) {
		col = 2; // right side
	}

	// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
	LRESULT hitTests[3][3] =
	{
	    { HTTOPLEFT,    fOnResizeBorder ? HTTOP : HTCAPTION,    HTTOPRIGHT },
	    { HTLEFT,       HTNOWHERE,     HTRIGHT },
	    { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
	};
	return hitTests[row][col];
}

//
// Message handler for handling the custom DWM messages.
//
LRESULT CALLBACK CustomDWMProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, BOOL* pfCallDWP)
{
	LRESULT lRet = S_OK;
	HRESULT hr = S_OK;
	BOOL fCallDWP = TRUE; // Pass on to DefWindowProc?

	fCallDWP = !DwmDefWindowProc(hwnd, message, wparam, lparam, &lRet);
	switch (message) {
	case WM_CREATE: 
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		// Inform application of the frame change.
		SetWindowPos(
			hwnd,
			NULL,
			rect.left,
			rect.top,
			RECT_WIDTH(&rect),
			RECT_HEIGHT(&rect),
			SWP_FRAMECHANGED
		);
		fCallDWP = TRUE;
		lRet = S_OK;
	}
		break;
	case WM_ACTIVATE:
	{
		// (GET_WM_ACTIVATE_STATE(wparam, lparam) == WA_INACTIVE)
		// Extend the frame into the client area.
		MARGINS margins = {
			LEFT_EXTENDWIDTH,
			RIGHT_EXTENDWIDTH,
			TOP_EXTENDWIDTH,
			BOTTOM_EXTENDWIDTH,
		};
		hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
		if (FAILED(hr)) {
			// Handle error.
			// MessageBox(hwnd, L"DwmExtendFrameIntoClientArea", L"", MB_OK);
		}
		fCallDWP = TRUE;
		lRet = S_OK;
	}
		break;
	/*
	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		PaintCustomCaption(hwnd, hdc);
		EndPaint(hwnd, &ps);
		fCallDWP = TRUE;
		lRet = S_OK;
	}
		break;
	*/
	case WM_NCCALCSIZE:
	{
		if (!wparam)
			break;
		// Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
		NCCALCSIZE_PARAMS *pncsp = (NCCALCSIZE_PARAMS *)lparam;
		//pncsp->rgrc[0].left = pncsp->rgrc[0].left + 0;
		//pncsp->rgrc[0].top = pncsp->rgrc[0].top + 0;
		//pncsp->rgrc[0].right = pncsp->rgrc[0].right - 0;
		//pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;
		// No need to pass the message on to the DefWindowProc.
		fCallDWP = FALSE;
		lRet = S_OK;
	}
		break;
	case WM_NCHITTEST:
	{
		if (lRet != S_OK)
			break;
		lRet = HitTestNCA(hwnd, wparam, lparam);
		if (lRet != HTNOWHERE)
			fCallDWP = FALSE;
	}
		break;
	default:
		break;
	}
	*pfCallDWP = fCallDWP;
	return lRet;
}

//
// Message handler for the application.
//
static LRESULT CALLBACK NormalProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_CREATE:
	{
	}
		break;
	case WM_COMMAND:
	{
	}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		PaintCustomCaption(hwnd, hdc);
		// Add any drawing code here...
		EndPaint(hwnd, &ps);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}

static LRESULT CALLBACK MainProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL callNormalProc = TRUE;
	BOOL enabled = FALSE;
	LRESULT lRet = S_OK;
	HRESULT hr = S_OK;

	// Winproc worker for custom frame issues.
	hr = DwmIsCompositionEnabled(&enabled);
	if (SUCCEEDED(hr) && enabled) {
		lRet = CustomDWMProc(hwnd, message, wparam, lparam, &callNormalProc);
	}

	// Winproc worker for the rest of the application.
	if (callNormalProc) {
		lRet = NormalProc(hwnd, message, wparam, lparam);
	}
	return lRet;
}

static void MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = appClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClassEx(&wc);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	// Localization
	setlocale(LC_CTYPE, "");
	MyRegisterClass(hInstance);
	HWND hwnd = CreateWindowEx(0, appClassName, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 640, 480, HWND_DESKTOP, NULL, hInstance, 0);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG	msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
