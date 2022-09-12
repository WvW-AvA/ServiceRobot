/*
 * Simulation@Home Competition
 * File: atri.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */
#pragma once

#include "cserver/plug.hpp"
#include "string"
#include "unordered_map"
using namespace std;
#define UNKNOWN -1
namespace _home
{
    class Object
    {
    public:
        int location;
        int id;
        string sort = "";
        Object(int id, const string &sort = "", int location = UNKNOWN) : sort(sort), location(location), id(id) {}
        Object();
        virtual string ToString()
        {
            return "id:" + to_string(id) + "    at:" + to_string(location) + "     sort:" + sort + "\n";
        }
        ~Object() {}
    };

    class Robot : public Object
    {
    public:
        int hold;
        int plate;

        Robot(int id, int location = UNKNOWN, int hold = UNKNOWN, int plate = UNKNOWN) : Object(id, "robot", location), hold(hold), plate(plate) {}
        Robot() : Robot(0) {}
        virtual string ToString() override
        {
            return Object::ToString() + "hold:" + to_string(hold) + "     plate:" + to_string(plate) + "\n";
        }
        ~Robot() {}
    };
    class SmallObject : public Object
    {
    public:
        string color = "";
        int inside = UNKNOWN;
        SmallObject(int id, int location = UNKNOWN, const string &sort = "", const string &color = "") : Object(location, sort, id), color(color) {}
        SmallObject(shared_ptr<Object> obj) : Object(*obj) {}
        virtual string ToString() override
        {
            return Object::ToString() + "color:" + color + "    inside:" + to_string(inside) + "\n";
        }
        ~SmallObject() {}
    };

    class BigObject : public Object
    {
    public:
        vector<shared_ptr<SmallObject>> smallObjectsOn;
        BigObject(int id, int location = UNKNOWN, string sort = "") : Object(location, sort, id) {}
        BigObject(shared_ptr<Object> obj) : Object(*obj) {}
        virtual string ToString() override
        {
            string out = Object::ToString() + "Small Objects On:\n";
            for (int i = 0; i < smallObjectsOn.size(); i++)
            {
                out += to_string(i) + " " + smallObjectsOn[i]->ToString();
            }
            return out;
        }
        ~BigObject() {}
    };

    class Container : public BigObject
    {
    public:
        vector<shared_ptr<SmallObject>> smallObjectsInside;
        bool isOpen;
        Container(int id, int location = UNKNOWN, bool isOpen = true, string sort = "") : BigObject(id, location, sort), isOpen(isOpen) {}
        Container(shared_ptr<Object> obj) : BigObject(obj) {}
        Container(shared_ptr<BigObject> obj) : BigObject(*obj) {}
        virtual string ToString() override
        {
            string out = BigObject::ToString() + "Small Objects Inside:\n";
            for (int i = 0; i < smallObjectsInside.size(); i++)
            {
                out += to_string(i) + " " + smallObjectsInside[i]->ToString();
            }
            return out;
        }
        ~Container() {}
    };

    class ATRI : public Plug
    {
    public:
        ATRI();

    protected:
        void Plan();
        vector<shared_ptr<Object>> objects;
        vector<shared_ptr<SmallObject>> smallObjects;
        vector<shared_ptr<Container>> containers;
        vector<shared_ptr<BigObject>> bigObjects;
        shared_ptr<Robot> robot;
        bool PraseEnv(const string &env);
        bool PraseEnvSentence(const string &str);
        void Fini();
    }; // Plug

} //_home

// end of file
