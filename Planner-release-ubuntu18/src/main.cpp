/*
 * Simulation@Home Competition
 * File: main.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "platform.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>


#ifdef WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

using namespace boost;
using namespace std;
using namespace _home;

int main(int argc, char* argv[])
{
    // reading the parameters
    unsigned int port = 7932;
    unsigned int test_idx = 0;
    unsigned int timeout = 5000;
    Platform::TMode mode = Platform::M_IT;
    string test_dir("./");
    bool log = false;
    string log_dir;
#ifdef _WIN32
    string grader_path = "asp";
#else
    string grader_path = "libasp";
#endif
    string grader_arg = "";

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-port") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    SSCANF(argv[i], "%d", &port);
                }
            }
            else if (strcmp(argv[i], "-td") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    test_dir = string(argv[i]);
                }
            }
            else if (strcmp(argv[i], "-mode") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    if (strcmp(argv[i], "nt") == 0)
                    {
                        mode = Platform::M_NT;
                    }
                    else if (strcmp(argv[i], "it") == 0)
                    {
                        mode = Platform::M_IT;
                    }
                }
            }
            else if (strcmp(argv[i], "-test") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    if (strcmp(argv[i], "all") == 0)
                    {
                        test_idx = 0;
                    }
                    else
                    {
                        SSCANF(argv[i], "%d", &test_idx);
                    }
                }
            }
            else if (strcmp(argv[i], "-to") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    SSCANF(argv[i], "%d", &timeout);
                    if (timeout == 0) timeout = 5000;
                }
            }
            else if (strcmp(argv[i], "-cheat") == 0)
            {
                timeout = 0;
            }
            else if (strcmp(argv[i], "-log") == 0)
            {
                log = true;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    log_dir = string(argv[++i]);
                }
            }
            else if (strcmp(argv[i], "-eval") == 0)
            {
                if (++i < argc && argv[i][0] != '-')
                {
                    grader_path = string(argv[i]);
                    if (++i < argc)
                    {
                        if (argv[i][0] != '-')
                            grader_arg = string(argv[i]);
                        else
                            --i;
                    }
                }
                else
                {
                    grader_path.clear();
                }
            }
            else if (strcmp(argv[i], "-help") == 0 ||
                strcmp(argv[i], "-?") == 0)
            {
                cout << "cserver [-port digit] [-td dir] [-mode 'nt'|'it'] [-test id|'all'] [-to time] [-cheat] [-log [dir]] [-eval [path [agr]]] [-help | -?]" << endl;
                cout << " -port digit\t sets the port for the network conversation, default 7932" << endl;
                cout << " -td dir\t sets 'dir' as the directory of the resource of tests, default is '.'" << endl;
                cout << " -mode 'nt'|'it' sets challenge mode, 'nt' or 'it'" << endl;
                cout << "                 'nt' means task is given via natural language" << endl;
                cout << "                 'it' means task is given via instruction" << endl;
                cout << " -test id\t runs the 'id'th test" << endl;
                cout << "         \t runs all the test as default, or in the following condition" << endl;
                cout << "         \t id='all' or id is out of range of the size of the test set" << endl;
                cout << " -to ms\t\t sets the period of timeout as 'ms'" << endl;
                cout << " -cheat\t\t just cheat!" << endl;
                cout << " -log [dir]\t logs all info and the test result in 'dir'" << endl;
                cout << "           \t 'dir' is optional, default as CWD" << endl;
                cout << " -eval [path [agr]] sets the evaluator in 'path'" << endl;
                cout << "                    'path' is default as 'asp' and optional, empty 'path' means no evaluator" << endl;
                cout << "                    'arg' is arguments for the evaluator" << endl;
                cout << "                    Please use double quotes to contain the arguments" << endl;
                cout << " -help|-?\t look for more commands" << endl;
                return 0;
            }
        }
        else
        {
            cout << "unrecognized command <" << argv[i] << ">\n";
            cout << "please type '-help' or '-?' for more commands" << endl;
            return 0;
        }
    }

    // runing the platform
    boost::shared_ptr<Platform> platform = Platform::GetInstance();
    if (platform->Init(port, log, log_dir, test_idx, timeout, mode, test_dir, grader_path, grader_arg))
    {
        cout << "=========================================\n";
        cout << "= Simulation @Home Competition Platform =\n";
        cout << "=========================================\n";
        platform->Run();
    }
    else
    {
        cout << "# Platform failed to be initialized" << endl;
    }

    return 0;
}
