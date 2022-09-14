#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

ostream &operator<<(ostream &os, shared_ptr<Object> obj);

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
    PraseEnv(GetEnvDes());
    cout << GetTaskDes() << endl;
    PraseTask(GetTaskDes());
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
        this->hold = stoi(words[1]);
    }
    else if (words[0] == "plate")
    {
        this->plate = stoi(words[1]);
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

bool ATRI::PraseTask(const string &task)
{
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