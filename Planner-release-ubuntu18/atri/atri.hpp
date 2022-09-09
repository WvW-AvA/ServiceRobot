/*
 * Simulation@Home Competition
 * File: atri.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_atri_HPP__
#define __home_atri_HPP__

#include "cserver/plug.hpp"
#include "string"
#include "unordered_map"
using namespace std;
#define UNKNOWN -1
namespace _home
{
    class Object;
    class SmallObject;
    class BigObject;
    class Container;

    class ATRI : public Plug
    {
    public:
        ATRI();

    protected:
        void Plan();
        unordered_map<int, shared_ptr<SmallObject>> smallObjects;
        unordered_map<int, shared_ptr<Container>> containers;
        unordered_map<int, shared_ptr<BigObject>> bigObjects;
        Robot robot;
        void PraseEnv(const string &env);
        void PraseEnvSentence(const string &str);
        void Fini();
    }; // Plug

    class Object
    {
    public:
        int location;
        int id;
        Object(int id, int location = UNKNOWN) : location(location), id(id) {}
        Object(int id) : id(id) {}
        ~Object() {}

    private:
        Object();
    };

    class Robot : Object
    {
    public:
        int hold;
        int plate;

        Robot(int id, int location = UNKNOWN, int hold = UNKNOWN, int plate = UNKNOWN) : Object(location, id), hold(hold), plate(plate) {}
        Robot() : Robot(0) {}
        ~Robot() {}
    };
    class SmallObject : Object
    {
    public:
        string sort;
        string color;
        SmallObject(int id, int location = UNKNOWN, string sort = "") : Object(location, id), sort(sort) {}
    };

    class BigObject : Object
    {
    public:
        string sort;
        vector<shared_ptr<SmallObject>> smallObjectsOn;
        BigObject(int id, int location = UNKNOWN, string sort = "") : Object(location, id), sort(sort) {}
    };

    class Container : Object
    {
    public:
        string sort;
        vector<shared_ptr<SmallObject>> smallObjectsInside;
        bool isOpen;
        Container(int id, int location = UNKNOWN, bool isOpen = true, string sort = "") : Object(location, id), sort(sort), isOpen(isOpen) {}
    };
} //_home

#endif //__home_atri_HPP__
// end of file
