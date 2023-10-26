#include "stdafx.h"

// TODO: 目前 BUG, (微软拼音-2010) 输入单个的声母字符时不会弹出候选窗口, 估计是哪里可以设置
// 如果获得原始输入的字母了？

const WCHAR appClassName[] = L"ime-test";

static int count_id = 0;
#define COUNT_ID()   (count_id++)

// Uses "%lc" for WCHAR and "%ls" for (WCHAR *)
static void trace(CHAR *format, ...)
{
	va_list rest;
	va_start(rest, format);
	CHAR buff[512];
	vsprintf_s(buff, sizeof(buff), format, rest);
	va_end(rest);
	OutputDebugStringA(buff);
}
struct composition_tag {
	RECT rect;
	int state;
	int cstart;
	int cursor;
	int wcslen;
	WCHAR buff[512];
};
struct candidate_tag {
	RECT rect;
	int query;
	int pagesize;
	int bytes;
	WCHAR buff[8 * 1024]; // 16KB
};
struct result_tag {
	RECT rect;
	int offset;
	WCHAR buff[512];
};
struct imetag {
	struct composition_tag composition;
	struct candidate_tag candidate;
	struct result_tag result;
	SIZE csize;      // char size
	HIMC ctxprev;    // the old himc context
	int conversion;  // ImmGetConversionStatus
	int sentence;    // ImmGetConversionStatus
	int property;    // ImmGetProperty
	int hideui;
} imet = {
	.hideui = 1,
};

static int ime_data_compositioninner(HIMC imc, DWORD kind, WCHAR *buf, int count)
{
	DWORD bytes = ImmGetCompositionString(imc, kind, buf, count);
	DWORD len = (bytes + (sizeof(WCHAR) - 1)) / sizeof(WCHAR);
	buf[len] = 0;
	return len;
}

static void ime_handle_composition(struct imetag *imet, HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct composition_tag *compst = &imet->composition;
	HIMC imc = ImmGetContext(hwnd);
	if (lparam & GCS_CURSORPOS) {
		// GCS_DELTASTART is the caret offset start of the last change
		// GCS_CURSORPOS is the caret offset of current
		compst->cstart = lparam & GCS_DELTASTART ? ImmGetCompositionString(imc, GCS_DELTASTART, NULL, 0) : -1;
		compst->cursor = ImmGetCompositionString(imc, GCS_CURSORPOS, NULL, 0);
	}
	if (lparam & GCS_RESULTSTR) {
		struct result_tag *result = &imet->result;
		if (result->offset >= (result->rect.right - result->rect.left) / imet->csize.cx)
			result->offset = 0;
		result->offset += ime_data_compositioninner(imc, GCS_RESULTSTR, result->buff + result->offset, (int)sizeof(result->buff) - result->offset * sizeof(WCHAR));
		InvalidateRect(hwnd, &imet->result.rect, 0);
		compst->wcslen = 0;
	}
	if (lparam & GCS_COMPSTR) {
		compst->wcslen = ime_data_compositioninner(imc, GCS_COMPSTR, compst->buff, sizeof(compst->buff));
	}
	ImmReleaseContext(hwnd, imc);
	InvalidateRect(hwnd, &compst->rect, 0);
	compst->state = (int)lparam;
}

static void ime_handle_notify(struct imetag *imet, HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DWORD hr = 1;
	switch (wparam) {
	case IMN_CLOSESTATUSWINDOW:
	case IMN_OPENSTATUSWINDOW:
		break;
	case IMN_CHANGECANDIDATE:
		// Defer reading data until WM_PAINT, Because it's too frequent from IMN_CHANGECANDIDATE
		imet->candidate.query = (int)lparam;
		/* FALL THRU */
	case IMN_CLOSECANDIDATE:
		InvalidateRect(hwnd, &imet->candidate.rect, 0);
		imet->candidate.bytes = 0; // reset
		break;
	case IMN_OPENCANDIDATE:
	{
		// When the candidate window cannot be turn off
		HIMC imc = ImmGetContext(hwnd);
		CANDIDATEFORM form = {
			.dwIndex = 0,
			.dwStyle = CFS_CANDIDATEPOS,
			.ptCurrentPos = {imet->composition.wcslen * imet->csize.cx, imet->composition.rect.top + 4},
		};
		ImmSetCandidateWindow(imc, &form);
		ImmReleaseContext(hwnd, imc);
		// trace("[%d] IMN_OPENCANDIDATE - x : %d, y : %d\n", COUNT_ID(), form.ptCurrentPos.x, form.ptCurrentPos.y);
	}
		break;
	case IMN_SETCONVERSIONMODE:
	{
		HIMC imc = ImmGetContext(hwnd);
		ImmGetConversionStatus(imc, &imet->conversion, &imet->sentence);
		trace("[%d] IMN_SETCONVERSIONMODE - conversion : 0x%x, sentence : 0x%x\n", COUNT_ID(), imet->conversion, imet->sentence);
		ImmReleaseContext(hwnd, imc);
		break;
	}
	case IMN_SETSENTENCEMODE:
	case IMN_SETOPENSTATUS:
	case IMN_SETCANDIDATEPOS:
	case IMN_SETCOMPOSITIONFONT:
	case IMN_SETCOMPOSITIONWINDOW:
	case IMN_SETSTATUSWINDOWPOS:
	case IMN_GUIDELINE:
	default:
		break;
	}
}

