/*
 * File: main.cpp
 * Author : ShiQiao Chen(陈世侨)
 * Affiliation: WuHan University of Technology
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
