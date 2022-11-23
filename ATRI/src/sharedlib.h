/*
 * Simulation@Home Competition
 * File: sharedlib.h
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __salt_sharedlib_H__
#define __salt_sharedlib_H__

#include <string>

namespace _home
{

    /** 
     * @class SharedLibrary 
     * defines a common interface for the usage of shared
     * libraries. The implementation of this class depends on the
     * platform and used compiler. 
     * Now only one implementation in Windows using VC++ compiler is available
     */
    class SharedLib
    {
        //
        //Interface
        //
    public:
        SharedLib() : mHandle(0) {}
        ~SharedLib() { Close(); }

        /** opens a sharedLibrary, releasing a previously opened library */
        bool Open(const std::string &libName);

        /**  
         * returns a pointer to the exported function procName of the
         * library or NULL if the function does not exist.
         */
        void* GetProcAddress(const std::string &procName);

        /** releases a previously opened library */
        void Close();

        /** returns the name of the library */
        const std::string& GetName() const { return mName; }

        //
        //Member
        //
    private:
        /** a platform dependent handle to the managed library */
        void *mHandle;

        /** the name of the library */
        std::string mName;
    };//end of SharedLib

}//end of _home

#endif//end of __salt_sharedlibrary_H__
//end of file
