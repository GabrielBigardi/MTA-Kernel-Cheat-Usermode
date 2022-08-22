#include <cstdint>
#pragma once

namespace globals
{
	inline std::uint32_t gtasa_process_id = 0;
	inline std::uint32_t mta_process_id = 0;

	inline bool cEspBoxes = false;
	inline bool cEspObjects = false;
	inline bool cEspVehicles = false;
	inline bool cTest = false;
}

namespace offsets
{
	inline DWORD_PTR dwPlayerPointer = 0xB6F5F0;
	inline DWORD_PTR dwEntityList = 0xB7CD98;
	inline DWORD_PTR dwCPedSize = 0x7C4;

	inline DWORD_PTR m_CurrentHealth = 0x540;
	inline DWORD_PTR m_MaxHealth = 0x544;
}