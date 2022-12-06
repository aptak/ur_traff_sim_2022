// x and y positions of cars
struct position
{
  int x;
  int y;
  bool inIntersection;
}

// get and set x and y positions of cars
int getx() { return x; }
int gety() { return y; }

void sety(int a) { y = a; }
void setx(int a) { x = a; }

// check if the car is in the intersection
if (x == intersection_coorinate +- 3 || y == intersection coordinate)
{
  bool inIntersection = true;
} else {
  bool inIntersection = false;
}

void move()
{
  //move North cars
  while isNSGreen == true {
    if (vehicleDirection == north) {
      for(int y : maximum) {
        y++;
        await input
      }
    } //end if north

    //move south cars
    else if (vehicleDirection == south)
    {
      for(int y : maximum)
      {
        y--;
        await input
      }
    } //end if south

    // move east cars
    else if (vehicleDirection == east)
    {
      for(int x : maximum)
      {
        x++ ;
        await input
      }
    } //end if east

    // move west cars
    else if (vehicleDirection == west)
    {
      for(int x : maximum)
      {
        x-- ;
        await input
      }
    } //end if east

  } //end while

  #while isEWGreen == true

} //end move
