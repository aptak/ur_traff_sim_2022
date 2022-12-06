#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include <sstream>


#include "Animator.h"
#include "VehicleBase.h"
#include "Random.h"

using namespace std;

void moveBeforeIntersection(vector<VehicleBase*>& lane, int num_sec) {
  // if there's a space in front of it, it can move
  for(int i = num_sec - 1; i > 0; i--) {
    // if only one spot is empty
    if(lane[i] == nullptr && lane[i-1] != nullptr) {
      lane[i] = lane[i-1];
      int vehicleSize = lane[i-1]->getVehicleSize();
      if((i - vehicleSize) >= 0) {
        lane[i - vehicleSize] = nullptr;
      } //end if
    } //end if
  } //end for
} //end moveBeforeIntersection


void movePastIntersection(vector<VehicleBase*>& lane, int num_sec) {
  // function to move cars that have already passed the intersection.
  // what is past the intersection = num_sec + 1

  // if last segment =/= previous statement
  // only true if a vehicle is exiting a lane
  if(lane[num_sec * 2 + 1] != lane[num_sec * 2] && lane[num_sec * 2 + 1] != nullptr) {
    delete lane[num_sec * 2 + 1];
    // already a pointer to the object
  } //end if

  //first do the ones that move since they're past the intersection
  for(int i = num_sec * 2 + 1; i > num_sec+2; i--) {
    if (!lane[i-1]->getInIntersection())
      lane[i] = lane[i - 1];
  } //end for loop

  //lane[num_sec] = nullptr;

} //end movePastIntersection

