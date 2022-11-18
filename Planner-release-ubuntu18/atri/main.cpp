/*
 * Simulation@Home Competition
 * File: main.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */
#include "atri.hpp"

int main(int argc, char **argv)
{
    auto atri = make_shared<_home::ATRI>();
    atri->Init(argc, argv);
    atri->Run();
    return 0;
    /*
    _home::ATRI atri;
    atri.Init();
    atri.Run();
    return 0;
    */
}
