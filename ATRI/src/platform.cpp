/*
 * Simulation@Home Competition
 * File: platform.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "platform.hpp"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "tinyxml/tinyxml.h"
#include "cserver/grader.hpp"
#include "sharedlib.h"
#include "cserver/message.hpp"
#include "cserver/param.hpp"
#include "cserver/xmllabel.hpp"

using namespace boost;
using namespace std;
using namespace _home;

static const unsigned int gBufferSize = 2048;
static boost::array<char, gBufferSize> gBuffer;

//////////////////////////////////////////////////////////////////////////
struct Platform::TChannel
{
    string name;
    boost::asio::ip::tcp::socket socket;

    TChannel(asio::io_service& _ioservice) :
    socket(_ioservice)
    {
    }
};

//////////////////////////////////////////////////////////////////////////
struct Platform::TGrader
{
    string path;
    Grader* grader;
    TBundle bundle;

    TGrader();
    ~TGrader();

    bool Init(const string& _bundle_path);
};

Platform::TGrader::TGrader() :
grader(0),
bundle(new SharedLib)
{
}

Platform::TGrader::~TGrader()
{
    delete grader;
    bundle.reset();
}

bool Platform::TGrader::Init(const string& _bundle_path)
{
    if (_bundle_path.empty()) return false;
    if (!bundle->Open(_bundle_path))
    {
        cout << "# Failed to open the evaluator '"
            << _bundle_path << "'\n";
        return false;
    }

    void(*export_grader)(Grader* &) = 0;
    export_grader =
        (void(*)(Grader* &))bundle->GetProcAddress("ExportGrader");
    if (export_grader == 0)
    {
        cout << "# Found no entry in '" << _bundle_path << "'\n";
        return false;
    }

    export_grader(grader);
    if (grader == 0) return false;

    path = _bundle_path;

    return true;
}

//////////////////////////////////////////////////////////////////////////
struct Platform::TestDesc
{
    bool is_empty;
    bool is_missing;
    bool is_erroneous;
    bool is_misleading;
    std::string info;
    std::string missing_info;
    std::string erroneous_info;
    std::string correct_info;
    std::string extra_info;
    std::string instruction;
    std::string nl;

    TestDesc() : 
    is_empty(true), 
    is_missing(false), 
    is_erroneous(false), 
    is_misleading(false) {}

    std::string getEnv4Grader()
    {
        stringstream ss;
        ss << "(:domain " << info << " "
            << missing_info << " "
            << correct_info << " "
            << extra_info << ")";
        return ss.str();
    }

    std::string getEnv4Plug()
    {
        stringstream ss;
        ss << "(:domain " << info << " " 
            << (is_missing ? "" : missing_info) << " "
            << (is_erroneous ? erroneous_info : correct_info) << ")";
        return ss.str();
    }
};

//////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Platform> Platform::GetInstance()
{
    static boost::shared_ptr<Platform> platform(new Platform);
    return platform;
}

//////////////////////////////////////////////////////////////////////////
Platform::Platform() :
mPort(0),
mMode(M_IT),
mTimeOut(5000),
mTestIdx(0),
mLog(false),
mIOService(new asio::io_service),
mResult(),
mTime(5.0)
{
}

//////////////////////////////////////////////////////////////////////////
Platform::~Platform()
{
}

//////////////////////////////////////////////////////////////////////////
bool Platform::Init(
                    unsigned int _port,
                    bool _log /* = false */,
                    const std::string& _log_dir /* = "" */,
                    unsigned int _test_idx /* = 0 */,
                    unsigned int _timeout /* = 5000 */,
                    TMode _mode /* = M_IT */,
                    const std::string& _test_dir /* = "." */,
                    const std::string& _grader_path /* = "asp" */,
                    const std::string& _grader_arg /* = "" */)
{
    mPort = _port;
    mLog = _log;
    if (_log_dir.size() && *_log_dir.rbegin() != '/')
    {
        mLogDir = _log_dir + "/";
    }
    else
    {
        mLogDir = _log_dir;
    }
    mMode = _mode;
    mTimeOut = _timeout;

    if (*_test_dir.rbegin() != '/')
    {
        mTestDir = _test_dir + "/";
    }
    else
    {
        mTestDir = _test_dir;
    }
    ifstream list_file((mTestDir + "test.list").c_str());
    if (list_file)
    {
        string test_file;
        while(getline(list_file, test_file))
        {
            mTestVec.push_back(test_file);
        }
    }
    else
    {
        cout << "# Error: no such file as 'test.list' in '" + mTestDir << "'\n";
        return false;
    }

    mTestIdx =
        (_test_idx < 1 || _test_idx > mTestVec.size()) ? 0 : _test_idx;

    TGraderPtr grader(new TGrader);
    if (grader->Init(_grader_path) &&
        grader->grader->Init(_grader_arg))
    {
        mGrader = grader;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
void Platform::Run()
{
    while (true)
    {
        if (!Listen() || !Prepare())
        {
            mChannel.reset();
            continue;
        }

        cout << "> Configurations:\n\tMode: ";
        if (mMode == M_IT) cout << "IT";
        else if (mMode == M_NT) cout << "NT";
        cout << endl
            << "\tTimeout: " << mTimeOut << endl
            << "\tTestDir: " << mTestDir << endl
            << "\tTestSize: " << mTestVec.size() << endl;
        if (mTestIdx)
            cout << "\tSingle test: " << mTestIdx << endl;
        else
            cout << "\tAll tests" << endl;
        if (mLog) LogConfInfo();

        if (mTestIdx)
        {
            RunTest(mTestIdx - 1);
            if (mGrader)
            {
                int score = mGrader->grader->EndEvaluation(mTime);
                cout << "# Score: " << score << endl;
                if (mLog) LogScore(score);
            }
        }
        else
        {
            int total_score = 0;
            for (unsigned int i = 0; i < mTestVec.size(); ++i)
            {
                RunTest(i);
                if (mGrader)
                {
                    int score = mGrader->grader->EndEvaluation(mTime);
                    cout << "# Score: " << score << endl;
                    if (mLog) LogScore(score);
                    total_score += score;
                }
            }
            if (mLog && mGrader.get()) LogTotalScore(total_score);
        }

        try
        {
            mChannel->socket.send(asio::buffer("<quit/>"));
            asio::deadline_timer timer(*mIOService, posix_time::seconds(1));
            timer.wait();
        }
        catch (boost::system::system_error& err)
        {
            cout << "# Connection with the team '" << mChannel->name
                << "' has been shut down\n\tError Message='" << err.what() << "'\n";
        }
    }
}

//////////////////////////////////////////////////////////////////////////
bool Platform::Listen()
{
    cout << "= Ready to accept a new team" << endl;

#ifdef _DEBUG
    cout << "# Listening " << mPort << endl;
#endif

    asio::ip::tcp::acceptor acceptor(
        *mIOService,
        asio::ip::tcp::endpoint(asio::ip::tcp::v4(), mPort));
    mChannel.reset(new TChannel(*mIOService));

    try
    {
        acceptor.accept(mChannel->socket);
    }
    catch (boost::system::system_error& err)
    {
        cout << "# Error occurs on accepting a connection\n\tMessage='"
            << err.what() << "'\n";
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
bool Platform::Prepare()
{
    // registration
    try
    {
        size_t recv_len = mChannel->socket.receive(asio::buffer(gBuffer));
        if (recv_len)
        {
            TiXmlDocument doc;
            doc.Parse(gBuffer.data());
#ifdef _DEBUG
            if (doc.Error())
            {
                cout << "# TinyXml Error: " << doc.ErrorDesc() << endl;
                return false;
            }
#endif
            TiXmlElement* name = doc.FirstChildElement("name");
            const char *res = name->GetText();
            if (res)
                mChannel->name = string(res);
            else
                mChannel->name = "<default>";
            cout << "# Registration from '" << mChannel->name << "'\n";
        }
        else
        {
            cout << "# Error: no name for registration" << endl;
            return false;
        }

        // sending the run-time parameters
        stringstream ss;
        ss << "<conf to=\"" << mTimeOut << "\"/>";
        mChannel->socket.send(asio::buffer(ss.str().c_str(), ss.str().size()));
    }
    catch (boost::system::system_error& err)
    {
        cout << "# Connection error occurs on preparing a test\n\tMessage='"
            << err.what() << "'\n";
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
void Platform::RunTest(unsigned int _test_idx)
{
    // getting the test
    string test_name(mTestVec[_test_idx]);
    boost::shared_ptr<TestDesc> test_desc = 
        getTestDesc(mTestDir + test_name);
    if (test_desc->is_empty) 
    {
        cout << "# Could not load the test '"
            << test_name << "'\n";
    }

    string es = test_desc->getEnv4Plug();
    string ts;
    if (mMode == M_NT)
        ts = test_desc->nl;
    else
        ts = test_desc->instruction;

    stringstream ss;
    ss << "<test>" << "<name>" << test_name << "</name>"
        << "<es>" << es << "</es>" 
        << "<ts>" << ts << "</ts>" << "</test>";

    // testing
    cout << "# Testing <" << test_name << ">\n";

    mTime = (double)mTimeOut / 1000.0;
    if (mGrader) mGrader->grader->BeginEvaluation(
        test_desc->getEnv4Grader(), 
        test_desc->instruction, 
        mChannel->name, 
        test_desc->is_misleading);

    try
    {
        mChannel->socket.send(asio::buffer(
            asio::const_buffer(ss.str().c_str(), ss.str().size())));
    }
    catch (boost::system::system_error& err)
    {
        cout << "# Error occurs on sending a test\n\tMessage='"
            << err.what() << "'\n";

        if (mLog) LogResult(_test_idx, es, ts, "");
        return;
    }

    Dialogue();

    cout << "# Result:\n" << mResult.str() << "# Time: " << mTime << "s\n";

    if (mLog) LogResult(_test_idx, es, ts, mResult.str());
}

//////////////////////////////////////////////////////////////////////////
void Platform::LogConfInfo()
{
    ofstream log((mLogDir + mChannel->name + ".log").c_str());
    if (log)
    {
        log << "<configuration info>" << endl
            << "TeamName: " << mChannel->name << endl
            << "Mode: ";
        if (mMode == M_IT) log << "IT";
        else if(mMode == M_NT) log << "NT";
        log << endl
            << "Timeout: " << mTimeOut << endl;
        log << "TestDir: " << mTestDir << endl;
        log << "TestSize: " << mTestVec.size() << endl;
        if (mTestIdx)
        {
            log << "Single test: " << mTestVec[mTestIdx - 1] << endl;
        }
        else
        {
            log << "All tests" << endl;
        }
        if (mGrader.get())
        {
            log << "Grader path: " << mGrader->path << endl << endl;
        }
    }
    else
    {
        cout << "# Failed to log the configuration info in '"
            << mLogDir + mChannel->name << ".log'\n";
    }
}

//////////////////////////////////////////////////////////////////////////
void Platform::LogResult(
    unsigned int _test_idx,
    const std::string& _es,
    const std::string& _ts,
    const std::string& _result)
{
    ofstream log;
    log.open(
        (mLogDir + mChannel->name + ".log").c_str(),
        ios_base::out | ios_base::app);
    if (log)
    {
        log << "<" << mTestVec[_test_idx] << ">" << endl;
        log << "# Environment Description:\n" << _es << endl;
        log << "# Task Description:\n" << _ts << endl;
        log << "# Results:\n" << _result << endl;
    }
    else
    {
        cout << "# Failed to log the result in '"
            << mLogDir + mChannel->name << ".log'\n";
    }
}

//////////////////////////////////////////////////////////////////////////
void Platform::LogScore(int _score)
{
    ofstream log;
    log.open(
        (mLogDir + mChannel->name + ".log").c_str(),
        ios_base::out | ios_base::app);
    if (log)
    {
        log << "# Time: " << mTime << "\n# Score: " << _score << endl << endl;
    }
    else
    {
        cout << "# Failed to log the score in '"
            << mLogDir + mChannel->name << ".log'\n";
    }
}

//////////////////////////////////////////////////////////////////////////
void Platform::LogTotalScore(int _score)
{
    ofstream log;
    log.open(
        (mLogDir + mChannel->name + ".log").c_str(),
        ios_base::out | ios_base::app);
    if (log)
    {
        log << "Total Score: " << _score << endl;
    }
    else
    {
        cout << "# Failed to log the total score in '"
            << mLogDir + mChannel->name << ".log'\n";
    }
}

//////////////////////////////////////////////////////////////////////////
void Platform::Dialogue()
{
    static boost::array<uint8_t, 4> msg_len;

    shared_array<uint8_t> buffer;
    uint32_t max_size = 0;
    uint32_t size = 0;

    mResult.str("");
    while (true)
    {
        try
        {
            asio::read(mChannel->socket, asio::buffer(msg_len.data(), 4));
            size = *((uint32_t*)msg_len.data());
            if (max_size < size) 
            {
                buffer.reset(new uint8_t[size]);
                max_size = size;
            }
            asio::read(mChannel->socket, asio::buffer(buffer.get(), size));

            MessagePtr msg(new Message);
            msg->parse(buffer, size);

            PStringPtr action_name;
            msg->getAttribute(XmlLabelProtocol::ActionName, action_name);
            ParamListPtr params;
            msg->deserialize(params);

            MessagePtr ret_msg(new Message);
            if (action_name->get() == "Move")
            {
                PUIntPtr x;
                PBoolPtr feedback(new PBool(false));
                getParams(params, x);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateMove(x->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[Move " << x->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "PickUp")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluatePickUp(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[PickUp " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "PutDown")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluatePutDown(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[PutDown " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "ToPlate")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateToPlate(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[ToPlate " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "FromPlate")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateFromPlate(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[FromPlate " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "Open")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateOpen(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[Open " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "Close")
            {
                PUIntPtr a;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateClose(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[Close " << a->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "PutIn")
            {
                PUIntPtr a;
                PUIntPtr b;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a, b);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluatePutIn(a->get(), b->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[PutIn " << a->get() << " " << b->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "TakeOut")
            {
                PUIntPtr a;
                PUIntPtr b;
                PBoolPtr feedback(new PBool(false));
                getParams(params, a, b);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateTakeOut(a->get(), b->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[TakeOut " << a->get() << " " << b->get() << "|" 
                    << (feedback->get() ? "true" : "false") << "]\n";
            }
            else if (action_name->get() == "AskLoc")
            {
                PUIntPtr a;
                PStringPtr feedback(new PString("not_known"));
                getParams(params, a);
                if (mGrader) 
                    feedback->get() = mGrader->grader->EvaluateAskLoc(a->get());

                ret_msg->serialze(wrapParams(feedback));
                mResult << "\t[AskLoc " << a->get() << "|" 
                    << feedback->get() << "]\n";
            }
            else if (action_name->get() == "Sense")
            {
                PArrayPtr feedback(new PArray);
                vector<unsigned int> A;
                if (mGrader) mGrader->grader->EvaluateSense(A);

                mResult << "\t[Sense |";
                for (unsigned int i = 0; i < A.size(); ++i)
                {
                    feedback->get().push_back(PUIntPtr(new PUInt(A[i])));
                    mResult << " " << A[i];
                }
                mResult << "]\n";

                ret_msg->serialze(wrapParams(feedback));
            }
            else if (action_name->get() == "Fini")
            {
                PDoublePtr time;
                getParams(params, time);
                mTime = time->get();

                break;
            }

            shared_array<uint8_t> write_buffer;
            uint32_t write_size = 0;
            
            write_size = ret_msg->getHead(write_buffer);
            asio::write(mChannel->socket, asio::buffer(write_buffer.get(), write_size));
            write_size = ret_msg->getBody(write_buffer);
            asio::write(mChannel->socket, asio::buffer(write_buffer.get(), write_size));
        }
        catch (boost::system::system_error& err)
        {
            if (err.code() != asio::error::connection_reset &&
                err.code() != asio::error::connection_aborted &&
                err.code() != asio::error::operation_aborted &&
                err.code() != asio::error::eof &&
                err.code() != asio::error::broken_pipe)
            {
                cout << "# Connection with the team '" << mChannel->name
                    << "' has been shut down\n\tError Message='" << err.what() << "'\n";
            }
            break;
        }
    }
}

boost::shared_ptr<Platform::TestDesc> Platform::getTestDesc(const std::string& file_path)
{
    TiXmlDocument doc(file_path);

    do 
    {
        if (!doc.LoadFile())
        {
            cout << "Could not parse '" << file_path << "'\n\tMessage='"
                << doc.ErrorDesc() << "'\n";
            break;
        }

        boost::shared_ptr<TestDesc> desc(new TestDesc);
        desc->is_empty = false;

        TiXmlElement* test_sec = doc.FirstChildElement("test");
        if (test_sec == NULL) break;

        TiXmlElement* env = test_sec->FirstChildElement("env");
        if (env == NULL) break;

        const char* att_mis = env->Attribute("mis");
        if (att_mis && strcmp(att_mis, "on") == 0) desc->is_missing = true;

        const char* att_err = env->Attribute("err");
        if (att_err && strcmp(att_err, "on") == 0) desc->is_erroneous = true;

        const char* att_ans = env->Attribute("ans");
        if (att_ans && strcmp(att_ans, "on") == 0) desc->is_misleading = true;

        TiXmlElement* info = env->FirstChildElement("info");
        if (info == NULL) break;
        const char* info_str = info->GetText();
        if (info_str == NULL) break;
        desc->info = info_str;

        TiXmlElement* mis = env->FirstChildElement("mis");
        if (mis)
        {
            const char* mis_str = mis->GetText();
            if (mis_str) desc->missing_info = mis_str;
        }

        TiXmlElement* err = env->FirstChildElement("err");
        if (err)
        {
            TiXmlElement* right = err->FirstChildElement("r");
            if (right)
            {
                const char* rstr = right->GetText();
                if (rstr) desc->correct_info = rstr;
            }

            TiXmlElement* wrong = err->FirstChildElement("w");
            if (wrong)
            {
                const char* wstr = wrong->GetText();
                if (wstr) desc->erroneous_info = wstr;
            }
        }

        TiXmlElement* extra = env->FirstChildElement("extra");
        if (extra)
        {
            const char* extra_str = extra->GetText();
            if (extra_str) desc->extra_info = extra_str;
        }

        TiXmlElement* instr = test_sec->FirstChildElement("instr");
        if (instr == NULL) break;
        const char* instr_str = instr->GetText();
        if (instr_str == NULL) break;
        desc->instruction = instr_str;

        TiXmlElement* nl = test_sec->FirstChildElement("nl");
        if (nl)
        {
            const char* nl_str = nl->GetText();
            if (nl_str) desc->nl = nl_str;
        }

        return desc;
    } while (false);

    return boost::shared_ptr<TestDesc>(new TestDesc);
}
