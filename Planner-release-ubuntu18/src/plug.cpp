/*
 * Simulation@Home Competition
 * File: plug.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "cserver/plug.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include "tinyxml/tinyxml.h"
#include <cstdio>
#include "cserver/message.hpp"
#include "cserver/param.hpp"
#include "cserver/xmllabel.hpp"
#include <exception>

#ifdef WIN32
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

using namespace _home;
using namespace boost;
using namespace std;

static const unsigned int gBufferSize = 4096;
static boost::array<char, gBufferSize> gBuffer;

//////////////////////////////////////////////////////////////////////////
class Plug::TKernel
{
public:
    TKernel() : mIP("127.0.0.1"),
                mPort(7932),
                mIOService(),
                mSocket(),
                mTimeout(0)
    {
    }

    ~TKernel()
    {
        mSocket.reset();
    }

public:
    std::string mIP;
    unsigned int mPort;
    scoped_ptr<asio::ip::tcp::socket> mSocket;
    asio::io_service mIOService;

    unsigned int mTimeout;
};

//////////////////////////////////////////////////////////////////////////
Plug::Plug(const std::string &_name) : mName(_name),
                                       mTestName("")
{
    mKernel = new TKernel;
    *((uint32_t *)mMsgLen.data()) = 0;

    ifstream ifile("plug.ini");
    if (ifile)
    {
        string line;
        if (getline(ifile, line))
        {
            SSCANF(line.c_str(), "%d", &mKernel->mPort);
        }
        if (getline(ifile, line))
        {
            mKernel->mIP = line;
        }
    }
}

Plug::~Plug()
{
    delete mKernel;
}

//////////////////////////////////////////////////////////////////////////
void Plug::Run()
{
    if (Conncet())
    {
        while (true)
        {
            // waits for the test
            size_t recv_len =
                mKernel->mSocket->receive(asio::buffer(gBuffer));

            if (recv_len)
            {
                TiXmlDocument doc;
                doc.Parse(gBuffer.data());
#ifdef _DEBUG
                if (doc.Error())
                {
                    cout << "#(Plug) TinyXml Error: " << doc.ErrorDesc() << endl;
                    continue;
                }
#endif
                TiXmlElement *test = doc.FirstChildElement("test");
                if (test)
                {
                    TiXmlElement *name = test->FirstChildElement("name");
                    const char *str = name->GetText();
                    if (str)
                        mTestName = string(str);
                    else
                        mTestName = "";
                    TiXmlElement *es = test->FirstChildElement("es");
                    str = es->GetText();
                    if (str)
                        mEnvDes = string(str);
                    else
                        mEnvDes = "";
                    TiXmlElement *ts = test->FirstChildElement("ts");
                    str = ts->GetText();
                    if (str)
                        mTaskDes = string(str);
                    else
                        mTaskDes = "";
                }
                else
                {
                    TiXmlElement *quit = doc.FirstChildElement("quit");
                    if (quit)
                    {
                        break;
                    }
#ifdef _DEBUG
                    else
                    {
                        cout << "#(Plug) Error: no predefined command match" << endl;
                    }
#endif
                    continue;
                }

                // runs the planner
                posix_time::ptime time_begin, time_end;
                posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;

                time_begin = posix_time::microsec_clock::universal_time();
                boost::thread planner(boost::bind(&Plug::Plan, this));
                if (mKernel->mTimeout != 0)
                {
                    try
                    {
                        cout << "Join Plan mTimeout:" << mKernel->mTimeout << endl;

                        planner.timed_join(posix_time::milliseconds(mKernel->mTimeout));
                    }
                    catch (boost::thread_interrupted *)
                    {
                    }
                }
                else
                    planner.join();
                time_end = posix_time::microsec_clock::universal_time();

                time_elapse = time_end - time_begin;

                unsigned int time = time_elapse.total_milliseconds();
                if (mKernel->mTimeout != 0 && time > mKernel->mTimeout)
                    time = mKernel->mTimeout;
                double time_sec = time / 1000.0;
                cout << "#(Plug): Elapsed time=" << time_sec << "s\n";

                // notify the server the planning is finished
                MessagePtr msg(new Message);
                msg->setAttribute(XmlLabelProtocol::ActionName, new PString("Fini"));
                msg->serialze(wrapParams(PDoublePtr(new PDouble(time_sec))));

                shared_array<uint8_t> buffer;
                uint32_t size = 0;

                size = msg->getHead(buffer);
                asio::write(*mKernel->mSocket, asio::buffer(buffer.get(), size));
                size = msg->getBody(buffer);
                asio::write(*mKernel->mSocket, asio::buffer(buffer.get(), size));

                Fini();
            } // if (recv_len)
        }     // while (true)

        mKernel->mSocket->close();
    }
}

//////////////////////////////////////////////////////////////////////////
bool Plug::Move(unsigned int x)
{
    PUIntPtr param_x(new PUInt(x));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("Move"));
    msg->serialze(wrapParams(param_x));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::PickUp(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("PickUp"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::PutDown(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("PutDown"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::ToPlate(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("ToPlate"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::FromPlate(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("FromPlate"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::Open(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("Open"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::Close(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("Close"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::PutIn(unsigned int a, unsigned int b)
{
    PUIntPtr param_a(new PUInt(a));
    PUIntPtr param_b(new PUInt(b));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("PutIn"));
    msg->serialze(wrapParams(param_a, param_b));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

bool Plug::TakeOut(unsigned int a, unsigned int b)
{
    PUIntPtr param_a(new PUInt(a));
    PUIntPtr param_b(new PUInt(b));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("TakeOut"));
    msg->serialze(wrapParams(param_a, param_b));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PBoolPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

std::string Plug::AskLoc(unsigned int a)
{
    PUIntPtr param_a(new PUInt(a));
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("AskLoc"));
    msg->serialze(wrapParams(param_a));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PStringPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    return feedback->get();
}

void Plug::Sense(std::vector<unsigned int> &A_)
{
    MessagePtr msg(new Message);
    msg->setAttribute(XmlLabelProtocol::ActionName, new PString("Sense"));

    MessagePtr ret_msg = SendMsg(msg);

    ParamListPtr list;
    PArrayPtr feedback;
    ret_msg->deserialize(list);
    getParams(list, feedback);

    A_.clear();
    typedef PArray::ParamVec ParamVec;
    const ParamVec &vec = feedback->get();
    for (ParamVec::const_iterator iter = vec.begin();
         iter != vec.end(); ++iter)
    {
        PUIntPtr a_ptr = dynamic_pointer_cast<PUInt>(*iter);
        A_.push_back(a_ptr->get());
    }
}

//////////////////////////////////////////////////////////////////////////
bool Plug::Conncet()
{
    asio::ip::tcp::endpoint ep(
        asio::ip::address_v4::from_string(mKernel->mIP),
        mKernel->mPort);
    boost::system::error_code ec;
    mKernel->mSocket.reset(new asio::ip::tcp::socket(mKernel->mIOService));
    mKernel->mSocket->connect(ep, ec);
    if (ec)
    {
        cout << "#(Plug) Conncetion Error: "
             << system::system_error(ec).what() << endl;
        return false;
    }

    stringstream ss;
    ss << "<name>" << mName << "</name>";
    mKernel->mSocket->send(
        asio::buffer(asio::const_buffer(ss.str().c_str(), ss.str().size())));

    size_t recv_len = mKernel->mSocket->receive(asio::buffer(gBuffer));
    if (recv_len)
    {
        string conf_str(gBuffer.data(), recv_len);
        TiXmlDocument doc;
        doc.Parse(gBuffer.data());
#ifdef _DEBUG
        if (doc.Error())
        {
            cout << "#(Plug) TinyXml Error: " << doc.ErrorDesc() << endl;
            return false;
        }
#endif
        TiXmlElement *conf = doc.FirstChildElement("conf");
        string to = conf->Attribute("to");
        SSCANF(to.c_str(), "%d", &mKernel->mTimeout);
        cout << "#(Plug): Timeout=" << mKernel->mTimeout << endl;
    }
    else
    {
        cout << "#(Plug) Error: wrong format of configuration" << endl;
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
MessagePtr Plug::SendMsg(const MessagePtr &msg)
{
    shared_array<uint8_t> buffer;
    uint32_t size = 0;

    size = msg->getHead(buffer);
    asio::write(*mKernel->mSocket, asio::buffer(buffer.get(), size));
    size = msg->getBody(buffer);
    asio::write(*mKernel->mSocket, asio::buffer(buffer.get(), size));

    asio::read(*mKernel->mSocket, asio::buffer(mMsgLen.data(), 4));
    size = *((uint32_t *)mMsgLen.data());
    buffer.reset(new uint8_t[size]);
    asio::read(*mKernel->mSocket, asio::buffer(buffer.get(), size));

    MessagePtr ret_msg(new Message);
    ret_msg->parse(buffer, size);
    return ret_msg;
}
