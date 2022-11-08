#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

static int totalScore = 0;

void split_string(vector<string> &out, const string &str_source, char mark);
ostream &operator<<(ostream &os, shared_ptr<Object> obj);
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn);
ostream &operator<<(ostream &os, const Instruction &instr);

ATRI::ATRI() : Plug("ATRI") {}

void ATRI::Init()
{
    score = 0;
    objects.push_back(shared_from_this());
    if (isErrorCorrection)
        posCorrectFlag.push_back(false);
    else
        posCorrectFlag.push_back(true);
}

void ATRI::Plan()
{

    if (ParseEnv(GetEnvDes()) == false)
    {
        LOG_ERROR("Env Prase Error");
        return;
    }
    ParseInstruction(GetTaskDes());
    // info补充
    for (auto v : infos)
        ParseInfo(v);

    tasks = TaskOptimization();

    //找到human
    for (auto o : objects)
        if (o->sort == "human")
            human = ObjectPtrCast<BigObject>(o);

    PrintEnv();
    // PrintInstruction();

    for (auto t = 0; t < tasks.size(); t++)
    {
        SolveTask(tasks[t]);
        PrintEnv();
        cout << endl;
    }
    // TestAutoBehave();
}

bool ATRI::ParseInstruction(const string &taskDis)
{
    shared_ptr<SyntaxNode> root = make_shared<SyntaxNode>();
    vector<shared_ptr<SyntaxNode>> leaf_path;
    shared_ptr<SyntaxNode> curr_leaf = root;
    leaf_path.push_back(curr_leaf);
    int tag1 = 0, tag2 = 0;
    for (int i = 0; i < taskDis.size(); i++)
    {
        if (taskDis[i] == '(')
        {
            curr_leaf->value += *(taskDis.substr(tag1, i - tag1).end() - 1) == ' ' ? taskDis.substr(tag1, i - tag1 - 1) : taskDis.substr(tag1, i - tag1);
            tag1 = i + 1;
            auto p = make_shared<SyntaxNode>();
            curr_leaf->sons.push_back(p);
            curr_leaf = p;
            leaf_path.push_back(curr_leaf);
        }
        else if (taskDis[i] == ')')
        {
            curr_leaf->value += *(taskDis.substr(tag1, i - tag1).end() - 1) == ' ' ? taskDis.substr(tag1, i - tag1 - 1) : taskDis.substr(tag1, i - tag1);
            tag1 = i + 1;
            curr_leaf = *(leaf_path.end() - 2);
            leaf_path.pop_back();
        }
    }
    for (auto v : root->sons[0]->sons)
    {
        if (v->value == ":task")
            tasks.push_back(Instruction(v, shared_from_this()));
        else if (v->value == ":cons_not")
        {
            if (v->sons[0]->value == ":task")
                not_taskConstrains.push_back(Instruction(v->sons[0], shared_from_this()));
            else if (v->sons[0]->value == ":info")
                not_infoConstrains.push_back(Instruction(v->sons[0], shared_from_this()));
        }
        else if (v->value == ":cons_notnot")
        {
            if (v->sons[0]->value == ":info")
                notnot_infoConstrains.push_back(Instruction(v->sons[0], shared_from_this()));
        }
        else if (v->value == ":info")
        {
            infos.push_back(Instruction(v, shared_from_this()));
        }
    }
    return true;
}
bool ATRI::ParseEnvSentence(const string &str)
{
    int pos = 1;
    vector<string> words;
    for (int i = 1; i < str.size(); i++)
    {
        if (str[i] == ' ' || str[i] == ')')
        {
            words.push_back(string(&str[pos], &str[i]));
            pos = i + 1;
        }
    }
    if (words.size() != 2 && words.size() != 3)
    {
        LOG_ERROR("Env Sentence error");
        return false;
    }
    if (words[0] == "hold")
    {
        this->hold_id = stoi(words[1]);
    }
    else if (words[0] == "plate")
    {
        this->plate_id = stoi(words[1]);
    }
    else
    {
        int ind = stoi(words[1]);
        int last_size = objects.size();
        while (ind >= last_size)
        {
            objects.push_back(make_shared<Object>(last_size++));
            if (isErrorCorrection)
                posCorrectFlag.push_back(false);
            else
                posCorrectFlag.push_back(true);
        }
        if (words[0] == "opened")
        {
            auto p = dynamic_pointer_cast<Container>(objects[ind]);
            if (p == nullptr)
            {
                auto p = make_shared<Container>(objects[ind]);
                objects[ind] = p;
            }
            p->isOpen = true;
        }
        else if (words[0] == "closed")
        {
            auto p = dynamic_pointer_cast<Container>(objects[ind]);
            if (p == nullptr)
            {
                auto p = make_shared<Container>(objects[ind]);
                objects[ind] = p;
            }
            p->isOpen = false;
        }
        else if (words[0] == "at")
        {
            objects[ind]->location = stoi(words[2]);
        }
        else if (words[0] == "sort")
        {
            objects[ind]->sort = words[2];
        }
        else if (words[0] == "size")
        {
            if (words[2] == "big")
            {
                if (dynamic_pointer_cast<BigObject>(objects[ind]) == nullptr)
                {
                    auto p = make_shared<BigObject>(objects[ind]);
                    objects[ind] = p;
                }
            }
            else if (words[2] == "small")
            {
                if (dynamic_pointer_cast<SmallObject>(objects[ind]) == nullptr)
                {
                    auto p = make_shared<SmallObject>(objects[ind]);
                    smallObjects.push_back(p);
                    objects[ind] = p;
                }
            }
        }
        else if (words[0] == "color")
        {
            auto p = dynamic_pointer_cast<SmallObject>(objects[ind]);
            if (p == nullptr)
            {
                p = make_shared<SmallObject>(objects[ind]);
                smallObjects.push_back(p);
                objects[ind] = p;
            }
            p->color = words[2];
        }
        else if (words[0] == "inside")
        {
            auto p = dynamic_pointer_cast<SmallObject>(objects[ind]);
            if (p == nullptr)
            {
                p = make_shared<SmallObject>(objects[ind]);
                smallObjects.push_back(p);
                objects[ind] = p;
            }
            p->inside = stoi(words[2]);
        }
        else if (words[0] == "type")
        {
            if (words[2] == "container")
            {
                if (dynamic_pointer_cast<Container>(objects[ind]) == nullptr)
                {
                    auto p = make_shared<Container>(objects[ind]);
                    objects[ind] = p;
                }
            }
        }
    }
    return true;
}
bool ATRI::ParseEnv(const string &env)
{
    regex reg("\\(.*?\\)");
    cmatch m;
    auto pos = env.data() + 1;
    auto end = env.data() + env.size();
    for (; regex_search(pos, end, m, reg); pos = m.suffix().first)
    {
        string str = m.str();
        if (ParseEnvSentence(str) == false)
        {
            LOG_ERROR("Parse Env Sentence ERROR");
            return false;
        }
    }

    if (hold_id > 0)
        SetHold(ObjectPtrCast<SmallObject>(objects[hold_id]));
    if (plate_id > 0)
        SetPlate(ObjectPtrCast<SmallObject>(objects[plate_id]));

    for (const auto &s : smallObjects)
    {
        if (s == plate || s == hold)
            continue;
        if (s->inside != UNKNOWN && s->inside != NONE)
        {
            auto p = dynamic_pointer_cast<Container>(objects[s->inside]);
            if (p != nullptr)
            {
                p->smallObjectsInside.push_back(s);
                s->location = p->location;
            }
            else
            {
                s->location = UNKNOWN;
                s->inside = UNKNOWN;
            }
        }
        else if (s->location != UNKNOWN)
            s->inside = NONE;
    }

    // for (auto v : objects)
    //    cout << v;

    return true;
}
void ATRI::ParseInfo(const Instruction &info)
{
    if (info.behave == "on")
    {
        for (auto v : info.X)
        {
            v->location = info.Y[0]->location;
        }
    }
    else if (info.behave == "near")
    {
        if (info.Y[0]->location != UNKNOWN)
            for (auto v : info.X)
                v->location = info.Y[0]->location;
        else if (info.X[0]->location != UNKNOWN)
        {
            for (auto v : info.Y)
                v->location = info.X[0]->location;
        }
    }
    else if (info.behave == "plate")
    {
        if (plate == nullptr)
        {
            plate = ObjectPtrCast<SmallObject>(info.X[0]);
            plate_id = info.X[0]->id;
        }
        else
        {
            LOG_ERROR("The plate already have small object (%d %s)", plate->id, plate->sort.c_str());
        }
    }
    else if (info.behave == "inside")
    {
        auto c = ObjectPtrCast<Container>(info.Y[0]);
        for (auto v : info.X)
        {
            auto p = ObjectPtrCast<SmallObject>(v);
            p->inside = c->id;
            c->smallObjectsInside.push_back(p);
        }
    }
    else if (info.behave == "opened")
    {
        for (auto v : info.X)
        {
            auto p = ObjectPtrCast<Container>(v);
            p->isOpen = true;
        }
    }
    else if (info.behave == "closed")
    {
        for (auto v : info.X)
        {
            auto p = ObjectPtrCast<Container>(v);
            p->isOpen = false;
        }
    }
}

