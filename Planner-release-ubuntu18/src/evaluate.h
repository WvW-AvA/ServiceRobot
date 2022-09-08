/*!
======================================================
 Simulation@Home Competition
 evaluate.h: Evaluate class for evaluateing and responsing a competitor
 Written by Jianmin JI (jianmin@ustc.edu.cn)
======================================================
2012-10-22
*/

#ifndef __EVALUATE_H_
#define __EVALUATE_H_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "sfsexp/sexp.h"

/*!
在推理过程中调用 ASP 的相关文件和程序名称。
*/
static const char* VLOGFILE     = "evaluelog.txt";
static const char* VEFILE       = "vstate.lp";
static const char* VBASE        = "vbase.lp";
static const char* VTFILE       = "vtask.lp";
static const char* VCFILE       = "vcons.lp";
static const char* VEVALUE      = "evaluate.lp";
static const char* VCONS        = "forcons.lp";
static const char* VTASK        = "fortask.lp";
static const char* VACT         = "vaction.lp";
static const char* VSHOW        = "show.lp";
static const char* VRESULT      = "vanswer.txt";
static const char* VASP         = "iclingo";
#ifdef WIN32
static const char* VRUNACT      = "vrunact.bat";
static const char* VRUNTASK     = "vruntask.bat";
#else
static const char* VRUNACT      = "./vrunact.sh";
static const char* VRUNTASK     = "./vruntask.sh";
#endif

/*!
将 s-expresion 重新打印出来，其中 buffer 的 size 大小值。
*/
static const int SEXPSIZE = 1000;

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

/*!
用来给参赛程序打分和反馈行动结果的类，具体流程为：
1. 读入场景描述，任务描述和场景配置；
2. 根据参赛程序的行动给出反馈：
   2.1 对提问行动，根据配置和场景给出回答。
   2.2 对观察行动，根据场景给出回答。
   2.3 对一般行动，判断当前场景下是否可执行，给出回答。
3. 在参赛程序结束时，根据最终状态，任务描述，历史行动和所用时间，给出最终得分。
完全采用 asp 计算所有的问题，c++ 只是一个翻译器和接口。
*/
class Evaluate
{
private:
	//! 当前的 team name。
	std::string teamname;
	//! team 的最终得分。
	int score;
	//! log 文件
	std::ofstream *flog;

	//! 当前任务和约束的序列数
	int count_ins;

	//! 是否给出错误答案。
	bool mislead;

	//! 场景中可能的 at 位置。
	std::vector<unsigned int> at_loc;
	//! 场景中可能的 inside 位置。
	std::vector<unsigned int> in_loc;

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


public:
	//! 初始化 log 文件，并测试推理中所有 ASP 相关文件是否都在，否则给出错误。wrongas 初始化为 0。
	Evaluate();

	//! 关闭 log 文件。
	~Evaluate();

	//! 根据场景描述，任务描述和场景配置初始化整个类。中间有错误返回 0.
	int init( const char* etfile, const char* itfile, const char* econfig );

	//! 根据完整场景描述的字符串得到 vstate.lp，有错返回 0.
	int init_et(const char* et, size_t len );

	//! 根据任务描述的字符串得到 vcons.lp 和 vtask.lp，而忽略 info 信息。有错返回 0.
	int init_it(const char* it, size_t len );

	//! 按场景描述，开始一个新的 team，score清空为0
	void newteam( const char* name, bool rmislead = 0 )
	{
		teamname = std::string(name);
		count_ins = 0;
		mislead = rmislead;
        score = 0;
		print_log(std::string("Start for a new team: ") + teamname);
		print_log("The initial score is 0.");
	}
	
	/*!
	反馈的原理为，将输入行动写入 vaction.lp，运行 asp，根据结果分析出反馈。
	*/
	//! 运行相应的 bat/bash 文件来调用 ASP，针对 action 和 cons。
	void runas_action()
	{
		std::system(VRUNACT);
	}
	//! 运行相应的 bat/bash 文件来调用 ASP，针对 task。
	void runas_task()
	{
		std::system(VRUNTASK);
	}

	//! 从 ASP 运行的结果中读取出 action 运行后的结果。
	bool read_as_act(std::string & result);

	//! 将 ASP 运行后结果转换为到新的 vstate.lp。
	int asp_to_state(const std::string & result);

	//! 分析读出的 as，计算出总分。
	int as_to_score(const std::string& result);

	//! 参赛程序调用 Move 行动后的反馈。
	bool EvaluateMove(unsigned int x);
	//! 参赛程序调用 PickUp 行动后的反馈。
	bool EvaluatePickUp(unsigned int a);
	//! 参赛程序调用 PutDown 行动后的反馈。
	bool EvaluatePutDown(unsigned int a);
	//! 参赛程序调用 ToPlate 行动后的反馈。
	bool EvaluateToPlate(unsigned int a);
	//! 参赛程序调用 FromPlate 行动后的反馈。
	bool EvaluateFromPlate(unsigned int a);
	//! 参赛程序调用 Open 行动后的反馈。
	bool EvaluateOpen(unsigned int a);
	//! 参赛程序调用 Close 行动后的反馈。
	bool EvaluateClose(unsigned int a);
	//! 参赛程序调用 PutIn 行动后的反馈。
	bool EvaluatePutIn(unsigned int a, unsigned int b);
	//! 参赛程序调用 TakeOut 行动后的反馈。
	bool EvaluateTakeOut(unsigned int a, unsigned int b);

	//! 参赛程序调用 askloc 行动，平台返回其位置信息。
	std::string EvaluateAskLoc(unsigned int a);

	//! 按概率回答错误的位置结果。
	std::string wrong_as(unsigned int a, const std::string& right);

	//! 参赛程序调用 sense 行动，平台返回位置上所有物体。
	void EvaluateSense(std::vector<unsigned int>& A_);

	//! 参赛程序规划结束，需要给出最终得分。
	int EndEvaluation(double time);

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
};






#endif  //__EVALUATE_H_
