#pragma once
#include "definitions.h"
#include "CVector.h"

struct view_matrix_t {
	float matrix[16];
};

void CalcScreenCoords(CVector*, CVector*);
bool WorldToScreen(CVector, Vector2&);
float Get3DDistance(CVector*, CVector*);
bool W2SCSGO(view_matrix_t, CVector, CVector&);

