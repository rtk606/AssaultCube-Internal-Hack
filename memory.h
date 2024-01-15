#pragma once

#include <vector>
#include <windows.h>

namespace Memory {
	// These functions modify a sequence of bytes in memory
	void PatchBytes(BYTE* dst, BYTE* src, unsigned int size);
	void NopBytes(BYTE* dst, unsigned int size);

	// Needed to calculate dynamic memory addrerss based on the base address and offsets
	std::uintptr_t GetDynamicAddress(std::uintptr_t dynamicBaseAddress, std::vector<unsigned int> offsets);
}