vector<Instruction> ATRI::TaskOptimization()
{
    vector<Instruction> temp;
    for (auto &t : tasks)
    {
        t.TaskSelfOptimization(shared_from_this());
    }

    auto task_evalue = [](const string &behave) -> int
    {
        if (behave == "putin" || behave == "puton" || behave == "give")
            return 0;
        else if (behave == "takeout" || behave == "pickup")
            return 1;
        else if (behave == "open" || behave == "close")
            return 2;
        else if (behave == "putdown" || behave == "goto")
            return 3;
    };
    for (int i = 0; i < 4; i++)
        for (auto &t : tasks)
            if (task_evalue(t.behave) == i)
                temp.push_back(t);

    return temp;
}

bool ATRI::DoBehavious(const string &behavious, unsigned int x)
{
    if (behavious == "move" || behavious == "Move" || behavious == "Goto" || behavious == "goto")
        return Move(x);
    else if (behavious == "pickup" || behavious == "PickUp")
        return PickUp(x);
    else if (behavious == "close" || behavious == "Close")
        return Close(x);
    else if (behavious == "open" || behavious == "Open")
        return Open(x);
    else if (behavious == "fromplate" || behavious == "FromPlate")
        return FromPlate(x);
    else if (behavious == "toplate" || behavious == "ToPlate")
        return ToPlate(x);
    else if (behavious == "putdown" || behavious == "PutDown")
        return PutDown(x);
    else if (behavious == "AskLoc" || behavious == "askloc")
        return AskLoc(x) == "";
    else if (behavious == "GetStatus" || behavious == "getstatus")
        GetSmallObjectStatus(x);
    else if (behavious == "give" || behavious == "Give")
    {
        if (human != nullptr)
            return PutOn(x, human->location);
        else
            LOG_ERROR("There are not human in Sence");
    }
    return false;
}
bool ATRI::DoBehavious(const string &behavious, unsigned int x, unsigned int y)
{
    if (behavious == "putin" || behavious == "PutIn")
        return PutIn(x, y);
    else if (behavious == "takeout" || behavious == "TakeOut")
        return TakeOut(x, y);
    else if (behavious == "puton" || behavious == "PutOn")
        return PutOn(x, y);

    return false;
}

