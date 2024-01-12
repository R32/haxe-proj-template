#include "stdafx.h"
#include "tf_uiless.h"

// NOTE: It doesn't work in win10+

static HRESULT STDMETHODCALLTYPE QueryInterface(ITfLangBarEventSink * sink, REFIID riid, void **ppvobj)
{
	if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_ITfLangBarEventSink)) {
		if (!ppvobj)
			return E_POINTER;
		*ppvobj = sink;
		sink->lpVtbl->AddRef(sink);
		return S_OK;
	}
	*ppvobj = NULL;
	return E_NOINTERFACE;
}

static HRESULT STDMETHODCALLTYPE OnSetFocus(ITfLangBarEventSink *sink, DWORD dwThreadId)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE OnThreadTerminate(ITfLangBarEventSink *sink, DWORD dwThreadId)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE OnThreadItemChange(ITfLangBarEventSink *sink, DWORD dwThreadId)
{
	struct tf_uiless *uiless = container_of(sink, struct tf_uiless, langevent_sink);
	ITfInputProcessorProfiles *profile = NULL;

	HRESULT hr = uiless->itflangmgr->lpVtbl->GetInputProcessorProfiles(uiless->itflangmgr, dwThreadId, &profile, &dwThreadId);
	if (FAILED(hr)) {
		goto cleanup;
	}
	LANGID langid = 0;
	hr = profile->lpVtbl->GetCurrentLanguage(profile, &langid);
	if (FAILED(hr)) {
		goto cleanup;
	}

	CLSID clsid;
	GUID guid;
	hr = profile->lpVtbl->GetDefaultLanguageProfile(profile, langid, &GUID_TFCAT_TIP_KEYBOARD, &clsid, &guid);
	if (FAILED(hr)) {
		goto cleanup;
	}

	BSTR bstr = NULL;
	hr = profile->lpVtbl->GetLanguageProfileDescription(profile, &clsid, langid, &guid, &bstr);
	trace("OnThreadItemChange langid : %d, hr : %d, desc : %ls\n", langid, hr, bstr ? bstr : L"EMPTY");
	//
	HDC hdc = GetDC(uiless->hwnd);
	RECT rect = {
		uiless->candidate.rect.right + 4,
		uiless->candidate.rect.top,
		uiless->composition.rect.right,
		uiless->candidate.rect.top + uiless->csize.cy,
	};
	FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOWFRAME + 1));

	if (FAILED(hr)) {
		goto cleanup;
	}
	TextOut(hdc, rect.left, rect.top, bstr, lstrlen(bstr));
	SysFreeString(bstr);
	ReleaseDC(uiless->hwnd, hdc);
cleanup:
	SAFERELEASE(profile);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE OnModalInput(ITfLangBarEventSink *sink, DWORD dwThreadId, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE ShowFloating(ITfLangBarEventSink *sink, DWORD dwFlags)
{
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE GetItemFloatingRect(ITfLangBarEventSink *sink, DWORD dwThreadId, REFGUID rguid, RECT *prc)
{
	return S_OK;
}

struct ITfLangBarEventSinkVtbl langbarevent_vtbl = {
	QueryInterface,
	ulpersist_addref_release,
	ulpersist_addref_release,
	OnSetFocus,
	OnThreadTerminate,
	OnThreadItemChange,
	OnModalInput,
	ShowFloating,
	GetItemFloatingRect,
};


void ullang_enable(struct tf_uiless *uiless)
{
	if (uiless->itflangmgr)
		return;
	HRESULT hr = CoCreateInstance(&CLSID_TF_LangBarMgr, NULL, CLSCTX_INPROC_SERVER, &IID_ITfLangBarMgr, &uiless->itflangmgr);
	if (SUCCEEDED(hr)) {
		hr = uiless->itflangmgr->lpVtbl->AdviseEventSink(uiless->itflangmgr, &uiless->langevent_sink, uiless->hwnd, 0, &uiless->cookie.langevent);
	}
	if (FAILED(hr)) {
		SAFERELEASE(uiless->itflangmgr);
	}
}

HRESULT ullang_englishkbl(struct tf_uiless *uiless, DWORD threadid)
{
	if (!uiless->itflangmgr)
		return E_FAIL;

	ITfInputProcessorProfiles *profile = NULL;
	HRESULT hr = uiless->itflangmgr->lpVtbl->GetInputProcessorProfiles(uiless->itflangmgr, threadid, &profile, &threadid);
	if (FAILED(hr)) {
		goto cleanup;
	}

	LANGID langid;
	hr = profile->lpVtbl->GetCurrentLanguage(profile, &langid);
	if (FAILED(hr)) {
		goto cleanup;
	}

	hr = profile->lpVtbl->ActivateLanguageProfile(profile, &CLSID_NULL, langid, &CLSID_NULL);

cleanup:
	SAFERELEASE(profile);
	return hr;
}
