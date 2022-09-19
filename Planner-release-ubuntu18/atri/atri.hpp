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

#define RESET "\033[0m"
#define BLACK "\033[30m"  /* Black */
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m"   /* Blue */

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

    class Robot : public Object
    {
    public:
        shared_ptr<SmallObject> hold;
        shared_ptr<SmallObject> plate;

        int hold_id, plate_id;

        Robot(int id, int location = UNKNOWN) : Object(id, "robot", location) {}
        Robot() : Robot(0) {}
        virtual string ToString() override
        {
            return Object::ToString() + "hold:\n" + hold->ToString() + "plate:\n" + plate->ToString();
        }
        ~Robot() {}
    };

    struct SyntaxNode
    {
        string value;
        vector<shared_ptr<SyntaxNode>> sons;
    };

    struct Condition
    {
        string sort = "";
        string color = "";

        string ToString();
    };
    class Instruction
    {
        class ATRI;
        string behave;
        Condition conditionX, conditionY;
        vector<shared_ptr<Object>> X, Y;

    public:
        Instruction(const shared_ptr<SyntaxNode> &node);
        void SearchConditionObject(const shared_ptr<ATRI> &atri);
        string ToString();

    private:
    };

    class ATRI : public Plug,
                 public Robot,
                 public enable_shared_from_this<ATRI>
    {
    public:
        ATRI();
        void Init();

    protected:
        void Plan();
        vector<shared_ptr<Object>> objects;
        vector<shared_ptr<SmallObject>> smallObjects;
        vector<shared_ptr<Container>> containers;
        vector<shared_ptr<BigObject>> bigObjects;

        vector<Instruction> tasks;
        vector<Instruction> not_taskConstrains;
        vector<Instruction> not_infoConstrains;
        vector<Instruction> notnot_infoCosntrains;

        bool PraseEnv(const string &env);
        bool PraseInstruction(const string &task);
        void PrintEnv();
        void Fini();

    private:
        bool PraseEnvSentence(const string &str);
        /**
         * Atomic action Move
         * @param x location number
         * @return if the action is successful or not
         */
        bool virtual Move(unsigned int x) override;

        /**
         * Atomic action PickUp
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual PickUp(unsigned int a) override;

        /**
         * Atomic action PutDown
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual PutDown(unsigned int a) override;

        /**
         * Atomic action ToPlate
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual ToPlate(unsigned int a) override;

        /**
         * Atomic action FromPlate
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual FromPlate(unsigned int a) override;

        /**
         * Atomic action Open
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual Open(unsigned int a) override;

        /**
         * Atomic action Close
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual Close(unsigned int a) override;

        /**
         * Atomic action PutIn
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        bool virtual PutIn(unsigned int a, unsigned int b) override;

        /**
         * Atomic action TakeOut
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        bool virtual TakeOut(unsigned int a, unsigned int b) override;
    }; // Plug

} //_home

// end of file
