/*
 * Simulation@Home Competition
 * File: param.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_param_HPP__
#define __home_param_HPP__

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <boost/cstdint.hpp>

namespace _home
{
    class Message;
    class Param;
    typedef boost::shared_ptr<Param>        ParamPtr;
    typedef std::list<ParamPtr>             ParamList;
    typedef boost::shared_ptr<ParamList>    ParamListPtr;

    class Param : private boost::noncopyable
    {
    private:
        typedef boost::function<ParamPtr()> ParamGenerator;
        typedef std::map<std::string, ParamGenerator> ParamGeneratorMap;

    public:
        virtual ~Param() {}

        virtual void serialize(boost::shared_ptr<Message>& _msg_) const = 0;

        static ParamPtr genParam(const std::string& type);

        template<class T>
        static boost::shared_ptr<T> genParam(const std::string& type)
        {
            return boost::dynamic_pointer_cast<T>(genParam(type));
        }

        static void addParamGenerator(const std::string& type, const ParamGenerator& func);

    protected:
        Param() {}

        static ParamGeneratorMap& getGeneratorMap();
    };//Param

    class PVoid : 
        public boost::enable_shared_from_this<PVoid>,
        public Param
    {
    public:
        PVoid() : Param() {}

        void serialize(boost::shared_ptr<Message>& _msg_) const;
    };//PVoid

    typedef boost::shared_ptr<PVoid> PVoidPtr;

    class PInt : 
        public boost::enable_shared_from_this<PInt>,
        public Param
    {
    public:
        PInt(int val = 0) : Param(), mVal(val) {}

        int& get() { return mVal; }

        const int& get() const { return mVal; }

        void serialize(boost::shared_ptr<Message>& _msg_) const;

    private:
        int mVal;
    };//PInt

    typedef boost::shared_ptr<PInt> PIntPtr;

    class PUInt : 
        public boost::enable_shared_from_this<PUInt>,
        public Param
    {
    public:
        PUInt(unsigned int val = 0) : Param(), mVal(val) {}

        void serialize(boost::shared_ptr<Message>& _msg_) const;

        unsigned int& get() { return mVal; }

        const unsigned int& get() const { return mVal; }

    private:
        unsigned int mVal;
    };//PUInt

    typedef boost::shared_ptr<PUInt> PUIntPtr;

    class PDouble : 
        public boost::enable_shared_from_this<PDouble>,
        public Param
    {
    public:
        PDouble(double val = 0.0) : Param(), mVal(val) {}

        void serialize(boost::shared_ptr<Message>& _msg_) const;

        double& get() { return mVal; }

        const double& get() const { return mVal; }

    private:
        double mVal;
    };//PDouble

    typedef boost::shared_ptr<PDouble> PDoublePtr;

    class PBool : 
        public boost::enable_shared_from_this<PBool>,
        public Param
    {
    public:
        PBool(bool val = false) : Param(), mVal(val) {}

        void serialize(boost::shared_ptr<Message>& _msg_) const;

        bool& get() { return mVal; }

        const bool& get() const { return mVal; }

    private:
        bool mVal;
    };//PBool

    typedef boost::shared_ptr<PBool> PBoolPtr;

    class PString : 
        public boost::enable_shared_from_this<PString>,
        public Param
    {
    public:
        PString() : Param(), mStr() {}
        PString(const std::string& str) : Param(), mStr(str) {}

        void serialize(boost::shared_ptr<Message>& _msg_) const;

        std::string& get() { return mStr; }

        const std::string& get() const { return mStr; }

    private:
        std::string mStr;
    };//PString

    typedef boost::shared_ptr<PString> PStringPtr;

    class PBuffer : 
        public boost::enable_shared_from_this<PBuffer>,
        public Param
    {
    public:
        PBuffer() : Param(), mSize(0), mBuffer() {}

        PBuffer(const boost::shared_array<boost::uint8_t>& buffer, boost::uint32_t size) :
        Param(), mSize(size), mBuffer(buffer)
        {}

        PBuffer(const boost::uint8_t* buffer, boost::uint32_t size);

        const boost::shared_array<boost::uint8_t>& get() const { return mBuffer; }

        const boost::uint32_t& size() const { return mSize; }

        void assign(const boost::shared_array<boost::uint8_t>& buffer, const boost::uint32_t& size)
        {
            mBuffer = buffer;
            mSize = size;
        }

        void assign(const boost::uint8_t* buffer, const boost::uint32_t& size);

        void serialize(boost::shared_ptr<Message>& _msg_) const;

    private:
        boost::uint32_t mSize;
        boost::shared_array<boost::uint8_t> mBuffer;
    };//PBuffer

    typedef boost::shared_ptr<PBuffer> PBufferPtr;

    class PArray : 
        public boost::enable_shared_from_this<PArray>,
        public Param
    {
    public:
        typedef std::vector<ParamPtr> ParamVec;

    public:
        PArray() : Param() {}

        ParamVec& get() { return mParams; }

        const ParamVec& get() const { return mParams; }

        template<class T>
        bool get(unsigned int idx, boost::shared_ptr<T>& ptr_) const 
        {
            ptr_ = boost::dynamic_pointer_cast<T>(mParams[idx]);
            return ptr_;
        }

        void serialize(boost::shared_ptr<Message>& _msg_) const;

    private:
        ParamVec mParams;
    };//PArray

    typedef boost::shared_ptr<PArray> PArrayPtr;

    class PStruct : 
        public boost::enable_shared_from_this<PStruct>,
        public Param
    {
    public:
        typedef std::map<std::string, ParamPtr> MemberMap;

    public:
        PStruct() : Param(), mXmlLabel() {}

        virtual ~PStruct() { mMembers.clear(); }

        template<class T>
        inline bool createMember(const std::string& member_name, T* value)
        {
            return createMember(member_name, boost::shared_ptr<T>(value));
        }

        bool createMember(const std::string& member_name, const ParamPtr& value);

        ParamPtr getMember(const std::string& member_name) const;

        template<class T>
        bool getMember(const std::string& member_name, boost::shared_ptr<T>& ptr_) const
        {
            ptr_ = boost::dynamic_pointer_cast<T>(getMember(member_name));
            return ptr_;
        }

        bool existMember(const std::string& member_name) const;

        const MemberMap& getMembers() const { return mMembers; }

        const std::string& getXmlLabel() const { return mXmlLabel; }

        void serialize(boost::shared_ptr<Message>& _msg_) const;

    protected:
        explicit PStruct(const std::string& xmlLabel) : 
        Param(), mXmlLabel(xmlLabel)
        {}

    private:
        std::string mXmlLabel;

        MemberMap mMembers;
    };//PStruct

    typedef boost::shared_ptr<PStruct> PStructPtr;

#define HOME_MACRO_JOINT(a, b) a##b

#define HOME_PARAM_DECLARE_BEGIN(register_name) \
    struct register_name \
    {\
        register_name();

#define HOME_PARAM_DECLARE(type) \
    static ParamPtr HOME_MACRO_JOINT(gen, type)()\
    {\
        return ParamPtr(new type);\
    }

#define HOME_PARAM_DECLARE_END(register_name) \
    };

#define HOME_PARAM_DEFINE_BEGIN(register_name) \
    register_name::register_name()\
    {\

#define HOME_PARAM_DEFINE(type, xml_label) \
    Param::addParamGenerator(xml_label, HOME_MACRO_JOINT(gen, type))

#define HOME_PARAM_DEFINE_END(register_name) \
    }\
    static register_name g##register_name;

    inline ParamListPtr wrapParams()
    {
        return ParamListPtr();
    }

    template<class T1>
    ParamListPtr wrapParams(const boost::shared_ptr<T1>& arg1)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        return ptr;
    }

    template<class T1, class T2>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        return ptr;
    }

    template<class T1, class T2, class T3>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4, class T5>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4,
        const boost::shared_ptr<T5>& arg5)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        ptr->push_back(arg5);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4,
        const boost::shared_ptr<T5>& arg5,
        const boost::shared_ptr<T6>& arg6)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        ptr->push_back(arg5);
        ptr->push_back(arg6);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4,
        const boost::shared_ptr<T5>& arg5,
        const boost::shared_ptr<T6>& arg6,
        const boost::shared_ptr<T7>& arg7)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        ptr->push_back(arg5);
        ptr->push_back(arg6);
        ptr->push_back(arg7);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4,
        const boost::shared_ptr<T5>& arg5,
        const boost::shared_ptr<T6>& arg6,
        const boost::shared_ptr<T7>& arg7,
        const boost::shared_ptr<T8>& arg8)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        ptr->push_back(arg5);
        ptr->push_back(arg6);
        ptr->push_back(arg7);
        ptr->push_back(arg8);
        return ptr;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    ParamListPtr wrapParams(
        const boost::shared_ptr<T1>& arg1,
        const boost::shared_ptr<T2>& arg2,
        const boost::shared_ptr<T3>& arg3,
        const boost::shared_ptr<T4>& arg4,
        const boost::shared_ptr<T5>& arg5,
        const boost::shared_ptr<T6>& arg6,
        const boost::shared_ptr<T7>& arg7,
        const boost::shared_ptr<T8>& arg8,
        const boost::shared_ptr<T9>& arg9)
    {
        ParamListPtr ptr(new ParamList);
        ptr->push_back(arg1);
        ptr->push_back(arg2);
        ptr->push_back(arg3);
        ptr->push_back(arg4);
        ptr->push_back(arg5);
        ptr->push_back(arg6);
        ptr->push_back(arg7);
        ptr->push_back(arg8);
        ptr->push_back(arg9);
        return ptr;
    }

    template<class T1>
    bool getParams(const ParamListPtr& args, boost::shared_ptr<T1>& arg1_)
    {
        if (args && args->size() == 1)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*iter);
                if (!arg1_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_)
    {
        if (args && args->size() == 2)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_)
    {
        if (args && args->size() == 3)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_)
    {
        if (args && args->size() == 4)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4, class T5>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_,
        boost::shared_ptr<T5>& arg5_)
    {
        if (args && args->size() == 5)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                arg5_ = boost::dynamic_pointer_cast<T5>(*(iter++));
                if (!arg5_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_,
        boost::shared_ptr<T5>& arg5_,
        boost::shared_ptr<T6>& arg6_)
    {
        if (args && args->size() == 6)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                arg5_ = boost::dynamic_pointer_cast<T5>(*(iter++));
                if (!arg5_) break;
                arg6_ = boost::dynamic_pointer_cast<T6>(*(iter++));
                if (!arg6_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_,
        boost::shared_ptr<T5>& arg5_,
        boost::shared_ptr<T6>& arg6_,
        boost::shared_ptr<T7>& arg7_)
    {
        if (args && args->size() == 7)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                arg5_ = boost::dynamic_pointer_cast<T5>(*(iter++));
                if (!arg5_) break;
                arg6_ = boost::dynamic_pointer_cast<T6>(*(iter++));
                if (!arg6_) break;
                arg7_ = boost::dynamic_pointer_cast<T7>(*(iter++));
                if (!arg7_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_,
        boost::shared_ptr<T5>& arg5_,
        boost::shared_ptr<T6>& arg6_,
        boost::shared_ptr<T7>& arg7_,
        boost::shared_ptr<T8>& arg8_)
    {
        if (args && args->size() == 8)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                arg5_ = boost::dynamic_pointer_cast<T5>(*(iter++));
                if (!arg5_) break;
                arg6_ = boost::dynamic_pointer_cast<T6>(*(iter++));
                if (!arg6_) break;
                arg7_ = boost::dynamic_pointer_cast<T7>(*(iter++));
                if (!arg7_) break;
                arg8_ = boost::dynamic_pointer_cast<T8>(*(iter++));
                if (!arg8_) break;
                return true;
            } while (false);
        }
        return false;
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    bool getParams(
        const ParamListPtr& args, 
        boost::shared_ptr<T1>& arg1_,
        boost::shared_ptr<T2>& arg2_,
        boost::shared_ptr<T3>& arg3_,
        boost::shared_ptr<T4>& arg4_,
        boost::shared_ptr<T5>& arg5_,
        boost::shared_ptr<T6>& arg6_,
        boost::shared_ptr<T7>& arg7_,
        boost::shared_ptr<T8>& arg8_,
        boost::shared_ptr<T9>& arg9_)
    {
        if (args && args->size() == 9)
        {
            const ParamList& list = *args;
            ParamList::const_iterator iter = list.begin();
            do 
            {
                arg1_ = boost::dynamic_pointer_cast<T1>(*(iter++));
                if (!arg1_) break;
                arg2_ = boost::dynamic_pointer_cast<T2>(*(iter++));
                if (!arg2_) break;
                arg3_ = boost::dynamic_pointer_cast<T3>(*(iter++));
                if (!arg3_) break;
                arg4_ = boost::dynamic_pointer_cast<T4>(*(iter++));
                if (!arg4_) break;
                arg5_ = boost::dynamic_pointer_cast<T5>(*(iter++));
                if (!arg5_) break;
                arg6_ = boost::dynamic_pointer_cast<T6>(*(iter++));
                if (!arg6_) break;
                arg7_ = boost::dynamic_pointer_cast<T7>(*(iter++));
                if (!arg7_) break;
                arg8_ = boost::dynamic_pointer_cast<T8>(*(iter++));
                if (!arg8_) break;
                arg9_ = boost::dynamic_pointer_cast<T9>(*(iter++));
                if (!arg8_) break;
                return true;
            } while (false);
        }
        return false;
    }
}//_home

#endif//__home_param_HPP__
