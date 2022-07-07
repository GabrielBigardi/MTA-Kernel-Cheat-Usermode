#include "hacks.h"
#include "gui.h"
#include "globals.h"
#include "memory.h"
#include "math.h"

#include <thread>
#include "draw.h"
#include "gtautils.h"

void hacks::VisualsThread() noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		DWORD_PTR LocalPlayer = Read<DWORD_PTR>(offsets::dwPlayerPointer);
		DWORD_PTR EntityList = Read<DWORD_PTR>(offsets::dwEntityList);

		DWORD_PTR pPosStructAddress = (DWORD)LocalPlayer + 0x14;
		DWORD pPosStruct = Read<DWORD>(pPosStructAddress);
		Vector3 pPlayerPos = Read<Vector3>(pPosStruct + 0x30);
		int EspColorByte[] = { round(globals::EspColor[0] * 255.0f),  round(globals::EspColor[1] * 255.0f), round(globals::EspColor[2] * 255.0f) };

		

		for (int i = 0; i < 140; i++)
		{
			if (globals::cEspBoxes == false)
				continue;

			DWORD_PTR CurrentPed = (DWORD)EntityList + (offsets::dwCPedSize * i);
			float CurrentPedHealth = Read<float>(CurrentPed + offsets::m_CurrentHealth);

			if (CurrentPedHealth < 1.0f || CurrentPed == LocalPlayer)
				continue;

			DWORD_PTR PosStructAddress = (DWORD)CurrentPed + 0x14;
			DWORD PosStruct = Read<DWORD>(PosStructAddress);
			Vector3 PlayerPos = Read<Vector3>(PosStruct + 0x30);

			if (PlayerPos.x == 0.0f || PlayerPos.y == 0.0f)
				continue;

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

				draw::box(screen.x - (width / 2.0f), screen.y - (height / 2.0f), width, height, 2, EspColorByte[0], EspColorByte[1], EspColorByte[2]);
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

						draw::box(screen.x - (width / 2.0f), screen.y - (height / 2.0f), width, height, 2, EspColorByte[0], EspColorByte[1], EspColorByte[2]);
					}
				}
				printf("\nModel: %i", NearestVehicleModelID);
			}

		}
	}
}