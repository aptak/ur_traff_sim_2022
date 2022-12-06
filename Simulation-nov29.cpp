
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
    }
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
          //turning right
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::north,true );
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::north,false);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightsuv) vn = new VehicleBase(VehicleType::suv, Direction::north,true);
          else vn = new VehicleBase(VehicleType::suv, Direction::north,false);
        }
        else{
          if (p_right<=p_righttruck) vn = new VehicleBase(VehicleType::truck, Direction::north, true);
          else vn = new VehicleBase(VehicleType::truck, Direction::north, false);
        }
      }
   //SOUTHBOUND LANE - determining if there is a new vehicle and what type
      if (prob_south<=nvsb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::south,true );
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::south,false);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::south,true );
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::south,false);
        }
        else{
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::south,true );
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::south,false);
        }
      }
   //EASTBOUND LANE - determining if there is a new vehicle and what type
      if (prob_east<nveb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::east,true );
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::east,false);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::east,true );
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::east,false);
        }
        else{
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::east,true );
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::east,false);
        }
      }
   //WESTBOUND LANE - determining if there is a new vehicle and what type
      if (prob_west<nvwb){
        //generating random number for turning right probabiliy
        double p_right=rand_gen.getNextProb();
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::car, Direction::west,true );
          //straight
          else vn = new VehicleBase(VehicleType::car, Direction::west,false);
        }
        else if (p_vt <= (psuv+pcar)){
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::suv, Direction::west,true );
          //straight
          else vn = new VehicleBase(VehicleType::suv, Direction::west,false);
        }
        else{
          if(p_right<=p_rightcar) vn = new VehicleBase(VehicleType::truck, Direction::west,true );
          //straight
          else vn = new VehicleBase(VehicleType::truck, Direction::west,false);
        }
      }

    // if (vn!=nullptr){
    //  northbound[]

      if (inc_gns >= gns) {
        isNSGreen = false;
        animator.setLightNorthSouth(LightColor::yellow);
        if (inc_yns >= yns) {                               //once time for yellow has run out
          animator.setLightNorthSouth(LightColor::red);
          animator.setLightEastWest(LightColor::green);
          inc_gns = 0;
          inc_yns = 0;
          isEWGreen = true;
        }
        else inc_yns++;
      }
      else if (inc_gew >= gew){
        isEWGreen = false;
        animator.setLightEastWest(LightColor::yellow);
        if (inc_yew >= yew){
          animator.setLightEastWest(LightColor::red);
          animator.setLightNorthSouth(LightColor::green);
          inc_gew = 0;
          inc_yew = 0;
          isNSGreen = true;
        }
        else inc_yew++;      

      }
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
    }
    
  }
  return 0;
} 











