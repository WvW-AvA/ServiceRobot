#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

void split_string(vector<string> &out, const string &str_source, char mark);
ostream &operator<<(ostream &os, shared_ptr<Object> obj);
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn);
ostream &operator<<(ostream &os, const Instruction &instr);

ATRI::ATRI() : Plug("ATRI") {}

void ATRI::Init()
{
    objects.push_back(shared_from_this());
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
    {
        cout << v << endl;
        ParseInfo(v);
    }
    PrintEnv();
    TestAutoBehave();
    // PrintInstruction();
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
        }
        if (words[0] == "opened")
        {
            auto p = dynamic_pointer_cast<Container>(objects[ind]);
            if (p == nullptr)
            {
                auto p = make_shared<Container>(objects[ind]);
                containers.push_back(p);
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
                containers.push_back(p);
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
                    bigObjects.push_back(p);
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
                    containers.push_back(p);
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
    {
        SetHold(ObjectPtrCast<SmallObject>(objects[hold_id]));
    }
    if (plate_id > 0)
    {
        SetPlate(ObjectPtrCast<SmallObject>(objects[plate_id]));
    }

    for (int i = 0; i < smallObjects.size(); i++)
    {
        if (smallObjects[i] == plate || smallObjects[i] == hold)
            continue;
        if (smallObjects[i]->inside != UNKNOWN)
        {
            auto p = dynamic_pointer_cast<Container>(objects[smallObjects[i]->inside]);
            p->smallObjectsInside.push_back(smallObjects[i]);
            smallObjects[i]->location = p->location;
        }
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

bool ATRI::DoBehavious(const string &behavious, unsigned int x)
{
    if (behavious == "move" || behavious == "Move")
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
    return false;
}

bool ATRI::DoBehavious(const string &behavious, unsigned int x, unsigned int y)
{
    if (behavious == "putin" || behavious == "PutIn")
        return PutIn(x, y);
    else if (behavious == "takeout" || behavious == "TakeOut")
        return TakeOut(x, y);
    return false;
}

void ATRI::SolveTask(const Instruction &task)
{
}
void ATRI::PrintInstruction()
{
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
}

void ATRI::PrintEnv()
{
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
        cout << "Pos" << i << ":";
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
}

void ATRI::Fini()
{
    cout << "#(ATRI): Fini" << endl;
    objects.clear();
    smallObjects.clear();
    bigObjects.clear();
    containers.clear();
    tasks.clear();
    infos.clear();
    not_infoConstrains.clear();
    not_taskConstrains.clear();
    notnot_infoConstrains.clear();
    objects.push_back(shared_from_this());
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
        {
            auto temp = vector<unsigned int>();
            Sense(temp);
        }
        else if (temp.size() == 2)
            DoBehavious(temp[0], stoi(temp[1]));
        else if (temp.size() == 3)
            DoBehavious(temp[0], stoi(temp[1]), stoi(temp[2]));
        PrintEnv();
    }
}

#pragma region override_ATRI_AtomBehavious
bool ATRI::TakeOut(unsigned int a, unsigned int b)
{
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    auto cont = ObjectPtrCast<Container>(objects[b]);
    if (hold == nullptr)
        if (small->inside == cont->id)
            if (location == objects[b]->location)
            {
                small->inside = false;
                cont->DeleteObjectInside(small);
                SetHold(small);
                LOG("TakeOut(%d,%s)(%d,%s)\n", a, cont->sort.c_str(), b, small->sort.c_str());
                score -= 2;
                return true;
            }
            else
                Move(objects[b]->location);
        else
        {
            LOG_ERROR("Small Object:(%d,%s) is not in Container (%d,%s)", small->id, small->sort, cont->id, cont->sort);
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
                small->inside = b;
                SetHold(nullptr);
                cont->smallObjectsInside.push_back(small);
                LOG("PutIn(%d,%s)(%d,%s)\n", a, cont->sort.c_str(), b, small->sort.c_str());
                score -= 2;
                return true;
            }
            else
                Move(b);
        else
            Open(b);
    else
    {
        if (plate == small)
            FromPlate(a);
        else
            PickUp(a);
    }
    PutIn(a, b);
}
bool ATRI::Close(unsigned int a)
{
    shared_ptr<Container> container = ObjectPtrCast<Container>(objects[a]);
    if (container != nullptr)
        if (container->isOpen = true)
            if (hold == nullptr)
                if (location == container->location)
                {
                    container->isOpen = false;
                    LOG("Close(%d,%s)\n", a, container->sort.c_str());
                    score -= 2;
                    return true;
                }
                else
                    Move(container->location);
            else
                PutDown(hold->id);
        else
            Open(a);
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
            if (hold == nullptr)
                if (location == container->location)
                {
                    container->isOpen = true;

                    LOG("Open(%d,%s)\n", a, container->sort.c_str());
                    score -= 2;
                    return true;
                }
                else
                    Move(container->location);
            else
                PutDown(hold->id);
        else
            Close(a);
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
    if (plate && plate->id == a)
        if (hold == nullptr)
        {
            SetHold(plate);
            SetPlate(nullptr);
            LOG("FromPlate(%d,%s)\n", a, hold->sort.c_str());
            score -= 2;
            return true;
        }
        else
            PutDown(hold->id);
    else
        ToPlate(a);
    return FromPlate(a);
}
bool ATRI::ToPlate(unsigned int a)
{
    if (hold && hold->id == a)
        if (plate == nullptr)
        {
            SetPlate(hold);
            SetHold(nullptr);
            LOG("ToPlate(%d,%s)\n", a, objects[a]->sort.c_str());
            score -= 2;
            return true;
        }
        else
            FromPlate(plate->id);
    else
        PickUp(a);
    return ToPlate(a);
}
bool ATRI::PutDown(unsigned int a)
{
    if (hold && hold->id == a)
    {
        SetHold(nullptr);
        LOG("PutDown(%d,%s)\n", a, objects[a]->sort.c_str());
        score -= 2;
        return true;
    }
    else if (plate && plate->id == a)
    {
        FromPlate(a);
        return PutDown(a);
    }
    else
    {
        LOG_ERROR("PutDown Error hold id != a");
        return false;
    }
}
bool ATRI::PickUp(unsigned int a)
{
    auto small = ObjectPtrCast<SmallObject>(objects[a]);
    if (hold == nullptr)
        if (location == small->location)
            if (small->inside == NONE)
            {
                SetHold(small);
                LOG("PickUp(%d,%s)\n", small->id, small->sort.c_str());
                score -= 2;
                return true;
            }
            else
            {
                TakeOut(small->id, small->inside);
            }
        else
        {
            if (small->location == UNKNOWN)
                FindObjectLocation(small);
            Move(small->location);
        }
    else
        PutDown(hold->id);
    return PickUp(a);
}
bool ATRI::Move(unsigned int a)
{
    location = a;
    if (hold)
        hold->location = a;
    if (plate)
        plate->location = a;
    LOG("Move(%d)\n", a);
    score -= 4;
    return true;
}

std::string ATRI::AskLoc(unsigned int a)
{
    auto str = Plug::AskLoc(a);
    cout << str << endl;
    return str;
}
void ATRI::Sense(std::vector<unsigned int> &A_)
{
    Plug::Sense(A_);
    cout << "Sense";
    for (auto a : A_)
        cout << a << " ";
    cout << endl;
}

void ATRI::FindObjectLocation(shared_ptr<SmallObject> ptr) {}
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

string Condition::ToString() const
{
    return "(Sort:" + sort + ",Color:" + color + ")";
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

string Instruction::ToString() const
{
    return "Behave:" MAGENTA + behave + RESET "\nConditionX:" MAGENTA + conditionX.ToString() + RESET "\nConditionY:" MAGENTA + conditionY.ToString() + RESET "\n";
}

int TaskSolution::SolveOneTaskCost(const Instruction &task)
{
}

ostream &operator<<(ostream &os, const Instruction &instr)
{
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
    return os;
}
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn)
{
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
    return os;
}
ostream &operator<<(ostream &os, shared_ptr<Object> obj)
{
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
    return os;
}