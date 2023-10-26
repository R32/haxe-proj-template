#include "stdafx.h"
#include "tf_uiless.h"

// persistent
ULONG STDMETHODCALLTYPE uiless_persist_addref(void *base)
{
	return 1;
}
ULONG STDMETHODCALLTYPE uiless_persist_release(void *base)
{
	return 1;
}

/*
 * external vtbl
 */
extern struct ITfUIElementSinkVtbl elementsink_vtbl; // tf_elementsink.c

HRESULT uiless_initialize(struct tf_uiless *uiless, HWND hwnd)
{
	uiless->enable = 1;
	uiless->hwnd = hwnd;
	// itf_
	uiless->itfmgr = NULL;
	uiless->itfuimgr = NULL;
	uiless->itfcandidate = NULL;
	// _sink
	uiless->element_sink.lpVtbl = &elementsink_vtbl;
	uiless->cookie.element = TF_INVALID_COOKIE;

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
	if (SUCCEEDED(hr)) {
		tfsource->lpVtbl->UnadviseSink(tfsource, uiless->cookie.element);
	}
	SAFERELEASE(tfsource);
	SAFERELEASE(uiless->itfcandidate);
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
			: data->count - data->pagestart;
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
		if (FAILED(itform->lpVtbl->GetString(itform, i, &bstr)) || !bstr)
			return;
		src = bstr;
		while (*dst++ = *src++){
		}
		SysFreeString(bstr);
		offset[j + 1] = (int)(size_t)(dst - data->buff);
	}
	// trace("candidate buffer usage : %.2fKB, total : %dKB\n", (int)(size_t)(dst - data->buff) * sizeof(data->buff[0]) / 1024., sizeof(data->buff) / 1024);
}
