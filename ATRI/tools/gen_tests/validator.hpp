/*
 * Simulation@Home Competition
 * File: validator.hpp
 * Author: Jianmin Ji and Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#ifndef __home_validator_HPP__
#define __home_validator_HPP__

#include "cserver/plug.hpp"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "sfsexp/sexp.h"

namespace _testhome
{
	/*!
	在推理过程中调用 ASP 的相关文件和程序名称。
	*/
	static const char* VLOGFILE     = "testlog.txt";
	static const char* VEFILE       = "testvstate.lp";
	static const char* VBASE        = "testvbase.lp";
	static const char* VTFILE       = "testvtask.lp";
	static const char* VCFILE       = "testvcons.lp";
	static const char* VEVALUE      = "testDevil.lp";
	static const char* VCONS        = "testforcons.lp";
	static const char* VTASK        = "testfortask.lp";
	static const char* VACT         = "testvaction.lp";
	static const char* VSHOW        = "testshow.lp";
	static const char* VRESULT      = "testvanswer.txt";
	static const char* VASP         = "iclingo";
#ifdef WIN32
	static const char* VRUNTASK     = "testvruntask.bat";
#else
	static const char* VRUNTASK     = "./testvruntask.sh";
#endif
#ifdef WIN32
    static const char* VRUNKILL     = "testkill.bat";
#else
    static const char* VRUNKILL     = "./testkill.sh";
#endif

	/*!
	将 s-expresion 重新打印出来，其中 buffer 的 size 大小值。
	*/
	static const int SEXPSIZE = 10000;

	/*!
	针对 domain 语法，s-expression 中出现的关键字。
	*/
	static const char* D_DOMAIN = ":domain";
	static const char* D_SORT = "sort";
	static const char* D_COLOR = "color";
	static const char* D_SIZE = "size";
	static const char* D_TYPE = "type";
	static const char* D_LOC = "at";
	static const char* D_IN = "inside";
	static const char* D_ON = "on";
	static const char* D_NEAR = "near";
	static const char* D_OPEN = "opened";
	static const char* D_CLOSE = "closed";
	static const char* D_PLATE = "plate";
	static const char* D_HOLD = "hold";

	/*!
	针对 ins 语法，s-expression 中出现的关键字。
	*/
	static const char* I_INS = ":ins";
	static const char* I_TASK = ":task";
	static const char* I_INFO = ":info";
	static const char* I_COND = ":cond";
	static const char* I_CONS = ":cons_not";
	static const char* I_NCONS = ":cons_notnot";

	/*!
	askloc 回答不知道
	*/
	static const char* NOTK = "not_known";
}// _testhome

namespace _testhome
{
    class Validator : public _home::Plug
    {
    public:
        Validator();

        inline const bool& isValid() const { return is_valid; }

    protected:
        void Plan();

        void Fini();

		void runasp();

	private:
		//! log 文件
		std::ofstream *flog;

		//! 当前任务和约束的序列数
		int count_ins;

		//! 场景中可能的 at 位置。
		std::vector<unsigned int> at_loc;
		//! 场景中可能的 inside 位置。
		std::vector<unsigned int> in_loc;

        //! 判断测试是否可行
        bool is_valid;

        //! 可行测试列表文件
        std::ofstream test_list;

		//! 将 sexp 形式的 domian 中 <state> 转换为 asp 的 string 形式返回。
		std::string state_to_asp(const sexp_t * sx, std::string & base);

		//! 将 sexp 形式的场景描述解读，并转换到 vstate.lp。
		int domain_to_asp(const sexp_t * sx);

		//! 将 sexp 形式的 <predicate> 转换为 P(X, Y) 形式。
		std::string pred_to_asp(const sexp_t * sx);

		//! 将 sexp 形式的 <tname> 转换为 P(X, Y) 形式。
		std::string tname_to_asp(const sexp_t * sx);

		//! 将 sexp 形式的 <cond> 转换为相应的 , 形式或 : 形式，通过输入的参数控制。
		std::string cond_to_asp(const sexp_t * sx, bool form);

		//! 将 sexp 形式的 ins 中 <task> (return 1) 和 <cons> (return 2) 转换为 asp 的 string 形式，通过参数反馈。
		int cti_to_asp(const sexp_t * sx, std::string & rule);

		//! 将 sexp 形式的任务描述解读，并转换为 vcons.lp 和 vtask.lp。
		int ins_to_asp(const sexp_t * sx);

		//! 数字到字符串的转换。
		void num2str(char* str, int num)
		{
#ifdef WIN32
			sprintf_s(str, 8, "%d", num);
#else
			snprintf(str, 8, "%d", num);
#endif
		}

		//! 根据场景描述，任务描述和场景配置初始化整个类。中间有错误返回 0.
		int init();

		//! 根据完整场景描述的字符串得到 vstate.lp，有错返回 0.
		int init_et(const char* et, size_t len );

		//! 根据任务描述的字符串得到 vcons.lp 和 vtask.lp，而忽略 info 信息。有错返回 0.
		int init_it(const char* it, size_t len );

		//! 从 ASP 运行的结果中读取出 action 运行后的结果。
		bool read_as_act(std::string & result);

		//! 从 ASP 结果读出的行动序列，直接转换为平台的行动操作。
		bool as_to_plate(const std::string & as);
		/*!
		输出错误的信息。目前用 cerr 输出。输入为 char *。
		*/
		void print_error(const char* s)
		{
			std::cerr << s << std::endl;
			if( flog->is_open() )
			{
				(*flog) << s << std::endl;
			}
		}
		/*!
		输出错误的信息。目前用 cerr 输出。输入为 string。
		*/
		void print_error(const std::string& s)
		{
			std::cerr << s << std::endl;
			if( flog->is_open() )
			{
				(*flog) << s << std::endl;
			}
		}

		/*!
		向 flog 中，写出相关 log 信息。
		*/
		void print_log(const char* s)
		{
			(*flog) << s << std::endl;
		}
		void print_log(const std::string& s)
		{
			(*flog) << s << std::endl;
		}
    };//Plug

}//_home

#endif//__home_validator_HPP__
//end of file
