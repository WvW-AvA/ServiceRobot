/*
 * Simulation@Home Competition
 * File: aspgrader.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_aspgrader_HPP__
#define __home_aspgrader_HPP__

#include "cserver/grader.hpp"
#include "evaluate.h"

namespace _home
{
    class ASPGrader : public Grader
    {
    public:
        bool Init(const std::string& arg);

        void BeginEvaluation(
            const std::string& es,
            const std::string& ts,
            const std::string& team_name,
            bool mislead);

        inline bool EvaluateMove(unsigned int x)
        {
            return mEvaluator.EvaluateMove(x);
        }

        inline bool EvaluatePickUp(unsigned int a)
        {
            return mEvaluator.EvaluatePickUp(a);
        }

        inline bool EvaluatePutDown(unsigned int a)
        {
            return mEvaluator.EvaluatePutDown(a);
        }

        inline bool EvaluateToPlate(unsigned int a)
        {
            return mEvaluator.EvaluateToPlate(a);
        }

        inline bool EvaluateFromPlate(unsigned int a)
        {
            return mEvaluator.EvaluateFromPlate(a);
        }

        inline bool EvaluateOpen(unsigned int a)
        {
            return mEvaluator.EvaluateOpen(a);
        }

        inline bool EvaluateClose(unsigned int a)
        {
            return mEvaluator.EvaluateClose(a);
        }

        inline bool EvaluatePutIn(unsigned int a, unsigned int b)
        {
            return mEvaluator.EvaluatePutIn(a, b);
        }

        inline bool EvaluateTakeOut(unsigned int a, unsigned int b)
        {
            return mEvaluator.EvaluateTakeOut(a, b);
        }

        inline std::string EvaluateAskLoc(unsigned int a)
        {
            return mEvaluator.EvaluateAskLoc(a);
        }

        inline void EvaluateSense(std::vector<unsigned int>& A_)
        {
            mEvaluator.EvaluateSense(A_);
        }

        inline int EndEvaluation(double time)
        {
            return mEvaluator.EndEvaluation(time);
        }

    private:
        Evaluate mEvaluator;
    };//ASPGrader
}//_home

#endif//__home_aspgrader_HPP__
