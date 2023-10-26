#pragma once
#include "stdafx.h"

ULONG STDMETHODCALLTYPE uiless_persist_addref(void *base);
ULONG STDMETHODCALLTYPE uiless_persist_release(void *base);

#define SAFERELEASE(ptr)    do { if (ptr) { (ptr)->lpVtbl->Release(ptr); (ptr) = NULL; } } while(0)

struct dt_result {
	RECT rect;
	int offset;
	int reserved;
	WCHAR buff[512];
};
struct dt_composition {
	RECT rect;
	int flags;
	int cstart;
	int cursor;
	int wcslen;
	WCHAR buff[512];
};
struct dt_candidate {
	RECT rect;
	int query;
	int count;       // GetCount()
	int selection;   // GetSelection()
	int pagecurrent; // GetCurrentPage()
	int pagestart;
	int pagestop;
	int offset[18];  // offset in WCHAR
	union {
		WCHAR buff[8 * 1024  / sizeof(WCHAR)]; // 8KB
		int lists[8 * 1024  / sizeof(int)];
	};
};
struct tf_uiless {
	HWND hwnd;
	ITfThreadMgrEx *itfmgr;
	ITfUIElementMgr *itfuimgr;
	ITfCandidateListUIElement *itfcandidate;
	ITfUIElementSink element_sink; // tf_elementsink.c
	struct {
		DWORD element;
	} cookie;
	DWORD enable;
	SIZE csize;
	struct dt_result result;
	struct dt_candidate candidate;
	struct dt_composition composition;
};

HRESULT uiless_initialize(struct tf_uiless *uiless, HWND hwnd);
HRESULT uiless_release(struct tf_uiless *uiless);

void ulflush_candidate(ITfCandidateListUIElement *itform, struct dt_candidate *data);
