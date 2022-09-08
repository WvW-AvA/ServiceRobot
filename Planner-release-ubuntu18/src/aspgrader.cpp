/*
 * Simulation@Home Competition
 * File: aspgrader.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "aspgrader.hpp"
#include <iostream>

using namespace boost;
using namespace std;
using namespace _home;

//////////////////////////////////////////////////////////////////////////
bool ASPGrader::Init(const std::string& arg)
{
    // TODO: get the path of iclingo from arg
    return true;
}

void ASPGrader::BeginEvaluation(
                                const std::string& es,
                                const std::string& ts,
                                const std::string& team_name,
                                bool mislead)
{
    if (mEvaluator.init_et(es.c_str(), es.size()) == 0 ||
        mEvaluator.init_it(ts.c_str(), ts.size()) == 0)
    {
        cout << "#(ASPGrader) Failed to begin the evaluation" << endl;
        return;
    }
    mEvaluator.newteam(team_name.c_str(), mislead);
}

EXPORT(ASPGrader);