void ATRI::SolveTask(const Instruction &task)
{
    if (task.isEnable == false)
        return;
    cout << task;
    if (task.behave == "puton" || task.behave == "putin" || task.behave == "takeout")
    {
        if (task.X[0] == nullptr || task.Y[0] == nullptr)
        {
            LOG_ERROR("Instruction Error");
            throw("Abort");
        }
        for (auto a : task.X)
            DoBehavious(task.behave, a->id, task.Y[0]->id);
    }
    else
    {
        if (task.X[0] == nullptr)
        {
            LOG_ERROR("Instruction Error");
            throw("Abort");
        }
        DoBehavious(task.behave, task.X[0]->id);
    }
}

void ATRI::UpdateTaskList(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y)
{
    for (auto &t : tasks)
        if (t.IsInstructionInvoke(behave, x, y))
        {
            score += 40;
            t.isEnable = false;
        }
}
bool ATRI::IsInvokeNot_infoConstracts(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y)
{
    for (auto &i : not_infoConstrains)
    {
    }
}
bool ATRI::IsObeyNotnot_infoConstracts(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y)
{
    for (auto &i : notnot_infoConstrains)
    {
    }
}

void ATRI::PrintInstruction()
{
#ifdef __DUBUG__
    cout << "Task:\n";
    for (auto v : tasks)
        cout << v;
    cout << "\nInfo:\n";
    for (auto v : infos)
        cout << v;
    cout << "\nNot_Info:\n";
    for (auto v : not_infoConstrains)
        cout << v;
    cout << "\nNot_Task:\n";
    for (auto v : not_taskConstrains)
        cout << v;
    cout << "\nNotNot_Info:\n";
    for (auto v : notnot_infoConstrains)
        cout << v;
#endif
}
void ATRI::PrintEnv()
{
#ifdef __DEBUG__
    cout << "Current Score:" << score << endl;
    vector<vector<shared_ptr<Object>>> objPos;
    vector<shared_ptr<Object>> unknownPos;
    for (auto v : objects)
    {
        if (v->location == UNKNOWN)
        {
            unknownPos.push_back(v);
            continue;
        }
        if (v->location >= objPos.size())
            objPos.resize(v->location + 1);
        objPos[v->location].push_back(v);
    }
    for (int i = 0; i < objPos.size(); i++)
    {
        cout << "Pos " << (i < 10 ? " " : "") << i << ":" << posCorrectFlag[i] << ":";
        for (auto v : objPos[i])
        {
            if (v->sort == "robot")
                cout << GREEN << "(" << v->sort << " hold:" << hold_id << " plate:" << plate_id << ")" << RESET;
            else if (dynamic_pointer_cast<BigObject>(v))
            {
                auto p = dynamic_pointer_cast<BigObject>(v);
                cout << YELLOW << "(" << p->id << " " << p->sort;
                if (dynamic_pointer_cast<Container>(v))
                {
                    auto p = dynamic_pointer_cast<Container>(v);
                    cout << " inside:[";
                    for (int c = 0; c < p->smallObjectsInside.size(); c++)
                        cout << (c == 0 ? "" : ",") << p->smallObjectsInside[c]->id;
                    cout << "]" << p->isOpen;
                }
                cout << ")" << RESET;
            }
            else
                cout << "(" << v->id << " " << v->sort << ")";
        }
        cout << endl;
    }
    cout << "UnknownPos:";
    for (auto v : unknownPos)
    {
        cout << BLUE << "(" << v->id << " " << v->sort << ")" << RESET;
    }
    cout << endl;
#endif
}

