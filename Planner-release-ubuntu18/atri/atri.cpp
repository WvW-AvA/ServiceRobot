#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

ostream &operator<<(ostream &os, shared_ptr<Object> obj);
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn);
ostream &operator<<(ostream &os, const Instruction &instr);

//////////////////////////////////////////////////////////////////////////
ATRI::ATRI() : Plug("ATRI")
{
}

void ATRI::Init()
{
    objects.push_back(shared_from_this());
}
//////////////////////////////////////////////////////////////////////////
void ATRI::Plan()
{
    //此处添加测试代码
    // cout << GetEnvDes() << endl;
    if (ParseEnv(GetEnvDes()) == false)
    {
        cout << RED << "Env Prase Error\n"
             << RESET;
        return;
    }
    PrintEnv();
    Move(5);
    PutDown(13);
    FromPlate(20);
    PrintEnv();
    //Move(2);
    PutDown(20);
    PrintEnv();
    cout << endl;
    //ParseInstruction(GetTaskDes());
    //PrintInstruction();
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
        cout << RED << "Env Sentence error\n"
             << RESET;
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
            cout << RED << "Parse Env Sentence ERROR\n"
                 << RESET;
            return false;
        }
    }
    if (hold_id != NONE)
        hold = dynamic_pointer_cast<SmallObject>(objects[hold_id]);
    if (plate_id != NONE)
        plate = dynamic_pointer_cast<SmallObject>(objects[plate_id]);

    for (int i = 0; i < smallObjects.size(); i++)
    {
        if (smallObjects[i]->inside != UNKNOWN)
        {
            auto p = dynamic_pointer_cast<Container>(objects[smallObjects[i]->inside]);
            if (p)
            {
                p->smallObjectsInside.push_back(smallObjects[i]);
                smallObjects[i]->location = p->location;
            }
            else
            {
                cout << RED << "Type ERROR\n";
                cout << dynamic_pointer_cast<Object>(smallObjects[i]) << endl;
                cout << objects[smallObjects[i]->inside] << endl
                     << RESET;
            }
        }
        //同一位置不一定就在上面
        // if (smallObjects[i]->location != UNKNOWN)
        // {
        //     for (auto big : bigObjects)
        //     {
        //         if (big->location == smallObjects[i]->location)
        //         {
        //             big->smallObjectsOn.push_back(smallObjects[i]);
        //             break;
        //         }
        //     }
        // }
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
            dynamic_pointer_cast<BigObject>(info.Y[0])->smallObjectsOn.push_back(dynamic_pointer_cast<SmallObject>(v));
        }
    }
    else if (info.behave == "near")
    {
        for (auto v : info.X)
            v->location = info.Y[0]->location;
    }
    else if (info.behave == "plate")
    {
    }
    else if (info.behave == "inside")
    {
    }
    else if (info.behave == "opened")
    {
    }
    else if (info.behave == "closed")
    {
    }
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
                cout << YELLOW << "(" << p->id << " " << p->sort << " on:[";
                for (int c = 0; c < p->smallObjectsOn.size(); c++)
                    cout << (c == 0 ? "" : ",") << p->smallObjectsOn[c]->id;
                cout << "]";
                if (dynamic_pointer_cast<Container>(v))
                {
                    auto p = dynamic_pointer_cast<Container>(v);
                    cout << " inside:[";
                    for (int c = 0; c < p->smallObjectsInside.size(); c++)
                        cout << (c == 0 ? "" : ",") << p->smallObjectsInside[c]->id;
                    cout << "]";
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

#pragma region override_ATRI_AtomBehavious
bool ATRI::TakeOut(unsigned int a, unsigned int b)
{
    bool res = Plug::TakeOut(a, b);
    if (res)
    {
        if (location == dynamic_pointer_cast<Container>(objects[b])->location && hold_id == UNKNOWN && dynamic_pointer_cast<SmallObject>(objects[a])->inside)
        {
            dynamic_pointer_cast<SmallObject>(objects[a])->inside = false;
            hold_id = a;
            cout << "TakeOut:" << res << endl;
        }
    }
    else
    {
        cout << "TakeOutfailed" << endl;
    }
    return res;
}
bool ATRI::PutIn(unsigned int a, unsigned int b)
{
    bool res = Plug::PutIn(a, b);
    if (res)
    {
        if (location == dynamic_pointer_cast<Container>(objects[b])->location && hold_id == a && dynamic_pointer_cast<Container>(objects[b])->isOpen)
        {
            dynamic_pointer_cast<SmallObject>(objects[a])->inside = true;
            hold_id = UNKNOWN;
            cout << "PutIn:" << res << endl;
        }
    }
    else
    {
        cout << "PutInfailed" << endl;
    }
    return res;
}
bool ATRI::Close(unsigned int a)
{
    bool res = Plug::Close(a);
    shared_ptr<Container> container1 = dynamic_pointer_cast<Container>(objects[a]);
    if (container1 != nullptr)
    {
        if (res)
        {
            if (dynamic_pointer_cast<Container>(objects[a])->isOpen = true && hold_id == UNKNOWN && location == dynamic_pointer_cast<Container>(objects[a])->location)
            {
                dynamic_pointer_cast<Container>(objects[a])->isOpen = false;
                cout << "Close:" << res << endl;
            }
        }
    }
    else
    {
        cout << "Closefailed" << endl;
    }
    return res;
}
bool ATRI::Open(unsigned int a)
{
    bool res = Plug::Open(a);
    if (res)
    {
        if (dynamic_pointer_cast<Container>(objects[a])->isOpen = false && hold_id == UNKNOWN && location == dynamic_pointer_cast<Container>(objects[a])->location)
        {
            dynamic_pointer_cast<Container>(objects[a])->isOpen = true;
            cout << "Open:" << res << endl;
        }
    }
    else
    {
        cout << "Openfailed" << endl;
    }
    return res;
}
bool ATRI::FromPlate(unsigned int a)
{
    bool res = Plug::FromPlate(a);
    if (res)
    {
        if (plate_id == a && hold_id == UNKNOWN)
        {
            hold_id = a;
            plate_id = UNKNOWN;
            cout << "FromPlate:" << res << endl;
        }
    }
    else
    {
        cout << "FromPlatefailed" << endl;
    }
    return res;
}
bool ATRI::ToPlate(unsigned int a)
{
    bool res = Plug::ToPlate(a);
    if (res)
    {
        if (hold_id == a && plate_id == UNKNOWN)
        {
            hold_id = UNKNOWN;
            plate_id = a;
            cout << "ToPlate:" << res << endl;
        }
    }
    else
    {
        cout << "ToPlatefailed" << endl;
    }
    return res;
}
bool ATRI::PutDown(unsigned int a)
{
    bool res = Plug::PutDown(a);
    if (res)
    {
        if (hold_id == a)
        {
            hold_id = UNKNOWN;
            cout << "PutDown:" << res << endl;
        }
    }
    else
    {
        cout << "PutDownfailed" << endl;
    }
    return res;
}
bool ATRI::PickUp(unsigned int a)
{
    bool res = Plug::PickUp(a);
    if (res)
    {
        if (((objects[a]) == smallObjects[a]) && hold_id == UNKNOWN && (!dynamic_pointer_cast<SmallObject>(objects[a])->inside))
        {
            hold_id = a;
            cout << "PickUp:" << res << endl;
        }
    }
    else
    {
        cout << "PickUpfailed" << endl;
    }
    return res;
}
bool ATRI::Move(unsigned int a)
{
    bool res = Plug::Move(a);
    if (res)
    {
        location = a;
        hold->location = a;
        plate->location = a;
        cout << "Move:" << res << endl;
    }
    else
    {
        cout << "Movefailed" << endl;
    }
    return res;
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
    return "Behave:" + behave + "\nConditionX:" + conditionX.ToString() + "\nConditionY:" + conditionY.ToString() + "\n";
}

ostream &operator<<(ostream &os, const Instruction &instr)
{
    os << instr.ToString();
    os << "X:\n";
    for (auto v : instr.X)
        os << v;
    if (instr.isUseY)
    {
        os << "Y:\n";
        for (auto v : instr.Y)
            os << v;
    }
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