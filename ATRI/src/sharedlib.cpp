/*
 * Simulation@Home Competition
 * File: sharedlib.cpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "sharedlib.h"
#include <iostream>

using namespace _home;
using namespace std;

#ifdef WIN32

#ifdef _UNICODE
#include <afx.h>
#endif

#include <windows.h>

//////////////////////////////////////////////////////////////////////////
bool SharedLib::Open(const std::string &libName)
{
    if (mHandle) Close();
#ifdef _UNICODE
    CString temp((libName + ".dll").c_str());
    mHandle = ::LoadLibrary(temp);
#else
    mHandle = ::LoadLibrary((libName + ".dll").c_str());
#endif

    return (mHandle != NULL);
}

//////////////////////////////////////////////////////////////////////////
void* SharedLib::GetProcAddress(const std::string &procName)
{
    if (mHandle)
    {
        return ::GetProcAddress((HMODULE)mHandle, procName.c_str());
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SharedLib::Close()
{
    if (mHandle)
    {
        ::FreeLibrary((HMODULE)mHandle);
        mHandle = NULL;
    }
}

#else

#include <dlfcn.h>

//////////////////////////////////////////////////////////////////////////
bool SharedLib::Open(const std::string &libName)
{
    if (mHandle) Close();

    mHandle = ::dlopen((libName + ".so").c_str(), RTLD_LAZY);
    if (mHandle == 0)
    {
        cout << "##(SharedLib) Error: failed to dlopen '" << libName << "' with error "
            << dlerror() << endl;
    }

    return mHandle != 0;
}

//////////////////////////////////////////////////////////////////////////
void* SharedLib::GetProcAddress(const std::string &procName)
{
    if (mHandle)
    {
        return ::dlsym(mHandle, procName.c_str());
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
void SharedLib::Close()
{
    if (mHandle)
    {
        ::dlclose(mHandle);
        mHandle = 0;
    }
}

#endif
