#include "stdafx.h"
#include "tf_uiless.h"


static HRESULT update_itfcandidate(struct tf_uiless *uiless, DWORD elemid)
{
	SAFERELEASE(uiless->itfcandidate);
	if (!uiless->itfuimgr) {
		return E_FAIL;
	}

	ITfUIElement *itfelement = NULL;
	HRESULT hr = uiless->itfuimgr->lpVtbl->GetUIElement(uiless->itfuimgr, elemid, &itfelement);
	if (FAILED(hr)) {
		goto cleanup;
	}

	hr = itfelement->lpVtbl->QueryInterface(itfelement, &IID_ITfCandidateListUIElement, &uiless->itfcandidate);
	if (FAILED(hr)) {
		goto cleanup;
	}

cleanup:
	SAFERELEASE(itfelement);
	return hr;
}

static HRESULT STDMETHODCALLTYPE QueryInterface(ITfUIElementSink *sink, REFIID riid, void **ppvobj)
{
	if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_ITfUIElementSink)) {
		if (!ppvobj)
			return E_POINTER;
		*ppvobj = sink;
		sink->lpVtbl->AddRef(sink);
		return S_OK;
	}
	*ppvobj = NULL;
	return E_NOINTERFACE;
}

static HRESULT STDMETHODCALLTYPE BeginUIElement(ITfUIElementSink *sink, DWORD elemid, BOOL *show)
{
	struct tf_uiless *uiless = container_of(sink, struct tf_uiless, element_sink);
	if (!uiless->enable || FAILED(update_itfcandidate(uiless, elemid))) {
		*show = 1;
	}
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE UpdateUIElement(ITfUIElementSink *sink, DWORD elemid)
{
	struct tf_uiless *uiless = container_of(sink, struct tf_uiless, element_sink);
	// Defer data reading in next frame/WM_PAINT, otherwise you might get NULL in first letter
	uiless->candidate.query = 1;
	InvalidateRect(uiless->hwnd, &uiless->candidate.rect, 0);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE EndUIElement(ITfUIElementSink *sink, DWORD elemid)
{
	struct tf_uiless *uiless = container_of(sink, struct tf_uiless, element_sink);
	if (!uiless->candidate.count)
		return S_OK;	
	uiless->candidate.count = 0;
	uiless->candidate.query = 0;
	InvalidateRect(uiless->hwnd, &uiless->candidate.rect, 0);
	SAFERELEASE(uiless->itfcandidate);
	return S_OK;
}

struct ITfUIElementSinkVtbl elementsink_vtbl = {
	QueryInterface,
	ulpersist_addref_release,
	ulpersist_addref_release,
	BeginUIElement,
	UpdateUIElement,
	EndUIElement,
};
