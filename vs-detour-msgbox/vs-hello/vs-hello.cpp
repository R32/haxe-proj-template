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
	//��ʼ����
	DetourTransactionBegin();
	//�����߳���Ϣ
	DetourUpdateThread(GetCurrentThread());
	//DetourFindFunction
	OrgMessageBoxW = (PFMessageBoxW)DetourFindFunction("User32.dll", "MessageBoxW");
	//�����صĺ������ӵ�ԭ�����ĵ�ַ��
	DetourAttach(&(PVOID&)OrgMessageBoxW, MyMessageBoxW);
	//��������
	DetourTransactionCommit();
}

//�������
void EndHook()
{
	//��ʼ����
	DetourTransactionBegin();
	//�����߳���Ϣ
	DetourUpdateThread(GetCurrentThread());
	//�����صĺ�����ԭ�����ĵ�ַ�Ͻ��
	DetourDetach(&(PVOID&)OrgMessageBoxW, MyMessageBoxW);
	//��������
	DetourTransactionCommit();
}

int _tmain(int argc, _TCHAR* argv[]) {
	//Ӧ�ı����
	StartHook();
	MessageBoxW(NULL, L"old MessageBox", L"Please", MB_OK);
	//Ӧԭ�����
	EndHook();
	MessageBoxW(NULL, L"old MessageBox", L"Please", MB_OK);
	return 0;
}

