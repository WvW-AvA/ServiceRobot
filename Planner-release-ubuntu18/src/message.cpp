/*
 * Simulation@Home Competition
 * File: message.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "cserver/message.hpp"
#include "tinyxml/tinyxml.h"
#include "cserver/serialization.hpp"
#include "cserver/xmllabel.hpp"
#include <memory>

using namespace _home;
using namespace boost;
using namespace std;

//////////////////////////////////////////////////////////////////////////
Message::MsgHead::MsgHead() :
root(new TiXmlElement(XmlLabel::Message)),
curPtr(root)
{
}

Message::MsgHead::~MsgHead()
{
    delete root;
}

//////////////////////////////////////////////////////////////////////////
Message::MsgBody::MsgBody() :
size(0),
capability(1 << 10)
{
    data.reset(new uint8_t[capability]);
    memset(data.get(), 0, capability);
}

void Message::MsgBody::expand()
{
    shared_array<uint8_t> tmp(new uint8_t[capability << 1]);
    capability <<= 1;
    memcpy(tmp.get(), data.get(), size);
    data.swap(tmp);
}

void Message::MsgBody::copy(const void* src, unsigned int _size)
{
    if (size + _size > capability) expand();
    memcpy(data.get() + size, src, _size);
    size += _size;
}

//////////////////////////////////////////////////////////////////////////
void Message::serialze(const ParamListPtr& params)
{
    for (ParamList::const_iterator iter = params->begin();
        iter != params->end();
        ++iter)
    {
        MessagePtr self = shared_from_this();
        (*iter)->serialize(self);
    }
}

bool Message::deserialize(ParamListPtr& params_) const
{
    if (!params_)
    {
        params_.reset(new ParamList);
    }
    
    for (TiXmlElement* iter = mHead.root->FirstChildElement();
        iter;
        iter = iter->NextSiblingElement())
    {
        mHead.curPtr = iter;

        const char* xml_label = iter->Value();
        if (strcmp(xml_label, XmlLabel::MsgAtt) == 0) continue;

        bool success = false;
        if (strcmp(xml_label, XmlLabel::PVoid) == 0)
        {
            boost::shared_ptr<PVoid> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PInt) == 0)
        {
            boost::shared_ptr<PInt> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PUInt) == 0)
        {
            boost::shared_ptr<PUInt> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PDouble) == 0)
        {
            boost::shared_ptr<PDouble> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PBool) == 0)
        {
            boost::shared_ptr<PBool> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PString) == 0)
        {
            boost::shared_ptr<PString> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PBuffer) == 0)
        {
            boost::shared_ptr<PBuffer> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else if (strcmp(xml_label, XmlLabel::PArray) == 0)
        {
            boost::shared_ptr<PArray> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }
        else
        {
            boost::shared_ptr<PStruct> ptr;
            success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
            params_->push_back(ptr);
        }

        mHead.curPtr = mHead.root;
        if (!success) return false;
    }

    return true;
}

void Message::setAttribute(const std::string& att_name, const ParamPtr& att_value)
{
    TiXmlElement* section = mHead.root->FirstChildElement(XmlLabel::MsgAtt);
    if (section == NULL)
    {
        section = new TiXmlElement(XmlLabel::MsgAtt);
        mHead.root->LinkEndChild(section);
    }

    TiXmlElement* attribute = new TiXmlElement(att_name);
    section->LinkEndChild(attribute);

    mHead.curPtr = attribute;
    MessagePtr self = shared_from_this();
    att_value->serialize(self);
    mHead.curPtr = mHead.root;
}

ParamPtr Message::getAttribute(const std::string& att_name) const
{
    TiXmlElement* section = mHead.root->FirstChildElement(XmlLabel::MsgAtt);
    if (section)
    {
        TiXmlElement* att = section->FirstChildElement(att_name);
        if (att)
        {
            att = att->FirstChildElement();
            if (att == NULL) return ParamPtr();

            mHead.curPtr = att;
            const char* xml_label = att->Value();
            bool success = false;
            ParamPtr att_value;
            if (strcmp(xml_label, XmlLabel::PVoid) == 0)
            {
                boost::shared_ptr<PVoid> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PInt) == 0)
            {
                boost::shared_ptr<PInt> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PUInt) == 0)
            {
                boost::shared_ptr<PUInt> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PDouble) == 0)
            {
                boost::shared_ptr<PDouble> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PBool) == 0)
            {
                boost::shared_ptr<PBool> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PString) == 0)
            {
                boost::shared_ptr<PString> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PBuffer) == 0)
            {
                boost::shared_ptr<PBuffer> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PArray) == 0)
            {
                boost::shared_ptr<PArray> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else
            {
                boost::shared_ptr<PStruct> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            mHead.curPtr = mHead.root;

            if (success) return att_value;
        }
    }

    return ParamPtr();
}

Message::AttributeMapPtr Message::getAttributes() const
{
    AttributeMapPtr atts;
    TiXmlElement* section = mHead.root->FirstChildElement(XmlLabel::MsgAtt);
    if (section)
    {
        atts.reset(new AttributeMap);
        for (TiXmlElement* iter = section->FirstChildElement();
            iter;
            iter = iter->NextSiblingElement())
        {
            mHead.curPtr = iter;
            const char* xml_label = iter->Value();
            bool success = false;
            ParamPtr att_value;
            if (strcmp(xml_label, XmlLabel::PVoid) == 0)
            {
                boost::shared_ptr<PVoid> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PInt) == 0)
            {
                boost::shared_ptr<PInt> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PUInt) == 0)
            {
                boost::shared_ptr<PUInt> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PDouble) == 0)
            {
                boost::shared_ptr<PDouble> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PBool) == 0)
            {
                boost::shared_ptr<PBool> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PString) == 0)
            {
                boost::shared_ptr<PString> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PBuffer) == 0)
            {
                boost::shared_ptr<PBuffer> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else if (strcmp(xml_label, XmlLabel::PArray) == 0)
            {
                boost::shared_ptr<PArray> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            else
            {
                boost::shared_ptr<PStruct> ptr;
                success = Deserializer::getInst().deserialize(shared_from_this(), ptr);
                att_value = ptr;
            }
            mHead.curPtr = mHead.root;

            if (success) 
                atts->insert(make_pair(string(iter->Value()), att_value));
            else
                return AttributeMapPtr();
        }
        return atts;
    }
    
    return AttributeMapPtr();
}

bool Message::parse(const boost::shared_array<boost::uint8_t>& data, boost::uint32_t size)
{
    uint32_t null_pos = size;
    for (uint32_t i = 0; i < size; ++i)
    {
        if ((char(data.get()[i])) == '\0')
        {
            null_pos = i;
            break;
        }
    }
    if (null_pos >= size) return false;

    TiXmlDocument doc;
    doc.Parse((char*)data.get());
    if (doc.Error())
    {
        cout << "Failed to parse the head of message\n\tMessage='"
            << doc.ErrorDesc() << "'\n";
        return false;
    }

    if (doc.FirstChildElement(XmlLabel::Message) == NULL)
    {
        cout << "Failed to locate the message XML label '"
            << XmlLabel::Message << "'\n";
        return false;
    }

    if (mHead.root) delete mHead.root;
    mHead.root = dynamic_cast<TiXmlElement*>(
        (doc.FirstChildElement(XmlLabel::Message))->Clone());
    mHead.curPtr = mHead.root;

    mBody.capability = size - null_pos - 1;
    if (mBody.capability > 0)
    {
        mBody.size = mBody.capability;
        mBody.data.reset(new uint8_t[mBody.capability]);
        memcpy(mBody.data.get(), data.get() + null_pos + 1, mBody.size);
    }

    return true;
}

boost::uint32_t Message::getHead(boost::shared_array<boost::uint8_t>& data) const
{
    TiXmlPrinter printer;
    printer.SetStreamPrinting();
    mHead.root->Accept(&printer);

    unsigned int size = printer.Size();
    data.reset(new uint8_t[size + 5]);
    *((uint32_t*)data.get()) = size + mBody.size + 1;
    memcpy(data.get() + 4, printer.CStr(), size);
    data.get()[size + 4] = 0;

    return size + 5;
}

boost::uint32_t Message::getBody(boost::shared_array<boost::uint8_t>& data_) const
{
    data_ = mBody.data;
    return mBody.size;
}

//////////////////////////////////////////////////////////////////////////
ostream& _home::operator<<(ostream& _os_, const Message& msg)
{
    TiXmlPrinter printer;
    msg.mHead.root->Accept(&printer);
    _os_ << printer.Str();
    return _os_;
}