int main(int argc, char *argv[])
{
  // checking command line arguments
  if (argc!=3){
    cerr<<"Usage: ./simulation [input-file] [random seed]"<<endl;
    exit(1);
  }

  //setting the input file to 'input'
  ifstream input(argv[1]);

  //checking to see if it opens correctly and exists
  if (input.good()) {
    string seed(argv[2]);
    int initialSeed = stoi(seed); //initialize the seed
    int i;

  //using our random class
    Random rand_gen(initialSeed);
    string line, label;
    double value;

    //-- assigning values to labels
    //-- using the vector and overloading the [] operator.
    double max_sim_time;
    double num_sec;
    //-- light and direction time
    double gns;
    double yns;
    double gew;
    double yew;
    double rns; // will stay red for sum of other light ticks
    double rew;
    //-- new vehicle & direction probability
    double nvnb;
    double nvsb;
    double nveb;
    double nvwb;
    //-- vehicle type proportion
    double pcar;
    double psuv;
    //-- right turn and straight probabilities
    double p_rightcar;
    double p_rightsuv;
    double p_righttruck;
    double p_straightcar;
    double p_straightsuv;
    double p_straighttruck;

    // move

    //grabbing data from input file
    while (getline(input, line)) {
      stringstream ss(line);
      getline(ss, label, ':'); //grabs first value up to and including :
      ss >> value;             //grabs the value after :

     if (label=="maximum_simulated_time") max_sim_time=value;
     else if (label=="number_of_sections_before_intersection") num_sec=value;
     else if (label=="green_north_south") gns=value;
     else if (label=="yellow_north_south") yns=value;
     else if (label=="green_east_west") gew=value;
     else if (label=="yellow_east_west") yew=value;
     else if (label=="prob_new_vehicle_northbound") nvnb=value;
     else if (label=="prob_new_vehicle_southbound") nvsb=value;
     else if (label=="prob_new_vehicle_eastbound") nveb=value;
     else if (label=="prob_new_vehicle_westbound") nvwb=value;
     else if (label=="proportion_of_cars") pcar=value;
     else if (label=="proportion_of_SUVs") psuv=value;
     else if (label=="proportion_right_turn_cars") p_rightcar=value;
     else if (label=="proportion_right_turn_SUVs") p_rightsuv=value;
     else if (label=="proportion_right_turn_trucks") p_righttruck=value;
   } //end while loop
    p_straightcar = 1-p_rightcar;
    p_straightsuv = 1-p_rightsuv;
    p_straighttruck = 1-p_righttruck;
    //traffic light time
    rns = (gew+yew);                      // will stay red for sum of other light ticks
    rew = (gns+yns);

    Animator animator(num_sec);
    //-- creating the vectors for each lane with num_sec
    vector<VehicleBase*> westbound(num_sec * 2 + 2, nullptr);
    vector<VehicleBase*> eastbound(num_sec * 2 + 2, nullptr);
    vector<VehicleBase*> southbound(num_sec * 2 + 2, nullptr);
    vector<VehicleBase*> northbound(num_sec * 2 + 2, nullptr);

    //-- default colors
    animator.setLightNorthSouth(LightColor::green);
    animator.setLightEastWest(LightColor::red);

    //-- master loop with master clock of max ticks (max_sim_time)
    //count variables for time lit up
    int inc_gns=0;  //aka time the light has been green for
    int inc_rns=0;
    int inc_yns=0;
    int inc_gew=0;
    int inc_rew=0;
    int inc_yew=0;
    int t = 0;
    char dummy;
    bool isNSGreen = true, isEWGreen = false;
    //new
    bool isNSYellow;
    bool isEWYellow;

    //add default vehicles

    while (t < max_sim_time){
      //null ptrs to use for determing direction and type
      VehicleBase *vn = nullptr; //north
      VehicleBase *vs = nullptr; //south
      VehicleBase *ve = nullptr; //east
      VehicleBase *vw = nullptr; //west

      double prob_north=rand_gen.getNextProb();
      double prob_south=rand_gen.getNextProb();
      double prob_east=rand_gen.getNextProb();
      double prob_west=rand_gen.getNextProb();

   //NORTHBOUND LANE - determining if there is a new vehicle and what type

   ///CHANGED ORDER FROM NVNB<PROB_NORTH)
      if (prob_north<=nvnb){
        //generating random number for turning right probability
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          cout << "creating northbound car" << endl;
          //turning right
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::north,true,2);
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::north,false,2);
        }
        else if (p_vt <= (psuv+pcar)){
          cout << "creating northbound suv" << endl;
          if(p_right<=p_rightsuv) vn = new VehicleBase(VehicleType::suv, Direction::north,true,3);
          else vn = new VehicleBase(VehicleType::suv, Direction::north,false,3);
        }
        else{
          cout << "creating northbound truck" << endl;
          if (p_right<=p_righttruck) vn = new VehicleBase(VehicleType::truck, Direction::north, true,4);
          else vn = new VehicleBase(VehicleType::truck, Direction::north, false,4);
        }
      }
   //SOUTHBOUND LANE - determining if there is a new vehicle and what type
      if (prob_south<=nvsb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          cout << "creating southbound car" << endl;
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::south,true,2);
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::south,false,2);
        }
        else if (p_vt <= (psuv+pcar)){
          cout << "creating southbound suv" << endl;
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::south,true,3);
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::south,false,3);
        }
        else{
          cout << "creating southbound truck" << endl;
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::south,true,4);
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::south,false,4);
        }
      }
   //EASTBOUND LANE - determining if there is a new vehicle and what type
      if (prob_east<nveb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          cout << "creating eastbound car" << endl;
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::east,true,2);
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::east,false,2);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::east,true,3);
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::east,false,3);
        }
        else{
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::east,true,4);
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::east,false,4);
        }
      }
   //WESTBOUND LANE - determining if there is a new vehicle and what type
      if (prob_west<nvwb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::west,true,2);
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::west,false,2);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::west,true,3);
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::west,false,3);
        }
        else{
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::west,true,4);
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::west,false,4);
        }
      }

    // if (vn!=nullptr){
    //  northbound[]

      if (inc_gns >= gns) {
        isNSGreen = false;
        animator.setLightNorthSouth(LightColor::yellow);
        //new
        isNSYellow=true;
        if (inc_yns >= yns) {                               //once time for yellow has run out
          animator.setLightNorthSouth(LightColor::red);
          animator.setLightEastWest(LightColor::green);
          //new
          isNSYellow=false;
          inc_gns = 0;
          inc_yns = 0;
          isEWGreen = true;
        }
        else inc_yns++;
      }
      else if (inc_gew >= gew){
        isEWGreen = false;
        animator.setLightEastWest(LightColor::yellow);
        //new
        isEWYellow=true;
        if (inc_yew >= yew){
          animator.setLightEastWest(LightColor::red);
          animator.setLightNorthSouth(LightColor::green);
          //new
          isEWYellow=false;
          inc_gew = 0;
          inc_yew = 0;
          isNSGreen = true;
        }
        else inc_yew++;
      } //end else if