void ATRI::Fini()
{
    totalScore += score;
    cout << "#(ATRI): Fini" << endl;
    cout << "Total score: " << totalScore << endl;
    human = nullptr;
    plate = nullptr;
    hold = nullptr;
    objects.clear();
    smallObjects.clear();
    tasks.clear();
    infos.clear();
    not_infoConstrains.clear();
    not_taskConstrains.clear();
    notnot_infoConstrains.clear();
    posCorrectFlag.clear();
    ATRI::Init();
}
void ATRI::TestAutoBehave()
{
    string inp;
    while (true)
    {
        getline(cin, inp);
        inp.push_back(' ');
        vector<string> temp;
        split_string(temp, inp, ' ');
        if (temp.size() == 1 && (temp[0] == "Sense" || temp[0] == "sense"))
            Sense();
        else if (temp.size() == 2)
            DoBehavious(temp[0], stoi(temp[1]));
        else if (temp.size() == 3)
            DoBehavious(temp[0], stoi(temp[1]), stoi(temp[2]));
        PrintEnv();
    }
}

#pragma region override_ATRI_Behavious
bool ATRI::TakeOut(unsigned int a, unsigned int b)
{
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    auto cont = ObjectPtrCast<Container>(objects[b]);
    if (hold == nullptr)
        if (small->inside == cont->id)
            if (location == objects[b]->location)
            {
                if (cont->isOpen)
                {
                    LOG("TakeOut(%d,%s)(%d,%s)", a, cont->sort.c_str(), b, small->sort.c_str());
                    score -= 2;
                    if (Plug::TakeOut(a, b) == false)
                    {
                        if (isErrorCorrection && posCorrectFlag[location] == false)
                        {
                            Sense();
                            return TakeOut(a, b);
                        }
                    }
                    small->inside = false;
                    cont->DeleteObjectInside(small);
                    SetHold(small);
                    UpdateTaskList("takeout", objects[a], objects[b]);
                    return true;
                }
                else
                    Open(b);
            }
            else
                Move(objects[b]->location);
        else
        {
            if (isErrorCorrection)
            {
                GetSmallObjectStatus(a);
                if (small->inside != cont->id)
                    PutIn(a, b);
            }
            else
                PutIn(a, b);
        }
    else
        PutDown(hold->id);
    TakeOut(a, b);
}
bool ATRI::PutIn(unsigned int a, unsigned int b)
{
    auto cont = ObjectPtrCast<Container>(objects[b]);
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    if (hold == small)
        if (cont->isOpen)
            if (location == cont->location)
            {
                LOG("PutIn(%d,%s)(%d,%s)", a, cont->sort.c_str(), b, small->sort.c_str());
                score -= 2;
                if (Plug::PutIn(a, b) == false)
                    return false;
                small->inside = b;
                SetHold(nullptr);
                cont->smallObjectsInside.push_back(small);
                UpdateTaskList("putin", objects[a], objects[b]);
                return true;
            }
            else
                Move(b);
        else
            Open(b);
    else
        HoldSmallObject(a);
    PutIn(a, b);
}
bool ATRI::Close(unsigned int a)
{
    shared_ptr<Container> container = ObjectPtrCast<Container>(objects[a]);
    if (container != nullptr)
        if (container->isOpen = true)
            if (location == container->location)
                if (hold == nullptr)
                {
                    LOG("Close(%d,%s)", a, container->sort.c_str());
                    score -= 2;
                    if (Plug::Close(a) == false)
                    {
                        if (isErrorCorrection)
                        {
                            container->isOpen = false;
                            LOG("(%d,%s) has closed", a, container->sort.c_str());
                            return true;
                        }
                        return false;
                    }
                    container->isOpen = false;
                    UpdateTaskList("close", objects[a]);
                    return true;
                }
                else
                    PutDown(hold->id);
            else
                Move(container->location);
        else
            return true;
    else
    {
        LOG_ERROR("Object (%d,%s) can't cast to Container", objects[a]->id, objects[a]->sort);
        return false;
    }
    Close(a);
}
bool ATRI::Open(unsigned int a)
{
    shared_ptr<Container> container = ObjectPtrCast<Container>(objects[a]);
    if (container != nullptr)
    {
        if (container->isOpen == false)
            if (location == container->location)
                if (hold == nullptr)
                {
                    LOG("Open(%d,%s)", a, container->sort.c_str());
                    score -= 2;
                    if (Plug::Open(a) == false)
                    {
                        if (isErrorCorrection)
                        {
                            container->isOpen = true;
                            LOG("(%d,%s) has opened", a, container->sort.c_str());
                            return false;
                        }
                        return false;
                    }
                    container->isOpen = true;
                    UpdateTaskList("Open", objects[a]);
                    return true;
                }
                else
                    PutDown(hold->id);
            else
                Move(container->location);
        else
            return true;
    }
    else
    {
        LOG_ERROR("Object (%d,%s) can't cast to Container", objects[a]->id, objects[a]->sort);
        return false;
    }
    Open(a);
}
bool ATRI::FromPlate(unsigned int a)
{

    if (hold == nullptr)
    {
        if (plate && plate->id == a)
        {
            LOG("FromPlate(%d,%s)", a, plate->sort.c_str());
            score -= 2;
            if (Plug::FromPlate(a) == false)
                return false;
            SetHold(plate);
            SetPlate(nullptr);
            return true;
        }
        else
            ToPlate(a);
    }
    else
        PutDown(hold->id);
    return FromPlate(a);
}
bool ATRI::ToPlate(unsigned int a)
{
    if (plate == nullptr)
        if (hold && hold->id == a)
        {
            LOG("ToPlate(%d,%s)", a, objects[a]->sort.c_str());
            score -= 2;
            if (Plug::ToPlate(a) == false)
                return false;
            SetPlate(hold);
            SetHold(nullptr);
            return true;
        }
        else
            HoldSmallObject(a);
    else
        FromPlate(plate->id);
    return ToPlate(a);
}
bool ATRI::PutDown(unsigned int a)
{
    if (hold && hold->id == a)
    {
        LOG("PutDown(%d,%s)", a, objects[a]->sort.c_str());
        score -= 2;
        if (Plug::PutDown(a) == false)
            return false;
        SetHold(nullptr);
        UpdateTaskList("purdown", objects[a]);
        return true;
    }
    else
    {
        HoldSmallObject(a);
        return PutDown(a);
    }
}
bool ATRI::PickUp(unsigned int a)
{
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    if (hold == nullptr)
        if (location == small->location)
            if (small->inside == NONE)
            {
                LOG("PickUp(%d,%s)", small->id, small->sort.c_str());
                score -= 2;
                if (Plug::PickUp(a) == false)
                {
                    if (isErrorCorrection && posCorrectFlag[location] == false)
                    {
                        Sense();
                        return PickUp(a);
                    }
                    else
                        return false;
                }
                SetHold(small);
                UpdateTaskList("pickup", objects[a]);
                return true;
            }
            else
            {
                TakeOut(small->id, small->inside);
            }
        else
        {
            if (small->location == UNKNOWN)
                GetSmallObjectStatus(small->id);
            Move(small->location);
        }
    else
        PutDown(hold->id);
    return PickUp(a);
}
bool ATRI::Move(unsigned int a)
{
    LOG("Move(%d)", a);
    score -= 4;
    if (Plug::Move(a) == false)
        return false;
    location = a;
    if (hold)
        hold->location = a;
    if (plate)
        plate->location = a;
    UpdateTaskList("goto", objects[a]);
    return true;
}

