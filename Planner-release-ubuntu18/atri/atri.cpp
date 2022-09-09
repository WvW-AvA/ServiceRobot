#include "atri.hpp"
#include <iostream>
#include "regex"
using namespace _home;
using namespace std;

//////////////////////////////////////////////////////////////////////////
ATRI::ATRI() : Plug("ATRI")
{
}

//////////////////////////////////////////////////////////////////////////
void ATRI::Plan()
{
    //此处添加测试代码
    PraseEnv(GetEnvDes());
}

void ATRI::PraseEnvSentence(const string &str)
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
    if (words[0] == "hold")
    {
        robot.hold = stoi(words[1]);
    }
    else if (words[0] == "plate")
    {
        robot.plate = stoi(words[1]);
    }
    else if (words[0] == "at")
    {
        }
}

void ATRI::PraseEnv(const string &env)
{
    vector<string> div;
    regex reg("\\(.*?\\)");
    cmatch m;
    auto pos = env.data() + 1;
    auto end = env.data() + env.size();
    for (; regex_search(pos, end, m, reg); pos = m.suffix().first)
    {
        string str = m.str();
        cout << str << endl;
        PraseEnvSentence(str);
    }
}

void ATRI::Fini()
{
    cout << "#(ATRI): Fini" << endl;
}