// first, move the cars past the intersection
      movePastIntersection(northbound, num_sec);
      movePastIntersection(westbound, num_sec);
      movePastIntersection(southbound, num_sec);
      movePastIntersection(eastbound, num_sec);

// EASTBOUND MOVE
      VehicleBase* v = eastbound[num_sec - 1];
      int vsize;
      bool vShallPass = false;
      if(v != nullptr) {
        vsize = v->getVehicleSize();
        cout << "v is not a nullpointer" << endl;
        if(v == eastbound[num_sec - vsize]) { // vehicle v is at the beginning of the intersection with a green or yellow light
          if(isEWGreen) {
            vShallPass = true;
          }
          if(isEWYellow){
            if(vsize<yew){ //not inclusive because time needed =size+1 aka (size +1 <=yew)
              if (vsize<=(yew-inc_yew)){ //3-1 =2 inclusive becuase it will iterate after the light is yellow
                vShallPass = true;
              }
            }
            //check to see if the vehicle can make it through the light regardless of how much time it's been yellow (max time)
          } //end if
          if(vShallPass) {
            v->setInIntersection(true);
          }
        } //end if
        if((isEWGreen || isEWYellow) && v->getInIntersection()) {
          //if turning
          if(v->getTurn()) {
            southbound[num_sec + 1] = v;
          } else {
            eastbound[num_sec] = v;
          }
          i = 0;
          while(i<vsize) {
            if(eastbound[num_sec - 1 - i] != v) {
              eastbound[num_sec - i] = nullptr;
              break;
            } // move car by erasing it from the last square it used to occupy
            i++;
          }
        }
      } //end if v!= nullptr

// WESTBOUND MOVE
      v = westbound[num_sec - 1];
      vShallPass = false;
      if(v != nullptr) {
        vsize = v->getVehicleSize();
        cout << "v is not a nullpointer" << endl;
        if(v == westbound[num_sec - vsize]) { // vehicle v is at the beginning of the intersection with a green or yellow light
          if(isEWGreen) {
            vShallPass = true;
          }
          if(isEWYellow){
            if(vsize<yew){ //not inclusive because time needed =size+1 aka (size +1 <=yew)
              if (vsize<=(yew-inc_yew)){ //3-1 =2 inclusive becuase it will iterate after the light is yellow
                vShallPass = true;
              }
            }
            //check to see if the vehicle can make it through the light regardless of how much time it's been yellow (max time)
          } //end if
          if(vShallPass) {
            v->setInIntersection(true);
          }
        } //end if
        if((isEWGreen || isEWYellow) && v->getInIntersection()) {
          //if turning
          if(v->getTurn()) {
            northbound[num_sec + 1] = v;
          } else {
            westbound[num_sec] = v;
          }
          i = 0;
          while(i<vsize) {
            if(westbound[num_sec - 1 - i] != v) {
              westbound[num_sec - i] = nullptr;
              break;
            } // move car by erasing it from the last square it used to occupy
            i++;
          }
        }
      } //end if v!= nullptr