bool ATRI::PutOn(unsigned int a, unsigned int b)
{
    if (hold && hold->id == a)
        if (location == b)
        {
            PutDown(a);

            UpdateTaskList("puton", objects[a], objects[b]);
            if (objects[b]->sort == "human")
                UpdateTaskList("give", objects[a]);
            return true;
        }
        else
            Move(b);
    else
        HoldSmallObject(a);
    PutOn(a, b);
}
bool ATRI::HoldSmallObject(unsigned int a)
{
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    if (hold && hold->id == a)
        return true;
    if (plate && plate->id == a)
        return FromPlate(a);
    if (small->location == UNKNOWN || small->inside == UNKNOWN)
    {
        GetSmallObjectStatus(a);
        return HoldSmallObject(a);
    }
    else
    {
        if (small->inside != NONE && small->inside != UNKNOWN)
            return TakeOut(a, small->inside);
        else
            return PickUp(a);
    }
}
void ATRI::GetSmallObjectStatus(unsigned int a)
{
    string sureRet;
    if (isErrorCorrection)
    {
        vector<string> ret;
        //纠错模式下反复问，直到问出两次相同结果,认为正确。
        auto checkDouble = [&](const string &str) -> bool
        {
            for (const auto &v : ret)
                if (v == str)
                {
                    sureRet = str;
                    return true;
                }
            ret.push_back(str);
            return false;
        };
        while (checkDouble(AskLoc(a)) == false)
        {
        }
    }
    else
    {
        sureRet = AskLoc(a);
    }
    vector<string> split;
    regex reg("[a-z 0-9]+");
    cmatch m;
    const char *pos = sureRet.data();
    const char *end = sureRet.data() + sureRet.size();
    for (; regex_search(pos, end, m, reg); pos = m.suffix().first)
    {
        split.push_back(m.str());
    }
    auto small = ObjectPtrCast<SmallObject>(objects[stoi(split[1])]);
    if (split[0] == "inside")
    {
        auto cont = dynamic_pointer_cast<Container>(objects[stoi(split[2])]);
        if (cont == nullptr)
            return;
        small->location = cont->location;
        small->inside = cont->id;
        cont->smallObjectsInside.push_back(small);
    }
    else if (split[0] == "at")
    {
        small->location = stoi(split[2]);
        small->inside = NONE;
    }
    if (isErrorCorrection)
        posCorrectFlag[small->location] = false;
}
std::string ATRI::AskLoc(unsigned int a)
{
    string str;
    do
    {
        str = Plug::AskLoc(a);
        LOG("AskLoc(%d)", a);
        score -= 2;
        if (str == "")
        {
            LOG_ERROR("AskLoc return empty string,may object (%d,%s) not exsit!", a, objects[a]->sort.c_str());
            throw("Abort");
        }
    } while (str == "not_known");
    return str;
}
void ATRI::Sense()
{
    //只有纠错模式启动且当前位置未被纠错过才Sense
    if ((isErrorCorrection && posCorrectFlag[location] == true) || isErrorCorrection == false)
        return;

    if (isErrorCorrection)
        posCorrectFlag[location] = true;

    vector<unsigned int> A_, B_;
    shared_ptr<Container> container = nullptr;
    auto checkSmallObejct = [&A_](unsigned int id) -> bool
    {
        for (auto a : A_)
            if (a == id)
                return true;
        return false;
    };
    if (dynamic_pointer_cast<Container>(objects[location]) != nullptr)
    {
        container = dynamic_pointer_cast<Container>(objects[location]);
        Open(location);
    }
    Plug::Sense(A_);
    LOG("Sense");
    score--;
    //检查当前位置物品正确性
    for (auto s : smallObjects)
    {
        if (s->location == location && checkSmallObejct(s->id) == false)
        {
            s->location = UNKNOWN;
            if (container != nullptr && container->id == s->inside)
                container->DeleteObjectInside(s);
            s->inside = UNKNOWN;
        }
    }
    //更新当前位置物品，利用容器合上关闭时返回值不同，确定物品状态。
    for (auto a : A_)
    {
        auto small = dynamic_pointer_cast<SmallObject>(objects[a]);
        if (small == nullptr || small == hold || small == plate)
            continue;
        if (small->location != location)
        {
            if (small->inside > 0)
            {
                ObjectPtrCast<Container>(objects[small->inside])->DeleteObjectInside(small);
            }
            small->location = location;
        }
        if (container)
        {
            small->inside = UNKNOWN;
            B_.push_back(a);
        }
        else
            small->inside = NONE;
    }
    if (container == nullptr || B_.size() == 0)
        return;
    container->smallObjectsInside.clear();
    Close(location);
    vector<unsigned int> C_;
    Plug::Sense(C_);
    LOG("Sense");
    score--;
    for (auto b : B_)
    {
        auto small = dynamic_pointer_cast<SmallObject>(objects[b]);
        for (auto c : C_)
        {
            if (b == c)
            {
                small->inside = NONE;
            }
        }
        if (small->inside == UNKNOWN)
        {
            if (small->inside != location)
            {
                small->inside = location;
                container->smallObjectsInside.push_back(small);
            }
        }
    }
    //更新PosCorrectFlag
}

