/*
 * Simulation@Home Competition
 * File: param.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "cserver/param.hpp"
#include "cserver/serialization.hpp"
#include "cserver/xmllabel.hpp"
#include <memory>

using namespace _home;
using namespace boost;
using namespace std;

//////////////////////////////////////////////////////////////////////////
HOME_PARAM_DECLARE_BEGIN(DefaultRegister)
HOME_PARAM_DECLARE(PVoid);
HOME_PARAM_DECLARE(PInt);
HOME_PARAM_DECLARE(PUInt);
HOME_PARAM_DECLARE(PDouble);
HOME_PARAM_DECLARE(PBool);
HOME_PARAM_DECLARE(PString);
HOME_PARAM_DECLARE(PBuffer);
HOME_PARAM_DECLARE(PArray);
HOME_PARAM_DECLARE(PStruct);
HOME_PARAM_DECLARE_END(DefaultRegister)

HOME_PARAM_DEFINE_BEGIN(DefaultRegister)
HOME_PARAM_DEFINE(PVoid, XmlLabel::PVoid);
HOME_PARAM_DEFINE(PInt, XmlLabel::PInt);
HOME_PARAM_DEFINE(PUInt, XmlLabel::PUInt);
HOME_PARAM_DEFINE(PDouble, XmlLabel::PDouble);
HOME_PARAM_DEFINE(PBool, XmlLabel::PBool);
HOME_PARAM_DEFINE(PString, XmlLabel::PString);
HOME_PARAM_DEFINE(PBuffer, XmlLabel::PBuffer);
HOME_PARAM_DEFINE(PArray, XmlLabel::PArray);
HOME_PARAM_DEFINE(PStruct, XmlLabel::PStruct);
HOME_PARAM_DEFINE_END(DefaultRegister)

//////////////////////////////////////////////////////////////////////////
ParamPtr Param::genParam(const std::string& type)
{
    const ParamGeneratorMap& map = getGeneratorMap();
    ParamGeneratorMap::const_iterator iter = map.find(type);
    if (iter == map.end())
    {
        cout << "Found no Param generator for the type '"
            << type << "'\n";
        return ParamPtr();
    }
    return (iter->second)();
}

void Param::addParamGenerator(const std::string& type, const ParamGenerator& func)
{
    ParamGeneratorMap& map = getGeneratorMap();
    ParamGeneratorMap::const_iterator iter = map.find(type);
    if (iter != map.end())
        cout << "There already exists a Param generator for the type '"
            << type << "'. Cover the former\n";
       
    map[type] = func;
}

Param::ParamGeneratorMap& Param::getGeneratorMap()
{
    static ParamGeneratorMap inst;
    return inst;
}

//////////////////////////////////////////////////////////////////////////
void PVoid::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PVoid::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PInt::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PInt::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PUInt::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PUInt::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PDouble::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PDouble::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PBool::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PBool::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PString::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PString::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
PBuffer::PBuffer(const boost::uint8_t* buffer, boost::uint32_t size)
{
    if (buffer && size != 0)
    {
        mBuffer.reset(new uint8_t[size]);
        mSize = size;
        memcpy(mBuffer.get(), buffer, size);
    }
    else
    {
        mBuffer.reset();
        mSize = 0;
    }
}

void PBuffer::assign(const boost::uint8_t* buffer, const boost::uint32_t& size)
{
    if (buffer && size != 0)
    {
        mBuffer.reset(new uint8_t[size]);
        mSize = size;
        memcpy(mBuffer.get(), buffer, size);
    }
    else
    {
        mBuffer.reset();
        mSize = 0;
    }
}

void PBuffer::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PBuffer::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PArray::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PArray::shared_from_this());
}

//////////////////////////////////////////////////////////////////////////
void PStruct::serialize(boost::shared_ptr<Message>& _msg_) const
{
    Serializer::getInst().serialize(_msg_, PStruct::shared_from_this());
}

bool PStruct::createMember(const std::string& member_name, const ParamPtr& value)
{
    if (mMembers.find(member_name) != mMembers.end()) 
    {
        cout << "Member '" << member_name << "' already exists\n";
        return false;
    }

    mMembers[member_name] = value;
    return true;
}

ParamPtr PStruct::getMember(const std::string& member_name) const
{
    MemberMap::const_iterator iter = mMembers.find(member_name);
    if (iter == mMembers.end()) return ParamPtr();
    return iter->second;
}

bool PStruct::existMember(const std::string& member_name) const
{
    return mMembers.find(member_name) != mMembers.end();
}
