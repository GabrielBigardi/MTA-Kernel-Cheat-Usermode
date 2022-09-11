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

uintptr_t game_base_address = 0;
uintptr_t client_module_address = 0;

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

static ULONG64 get_module_base_address(uintptr_t pid, const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = pid;
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

static ULONG64 get_module_base_address_x86(uintptr_t pid, const char* module_name)
{
	NULL_MEMORY instructions = { 0 };
	instructions.pid = pid;
	instructions.req_base_x86 = TRUE;
	instructions.read = FALSE;
	instructions.write = FALSE;
	instructions.draw_box = FALSE;
	instructions.module_name = module_name;
	call_hook(&instructions);

	ULONG64 base = NULL;
	base = instructions.base_address;
	return base;
}

HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

int main()
{
	SetConsoleTextAttribute(hConsole, 6);
//	std::cout << R"(
//:::    ::: ::::::::::: :::    :::       :::     :::   :::        :::::::  
//:+:    :+:     :+:     :+:    :+:       :+:     :+: :+:+:       :+:   :+: 
//+:+    +:+     +:+      +:+  +:+        +:+     +:+   +:+       +:+  :+:+ 
//+#++:++#++     +#+       +#++:+         +#+     +:+   +#+       +#+ + +:+ 
//+#+    +#+     +#+      +#+  +#+         +#+   +#+    +#+       +#+#  +#+ 
//#+#    #+#     #+#     #+#    #+#         #+#+#+#     #+#   #+# #+#   #+# 
//###    ###     ###     ###    ###           ###     ####### ###  #######  
//)" << std::endl;
	std::cout << R"(
88      888888888888                          88         ,a8888a,     
88           88                             ,d88       ,8P"'  `"Y8,   
88           88                           888888      ,8P        Y8,  
88,dPPYba,   88  8b,     ,d8     8b       d8  88      88          88  
88P'    "8a  88   `Y8, ,8P'      `8b     d8'  88      88          88  
88       88  88     )888(         `8b   d8'   88      `8b        d8'  
88       88  88   ,d8" "8b,        `8b,d8'    88  888  `8ba,  ,ad8'   
88       88  88  8P'     `Y8         "8"      88  888    "Y8888P"     
)" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "Trying to locate processes... " << std::endl;
	globals::gtasa_process_id = get_process_id("gta_sa.exe");
	globals::mta_process_id = get_process_id("Multi Theft Auto.exe");

	std::cout << globals::gtasa_process_id << std::endl;
	std::cout << globals::mta_process_id << std::endl;

	std::cout << "gta_sa.exe: ";

	if (globals::gtasa_process_id == 0) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "Multi Theft Auto.exe: ";

	if (globals::mta_process_id == 0) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "\nTrying to locate base modules... " << std::endl;
	game_base_address = get_module_base_address(globals::gtasa_process_id, "gta_sa.exe");
	client_module_address = get_module_base_address(globals::mta_process_id, "Multi Theft Auto.exe");

	std::cout << "gta_sa.exe: ";

	if (!game_base_address) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 7);
	std::cout << "client.dll: ";
	
	if (!client_module_address) {
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "NOT FOUND" << std::endl;
		Sleep(5000);
		return NULL;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "SUCCESS" << std::endl;
	}

	SetConsoleTextAttribute(hConsole, 2);
	std::cout << "\nStarting in 3s...\n\n" << std::endl;
	SetConsoleTextAttribute(hConsole, 7);
	Sleep(3000);

	bool F2 = false;
	bool F3 = false;
	bool F4 = false;
	bool F5 = false;
	bool F6 = false;
	bool F7 = false;

	while (true)
	{
		// ==== Dynamic ESP ====
		if (GetAsyncKeyState(VK_F2) < 0 && F2 == false)
		{
			globals::cEspBoxes = !globals::cEspBoxes;
			//const char* cEspState = globals::cEspBoxes ? "[ON]" : "[OFF]"

			std::cout << "Player/Zombies ESP: ";
			if (globals::cEspBoxes) {
				SetConsoleTextAttribute(hConsole, 2);
				std::cout << "[ON]" << std::endl;
			}
			else {
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << "[OFF]" << std::endl;
			}
			SetConsoleTextAttribute(hConsole, 7);

			F2 = true;
		}
		if (GetAsyncKeyState(VK_F2) == 0 && F2 == true) F2 = false;

		// ==== Vehicle ESP ====
		if (GetAsyncKeyState(VK_F4) < 0 && F4 == false)
		{
			globals::cEspVehicles = !globals::cEspVehicles;

			std::cout << "Vehicle ESP: ";
			if (globals::cEspVehicles) {
				SetConsoleTextAttribute(hConsole, 2);
				std::cout << "[ON]" << std::endl;
			}
			else {
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << "[OFF]" << std::endl;
			}
			SetConsoleTextAttribute(hConsole, 7);

			F4 = true;
		}
		if (GetAsyncKeyState(VK_F4) == 0 && F4 == true) F4 = false;

		// ==== Farclip Distance ====
		if (GetAsyncKeyState(VK_F5) < 0 && F5 == false)
		{
			std::cout << "Farclip: ";
			SetConsoleTextAttribute(hConsole, 2);
			std::cout << "[1500f]" << std::endl;
			SetConsoleTextAttribute(hConsole, 7);
			Write(0xB7C4F0, 1500.0f);
			F5 = true;
		}
		if (GetAsyncKeyState(VK_F5) == 0 && F5 == true) F5 = false;

		// ==== FOG Distance ====
		if (GetAsyncKeyState(VK_F6) < 0 && F6 == false)
		{
			std::cout << "FOG: ";
			SetConsoleTextAttribute(hConsole, 2);
			std::cout << "[500f]" << std::endl;
			SetConsoleTextAttribute(hConsole, 7);
			Write(0xB7C4F4, 500.0f);
			F6 = true;
		}
		if (GetAsyncKeyState(VK_F6) == 0 && F6 == true) F6 = false;

		// ==== Test ====
		if (GetAsyncKeyState(VK_F7) < 0 && F7 == false)
		{
			globals::cTest = !globals::cTest;

			F7 = true;
		}
		if (GetAsyncKeyState(VK_F7) == 0 && F7 == true) F7 = false;


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
					byte PlayerCheckByte = Read<byte>(CurrentPed + 0x530);
					int PlayerStatus = (int)PlayerCheckByte;

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

							if (PlayerStatus == 50) // 0 = leaving a car, falling down from a bike or something like this 1 = normal case 50 = driving 55 = wasted 63 = busted
							{
								draw::box(HeadSP.x - BoxSX / 2 - 1, HeadSP.y - 1, BoxSX + 2, BoxSY + 2, 3, 175, 0, 0);
								draw::box(HeadSP.x - BoxSX / 2, HeadSP.y, BoxSX, BoxSY, 1, 255, 255, 255);
							}
							else
							{
								draw::box(HeadSP.x - BoxSX / 2 - 1, HeadSP.y - 1, BoxSX + 2, BoxSY + 2, 3, 0, 0, 175);
								draw::box(HeadSP.x - BoxSX / 2, HeadSP.y, BoxSX, BoxSY, 1, 255, 255, 255);
							}
							
						}
					}
				}
			}
			//globals::cEspBoxes = false;
		}

		if (globals::cEspObjects)
		{
			DWORD_PTR ObjectPool = Read<DWORD_PTR>(0xB7449C);
			DWORD_PTR ObjectPoolStart = Read<DWORD_PTR>((DWORD)ObjectPool);
			int ObjectPoolSize = Read<int>((DWORD)ObjectPool + 0x8);

			for (int i = 0; i < ObjectPoolSize; i++)
			{
				DWORD_PTR CurrentObject = (DWORD)ObjectPoolStart + (0x19c * i);
				CVector CurrentObjectPos = Read<CVector>((DWORD)CurrentObject + 0x4);
				int CurrentObjectModel = Read<int>((DWORD)CurrentObject + 0x22);

				if (!IsPositionValid(CurrentObjectPos) || !IsObjectValid(CurrentObjectModel))
					continue;

				//printf("\n Object %i -> ", i);
				//printf("%i - ", CurrentObjectModel);
				//printf("%f - ", CurrentObjectPos.fX);
				//printf("%f - ", CurrentObjectPos.fY);
				//printf("%f", CurrentObjectPos.fZ);

				Vector2 FeetSP;
				Vector2 HeadSP;

				CVector PlayerFeetPos = CurrentObjectPos - CVector(0.f, 0.f, 1.f);
				CVector PlayerHeadPos = CurrentObjectPos + CVector(0.f, 0.f, 0.775f);

				Vector2 screen;

				if (WorldToScreen(PlayerFeetPos, FeetSP) && WorldToScreen(PlayerHeadPos, HeadSP))
				{
					float BoxSY = FeetSP.y - HeadSP.y;
					float BoxSX = BoxSY / 2.25f;
					float BoxX = HeadSP.x - BoxSX / 2;
					float BoxY = HeadSP.y;

					draw::box(HeadSP.x - BoxSX / 2 - 1, HeadSP.y - 1, BoxSX + 2, BoxSY + 2, 3, 0, 0, 175);
					draw::box(HeadSP.x - BoxSX / 2, HeadSP.y, BoxSX, BoxSY, 1, 0, 255, 0);
				}
			}
		}


		if (globals::cEspVehicles)
		{
			DWORD_PTR VehiclePoolUsage = Read<DWORD_PTR>(0xB74494);
			DWORD_PTR VehiclePoolStart = Read<DWORD_PTR>((DWORD)VehiclePoolUsage);
			int MaximumVehicle = Read<int>((DWORD)VehiclePoolUsage + 0x8);

			for (int i = 0; i < MaximumVehicle; i++)
			{
				DWORD_PTR CurrentVehicle = (DWORD)VehiclePoolStart + (0xA18 * i);
				DWORD_PTR CurrentVehiclePosPointer = Read<DWORD_PTR>((DWORD)CurrentVehicle + 0x14);
				CVector CurrentVehiclePos = Read<CVector>((DWORD)CurrentVehiclePosPointer + 0x30);
			
				if (CurrentVehiclePos.fX != 0.0f && CurrentVehiclePos.fY != 0.0f)// && CurrentObjectModel == 3243)
				{
					Vector2 screen;
					int size = 4;
				
					if (WorldToScreen(CurrentVehiclePos, screen))
					{
						draw::box(screen.x - size / 2 - 1, screen.y - 1, size + 2, size + 2, 3, 0, 0, 0);
						draw::box(screen.x - size / 2, screen.y, size, size, 1, 0, 255, 0);
					}
				}
			}
		}

		if (globals::cTest)
		{
			for (int i = 0; i < 1; i++)
			{
				CVector currentBonePos = Read<CVector>(0x19ABE734 + (0x54 * i));

				if (!IsPositionValid(currentBonePos))
					continue;

				Vector2 screen;
				int size = 4;

				if (WorldToScreen(currentBonePos, screen))
				{
					draw::box(screen.x - size / 2 - 1, screen.y - 1, size + 2, size + 2, 3, 0, 0, 0);
					draw::box(screen.x - size / 2, screen.y, size, size, 1, 0, 255, 0);
				}
			}
		}
	}
}