#pragma endregion

void split_string(vector<string> &out, const string &str_source, char mark)
{
    int last = 0;
    for (int i = 0; i < str_source.size(); i++)
    {
        if (str_source[i] == mark)
        {
            out.push_back(str_source.substr(last, i - last));
            last = i + 1;
        }
    }
    if (last != str_source.size())
        out.push_back(str_source.substr(last, str_source.size() - 1));
}

bool Condition::IsObjectSatisfy(const shared_ptr<Object> &target) const
{
    bool ret = true;
    if (sort != "" && sort != target->sort)
        ret *= false;
    if (color != "")
    {
        auto tem = dynamic_pointer_cast<SmallObject>(target);
        if (tem && tem->color != color)
            ret *= false;
    }
    return ret;
}

Instruction::Instruction(const shared_ptr<SyntaxNode> &node, const shared_ptr<ATRI> &atri)
{
    vector<string> disc;
    split_string(disc, node->sons[0]->value, ' ');
    behave = disc[0];
    for (auto n : node->sons[1]->sons)
    {
        vector<string> temp;
        split_string(temp, n->value, ' ');
        Condition *con = &conditionX;
        if (temp[1] == "Y")
        {
            isUseY = true;
            con = &conditionY;
        }
        if (temp[0] == "color")
            con->color = temp[2];
        else if (temp[0] == "sort")
            con->sort = temp[2];
    }
    SearchConditionObject(atri);
}

