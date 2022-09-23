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
#include "debuglog.hpp"

using namespace std;

#define UNKNOWN -1
#define NONE 0
namespace _home
{
    class Instruction;
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
        int isOpen;
        Container(int id, int location = UNKNOWN, bool isOpen = true, string sort = "") : BigObject(id, location, sort), isOpen(isOpen) {}
        Container(shared_ptr<Object> obj) : BigObject(obj), isOpen(UNKNOWN) {}
        Container(shared_ptr<BigObject> obj) : BigObject(*obj), isOpen(UNKNOWN) {}
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

        int hold_id = NONE, plate_id = NONE;

        Robot(int id, int location = UNKNOWN) : Object(id, "robot", location) {}
        Robot() : Robot(0) {}
        virtual string ToString() override
        {
            return Object::ToString() + "hold:\n" + (hold != nullptr ? hold->ToString() : "") + "plate:\n" + (plate != nullptr ? plate->ToString() : "");
        }
        ~Robot() {}
    };

    //安全的Object转换，保证返回不为nullptr
    template <class T>
    __inline__ __attribute__((always_inline)) shared_ptr<T> ObjectPtrCast(const shared_ptr<Object> &obj)
    {
        auto p = dynamic_pointer_cast<T>(obj);
        if (p == nullptr)
        {
            LOG_ERROR("Object (%d %s) cast error.", obj->id, obj->sort.c_str());
            backtrace();
            throw("Abort");
        }
        return p;
    }

    struct SyntaxNode
    {
        string value;
        vector<shared_ptr<SyntaxNode>> sons;
    };
    struct Condition
    {
        string sort = "";
        string color = "";

        string ToString() const;
        bool IsObjectSatisfy(const shared_ptr<Object> &target) const;
    };

    class ATRI : public Plug,
                 public Robot,
                 public enable_shared_from_this<ATRI>
    {
    public:
        ATRI();
        void Init();

        void Plan();
        // 场景中所有Object,id为索引
        vector<shared_ptr<Object>> objects;
        //场景中所有SmallObject
        vector<shared_ptr<SmallObject>> smallObjects;
        //场景中所有Container
        vector<shared_ptr<Container>> containers;
        //场景中所有BigObject
        vector<shared_ptr<BigObject>> bigObjects;

        //需完成任务list
        vector<Instruction> tasks;
        //补充 info list
        vector<Instruction> infos;
        //禁止行动 约束list
        vector<Instruction> not_taskConstrains;
        //禁止出现的状态 约束list
        vector<Instruction> not_infoConstrains;
        //必须维护的约束list
        vector<Instruction> notnot_infoConstrains;

        //环境解析
        bool ParseEnv(const string &env);
        //指令解析
        bool ParseInstruction(const string &task);

        //根据参数执行动作
        bool DoBehavious(const string &behavious, unsigned int x);
        bool DoBehavious(const string &behavious, unsigned int x, unsigned int y);

        //解析补充info list
        void ParseInfo(const Instruction &info);
        //完成一项任务
        void SolveTask(const Instruction &task);

        //测试原子行为
        void TestAutoBehave();
        //输出所有指令
        void PrintInstruction();
        //输出场景信息
        void PrintEnv();

        void Fini();

    private:
        bool ParseEnvSentence(const string &str);

#pragma region override_ATRI_AtomBehavious
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
#pragma endregion
    }; // Plug
    class Instruction
    {
    public:
        string behave;
        Condition conditionX, conditionY;
        vector<shared_ptr<Object>> X, Y;
        bool isUseY = false;
        Instruction(const shared_ptr<SyntaxNode> &node, const shared_ptr<ATRI> &atri);
        void SearchConditionObject(const shared_ptr<ATRI> &atri);
        string ToString() const;

    private:
    };

} //_home
