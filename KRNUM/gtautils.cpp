#include "math.h"

bool IsPositionValid(CVector position)
{
	return position.fX > -3000.0f && position.fX < 3000.0f && position.fX != 0.0f && position.fY > -3000.0f && position.fY < 3000.0f && position.fY != 0.0f;
}

bool IsVehicleIdValid(int vehicleId)
{
	return vehicleId >= 400 && vehicleId <= 611;
}