static void ime_paint_composition(struct imetag *imet, HWND hwnd, HDC hdc)
{
	struct composition_tag *compst = &imet->composition;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &compst->rect, gray);
	if (compst->state & GCS_CURSORPOS) {
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
	if (compst->state & GCS_RESULTSTR) {}
	if (compst->wcslen) {
		TextOut(hdc, compst->rect.left, compst->rect.top, (WCHAR *)compst->buff, compst->wcslen);
	}
}

static int read_candidate(struct candidate_tag *candidate, HWND hwnd)
{
	int index = 0;
	_BitScanForward(&index, candidate->query);
	candidate->query = 0; // reset
	HIMC imc = ImmGetContext(hwnd);
	// int bytes = ImmGetCandidateList(imc, index, NULL, 0);
	int bytes = ImmGetCandidateList(imc, index, (CANDIDATELIST *)candidate->buff, sizeof(candidate->buff));
	trace("[%d] buff usage : %.3fKB, total : %dKB\n", COUNT_ID(), bytes / 1024., sizeof(candidate->buff) / 1024);
	ImmReleaseContext(hwnd, imc);
	return bytes;
}
static void ime_paint_candidate(struct imetag *imet, HWND hwnd, HDC hdc)
{
	struct candidate_tag *candidate = &imet->candidate;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &candidate->rect, gray);
	CANDIDATELIST *ptrcd = (CANDIDATELIST *)candidate->buff;
	// if candidate->bytes is not 0 that means recovering from WM_PAINT
	if (candidate->query)
		candidate->bytes = read_candidate(candidate, hwnd); // read data
	if (!candidate->bytes || ptrcd->dwStyle) // TODO: dwStyle
		return;
	// update max pagesize
	if ((int)ptrcd->dwPageSize > candidate->pagesize)
		candidate->pagesize = ptrcd->dwPageSize;
	// paint
	static WCHAR tmp[32];
	int count = 0;
	for (DWORD i = ptrcd->dwPageStart, cy = candidate->rect.top, *offset = ptrcd->dwOffset + ptrcd->dwPageStart;
		i < ptrcd->dwPageStart + ptrcd->dwPageSize && i < ptrcd->dwCount;
		i++, offset++, cy += imet->csize.cy) {
		const WCHAR *hanzi = (WCHAR *)((char *)ptrcd + *offset);
		const WCHAR *format = i != ptrcd->dwSelection ? L"%d: %s" : L"%d: [%s]";
		count = _snwprintf_s(tmp, sizeof(tmp), ARRAYSIZE(tmp), format, i - ptrcd->dwPageStart + 1, hanzi);
		TextOut(hdc, candidate->rect.left, cy, tmp, count);
	}
	int total = (ptrcd->dwCount + candidate->pagesize - 1) / candidate->pagesize;
	int curr = 1 + ptrcd->dwPageStart / candidate->pagesize; // dwPageStart is Zero-based index
	count = _snwprintf_s(tmp, sizeof(tmp), ARRAYSIZE(tmp), L"> %d/%d", curr, total);
	TextOut(hdc, candidate->rect.left, candidate->rect.top + imet->csize.cy * ptrcd->dwPageSize, tmp, count);
}

static void ime_paint_result(struct imetag *imet, HWND hwnd, HDC hdc)
{
	struct result_tag *result = &imet->result;
	HBRUSH gray = GetStockObject(COLOR_SCROLLBAR + 1);
	FillRect(hdc, &result->rect, gray);
	if (result->offset) {
		TextOut(hdc, result->rect.left, result->rect.top, (WCHAR *)result->buff, result->offset);
	}
}

