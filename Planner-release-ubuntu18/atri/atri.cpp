#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

ostream &operator<<(ostream &os, shared_ptr<Object> obj);
ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn);

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
    // PraseEnv(GetEnvDes());
    // PrintEnv();
    PraseInstruction(GetTaskDes());
    // Move(1);
    // Move(2);
    // Move(3);
    // Move(4);
}

bool ATRI::PraseInstruction(const string &taskDis)
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
            curr_leaf->value += taskDis.substr(tag1, i - tag1);
            tag1 = i;
            auto p = make_shared<SyntaxNode>();
            curr_leaf->sons.push_back(p);
            curr_leaf = p;
            leaf_path.push_back(curr_leaf);
        }
        else if (taskDis[i] == ')')
        {
            curr_leaf->value += taskDis.substr(tag1, i - tag1);
            tag1 = i + 1;
            curr_leaf = *(leaf_path.end() - 1);
            leaf_path.pop_back();
        }
    }
    cout << root;
    for (auto v : root->sons)
    {
        if (v->value == ":task")
            tasks.push_back(Instruction(v));
        else if (v->value == ":cons_not")
        {
            if (v->sons[0]->value == ":task")
                not_taskConstrains.push_back(Instruction(v->sons[0]));
            else if (v->sons[0]->value == ":info")
                not_infoConstrains.push_back(Instruction(v->sons[0]));
        }
        else if (v->value == ":cons_notnot")
        {
            if (v->sons[0]->value == ":task")
                not_taskConstrains.push_back(Instruction(v->sons[0]));
            else if (v->sons[0]->value == ":info")
                not_infoConstrains.push_back(Instruction(v->sons[0]));
        }
    }

    for (auto v : tasks)
        cout << v.ToString();
    for (auto v : not_infoConstrains)
        cout << v.ToString();
    for (auto v : not_taskConstrains)
        cout << v.ToString();
    for (auto v : notnot_infoCosntrains)
        cout << v.ToString();
    return true;
}
bool ATRI::PraseEnvSentence(const string &str)
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
        return false;

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
                return false;
            p->isOpen = true;
        }
        else if (words[0] == "closed")
        {
            auto p = dynamic_pointer_cast<Container>(objects[ind]);
            if (p == nullptr)
                return false;
            p->isOpen = false;
        }
        if (words[0] == "at")
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
                auto p = make_shared<BigObject>(objects[ind]);
                bigObjects.push_back(p);
                objects[ind] = p;
            }
            else if (words[2] == "small")
            {
                auto p = make_shared<SmallObject>(objects[ind]);
                smallObjects.push_back(p);
                objects[ind] = p;
            }
        }
        else if (words[0] == "color")
        {
            auto p = dynamic_pointer_cast<SmallObject>(objects[ind]);
            if (p == nullptr)
                return false;
            p->color = words[2];
        }
        else if (words[0] == "inside")
        {
            auto p = dynamic_pointer_cast<SmallObject>(objects[ind]);
            if (p == nullptr)
                return false;
            p->inside = stoi(words[2]);
        }
        else if (words[0] == "type")
        {
            if (words[2] == "container")
            {
                auto p = make_shared<Container>(objects[ind]);
                containers.push_back(p);
                objects[ind] = p;
            }
        }
    }
    return true;
}
bool ATRI::PraseEnv(const string &env)
{
    regex reg("\\(.*?\\)");
    cmatch m;
    auto pos = env.data() + 1;
    auto end = env.data() + env.size();
    for (; regex_search(pos, end, m, reg); pos = m.suffix().first)
    {
        string str = m.str();
        if (PraseEnvSentence(str) == false)
            return false;
    }
    hold = dynamic_pointer_cast<SmallObject>(objects[hold_id]);
    plate = dynamic_pointer_cast<SmallObject>(objects[plate_id]);

    for (int i = 0; i < smallObjects.size(); i++)
    {
        if (smallObjects[i]->inside != UNKNOWN)
        {
            dynamic_pointer_cast<Container>(objects[smallObjects[i]->inside])->smallObjectsInside.push_back(smallObjects[i]);
        }
        if (smallObjects[i]->location != UNKNOWN)
        {
            for (auto big : bigObjects)
            {
                if (big->location == smallObjects[i]->location)
                {
                    big->smallObjectsOn.push_back(smallObjects[i]);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < objects.size(); i++)
    {
        cout << objects[i] << endl;
    }
    return true;
}
void ATRI::Fini()
{
    cout << "#(ATRI): Fini" << endl;
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
                cout << GREEN << "(" << v->id << " " << v->sort << ")" << RESET;
            else
                cout << "(" << v->id << " " << v->sort << ")";
        }
        cout << endl;
    }
    cout << "UnknownPos:";
    for (auto v : unknownPos)
    {
        cout << RED << "(" << v->id << " " << v->sort << ")" << RESET;
    }
    cout << endl;
}
bool ATRI::TakeOut(unsigned int a, unsigned int b)
{
    bool res = Plug::TakeOut(a, b);
    cout << "TakeOut:" << res << endl;
    return res;
}
bool ATRI::PutIn(unsigned int a, unsigned int b)
{
    bool res = Plug::PutIn(a, b);
    cout << "PutIn:" << res << endl;
    return res;
}
bool ATRI::Close(unsigned int a)
{
    bool res = Plug::Close(a);
    cout << "Close:" << res << endl;
    return res;
}
bool ATRI::Open(unsigned int a)
{
    bool res = Plug::Open(a);
    cout << "Open:" << res << endl;
    return res;
}
bool ATRI::FromPlate(unsigned int a)
{
    bool res = Plug::FromPlate(a);
    cout << "FromPlate:" << res << endl;
    return res;
}
bool ATRI::ToPlate(unsigned int a)
{
    bool res = Plug::ToPlate(a);
    cout << "ToPlate:" << res << endl;
    return res;
}
bool ATRI::PutDown(unsigned int a)
{
    bool res = Plug::PutDown(a);
    cout << "PutDown:" << res << endl;
    return res;
}
bool ATRI::PickUp(unsigned int a)
{
    bool res = Plug::PickUp(a);

    cout << "PickUp:" << res << endl;
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
    }
    cout << "Move:" << res << endl;
    return res;
}

