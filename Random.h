#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <iostream>
#include <random>


class Random {
    int initial;
    std::mt19937 rng;
     
    public:
        Random(int seed_);
        double getNextProb();
        void setSeed(int val);
};

#endif