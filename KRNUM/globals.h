#include <cstdint>
#pragma once

namespace globals
{
	inline std::uint32_t csgo_process_id = 0;
	inline uintptr_t game_base_address = 0;
	inline uintptr_t client_module_address = 0;
	inline bool cEspBoxes = false;
}

namespace offsets
{
	inline DWORD_PTR dwEntityList = 0x4DDD91C;
	inline DWORD_PTR dwViewMatrix = 0x4DCF234;
	inline DWORD_PTR dwLocalPlayer = 0xDC14CC;
	inline DWORD_PTR m_bDormant = 0xED;
	inline DWORD_PTR m_iHealth = 0x100;
	inline DWORD_PTR m_vecOrigin = 0x138;
	inline DWORD_PTR m_iTeamNum = 0xF4;
}