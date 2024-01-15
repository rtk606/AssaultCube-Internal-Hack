#pragma once

#include <vector>
#include <string>

#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcessId(const std::wstring_view processName);
std::uintptr_t GetModuleBaseAddress(DWORD processId, const std::wstring_view moduleName);


