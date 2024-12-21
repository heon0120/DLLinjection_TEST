// TEST.dll 소스코드
#include "pch.h"
#include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        MessageBox(NULL, L"Injection Successful!", L"Test DLL", MB_OK);
    }
    return TRUE;
}

