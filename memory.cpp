#include "memory.h"

// Patches a sequence of bytes in memory with new data
void Memory::PatchBytes(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldMemoryProtection;

    // Change memory protection of the destination to allow write operations
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldMemoryProtection);

    // Copy the bytes from source to destination
    memcpy(dst, src, size);

    // Restore the original memory protection settings of the destination
    VirtualProtect(dst, size, oldMemoryProtection, &oldMemoryProtection);
}

void Memory::NopBytes(BYTE* dst, unsigned int size) {
	DWORD oldMemoryProtection;

    // Change memory protection of the destination to allow write operations
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldMemoryProtection);

    // Fill the destination with the x86 NOP instructions (0x90)
    memset(dst, 0x90, size);

    // Restore the original memory protection settings of the destination
    VirtualProtect(dst, size, oldMemoryProtection, &oldMemoryProtection);
}

// Calculates a dynamic memory address by navigating through a series of offsets
std::uintptr_t Memory::GetDynamicAddress(std::uintptr_t baseAddress, std::vector<unsigned int> offsets)
{
    // Initialize the address with the base addres
    std::uintptr_t address = baseAddress;

    for (unsigned int i = 0; i < offsets.size(); ++i) {
        address = *reinterpret_cast<std::uintptr_t*>(address);
        address = address + offsets[i];
    }

    return address;
}


