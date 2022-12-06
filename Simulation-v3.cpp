
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
    vector<double> data; // created to store data from input file
    double value;

    while (getline(input, line)) {
      stringstream ss(line);
      getline(ss, label, ':'); //grabs first value up to and including :
      ss >> value;             //grabs the value after :
      data.push_back(value);
    }


    //-- assigning values to labels
    //-- using the vector and overloading the [] operator.
    double max_sim_time = data[0];
    double num_sec = data[1];
       
    //-- light and direction time
    double gns = data[2];
    double yns = data[3];
    double gew = data[4];
    double yew = data[5];
    double rns = (gew+yew); // will stay red for sum of other light ticks
    double rew = (gns+yns);

    //-- new vehicle & direction probability
    double nvnb = data[6];
    double nvsb = data[7];
    double nveb = data[8];
    double nvwb = data[9];
        
    //-- vehicle type proportion
    double pcar = data[10];
    double psuv = data[11];
    double ptruck = 1 - (pcar + psuv);

    //-- right and left turn probabilities
    double p_rightcar = data[12];
    double p_leftcar = data[13];
    double p_rightsuv = data[14];
    double p_leftsuv = data[15];
    double p_righttruck = data[16];
    double p_lefttruck = data[17];

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
      if (nvnb<=prob_north){
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          vn = new VehicleBase(VehicleType::car, Direction::north);
          //VehicleBase vb3(VehicleType::car, Direction::north);
        }
        else if (p_vt <= (psuv+pcar)){
          vn = new VehicleBase(VehicleType::car, Direction::north);
        }
        else{
          vn = new VehicleBase(VehicleType::car, Direction::north);
        }
      }
   //SOUTHBOUND LANE - determining if there is a new vehicle and what type
      if (nvsb<=prob_south){
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          vs = new VehicleBase(VehicleType::car, Direction::south);
          //VehicleBase vb3(VehicleType::car, Direction::north);
        }
        else if (p_vt <= (psuv+pcar)){
          vs = new VehicleBase(VehicleType::car, Direction::south);
        }
        else{
          vs = new VehicleBase(VehicleType::car, Direction::south);
        }
      }
   //EASTBOUND LANE - determining if there is a new vehicle and what type
      if (nveb<=prob_east){
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          ve = new VehicleBase(VehicleType::car, Direction::east);
          //VehicleBase vb3(VehicleType::car, Direction::north);
        }
        else if (p_vt <= (psuv+pcar)){
          ve = new VehicleBase(VehicleType::car, Direction::east);
        }
        else{
          ve = new VehicleBase(VehicleType::car, Direction::east);
        }
      }
   //WESTBOUND LANE - determining if there is a new vehicle and what type
      if (nvwb<=prob_west){
        //probability of vehicle type
        double p_vt=rand_gen.getNextProb();
        if (p_vt <= pcar){
          vw = new VehicleBase(VehicleType::car, Direction::west);
          //VehicleBase vb3(VehicleType::car, Direction::north);
        }
        else if (p_vt <= (psuv+pcar)){
          vw = new VehicleBase(VehicleType::car, Direction::west);
        }
        else{
          vw = new VehicleBase(VehicleType::car, Direction::west);
        }
      }
      //
     // if (vn!=nullptr){
            //indicates a car can be placed
     //}

    

        //testing vehicle 
       /* int pos=0;
        VehicleBase vb1(VehicleType::car, Direction::east);
        eastbound[pos] = &vb1;
        VehicleBase vb2(VehicleType::suv, Direction::west);
        westbound[pos] = westbound[pos+1] = &vb2;
        VehicleBase vb3(VehicleType::truck, Direction::north);
        northbound[pos] = northbound[pos+1] = northbound[pos+2]= &vb3;
        VehicleBase vb4(VehicleType::car, Direction::south);
        southbound[pos] = &vb4;*/

     
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













