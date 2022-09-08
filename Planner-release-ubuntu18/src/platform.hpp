/*
 * Simulation@Home Competition
 * File: platform.hpp
 * Author: Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_platform_HPP__
#define __home_platform_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <list>
#include <sstream>

namespace _home
{

    class SharedLib;
    class Grader;

    class Platform
    {
        //
        // type
        //
    public:
        enum TMode
        {
            M_IT,
            M_NT
        };

    protected:
        struct TChannel;

        typedef boost::scoped_ptr<SharedLib> TBundle;
        struct TGrader;
        typedef boost::shared_ptr<TGrader> TGraderPtr;

        struct TestDesc;

        //
        // interface
        //
    public:
        static boost::shared_ptr<Platform> GetInstance();
        ~Platform();

        bool Init(
            unsigned int _port,
            bool _log = false,
            const std::string &_log_dir = "",
            unsigned int _test_idx = 0,
            unsigned int _timeout = 5000,
            TMode _mode = M_IT,
            const std::string &_test_dir = ".",
            const std::string &_grader_path = "asp",
            const std::string &_grader_arg = "");

        void Run();

    protected:
        Platform();
        Platform(const Platform &);

        //
        // function
        //
    protected:
        bool Listen();

        bool Prepare();

        boost::shared_ptr<TestDesc> getTestDesc(const std::string &file_path);

        void RunTest(unsigned int _test_idx);

        void Dialogue();

        void LogConfInfo();

        void LogResult(
            unsigned int _test_idx,
            const std::string &_es,
            const std::string &_ts,
            const std::string &_result);

        void LogScore(int _score);

        void LogTotalScore(int _score);

        //
        // member
        //
    private:
        unsigned short mPort;

        TMode mMode;
        unsigned int mTimeOut;

        std::string mTestDir;
        std::vector<std::string> mTestVec;

        unsigned int mTestIdx;

        bool mLog;
        std::string mLogDir;

        TGraderPtr mGrader;

        boost::scoped_ptr<boost::asio::io_service> mIOService;
        boost::scoped_ptr<TChannel> mChannel;

        std::stringstream mResult;

        double mTime;
    }; // Platform

} //_home

#endif //__home_platform_HPP__
