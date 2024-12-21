# DLLinjection_TEST
DLL 인젝션 실습코드

> [!Warning]
> 경고! 본 프로그램은 반드시 학습용도로 사용하십시오.
> 본 코드를 실행하여 발생한 모든 책임은 사용자에게 있습니다.

# 코드설명
이 프로젝트는 Windows에서 특정 프로세스에 DLL을 주입하기 위한 테스트 코드를 포함하고 있습니다. 이 코드는 `notepad.exe` 프로세스를 대상으로 하며, 주입할 DLL의 경로를 지정할 수 있습니다. 


### 헤더 파일
```cpp
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string>
```
필요한 Windows API와 표준 라이브러리를 포함합니다.

### 대상 프로세스 이름
```cpp
#define TARGET_PROCESS_NAME L"notepad.exe"
```
주입할 대상 프로세스의 이름을 정의합니다.

### ANSI 문자열을 Unicode로 변환하는 함수
```cpp
std::wstring ConvertToWide(const std::string& ansiStr) {
    // ...
}
```
ANSI 문자열을 Unicode로 변환하는 함수입니다.

### 대상 프로세스 ID 찾기
```cpp
DWORD FindTargetProcessID(const wchar_t* targetProcessName) {
    // ...
}
```
주어진 프로세스 이름에 해당하는 프로세스 ID를 찾는 함수입니다.

### DLL을 대상 프로세스에 주입
```cpp
BOOL InjectDLL(DWORD processID, const char* dllPath) {
    // ...
}
```
주어진 프로세스 ID에 DLL을 주입하는 함수입니다.

### 메인 함수
```cpp
int main() {
    const char* dllPath = "test.dll"; // 주입할 DLL 경로
    const wchar_t* targetProcessName = TARGET_PROCESS_NAME;

    printf("Finding process ID for %S...\n", targetProcessName);
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
```
주요 실행 흐름을 포함하고 있습니다. 대상 프로세스를 찾고, DLL을 주입하는 과정을 수행합니다.


