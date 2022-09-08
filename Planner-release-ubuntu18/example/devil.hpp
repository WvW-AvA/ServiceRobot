/*
 * Simulation@Home Competition
 * File: devil.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_devil_HPP__
#define __home_devil_HPP__

#include "cserver/plug.hpp"

namespace _home
{

    class Devil : public Plug
    {
    public:
        Devil();

    protected:
        void Plan();

        void Fini();
    }; // Plug

} //_home

#endif //__home_devil_HPP__
// end of file
