#include "memory.h"
#include "definitions.h"
#include <iostream>

void CalcScreenCoords(Vector3* vecWorld, Vector3* vecScreen)
{
	Matrix16 m = Read<Matrix16>(0xB6FA2C);

	int dwLenX = Read<int>((DWORD)0xC17044);
	int dwLenY = Read<int>((DWORD)0xC17048);

	vecScreen->x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen->y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen->z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	double fRecip = (double)1.0 / vecScreen->z;
	vecScreen->x *= (float)(fRecip * (dwLenX));
	vecScreen->y *= (float)(fRecip * (dwLenY));
}

bool WorldToScreen(Vector3 world, Vector2& screen)
{
	Vector3 vecScreen, vecWorld;
	vecWorld.x = world.x;
	vecWorld.y = world.y;
	vecWorld.z = world.z;

	CalcScreenCoords(&vecWorld, &vecScreen);

	screen.x = vecScreen.x;
	screen.y = vecScreen.y;

	if (vecScreen.z > 1.0f)
		return true;

	return false;
}

float Get3DDistance(Vector3* myCoords, Vector3* enemyCoords) {
	return sqrt(
		pow(double(enemyCoords->x - myCoords->x), 2.0) +
		pow(double(enemyCoords->y - myCoords->y), 2.0) +
		pow(double(enemyCoords->z - myCoords->z), 2.0));
}