
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

bool draw_box(int x, int y, int w, int h, int t, int r, int g, int b)
{
	NULL_MEMORY instructions;
	instructions.write = FALSE;
	instructions.read = FALSE;
	instructions.req_base = FALSE;
	instructions.draw_box = TRUE;

	instructions.x = x;
	instructions.y = y;
	instructions.w = w;
	instructions.h = h;
	instructions.t = t;

	instructions.r = r;
	instructions.g = g;
	instructions.b = b;


	call_hook(&instructions);

	return true;
}

int main()
{
	printf("Starting");
	printf("\n\nTrying to locate proccess...");
	globals::process_id = get_process_id("gta_sa.exe");

	if (globals::process_id == 0) {
		printf("\nERROR: Process not found!");
		Sleep(5000);
		return NULL;
	}

	printf("\nTrying to locate base module...");
	base_address = get_module_base_address("gta_sa.exe");

	if (!base_address) {
		printf("\nERROR: Base module not found!");
		Sleep(5000);
		return NULL;
	}

	printf("\n\nStarting in 3s...\n");
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
			globals::cEspBoxes ? printf("\nDYNAMIC ESP LIGADO.") : printf("\nDYNAMIC ESP DESLIGADO.");
			F2 = true;
		}
		if (GetAsyncKeyState(VK_F2) == 0 && F2 == true) F2 = false;

		// ==== Fixed Size ESP ====
		if (GetAsyncKeyState(VK_F3) < 0 && F3 == false)
		{
			globals::cEspBoxesFixedSize = !globals::cEspBoxesFixedSize;
			if (globals::cEspBoxes) globals::cEspBoxes = false;
			globals::cEspBoxesFixedSize ? printf("\nFIXED ESP LIGADO.") : printf("\nFIXED ESP DESLIGADO.");
			F3 = true;
		}
		if (GetAsyncKeyState(VK_F3) == 0 && F3 == true) F3 = false;

		// ==== Fixed Size ESP ====
		if (GetAsyncKeyState(VK_F4) < 0 && F4 == false)
		{
			globals::cEspBoxesNearestVehicle = !globals::cEspBoxesNearestVehicle;
			globals::cEspBoxesNearestVehicle ? printf("\nVEHICLE ESP LIGADO.") : printf("\nVEHICLE ESP DESLIGADO.");
			F4 = true;
		}
		if (GetAsyncKeyState(VK_F4) == 0 && F4 == true) F4 = false;

		if (globals::cEspBoxes)
		{
			DWORD_PTR LocalPlayer = Read<DWORD_PTR>(offsets::dwPlayerPointer);
			DWORD_PTR EntityList = Read<DWORD_PTR>(offsets::dwEntityList);

			DWORD_PTR pPosStructAddress = (DWORD)LocalPlayer + 0x14;
			DWORD pPosStruct = Read<DWORD>(pPosStructAddress);
			Vector3 pPlayerPos = Read<Vector3>(pPosStruct + 0x30);

			for (int i = 0; i < 140; i++)
			{
				DWORD_PTR CurrentPed = (DWORD)EntityList + (offsets::dwCPedSize * i);
				float CurrentPedHealth = Read<float>(CurrentPed + offsets::m_CurrentHealth);

				if (CurrentPedHealth >= 1.0f && CurrentPed != LocalPlayer)
				{
					DWORD_PTR PosStructAddress = (DWORD)CurrentPed + 0x14;
					DWORD PosStruct = Read<DWORD>(PosStructAddress);
					Vector3 PlayerPos = Read<Vector3>(PosStruct + 0x30);

					if (PlayerPos.x != 0.0f && PlayerPos.y != 0.0f)
					{
						Vector2 screen;
						if (WorldToScreen(PlayerPos, screen))
						{
							float height = 80.0f;
							float width = 40.0f;
							float distance_factor = 10.0f;

							float max_height = 80.0f;
							float max_width = 40.0f;
							float min_height = 5.0f;
							float min_width = 5.0f;

							height = height / Get3DDistance(&pPlayerPos, &PlayerPos) * distance_factor;
							width = width / Get3DDistance(&pPlayerPos, &PlayerPos) * distance_factor;

							if (height >= max_height) {
								height = max_height;
								width = max_width;
							}

							if (width <= min_width) {
								height = min_height;
								width = min_width;
							}

							draw_box(screen.x - (width / 2.0f), screen.y - (height / 2.0f), width, height, 2, 255, 0, 0);
						}
					}
				}
			}
		}

		if (globals::cEspBoxesFixedSize)
		{
			DWORD_PTR LocalPlayer = Read<DWORD_PTR>(offsets::dwPlayerPointer);
			DWORD_PTR EntityList = Read<DWORD_PTR>(offsets::dwEntityList);

			//LOCALPLAYER
			DWORD_PTR pPosStructAddress = (DWORD)LocalPlayer + 0x14;
			DWORD pPosStruct = Read<DWORD>(pPosStructAddress);
			Vector3 pPlayerPos = Read<Vector3>(pPosStruct + 0x30);

			for (int i = 0; i < 140; i++)
			{
				DWORD_PTR CurrentPed = (DWORD)EntityList + (offsets::dwCPedSize * i);
				float CurrentPedHealth = Read<float>(CurrentPed + offsets::m_CurrentHealth);
				float MaxPedHealth = Read<float>(CurrentPed + offsets::m_MaxHealth);

				if (CurrentPedHealth >= 1.0f && CurrentPed != LocalPlayer)
				{
					// default max health: 100.143997
					printf("\nMax Health: %f", MaxPedHealth);

					//PED
					DWORD_PTR PosStructAddress = (DWORD)CurrentPed + 0x14;
					DWORD PosStruct = Read<DWORD>(PosStructAddress);
					Vector3 PlayerPos = Read<Vector3>(PosStruct + 0x30);

					if (PlayerPos.x != 0.0f && PlayerPos.y != 0.0f)
					{
						Vector2 screen;
						if (WorldToScreen(PlayerPos, screen))
						{
							float height = 4.0f;
							float width = 4.0f;

							draw_box(screen.x - (width / 2.0f), screen.y - (height / 2.0f), width, height, 2, 255, 0, 0);
						}
					}
				}
			}
		}

		if (globals::cEspBoxesNearestVehicle)
		{
			DWORD_PTR LocalPlayer = Read<DWORD_PTR>(offsets::dwPlayerPointer);
			DWORD_PTR AnimStruct = Read<DWORD_PTR>(LocalPlayer + 0x47C);
			DWORD NearestVehiclePointer = (DWORD)(AnimStruct + 0x120);
			DWORD_PTR NearestVehicle = Read<DWORD_PTR>(NearestVehiclePointer);
			DWORD NearestVehiclePosRotPointer = (DWORD)(NearestVehicle + 0x14);
			DWORD NearestVehicleModelIDPointer = (DWORD)(NearestVehicle + 0x22);
			WORD NearestVehicleModelID = Read<WORD>(NearestVehicleModelIDPointer);
			DWORD_PTR NearestVehiclePosRot = Read<DWORD_PTR>(NearestVehiclePosRotPointer);
			DWORD NearestVehiclePosPointer = (DWORD)(NearestVehiclePosRot + 0x30);
			Vector3 NearestVehiclePos = Read<Vector3>(NearestVehiclePosPointer);

			if (IsPositionValid(NearestVehiclePos) && IsVehicleIdValid(NearestVehicleModelID))
			{
				if (NearestVehiclePos.x != 0.0f && NearestVehiclePos.y != 0.0f)
				{
					Vector2 screen;
					if (WorldToScreen(NearestVehiclePos, screen))
					{
						float height = 4.0f;
						float width = 4.0f;

						draw_box(screen.x - (width / 2.0f), screen.y - (height / 2.0f), width, height, 2, 0, 255, 0);
					}
				}
				printf("\nModel: %i", NearestVehicleModelID);
			}

		}
	}
}