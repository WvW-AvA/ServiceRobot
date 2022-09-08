/*
 * Simulation@Home Competition
 * File: xmllabel.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_xmllabel_HPP__
#define __home_xmllabel_HPP__

namespace _home
{
    struct XmlLabel
    {
        static const char* Message;
        static const char* MsgAtt;

        static const char* PVoid;
        static const char* PInt;
        static const char* PUInt;
        static const char* PDouble;
        static const char* PBool;
        static const char* PString;
        static const char* PBuffer;
        static const char* PArray;
        static const char* PStruct;

        static const char* AttBeginPos;
        static const char* AttEndPos;
    };//XmlLabel

    struct XmlLabelProtocol
    {
        static const char* ActionName;
    };//XmlLabelRPC
}//_home

#endif//__home_xmllabel_HPP__
