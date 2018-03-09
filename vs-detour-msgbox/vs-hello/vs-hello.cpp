// vs-hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef int (WINAPI *PFMessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

static PFMessageBoxW OrgMessageBoxW = NULL;

int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
	return ((PFMessageBoxW)OrgMessageBoxW)(hWnd, L"Hook This!", L"My hook", uType);
}

void StartHook()
{
	//开始事务
	DetourTransactionBegin();
	//更新线程信息
	DetourUpdateThread(GetCurrentThread());
	//DetourFindFunction
	OrgMessageBoxW = (PFMessageBoxW)DetourFindFunction("User32.dll", "MessageBoxW");
	//将拦截的函数附加到原函数的地址上
	DetourAttach(&(PVOID&)OrgMessageBoxW, MyMessageBoxW);
	//结束事务
	DetourTransactionCommit();
}

//解除钩子
void EndHook()
{
	//开始事务
	DetourTransactionBegin();
	//更新线程信息
	DetourUpdateThread(GetCurrentThread());
	//将拦截的函数从原函数的地址上解除
	DetourDetach(&(PVOID&)OrgMessageBoxW, MyMessageBoxW);
	//结束事务
	DetourTransactionCommit();
}

int _tmain(int argc, _TCHAR* argv[]) {
	//应改变输出
	StartHook();
	MessageBoxW(NULL, L"old MessageBox", L"Please", MB_OK);
	//应原样输出
	EndHook();
	MessageBoxW(NULL, L"old MessageBox", L"Please", MB_OK);
	return 0;
}

