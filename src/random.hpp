//
//  random.hpp
//  TJE_XCODE
//
//  Created by manu on 27/6/23.
//  Copyright © 2023 manu. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <random>

int get_random_sign()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> dis(0, 1); // range [0, 1)
    return (dis(e) > .5f) ? -1 : 1;
}

float get_random_dist()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> dis(750, 1000); // range [400, 800)
    static std::normal_distribution<float> norm(300, 75);
    return dis(e) + get_random_sign()*norm(e);
}

int get_random_enemy_num(int diff)
{
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> dis(1, 20); // range [1, 20]
    return dis(e) + diff; // in essence a gaussian area surrounding the spawn square to distribute enemies evenly
}

float get_random_spd()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> dis(20.f, 50.f); // range [min, max)
    return dis(e);
}

float get_random_cdShot()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> dis(0.3f, 3.f); // range [min, max)
    return dis(e);
}

float get_random_disp()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<float> dis(0.3f); // range [min, max)
    return dis(e);
}

float get_random_dispBullet(float val)
{
    std::default_random_engine e;
    std::uniform_real_distribution<float> dis(-val, val); // range [min, max)
    return dis(e);
}

int get_random_dir()
{
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> dis(-5, 5); // range [-5, 5]
    return dis(e);
}
