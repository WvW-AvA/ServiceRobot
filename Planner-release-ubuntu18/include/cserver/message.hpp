/*
 * Simulation@Home Competition
 * File: message.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_message_HPP__
#define __home_message_HPP__

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <boost/cstdint.hpp>

class TiXmlElement;

namespace _home
{
    class Param;
    typedef boost::shared_ptr<Param>        ParamPtr;
    typedef std::list<ParamPtr>             ParamList;
    typedef boost::shared_ptr<ParamList>    ParamListPtr;

    class Message : 
        public boost::enable_shared_from_this<Message>,
        private boost::noncopyable
    {
    public:
        typedef std::map<std::string, ParamPtr> AttributeMap;
        typedef boost::shared_ptr<AttributeMap> AttributeMapPtr;

    protected:
        class MsgHead
        {
        public:
            TiXmlElement* root;
            TiXmlElement* curPtr;

            MsgHead();
            ~MsgHead();
        };

        class MsgBody
        {
        public:
            boost::shared_array<boost::uint8_t> data;
            boost::uint32_t capability;
            boost::uint32_t size;

            MsgBody();
            MsgBody(void* placeholder) : data(), capability(0), size(0) {}

            void expand();

            void copy(const void* src, unsigned int size);
        };

    public:
        Message() : mHead(), mBody() {}

        void serialze(const ParamListPtr& params);

        bool deserialize(ParamListPtr& params_) const;

        template<class T>
        void setAttribute(const std::string& att_name, T* att_value)
        {
            setAttribute(att_name, boost::shared_ptr<T>(att_value));
        }

        void setAttribute(const std::string& att_name, const ParamPtr& att_value);

        ParamPtr getAttribute(const std::string& att_name) const;

        template<class T>
        bool getAttribute(const std::string& att_name, boost::shared_ptr<T>& ptr_) const
        {
            ptr_ = boost::dynamic_pointer_cast<T>(getAttribute(att_name));
            return ptr_!=nullptr;
        }

        AttributeMapPtr getAttributes() const;

        bool parse(
            const boost::shared_array<boost::uint8_t>& data,
            boost::uint32_t size);

        boost::uint32_t getHead(boost::shared_array<boost::uint8_t>& data_) const;

        boost::uint32_t getBody(boost::shared_array<boost::uint8_t>& data_) const;

    private:
        mutable MsgHead mHead;
        MsgBody mBody;

        friend class Serializer;
        friend class Deserializer;
        friend std::ostream& operator<<(std::ostream& os, const Message& msg);
    };//Message

    std::ostream& operator<<(std::ostream& _os_, const Message& msg);
    
    typedef boost::shared_ptr<Message> MessagePtr;
}//_home

#endif//__home_message_HPP__
