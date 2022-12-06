#ifndef __VEHICLE_BASE_CPP__
#define __VEHICLE_BASE_CPP__

#include "VehicleBase.h"

int VehicleBase::vehicleCount = 0;

VehicleBase::VehicleBase(VehicleType type, Direction direction, bool turn, int size)
    : vehicleID(VehicleBase::vehicleCount++), 
      vehicleType(type),
      vehicleDirection(direction),
      turnRight(turn),
      vehicleSize(size),
      inIntersection(false)

{}

VehicleBase::VehicleBase(const VehicleBase& other)
    : vehicleID(other.vehicleID),
      vehicleType(other.vehicleType),
      vehicleDirection(other.vehicleDirection),
      turnRight(other.turnRight),
      vehicleSize(other.vehicleSize),
      inIntersection(other.inIntersection)


{}

VehicleBase::~VehicleBase() {}

#endif