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

class parser;
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
        //小物品所在容器id
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
        BigObject(int id, int location = UNKNOWN, string sort = "") : Object(location, sort, id) {}
        BigObject(shared_ptr<Object> obj) : Object(*obj) {}

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

        void DeleteObjectInside(shared_ptr<SmallObject> target)
        {
            for (int i = 0; i < smallObjectsInside.size(); i++)
            {
                if (smallObjectsInside[i]->id == target->id)
                {
                    smallObjectsInside.erase(smallObjectsInside.begin() + i);
                }
            }
        }

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

        int cost;
        int score;

        int hold_id = NONE, plate_id = NONE;

        Robot(int id, int location = UNKNOWN) : Object(id, "robot", location) {}
        Robot() : Robot(0) {}

        void SetHold(const shared_ptr<SmallObject> &hold)
        {
            this->hold = hold;
            if (hold != nullptr)
            {
                this->hold->location = location;
                hold_id = hold->id;
                hold->inside = NONE;
            }
            else
                hold_id = NONE;
        }

        void SetPlate(const shared_ptr<SmallObject> &plate)
        {

            this->plate = plate;
            if (plate != nullptr)
            {
                this->plate->location = location;
                plate_id = plate->id;
                plate->inside = NONE;
            }
            else
                plate_id = NONE;
        }

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

        string ToString() const
        {
            return "(Sort:" + sort + ",Color:" + color + ")";
        }
        bool IsObjectSatisfy(const shared_ptr<Object> &target) const;
    };

    class ATRI : public Plug,
                 public Robot,
                 public enable_shared_from_this<ATRI>
    {
    public:
        ATRI();
        void Init(int argc, char **argv);

        void Plan();
        shared_ptr<BigObject> human;
        // 场景中所有Object,id为索引
        vector<shared_ptr<Object>> objects;
        //场景中所有SmallObject
        vector<shared_ptr<SmallObject>> smallObjects;

        //需完成任务list
        vector<Instruction> tasks;
        //补充 info list
        vector<Instruction> infos;
        //禁止任务 约束list
        vector<Instruction> not_taskConstrains;
        //禁止出现的状态 约束list
        vector<Instruction> not_infoConstrains;
        //必须维护的约束list
        vector<Instruction> notnot_infoConstrains;

        // 总分
        int score;

        bool isKeepConstrain = 0;
        bool isAskTwice = 0;
        //是否开启纠错模式
        bool isErrorCorrection = 0;
        //是否开启自然语言处理
        bool isNaturalParse = 0;

        bool isPass = false;
        // 位置物品正确性标识
        vector<bool> posCorrectFlag;

        //环境解析
        bool ParseEnv(const string &env);
        //指令解析
        bool ParseInstruction(const string &task);
        //自然语言解析
        void ParseNaturalLanguage(const string &src);
        bool ParseNaturalLanguageSentence(const string &s);
        parser *nlp_parser;
        vector<string> errorlist;

        //优化task
        vector<Instruction> TaskOptimization();

        //根据参数执行动作
        bool DoBehavious(const string &behavious, unsigned int x);
        bool DoBehavious(const string &behavious, unsigned int x, unsigned int y);

        //行动时更新task list
        void UpdateTaskList(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y = nullptr);

        //判断是否触发禁止的行动约束
        bool IsInvokeNot_TaskConstracts(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y = nullptr);
        //判断是否触发禁止出现状态约束
        bool IsInvokeNot_infoConstracts(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y = nullptr);
        //判断是否触发必须维护约束
        bool IsInvokeNotnot_infoConstracts(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y = nullptr);

        //解析补充info list
        void ParseInfo(const Instruction &info);

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

#pragma region override_ATRI_Behavious
        /**
         * Recursion action Move
         * @param x location number
         * @return if the action is successful or not
         */
        bool virtual Move(unsigned int x) override;

        /**
         * Recursion action PickUp
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual PickUp(unsigned int a) override;

        /**
         * Recursion action PutDown
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual PutDown(unsigned int a) override;

        /**
         * Recursion action ToPlate
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual ToPlate(unsigned int a) override;

        /**
         * Recursion action FromPlate
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual FromPlate(unsigned int a) override;

        /**
         * Recursion action Open
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual Open(unsigned int a) override;

        /**
         * Recursion action Close
         * @param a object number
         * @return if the action is successful or not
         */
        bool virtual Close(unsigned int a) override;

        /**
         * Recursion action PutIn
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        bool virtual PutIn(unsigned int a, unsigned int b) override;

        /**
         * Recursion action TakeOut
         * @param a small object number
         * @param b big object number
         * @return if the action is successful or not
         */
        bool virtual TakeOut(unsigned int a, unsigned int b) override;
        /**
         * Recursion action AskLoc
         * @param a object number
         * @return "not_known" means don't know where 'a' is
         *  "(on a b)", "(near a b)", or "(inside a b)" means
         *  the spacial relation with 'b'
         */
        std::string virtual AskLoc(unsigned int a) override;

        // 检测当前位置的准确物品信息，包含检测容器内物品，开销：1，4（检测容器内物品）
        void Sense();

        bool HoldSmallObject(unsigned int a);

        void GetSmallObjectStatus(unsigned int a);

        /// @brief Put a on b
        /// @param a small object
        /// @param b big object
        bool PutOn(unsigned int a, unsigned int b);

#pragma endregion
    }; // Plug

    class Instruction
    {
    public:
        string behave;
        Condition conditionX, conditionY;
        vector<shared_ptr<Object>> X, Y;
        bool isUseY = false;
        bool isEnable = true;
        Instruction();
        Instruction(const shared_ptr<SyntaxNode> &node, const shared_ptr<ATRI> &atri);
        void SearchConditionObject(const shared_ptr<ATRI> &atri, bool is_every = false);
        void TaskSelfOptimization(const shared_ptr<ATRI> &atri);
        __inline__ __attribute__((__always_inline__)) bool IsInstructionInvoke(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y = nullptr);

        string ToString() const
        {
            return "Behave:" MAGENTA + behave + RESET "\nConditionX:" MAGENTA + conditionX.ToString() + RESET "\nConditionY:" MAGENTA + conditionY.ToString() + RESET "\n";
        }

    private:
    };

} //_home
