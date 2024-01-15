#include "process.h"

// Iterates over the running processes until we find 'ac_client.exe' and then returns the process ID
DWORD GetProcessId(std::wstring_view processName) {
	DWORD processId = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 processEntry;
		processEntry.dwSize = sizeof(processEntry);

		if (Process32First(snapshot, &processEntry)) {
			do {
				if (std::wstring_view(processEntry.szExeFile) == processName) {
					processId = processEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &processEntry));
		}
	}

	CloseHandle(snapshot);
	return processId;
}

// Iterates over the modules of a given process and returns the module base address (using the specified process ID)
std::uintptr_t GetModuleBaseAddress(DWORD processId, const std::wstring_view moduleName) {
	std::uintptr_t moduleBaseAddress = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	if (snapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(moduleEntry);

		if (Module32First(snapshot, &moduleEntry)) {
			do {
				if (std::wstring_view(moduleEntry.szModule) == moduleName) {
					moduleBaseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
					break;
				}
			} while (Module32Next(snapshot, &moduleEntry));
		}
	}

	CloseHandle(snapshot);
	return moduleBaseAddress;
}

/*										*
	Code above is for use w/ external 
* 										*/