// NORTHBOUND MOVE
      v = northbound[num_sec - 1];
      vShallPass = false;
      if(v != nullptr) {
        vsize = v->getVehicleSize();
        cout << "v is not a nullpointer" << endl;
        if(v == northbound[num_sec - vsize]) { // vehicle v is at the beginning of the intersection with a green or yellow light
          cout << "Vehicle is at northbound intersection" << endl;
          if(isNSGreen) {
            vShallPass = true;
          }
          if(isNSYellow){
            if(vsize<yew){ //not inclusive because time needed =size+1 aka (size +1 <=yew)
              if (vsize<=(yns-inc_yns)){ //3-1 =2 inclusive becuase it will iterate after the light is yellow
                vShallPass = true;
              }
            }
            //check to see if the vehicle can make it through the light regardless of how much time it's been yellow (max time)
          } //end if
          if(vShallPass) {
            v->setInIntersection(true);
          }
        } //end if
        if((isNSGreen || isNSYellow) && v->getInIntersection()) {
          //if turning
          if(v->getTurn()) {
            cout << "Northbound vehicle is turning" << endl;
            eastbound[num_sec + 1] = v;
          } else {
            cout << "Northbound vehicle is going straight" << endl;
            northbound[num_sec] = v;
          }
          i = 0;
          while(i<vsize) {
            if(northbound[num_sec - 1 - i] != v) {
              northbound[num_sec - i] = nullptr;
              break;
            } // move car by erasing it from the last square it used to occupy
            i++;
          }
        }
      } //end if v!= nullptr

// SOUTHBOUND MOVE
      v = southbound[num_sec - 1];
      vShallPass = false;
      if(v != nullptr) {
        vsize = v->getVehicleSize();
        cout << "v is not a nullpointer" << endl;
        if(v == southbound[num_sec - vsize]) { // vehicle v is at the beginning of the intersection with a green or yellow light
          if(isNSGreen) {
            vShallPass = true;
          }
          if(isNSYellow){
            if(vsize<yew){ //not inclusive because time needed =size+1 aka (size +1 <=yew)
              if (vsize<=(yns-inc_yns)){ //3-1 =2 inclusive becuase it will iterate after the light is yellow
                vShallPass = true;
              }
            }
            //check to see if the vehicle can make it through the light regardless of how much time it's been yellow (max time)
          } //end if
          if(vShallPass) {
            v->setInIntersection(true);
          }
        } //end if
        if((isNSGreen || isNSYellow) && v->getInIntersection()) {
          //if turning
          if(v->getTurn()) {
            westbound[num_sec + 1] = v;
          } else {
            southbound[num_sec] = v;
          }
          i = 0;
          while(i<vsize) {
            if(southbound[num_sec - 1 - i] != v) {
              southbound[num_sec - i] = nullptr;
              break;
            } // move car by erasing it from the last square it used to occupy
            i++;
          }
        }
      } //end if v!= nullptr


    // copy 3 times, change directions, call moveBeforeIntersection for all four lanes

      moveBeforeIntersection(northbound, num_sec);
      moveBeforeIntersection(westbound, num_sec);
      moveBeforeIntersection(southbound, num_sec);
      moveBeforeIntersection(eastbound, num_sec);


//generate new car
      if(northbound[0] == nullptr) {
        northbound[0] = vn;
        if(vn != nullptr) {
          cout << "Vehicle entering northbound lane" << endl;
        }
      }

      if(eastbound[0] == nullptr) {
        eastbound[0] = ve;
        if(ve != nullptr) {
          cout << "Vehicle entering eastbound lane" << endl;
        }
      }

      if(southbound[0] == nullptr) {
        southbound[0] = vs;
        if(vs != nullptr) {
          cout << "Vehicle entering southbound lane" << endl;
        }
      }

      if(westbound[0] == nullptr) {
        westbound[0] = vw;
        if(vw != nullptr) {
          cout << "Vehicle entering westbound lane" << endl;
        }
      }


// make animator draw the cars
      animator.setVehiclesNorthbound(northbound);
      animator.setVehiclesWestbound(westbound);
      animator.setVehiclesSouthbound(southbound);
      animator.setVehiclesEastbound(eastbound);

      animator.draw(t);

      std::cin.get(dummy);

    // to test the increments
      cout << "G Value of N-S: " << inc_gns << endl;
      cout << "G Value of E-W: " << inc_gew << endl;
      cout << "Y Value of N-S: " << inc_yns << endl;
      cout << "Y Value of E-W: " << inc_yew << endl;
    //incrementing the count variables
       if (isNSGreen) {
         inc_gns++;
       }
       if (isEWGreen) {
         inc_gew++;
       }
       t++;

    // call things.
      // call mofepast first

    } // end while

  } // end if
  return 0;

}
