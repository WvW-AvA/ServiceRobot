/*
 * Simulation@Home Competition
 * File: serialization.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_serialization_HPP__
#define __home_serialization_HPP__

#include <string>
#include "message.hpp"
#include "param.hpp"

namespace _home
{
    class Serializer
    {
    public:
        static const Serializer& getInst();

        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PVoid>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PInt>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PUInt>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PDouble>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PBool>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PString>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PBuffer>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PArray>& param) const;
        void serialize(boost::shared_ptr<Message>& _msg_, const boost::shared_ptr<const PStruct>& param) const;

    protected:
        Serializer() {}
    };//Serializer

    class Deserializer
    {
    public:
        static const Deserializer& getInst();

        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PVoid>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PInt>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PUInt>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PDouble>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PBool>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PString>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PBuffer>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PArray>& param_) const;
        bool deserialize(const boost::shared_ptr<const Message>& msg, boost::shared_ptr<PStruct>& param_) const;

    protected:
        Deserializer() {}
    };//Deserializer
}//_home

#endif//__home_serialization_HPP__