void Instruction::SearchConditionObject(const shared_ptr<ATRI> &atri)
{
    for (auto v : atri->objects)
    {
        if (conditionX.IsObjectSatisfy(v))
            X.push_back(v);
        if (isUseY && conditionY.IsObjectSatisfy(v))
            Y.push_back(v);
    }
}

void Instruction::TaskSelfOptimization(const shared_ptr<ATRI> &atri)
{
    if (X.size() > 3 || Y.size() > 3)
    {
        isEnable = false;
        return;
    }
    int evalue;
    for (int i = 0; i < X.size(); i++)
    {
        //禁止行动的约束
        for (const auto &t : atri->not_taskConstrains)
        {
            if (t.behave == behave)
            {
                for (auto a : t.X)
                    if (a == X[i])
                        evalue -= 20;
            }
        }
    }
}

bool Instruction::IsInstructionInvoke(const string &behave, const shared_ptr<Object> &x, const shared_ptr<Object> &y)
{
    if (isEnable && behave == this->behave && x == this->X[0] && y == ((this->Y.size() == 0) ? nullptr : this->Y[0]))
        return true;
    return false;
}

ostream &operator<<(ostream &os, const Instruction &instr)
{
#ifdef __DEBUG__
    os << instr.ToString();
// os << "X:\n";
// for (auto v : instr.X)
//     os << v;
// if (instr.isUseY)
// {
//     os << "Y:\n";
//     for (auto v : instr.Y)
//         os << v;
// }
#endif
    return os;
}
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn)
{
#ifdef __DEBUG__
    static int layer = 0;
    for (int i = 0; i < layer; i++)
        os << "-";
    os << sn->value << '|' << endl;
    layer++;
    for (int i = 0; i < sn->sons.size(); i++)
    {
        os << sn->sons[i];
    }
    layer--;
#endif
    return os;
}
ostream &operator<<(ostream &os, shared_ptr<Object> obj)
{
#ifdef __DEBUG__
    if (dynamic_pointer_cast<SmallObject>(obj) != nullptr)
    {
        os << "SmallObject " << dynamic_pointer_cast<SmallObject>(obj)->ToString();
    }
    else if (dynamic_pointer_cast<Robot>(obj) != nullptr)
    {
        os << "this " << dynamic_pointer_cast<Robot>(obj)->ToString();
    }
    else if (dynamic_pointer_cast<BigObject>(obj) != nullptr)
    {
        if (dynamic_pointer_cast<Container>(obj) != nullptr)
        {
            os << "Container " << dynamic_pointer_cast<Container>(obj)->ToString();
        }
        else
        {
            os << "BigObject  " << dynamic_pointer_cast<BigObject>(obj)->ToString();
        }
    }
#endif
    return os;
}