void split_string(vector<string> out, const string &str_source, char mark)
{
    int last = 0;
    for (int i = 0; i < str_source.size(); i++)
    {
        if (str_source[i] == mark)
        {
            out.push_back(str_source.substr(last, i - last));
            last = i;
        }
    }
}
string Condition::ToString()
{
    return "(Sort:" + sort + ",Color:" + color + ")";
}

Instruction::Instruction(const shared_ptr<SyntaxNode> &node)
{
    vector<string> disc;

    split_string(disc, node->sons[0]->value, ' ');
    behave = disc[0];

    for (auto n : node->sons[1]->sons)
    {
        vector<string> temp;
        split_string(temp, n->value, ' ');
        Condition &con = conditionX;
        if (temp[1] == "Y")
            con = conditionY;
        if (temp[0] == "color")
        {
            con.color = temp[2];
        }
        else if (temp[0] == "sort")
        {
            con.sort = temp[2];
        }
    }
}
string Instruction::ToString()
{
    return "Behave:" + behave + "\nConditionX:" + conditionX.ToString() + "\nConditionY:" + conditionY.ToString() + "\n";
}
void Instruction::SearchConditionObject(const shared_ptr<ATRI> &atri)
{
}

ostream &operator<<(ostream &os, shared_ptr<SyntaxNode> sn)
{
    os << sn->value << endl;
    for (int i = 0; i < sn->sons.size(); i++)
    {
        os << sn->sons[i];
    }
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