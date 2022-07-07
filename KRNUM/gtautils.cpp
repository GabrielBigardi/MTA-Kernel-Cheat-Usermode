#include "math.h"

bool IsPositionValid(Vector3 position)
{
	return position.x > -3000.0f && position.x < 3000.0f && position.x != 0.0f && position.y > -3000.0f && position.y < 3000.0f && position.y != 0.0f;
}

bool IsVehicleIdValid(int vehicleId)
{
	return vehicleId >= 400 && vehicleId <= 611;
}