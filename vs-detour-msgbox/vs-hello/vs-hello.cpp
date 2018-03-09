// vs-hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//���溯��ԭ��
static int (WINAPI *OldMesssageBoxW)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType) = MessageBoxW;

//��д����
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
	//��ʼ����
	DetourTransactionBegin();
	//�����߳���Ϣ
	DetourUpdateThread(GetCurrentThread());
	//�����صĺ������ӵ�ԭ�����ĵ�ַ��
	DetourAttach(&(PVOID&)OldMesssageBoxW, NewMessageBoxW);
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
	DetourDetach(&(PVOID&)OldMesssageBoxW, NewMessageBoxW);
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
	system("pause");
	return 0;
}

