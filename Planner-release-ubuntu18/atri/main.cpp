/*
 * Simulation@Home Competition
 * File: main.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */
#include "atri.hpp"

int main()
{
    auto atri = make_shared<_home::ATRI>();
    atri->Init();
    atri->Run();
    return 0;
    /*
    _home::ATRI atri;
    atri.Init();
    atri.Run();
    return 0;
    */
}
