#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>

#include "process.h"

// Function to inject a DLL into a process
bool InjectDLL(std::string_view dllPath, DWORD processId) {
    // Open the target process with all access rights
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
    if (!processHandle || processHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Allocate memory in the target process for the DLL path
    void* location = VirtualAllocEx(processHandle, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!location) {
        CloseHandle(processHandle);
        return false;
    }

    // Write the DLL path into the allocated memory
    if (!WriteProcessMemory(processHandle, location, std::string(dllPath).c_str(), dllPath.size() + 1, nullptr)) {
        VirtualFreeEx(processHandle, location, 0, MEM_RELEASE); // Free allocated memory
        CloseHandle(processHandle); // Close process handle
        return false;
    }

    // Create a remote thread in the target process to load the DLL
    HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0, nullptr);
    if (!threadHandle) {
        VirtualFreeEx(processHandle, location, 0, MEM_RELEASE); // Free allocated memory
        CloseHandle(processHandle); // Close process handle
        return false;
    }

    // Close the handles for the thread and the process
    CloseHandle(threadHandle);
    CloseHandle(processHandle);
    return true; // Return true if injection was successful
}

int main() {
    // Specify the path to the DLL and the name of the target process
    std::string dllPath = "C:\\Path\\To\\Your\\DLL.dll";
    std::wstring processName = L"target_process.exe";

    // Loop until the process ID is found
    DWORD processId = 0;
    while (!processId) {
        processId = GetProcessId(processName);
        Sleep(30); // Sleep to prevent high CPU usage
    }

    // Attempt to inject the DLL and report success or failure
    if (InjectDLL(dllPath, processId)) {
        std::cout << "DLL injected successfully." << std::endl;
    }
    else {
        std::cerr << "DLL injection failed." << std::endl;
    }

    return 0;
}