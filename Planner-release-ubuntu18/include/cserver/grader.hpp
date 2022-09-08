/*
 * Simulation@Home Competition
 * File: grader.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_grader_HPP__
#define __home_grader_HPP__

#include <string>
#include <vector>
#include "message.hpp"

namespace _home
{
    /**
     * @class A common entry for the evaluation of one test.
     */
    class Grader
    {
    public:
        /** 
         * Destructor
         */
        virtual ~Grader() {}

        /** 
         * Initializes the Grader.
         * @param arg is the arguments for the initialization of the Grader
         */
        virtual bool Init(const std::string& arg) = 0;

        /**
         * Begin the evaluation.
         * @param es means the environment description.
         * @param ts means the task description.
         * @param team_name as it looks like
         * @param mislead whether gives the misleading answers when asked
         */
        virtual void BeginEvaluation(
            const std::string& es,
            const std::string& ts,
            const std::string& team_name,
            bool mislead) = 0;

        /** 
         * Evaluate the action Move
         * @param x location number
         * @return if the action is successful or not
         */
        virtual bool EvaluateMove(unsigned int x) = 0;

        /** 
         * Evaluate the action PickUp
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluatePickUp(unsigned int a) = 0;

        /** 
         * Evaluate the action PutDown
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluatePutDown(unsigned int a) = 0;

        /** 
         * Evaluate the action ToPlate
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluateToPlate(unsigned int a) = 0;

        /** 
         * Evaluate the action FromPlate
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluateFromPlate(unsigned int a) = 0;

        /** 
         * Evaluate the action Open
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluateOpen(unsigned int a) = 0;

        /** 
         * Evaluate the action Close
         * @param a object number
         * @return if the action is successful or not
         */
        virtual bool EvaluateClose(unsigned int a) = 0;

        /** 
         * Evaluate the action PutIn
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        virtual bool EvaluatePutIn(unsigned int a, unsigned int b) = 0;

        /** 
         * Evaluate the action TakeOut
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        virtual bool EvaluateTakeOut(unsigned int a, unsigned int b) = 0;

        /** 
         * Evaluate the action AskLoc
         * @param a object number
         * @return "failure" means the action is failed
         *  "unknown" means don't know where 'a' is
         *  "(on a b)", "(near a b)", or "(inside a b)" means
         *  the spacial relation with 'b'
         */
        virtual std::string EvaluateAskLoc(unsigned int a) = 0;

        /**
         * Evaluate the action Sense
         * @param A_ return set of numbers for the object observed
         */
        virtual void EvaluateSense(std::vector<unsigned int>& A_) = 0;

        /** 
         * End of the evaluation.
         * @param time the elapsed time for the planning
         * @return the score that the client finally gets.
         */
        virtual int EndEvaluation(double time) = 0;
    };//Grader

}//_home

#ifdef _MSC_VER
#define SHARED_LIB_EXPORT __declspec(dllexport)
#else
#define SHARED_LIB_EXPORT
#endif

#define EXPORT(class_name)\
    extern "C"\
    {\
        SHARED_LIB_EXPORT void ExportGrader(Grader* &obj)\
        {\
            obj = new class_name;\
        }\
    }

#endif//__home_grader_HPP__
//end of file
