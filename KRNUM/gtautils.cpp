#include "math.h"

const float ROUNDING_ERROR_f32 = 0.000001f;

//! returns if a equals b, taking possible rounding errors into account
inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

bool IsPositionValid(CVector position)
{
	return position.fX > -3000.0f && position.fX < 3000.0f && !(equals(position.fX, 0.0f)) && position.fY > -3000.0f && position.fY < 3000.0f && !(equals(position.fY, 0.0f));
}

bool IsVehicleIdValid(int vehicleId)
{
	return vehicleId >= 400 && vehicleId <= 611;
}

bool IsObjectValid(int objectId)
{
	return objectId > 0;
}