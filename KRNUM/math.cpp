#include "memory.h"
#include "definitions.h"
#include <iostream>
#include "CVector.h"

void CalcScreenCoords(CVector* vecWorld, CVector* vecScreen)
{
	Matrix16 m = Read<Matrix16>(0xB6FA2C);

	int dwLenX = Read<int>((DWORD)0xC17044);
	int dwLenY = Read<int>((DWORD)0xC17048);

	vecScreen->fX = (vecWorld->fZ * m._31) + (vecWorld->fY * m._21) + (vecWorld->fX * m._11) + m._41;
	vecScreen->fY = (vecWorld->fZ * m._32) + (vecWorld->fY * m._22) + (vecWorld->fX * m._12) + m._42;
	vecScreen->fZ = (vecWorld->fZ * m._33) + (vecWorld->fY * m._23) + (vecWorld->fX * m._13) + m._43;

	double fRecip = (double)1.0 / vecScreen->fZ;
	vecScreen->fX *= (float)(fRecip * (dwLenX));
	vecScreen->fY *= (float)(fRecip * (dwLenY));
}

bool WorldToScreen(CVector world, Vector2& screen)
{
	CVector vecScreen, vecWorld;
	vecWorld.fX = world.fX;
	vecWorld.fY = world.fY;
	vecWorld.fZ = world.fZ;

	CalcScreenCoords(&vecWorld, &vecScreen);

	screen.x = vecScreen.fX;
	screen.y = vecScreen.fY;

	if (vecScreen.fZ > 1.0f)
		return true;

	return false;
}

float Get3DDistance(CVector* myCoords, CVector* enemyCoords) {
	return sqrt(
		pow(double(enemyCoords->fX - myCoords->fX), 2.0) +
		pow(double(enemyCoords->fY - myCoords->fY), 2.0) +
		pow(double(enemyCoords->fZ - myCoords->fZ), 2.0));
}