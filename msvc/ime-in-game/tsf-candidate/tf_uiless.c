#include "stdafx.h"
#include "tf_uiless.h"

// persistent
ULONG STDMETHODCALLTYPE ulpersist_addref_release(void *base)
{
	return 1;
}

/*
 * external vtbl
 */
extern struct ITfUIElementSinkVtbl elementsink_vtbl; // tf_elementsink.c
extern struct ITfLangBarEventSinkVtbl langbarevent_vtbl; // tf_langbareventsink.c

HRESULT uiless_initialize(struct tf_uiless *uiless, HWND hwnd)
{
	uiless->enable = 1;
	uiless->hwnd = hwnd;
	// itf_
	uiless->itfmgr = NULL;
	uiless->itfuimgr = NULL;
	uiless->itflangmgr = NULL;
	uiless->itfcandidate = NULL;
	// _sink
	uiless->element_sink.lpVtbl = &elementsink_vtbl;
	uiless->langevent_sink.lpVtbl = &langbarevent_vtbl;
	uiless->cookie.element = TF_INVALID_COOKIE;
	uiless->cookie.langevent = TF_INVALID_COOKIE;

	ITfSource *itfsource = NULL;
	ITfThreadMgrEx *itfmgr = NULL;
	HRESULT hr = CoCreateInstance(&CLSID_TF_ThreadMgr, NULL, CLSCTX_INPROC_SERVER, &IID_ITfThreadMgrEx, &itfmgr);
	if (FAILED(hr)) {
		goto cleanup;
	}

	TfClientId cid; // not used
	hr = itfmgr->lpVtbl->ActivateEx(itfmgr, &cid, TF_TMAE_UIELEMENTENABLEDONLY);
	if (FAILED(hr)) {
		goto cleanup;
	}

	hr = itfmgr->lpVtbl->QueryInterface(itfmgr, &IID_ITfSource, &itfsource);
	if (FAILED(hr)) {
		goto cleanup;
	}

	hr = itfsource->lpVtbl->AdviseSink(itfsource, &IID_ITfUIElementSink, (IUnknown *)&uiless->element_sink, &uiless->cookie.element);
	if (FAILED(hr)) {
		goto cleanup;
	}

	// ITfUIElementMgr
	hr = itfmgr->lpVtbl->QueryInterface(itfmgr, &IID_ITfUIElementMgr, &uiless->itfuimgr);
	if (FAILED(hr)) {
		goto cleanup;
	}

	uiless->itfmgr = itfmgr;
	itfmgr = NULL; // prevents cleanup
	trace("done!\n");
cleanup:
	SAFERELEASE(itfsource);
	SAFERELEASE(itfmgr);
	return hr;
}

HRESULT uiless_release(struct tf_uiless *uiless)
{
	ITfSource *tfsource = NULL;
	ITfThreadMgrEx *tfmgr = uiless->itfmgr;

	HRESULT hr = tfmgr->lpVtbl->QueryInterface(tfmgr, &IID_ITfSource, &tfsource);
	if (SUCCEEDED(hr) && uiless->cookie.element != TF_INVALID_COOKIE) {
		tfsource->lpVtbl->UnadviseSink(tfsource, uiless->cookie.element);
	}
	if (uiless->cookie.langevent != TF_INVALID_COOKIE) {
		uiless->itflangmgr->lpVtbl->UnadviseEventSink(uiless->itflangmgr, uiless->cookie.langevent);
	}
	SAFERELEASE(tfsource);
	SAFERELEASE(uiless->itfcandidate);
	SAFERELEASE(uiless->itflangmgr);
	SAFERELEASE(uiless->itfuimgr);
	SAFERELEASE(uiless->itfmgr);
	return S_OK;
}


void ulflush_candidate(ITfCandidateListUIElement *itform, struct dt_candidate *data)
{
	if (!data->query || !itform)
		return;
	data->query = 0;
	int pagecnt = 0;
	int *offset = data->offset;

	itform->lpVtbl->GetCount(itform, &data->count);
	itform->lpVtbl->GetCurrentPage(itform, &data->pagecurrent);
	itform->lpVtbl->GetSelection(itform, &data->selection);
	itform->lpVtbl->GetPageIndex(itform, data->lists, ARRAYSIZE(data->lists), &pagecnt);

	offset[0] = pagecnt * (sizeof(data->lists[0]) / sizeof(data->buff[0]));

	if (data->count && data->pagecurrent < pagecnt) {
		int *pagelist = data->lists;
		data->pagestart = pagelist[data->pagecurrent];
		data->pagestop = data->pagecurrent < pagecnt - 1
			? min(data->count, pagelist[data->pagecurrent + 1])
			: data->count;
	} else {
		data->pagestart = data->selection;
		data->pagestop = min(data->selection + 9, data->count);
	}
	// trace("selection : %d, count : %d, pcur : %d, pstart : %d, pstop : %d\n", data->selection, data->count, data->pagecurrent, data->pagestart, data->pagestop);
	// copy characters 1~9 to buff[offset]
	BSTR bstr;
	WCHAR *src;
	WCHAR *dst = data->buff + offset[0];
	for (int i = data->pagestart, j = 0; i < data->pagestop; i++, j++) {
		if (FAILED(itform->lpVtbl->GetString(itform, i, &bstr)) || !bstr) {
			return;
		}
		src = bstr;
		while (*dst++ = *src++){
		}
		SysFreeString(bstr);
		offset[j + 1] = (int)(size_t)(dst - data->buff);
	}
	// trace("start : %d, stop : %d, count : %d, page current : %d, page count : %d, selection : %d\n", data->pagestart, data->pagestop, data->count, data->pagecurrent, pagecnt, data->selection);
	// trace("candidate buffer usage : %.2fKB, total : %dKB\n", (int)(size_t)(dst - data->buff) * sizeof(data->buff[0]) / 1024., (int)sizeof(data->buff) / 1024);
}

static int ulflush_compositioninner(HIMC imc, DWORD kind, WCHAR *buf, int count)
{
	DWORD bytes = ImmGetCompositionString(imc, kind, buf, count);
	DWORD len = (bytes + (sizeof(WCHAR) - 1)) / sizeof(WCHAR);
	buf[len] = 0;
	return len;
}

void ulflush_composition(struct tf_uiless *uiless, HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct dt_composition *compst = &uiless->composition;
	HIMC imc = ImmGetContext(hwnd);
	if (lparam & GCS_CURSORPOS) {
		// GCS_DELTASTART is the caret offset start of the last change
			// GCS_CURSORPOS is the caret offset of current
		compst->cstart = lparam & GCS_DELTASTART ? ImmGetCompositionString(imc, GCS_DELTASTART, NULL, 0) : -1;
		compst->cursor = ImmGetCompositionString(imc, GCS_CURSORPOS, NULL, 0);
	}
	if (lparam & GCS_RESULTSTR) {
		struct dt_result *result = &uiless->result;
		if (result->offset >= (result->rect.right - result->rect.left) / uiless->csize.cx)
			result->offset = 0;
		result->offset += ulflush_compositioninner(imc, GCS_RESULTSTR, result->buff + result->offset, (int)sizeof(result->buff) - result->offset * sizeof(WCHAR));
		InvalidateRect(hwnd, &uiless->result.rect, 0);
		compst->wcslen = 0;
	}
	if (lparam & GCS_COMPSTR) {
		compst->wcslen = ulflush_compositioninner(imc, GCS_COMPSTR, compst->buff, sizeof(compst->buff));
	}
	ImmReleaseContext(hwnd, imc);
	InvalidateRect(hwnd, &compst->rect, 0);
	compst->flags = (int)lparam;
}