static void ime_paint(struct imetag *imet, HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	ime_paint_composition(imet, hwnd, hdc);
	ime_paint_candidate(imet, hwnd, hdc);
	ime_paint_result(imet, hwnd, hdc);
	EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CREATE:
	{
		HDC hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		GetTextExtentPoint32(hdc, L"高", 1, &imet.csize);
		ReleaseDC(hwnd, hdc);
		if (!imet.ctxprev) {
			HIMC imc = ImmCreateContext();
			imet.ctxprev = ImmAssociateContext(hwnd, imc);
		}
		imet.property = (int)ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);
	}
		break;
	case WM_DESTROY:
	{
		HIMC imc = ImmAssociateContext(hwnd, imet.ctxprev);
		ImmDestroyContext(imc);
		PostQuitMessage(0);
	}
		break;
	case WM_SIZE:
	{
		RECT rect;
		long cy = imet.csize.cy;
		long cx = imet.csize.cx;
		GetClientRect(hwnd, &rect);
		imet.result.rect      = (RECT){0, cy * 0, rect.right, cy * (0 + 1) };
		imet.composition.rect = (RECT){0, cy * 2, rect.right, cy * (2 + 1) };
		imet.candidate.rect   = (RECT){0, cy * 5, cx * 10   , cy * (5 + 10)};
	}
		break;
	case WM_PAINT:
		ime_paint(&imet, hwnd);
		break;
	case WM_IME_SETCONTEXT:
		trace("[%d] WM_IME_SETCONTEXT - state : %s, lparam : 0x%x\n", COUNT_ID(), wparam ? "Active" : "InActive", (int)lparam);
		if (imet.hideui)
			lparam = 0; // disable all for game, but not aways works
		goto defwinproc;
		break;
	case WM_IME_STARTCOMPOSITION:
		// trace("[%d] WM_IME_STARTCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_IME_COMPOSITION:
		// trace("[%d] WM_IME_COMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		ime_handle_composition(&imet, hwnd, wparam, lparam);
		break;
	case WM_IME_ENDCOMPOSITION:
		// trace("[%d] WM_IME_ENDCOMPOSITION - wparam : %d, lparam : 0x%x\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_IME_COMPOSITIONFULL:
		trace("[%d] WM_IME_COMPOSITIONFULL - wparam : %d, lparam : %d\n", COUNT_ID(), (int)wparam, (int)lparam);
		break;
	case WM_INPUTLANGCHANGE:
		imet.property = (int)ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);
		trace("[%d] WM_INPUTLANGCHANGE - wparam : %d, lparam : %d\n", COUNT_ID(), (int)wparam, (int)lparam);
		goto defwinproc;
		break;
	case WM_IME_SELECT:
		trace("[%d] WM_IME_SELECT - wparam : %d, lparam : %d\n", COUNT_ID(), (int)wparam, (int)lparam);
		goto defwinproc;
		break;
	case WM_IME_NOTIFY:
	{
		static char *list[] = {
			"IMN_CLOSESTATUSWINDOW", "IMN_OPENSTATUSWINDOW", "IMN_CHANGECANDIDATE","IMN_CLOSECANDIDATE",
			"IMN_OPENCANDIDATE", "IMN_SETCONVERSIONMODE", "IMN_SETSENTENCEMODE", "IMN_SETOPENSTATUS",
			"IMN_SETCANDIDATEPOS", "IMN_SETCOMPOSITIONFONT", "IMN_SETCOMPOSITIONWINDOW",
			"IMN_SETSTATUSWINDOWPOS", "IMN_GUIDELINE"//, "IMN_PRIVATE"
		};
		char *desc = wparam > 0 && wparam < (ARRAYSIZE(list) - 1) ? list[wparam - 1] : NULL;
		if (desc) {
			// trace("[%d] WM_IME_NOTIFY - wparam : %s , lparam : %d\n", COUNT_ID(), desc, (int)lparam);
		} else {
			// trace("[%d] WM_IME_NOTIFY - wparam : 0x%x , lparam : %d\n", COUNT_ID(), (int)wparam, (int)lparam);
		}
		ime_handle_notify(&imet, hwnd, wparam, lparam);
	}
		break;
	case WM_CHAR:
	{
		struct result_tag *result = &imet.result;
		if (wparam == VK_BACK) {
			if (result->offset)
				result->offset--;
		} else {
			if (result->offset >= (result->rect.right - result->rect.left) / imet.csize.cx)
				result->offset = 0;
			result->buff[result->offset++] = (WCHAR)wparam;
		}
		InvalidateRect(hwnd, &result->rect, 0);
	}
		break;
	case WM_COMMAND:
		trace("%d WM_COMMAND - high : %d, lower : %d, lparam : %d\n", COUNT_ID(), HIWORD(wparam), LOWORD(wparam), lparam);
		break;
	case WM_RBUTTONUP:
	{
		// some testing in here
		HIMC imc = ImmGetContext(hwnd);
		HWND hmm = ImmGetDefaultIMEWnd(hwnd);
		//BOOL ret = ImmSimulateHotKey(hwnd, 0x10);
		WCHAR tmp[128] = {0};
		DWORD conversion, sentence;
		int succ = ImmGetConversionStatus(imc, &conversion, &sentence);
		HDC hdc = GetDC(hwnd);
		int count = swprintf(tmp, 256, L"[%d] - ret : %d, con : 0x%x, sen : 0x%x", COUNT_ID(), succ, conversion, sentence);
		TextOut(hdc, imet.candidate.rect.right, imet.candidate.rect.top, tmp, count);
		ReleaseDC(hwnd, hdc);
		ImmReleaseContext(hwnd, imc);
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
	MyRegisterClass(hInstance);
	HWND hwnd = CreateWindow(appClassName, L"IME 测试", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 500, 390, HWND_DESKTOP, NULL, hInstance, 0);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
