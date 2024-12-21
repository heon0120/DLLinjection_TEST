#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string>

// 대상 프로세스 이름
#define TARGET_PROCESS_NAME L"notepad.exe" // 유니코드 문자열로 수정

// ANSI 문자열을 Unicode로 변환하는 함수
std::wstring ConvertToWide(const std::string& ansiStr) {
    int size_needed = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, NULL, 0);
    std::wstring wideStr(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, &wideStr[0], size_needed);
    return wideStr;
}

// 대상 프로세스 ID 찾기
DWORD FindTargetProcessID(const wchar_t* targetProcessName) { // 매개변수 타입 변경
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to take process snapshot.\n");
        return 0;
    }

    PROCESSENTRY32W pe32; // UNICODE 호환 구조체 사용
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (wcscmp(pe32.szExeFile, targetProcessName) == 0) { // wcscmp로 문자열 비교
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// DLL을 대상 프로세스에 주입
BOOL InjectDLL(DWORD processID, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        printf("Failed to open target process.\n");
        return FALSE;
    }

    size_t dllPathSize = (strlen(dllPath) + 1) * sizeof(char);
    void* pRemoteMemory = VirtualAllocEx(hProcess, NULL, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pRemoteMemory == NULL) {
        printf("Failed to allocate memory in target process.\n");
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath, dllPathSize, NULL)) {
        printf("Failed to write DLL path to target process memory.\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!loadLibraryAddr) {
        printf("Failed to find LoadLibraryA function.\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, loadLibraryAddr, pRemoteMemory, 0, NULL);
    if (hRemoteThread == NULL) {
        printf("Failed to create remote thread in target process.\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);
    CloseHandle(hRemoteThread);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    printf("DLL Injection successful.\n");
    return TRUE;
}

int main() {
    const char* dllPath = "test.dll"; // 주입할 DLL 경로
    const wchar_t* targetProcessName = TARGET_PROCESS_NAME; // UNICODE 문자열 사용

    printf("Finding process ID for %S...\n", targetProcessName); // %S는 UNICODE 출력에 사용
    DWORD processID = FindTargetProcessID(targetProcessName);

    if (processID == 0) {
        printf("Target process not found.\n");
        return 1;
    }

    printf("Target process ID: %d\n", processID);
    printf("Injecting DLL...\n");

    if (InjectDLL(processID, dllPath)) {
        printf("DLL Injection completed.\n");
    }
    else {
        printf("DLL Injection failed.\n");
    }

    return 0;
}
