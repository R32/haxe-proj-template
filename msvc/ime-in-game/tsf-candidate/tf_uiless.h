#pragma once
#include "stdafx.h"

// AddRef or Release
ULONG STDMETHODCALLTYPE ulpersist_addref_release(void *base);

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
	DWORD enable;
	SIZE csize;
	struct {
		DWORD element;
		DWORD langevent;            // deprecated
	} cookie;
	// itf_
	ITfThreadMgrEx *itfmgr;
	ITfUIElementMgr *itfuimgr;
	ITfLangBarMgr *itflangmgr;          // deprecated
	ITfCandidateListUIElement *itfcandidate;
	// _sink
	ITfUIElementSink element_sink;      // tf_elementsink.c
	ITfLangBarEventSink langevent_sink; // deprecated tf_langbareventsink.c
	struct dt_result result;
	struct dt_candidate candidate;
	struct dt_composition composition;
};

HRESULT uiless_initialize(struct tf_uiless *uiless, HWND hwnd);
HRESULT uiless_release(struct tf_uiless *uiless);

void ulflush_candidate(ITfCandidateListUIElement *itform, struct dt_candidate *data);
void ulflush_composition(struct tf_uiless *uiless, HWND hwnd, WPARAM wparam, LPARAM lparam);

void ullang_enable(struct tf_uiless *uiless); // deprecated
HRESULT ullang_englishkbl(struct tf_uiless *uiless, DWORD threadid); // deprecated
