#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <vector>

#include "definitions.h"
#include "globals.h"
#include "gtautils.h"
#include "hook.h"
#include "math.h"
#include "memory.h"
#include "draw.h"

uintptr_t base_address = 0;

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

static std::uint32_t get_process_id(std::string_view process_name)
{
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return NULL;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE)
	{
		if (process_name.compare(processentry.szExeFile) == NULL)
		{
			return processentry.th32ProcessID;
		}
	}
	return NULL;
}

static ULONG64 get_module_base_address(const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = globals::process_id;
	instructions.req_base = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.draw_box = FALSE;
	instructions.module_name = module_name;
	call_hook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}

int main()
{
	std::cout << "Trying to locate process..." << std::endl;
	globals::process_id = get_process_id("gta_sa.exe");

	if (globals::process_id == 0) {
		std::cout << RED << "\nERROR: Process not found!" << std::endl;
		Sleep(5000);
		return NULL;
	}

	std::cout << "Trying to locate base module..." << std::endl;
	base_address = get_module_base_address("gta_sa.exe");

	if (!base_address) {
		std::cout << RED << "\nERROR: Base module not found!" << std::endl;
		printf("");
		Sleep(5000);
		return NULL;
	}

	std::cout << "Starting in 3s...\n\n" << std::endl;
	Sleep(3000);

	bool F2 = false;
	bool F3 = false;
	bool F4 = false;

	while (true)
	{
		// ==== Dynamic ESP ====
		if (GetAsyncKeyState(VK_F2) < 0 && F2 == false)
		{
			globals::cEspBoxes = !globals::cEspBoxes;
			if (globals::cEspBoxesFixedSize) globals::cEspBoxesFixedSize = false;
			globals::cEspBoxes ? std::cout << "Dynamic ESP Boxes: " << GREEN << "[ON]" << RESET << std::endl : std::cout << "ESP Boxes: " << RED << "[OFF]" << RESET << std::endl;
			F2 = true;
		}
		if (GetAsyncKeyState(VK_F2) == 0 && F2 == true) F2 = false;


		if (globals::cEspBoxes)
		{
			DWORD_PTR LocalPlayer = Read<DWORD_PTR>(offsets::dwPlayerPointer);
			DWORD_PTR EntityList = Read<DWORD_PTR>(offsets::dwEntityList);

			DWORD_PTR pPosStructAddress = (DWORD)LocalPlayer + 0x14;
			DWORD pPosStruct = Read<DWORD>(pPosStructAddress);
			CVector pPlayerPos = Read<CVector>(pPosStruct + 0x30);

			for (int i = 0; i < 140; i++)
			{
				DWORD_PTR CurrentPed = (DWORD)EntityList + (offsets::dwCPedSize * i);
				float CurrentPedHealth = Read<float>(CurrentPed + offsets::m_CurrentHealth);

				if (CurrentPedHealth >= 1.0f && CurrentPed != LocalPlayer)
				{
					DWORD_PTR PosStructAddress = (DWORD)CurrentPed + 0x14;
					DWORD PosStruct = Read<DWORD>(PosStructAddress);
					CVector PlayerPos = Read<CVector>(PosStruct + 0x30);

					if (PlayerPos.fX != 0.0f && PlayerPos.fX != 0.0f)
					{
						Vector2 FeetSP;
						Vector2 HeadSP;

						CVector PlayerFeetPos = PlayerPos - CVector(0.f, 0.f, 1.f);
						CVector PlayerHeadPos = PlayerPos + CVector(0.f, 0.f, 0.775f);

						Vector2 screen;

						if (WorldToScreen(PlayerFeetPos, FeetSP) && WorldToScreen(PlayerHeadPos, HeadSP))
						{
							float BoxSY = FeetSP.y - HeadSP.y;
							float BoxSX = BoxSY / 2.25f;
							float BoxX = HeadSP.x - BoxSX / 2;
							float BoxY = HeadSP.y;

							draw::box(HeadSP.x - BoxSX / 2 - 1, HeadSP.y - 1, BoxSX + 2, BoxSY + 2, 3, 0, 0, 175);
							draw::box(HeadSP.x - BoxSX / 2, HeadSP.y, BoxSX, BoxSY, 1, 255, 255, 255);
						}
					}
				}
			}
		}
	}
}