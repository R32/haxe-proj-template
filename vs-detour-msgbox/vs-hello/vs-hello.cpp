// vs-hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//保存函数原型
static int (WINAPI *OldMesssageBoxW)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType) = MessageBoxW;

//改写函数
static int WINAPI NewMessageBoxW(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	return OldMesssageBoxW(NULL, L"new MessageBox", L"Please", MB_OK);

}

void StartHook()
{
	//开始事务
	DetourTransactionBegin();
	//更新线程信息
	DetourUpdateThread(GetCurrentThread());
	//将拦截的函数附加到原函数的地址上
	DetourAttach(&(PVOID&)OldMesssageBoxW, NewMessageBoxW);
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
	DetourDetach(&(PVOID&)OldMesssageBoxW, NewMessageBoxW);
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
	system("pause");
	return 0;
}

