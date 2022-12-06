#ifndef __VEHICLE_BASE_H__
#define __VEHICLE_BASE_H__

// enum: see http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-enum
enum class Direction   {north, south, east, west};
enum class VehicleType {car, suv, truck};
enum class LightColor  {green, yellow, red};

class VehicleBase
{
   public:
      static int vehicleCount;

   private:
      int         vehicleID;
      VehicleType vehicleType;
      Direction   vehicleDirection;
      bool turnRight;
      bool inIntersection;
      int intersectionCounter; // Keep track of how far into intersection vehicle is
      int vehicleSize;

   public:
      VehicleBase(VehicleType type, Direction originalDirection, bool turn, int size);
      VehicleBase(const VehicleBase& other);
      ~VehicleBase();

      inline int getVehicleID() const { return this->vehicleID; }

      inline VehicleType getVehicleType() const { return this->vehicleType; }
      inline Direction   getVehicleOriginalDirection() const { return this->vehicleDirection; }
      inline bool getTurn() {return this->turnRight;}
      inline bool getInIntersection() { return this->inIntersection; }
      inline void setInIntersection(bool flag) { this->inIntersection = flag; }
      inline int getVehicleSize() {return this->vehicleSize; }
      inline void incrementInterectionCounter(void) {this->intersectionCounter++;}
      inline int getIntersectionCount() {return this->intersectionCounter;}
};

#endif
