/*
 * Simulation@Home Competition
 * File: serialization.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "cserver/serialization.hpp"
#include "tinyxml/tinyxml.h"
#include "cserver/xmllabel.hpp"

using namespace _home;
using namespace boost;
using namespace std;

//////////////////////////////////////////////////////////////////////////
const Serializer& Serializer::getInst()
{
    static Serializer instance;
    return instance;
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PVoid>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    _msg_->mHead.curPtr->LinkEndChild(new TiXmlElement(XmlLabel::PVoid));
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PInt>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PInt);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(&param->get(), 4);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PUInt>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PUInt);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(&param->get(), 4);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PDouble>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PDouble);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(&param->get(), 8);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PBool>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PBool);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(&param->get(), 1);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PString>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PString);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(param->get().c_str(), param->get().size());
    elem->SetAttribute(XmlLabel::AttEndPos, _msg_->mBody.size);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PBuffer>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PBuffer);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    elem->SetAttribute(XmlLabel::AttBeginPos, _msg_->mBody.size);
    _msg_->mBody.copy(param->get().get(), param->size());
    elem->SetAttribute(XmlLabel::AttEndPos, _msg_->mBody.size);
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PArray>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = new TiXmlElement(XmlLabel::PArray);
    _msg_->mHead.curPtr->LinkEndChild(elem);

    TiXmlElement* reserved = _msg_->mHead.curPtr;
    _msg_->mHead.curPtr = elem;

    const PArray::ParamVec& vec = param->get();
    for (PArray::ParamVec::const_iterator iter = vec.begin();
        iter != vec.end();
        iter++)
    {
        (*iter)->serialize(_msg_);
    }

    _msg_->mHead.curPtr = reserved;
}

void Serializer::serialize(
                           boost::shared_ptr<Message>& _msg_, 
                           const boost::shared_ptr<const PStruct>& param) const
{
    if (!_msg_) _msg_.reset(new Message);

    TiXmlElement* elem = 
        new TiXmlElement(param->getXmlLabel().empty() ? XmlLabel::PStruct : param->getXmlLabel());
    _msg_->mHead.curPtr->LinkEndChild(elem);

    TiXmlElement* reserved = _msg_->mHead.curPtr;

    const PStruct::MemberMap& map = param->getMembers();
    for (PStruct::MemberMap::const_iterator iter = map.begin();
        iter != map.end();
        ++iter)
    {
        const string& name = iter->first;
        const ParamPtr& ptr = iter->second;
        TiXmlElement* member = new TiXmlElement(name);

        elem->LinkEndChild(member);
        _msg_->mHead.curPtr = member;
        ptr->serialize(_msg_);
    }

    _msg_->mHead.curPtr = reserved;
}

//////////////////////////////////////////////////////////////////////////
const Deserializer& Deserializer::getInst()
{
    static Deserializer instance;
    return instance;
}

#if _MSC_VER >= 1500
#   define SSCANF sscanf_s
#else
#   define SSCANF sscanf
#endif

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PVoid>& param_) const
{
    param_ = Param::genParam<PVoid>(XmlLabel::PVoid);
    return param_!=nullptr;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PInt>& param_) const
{
    param_ = Param::genParam<PInt>(XmlLabel::PInt);
    if (!param_) return false;

    uint32_t begin = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);

    SSCANF(begin_str, "%u", &begin);

    param_->get() = *((int*)(msg->mBody.data.get() + begin));

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PUInt>& param_) const
{
    param_ = Param::genParam<PUInt>(XmlLabel::PUInt);
    if (!param_) return false;

    uint32_t begin = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);

    SSCANF(begin_str, "%u", &begin);

    param_->get() = *((unsigned int*)(msg->mBody.data.get() + begin));

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PDouble>& param_) const
{
    param_ = Param::genParam<PDouble>(XmlLabel::PDouble);
    if (!param_) return false;

    uint32_t begin = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);

    SSCANF(begin_str, "%u", &begin);

    param_->get() = *((double*)(msg->mBody.data.get() + begin));

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PBool>& param_) const
{
    param_ = Param::genParam<PBool>(XmlLabel::PBool);
    if (!param_) return false;

    uint32_t begin = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);

    SSCANF(begin_str, "%u", &begin);

    param_->get() = *((bool*)(msg->mBody.data.get() + begin));

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PString>& param_) const
{
    param_ = Param::genParam<PString>(XmlLabel::PString);
    if (!param_) return false;

    uint32_t begin = 0, end = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);
    const char* end_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttEndPos);

    SSCANF(begin_str, "%u", &begin);
    SSCANF(end_str, "%u", &end);

    param_->get().assign((char*)(msg->mBody.data.get() + begin), end - begin);

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PBuffer>& param_) const
{
    param_ = Param::genParam<PBuffer>(XmlLabel::PBuffer);
    if (!param_) return false;

    uint32_t begin = 0, end = 0;
    const char* begin_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttBeginPos);
    const char* end_str =
        msg->mHead.curPtr->Attribute(XmlLabel::AttEndPos);

    SSCANF(begin_str, "%u", &begin);
    SSCANF(end_str, "%u", &end);

    param_->assign(msg->mBody.data.get() + begin, end - begin);

    return true;
}

#define CASES(xml_label, msg, param_) \
bool success = false;\
if (strcmp(xml_label, XmlLabel::PVoid) == 0)\
{\
    boost::shared_ptr<PVoid> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PInt) == 0)\
{\
    boost::shared_ptr<PInt> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PUInt) == 0)\
{\
    boost::shared_ptr<PUInt> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PDouble) == 0)\
{\
    boost::shared_ptr<PDouble> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PBool) == 0)\
{\
    boost::shared_ptr<PBool> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PString) == 0)\
{\
    boost::shared_ptr<PString> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PBuffer) == 0)\
{\
    boost::shared_ptr<PBuffer> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else if (strcmp(xml_label, XmlLabel::PArray) == 0)\
{\
    boost::shared_ptr<PArray> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}\
else\
{\
    boost::shared_ptr<PStruct> ptr;\
    success = deserialize(msg, ptr);\
    param_ = ptr;\
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PArray>& param_) const
{
    param_ = Param::genParam<PArray>(XmlLabel::PArray);
    if (!param_) return false;

    TiXmlElement* cur_ptr = msg->mHead.curPtr;
    for (TiXmlElement* iter = cur_ptr->FirstChildElement();
        iter;
        iter = iter->NextSiblingElement())
    {
        msg->mHead.curPtr = iter;
        ParamPtr elem;
        CASES(iter->Value(), msg, elem)
        msg->mHead.curPtr = cur_ptr;

        if (!success) return false;
        param_->get().push_back(elem);
    }

    return true;
}

bool Deserializer::deserialize(
                 const boost::shared_ptr<const Message>& msg, 
                 boost::shared_ptr<PStruct>& param_) const
{
    param_ = Param::genParam<PStruct>(msg->mHead.curPtr->Value());
    if (!param_) return false;

    TiXmlElement* cur_ptr = msg->mHead.curPtr;
    for (TiXmlElement* iter = cur_ptr->FirstChildElement();
        iter;
        iter = iter->NextSiblingElement())
    {
        TiXmlElement* value = iter->FirstChildElement();
        if (!value) return false;

        string name = iter->ValueStr();
        msg->mHead.curPtr = value;
        ParamPtr elem;
        CASES(value->Value(), msg, elem)
        msg->mHead.curPtr = cur_ptr;

        if (!success) return false;
        if (!param_->createMember(name, elem)) return false;
    }

    return true;
}

