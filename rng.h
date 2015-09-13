/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef rng_h
#define rng_h

#include <string>
#include <stdint.h>

const uint32_t RNG_WEIGHT_WEAK=3;
const uint32_t RNG_WEIGHT_NORMAL=6;
const uint32_t RNG_WEIGHT_STRONG=18;
const uint32_t RNG_WEIGHT_VERY_STRONG=256;

class RNG{
public:

    uint32_t Q[4096];
    uint32_t c;
    uint32_t u;

    RNG();
    RNG(uint32_t seed_value);

    void seed(uint32_t seed_value);

    uint32_t get_number();

    uint32_t random_range(uint32_t lownum,uint32_t highnum);

    //weight must be >0. weight must also be greater than 1 or the result will be unweighted.
    uint32_t weighted_random_range(uint32_t lownum,uint32_t highnum,uint32_t target,uint32_t weight);

    //Valid values for weight:
    //weak
    //normal
    //strong
    //very_strong
    uint32_t weighted_random_range(uint32_t lownum,uint32_t highnum,uint32_t target,std::string weight="normal");
};

#endif
