#include "devil.hpp"
#include <iostream>

using namespace _home;
using namespace std;

//////////////////////////////////////////////////////////////////////////
Devil::Devil() : Plug("Devil")
{
}

//////////////////////////////////////////////////////////////////////////
void Devil::Plan()
{
    //此处添加测试代码
    cout << "Test Name: <" << GetTestName() << ">" << endl;
    cout << "Environment Discription:\n"
         << GetEnvDes() << endl;
    cout << "Task Discription:\n"
         << GetTaskDes() << endl;
}

void Devil::Fini()
{
    cout << "#(Devil): Fini" << endl;
}
