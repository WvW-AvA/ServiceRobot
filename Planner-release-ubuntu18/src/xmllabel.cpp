/*
 * Simulation@Home Competition
 * File: xmllabel.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "cserver/xmllabel.hpp"

using namespace _home;

//////////////////////////////////////////////////////////////////////////
#ifdef HOME_XML_DEBUG
const char* XmlLabel::Message   = "message";
const char* XmlLabel::MsgAtt    = "attribute";

const char* XmlLabel::PVoid     = "void";
const char* XmlLabel::PInt      = "int";
const char* XmlLabel::PUInt     = "uint";
const char* XmlLabel::PDouble   = "double";
const char* XmlLabel::PBool     = "bool";
const char* XmlLabel::PString   = "string";
const char* XmlLabel::PBuffer   = "buffer";
const char* XmlLabel::PArray    = "array";
const char* XmlLabel::PStruct   = "struct";

const char* XmlLabel::AttBeginPos   = "begin";
const char* XmlLabel::AttEndPos     = "end";
#else
const char* XmlLabel::Message   = "msg";
const char* XmlLabel::MsgAtt    = "att";

const char* XmlLabel::PVoid     = "v";
const char* XmlLabel::PInt      = "i";
const char* XmlLabel::PUInt     = "u";
const char* XmlLabel::PDouble   = "d";
const char* XmlLabel::PBool     = "b";
const char* XmlLabel::PString   = "s";
const char* XmlLabel::PBuffer   = "f";
const char* XmlLabel::PArray    = "a";
const char* XmlLabel::PStruct   = "t";

const char* XmlLabel::AttBeginPos   = "b";
const char* XmlLabel::AttEndPos     = "e";
#endif

//////////////////////////////////////////////////////////////////////////
#ifdef HOME_XML_DEBUG
const char* XmlLabelProtocol::ActionName = "action_name";
#else
const char* XmlLabelProtocol::ActionName = "act";
#endif
