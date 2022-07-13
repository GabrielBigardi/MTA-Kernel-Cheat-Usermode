#pragma once
#include "definitions.h"
#include "CVector.h"

void CalcScreenCoords(CVector*, CVector*);
bool WorldToScreen(CVector, Vector2&);
float Get3DDistance(CVector*, CVector*);