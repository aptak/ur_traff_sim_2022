#include <random>
#include "Random.h"

    Random::Random(int seed_){
        initial=seed_;
        rng.seed(initial);
    }
    double Random::getNextProb(){
        std::uniform_real_distribution<double> rand_double(0.0, 1.0);
        
        return rand_double(rng);
    }
    void Random::setSeed(int val){
        initial=val;
        rng.seed(initial);
    }