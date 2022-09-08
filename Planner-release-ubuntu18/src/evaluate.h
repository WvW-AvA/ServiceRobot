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
����������е��� ASP ������ļ��ͳ������ơ�
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
�� s-expresion ���´�ӡ���������� buffer �� size ��Сֵ��
*/
static const int SEXPSIZE = 1000;

/*!
��� domain �﷨��s-expression �г��ֵĹؼ��֡�
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
��� ins �﷨��s-expression �г��ֵĹؼ��֡�
*/
static const char* I_INS = ":ins";
static const char* I_TASK = ":task";
static const char* I_INFO = ":info";
static const char* I_COND = ":cond";
static const char* I_CONS = ":cons_not";
static const char* I_NCONS = ":cons_notnot";

/*!
askloc �ش�֪��
*/
static const char* NOTK = "not_known";

/*!
���������������ֺͷ����ж�������࣬��������Ϊ��
1. ���볡�����������������ͳ������ã�
2. ���ݲ���������ж�����������
   2.1 �������ж����������úͳ��������ش�
   2.2 �Թ۲��ж������ݳ��������ش�
   2.3 ��һ���ж����жϵ�ǰ�������Ƿ��ִ�У������ش�
3. �ڲ����������ʱ����������״̬��������������ʷ�ж�������ʱ�䣬�������յ÷֡�
��ȫ���� asp �������е����⣬c++ ֻ��һ���������ͽӿڡ�
*/
class Evaluate
{
private:
	//! ��ǰ�� team name��
	std::string teamname;
	//! team �����յ÷֡�
	int score;
	//! log �ļ�
	std::ofstream *flog;

	//! ��ǰ�����Լ����������
	int count_ins;

	//! �Ƿ��������𰸡�
	bool mislead;

	//! �����п��ܵ� at λ�á�
	std::vector<unsigned int> at_loc;
	//! �����п��ܵ� inside λ�á�
	std::vector<unsigned int> in_loc;

	//! �� sexp ��ʽ�� domian �� <state> ת��Ϊ asp �� string ��ʽ���ء�
	std::string state_to_asp(const sexp_t * sx, std::string & base);

	//! �� sexp ��ʽ�ĳ��������������ת���� vstate.lp��
	int domain_to_asp(const sexp_t * sx);

	//! �� sexp ��ʽ�� <predicate> ת��Ϊ P(X, Y) ��ʽ��
	std::string pred_to_asp(const sexp_t * sx);

	//! �� sexp ��ʽ�� <tname> ת��Ϊ P(X, Y) ��ʽ��
	std::string tname_to_asp(const sexp_t * sx);

	//! �� sexp ��ʽ�� <cond> ת��Ϊ��Ӧ�� , ��ʽ�� : ��ʽ��ͨ������Ĳ������ơ�
	std::string cond_to_asp(const sexp_t * sx, bool form);

	//! �� sexp ��ʽ�� ins �� <task> (return 1) �� <cons> (return 2) ת��Ϊ asp �� string ��ʽ��ͨ������������
	int cti_to_asp(const sexp_t * sx, std::string & rule);

	//! �� sexp ��ʽ�����������������ת��Ϊ vcons.lp �� vtask.lp��
	int ins_to_asp(const sexp_t * sx);

	//! ���ֵ��ַ�����ת����
	void num2str(char* str, int num)
	{
#ifdef WIN32
        sprintf_s(str, 8, "%d", num);
#else
		snprintf(str, 8, "%d", num);
#endif
	}


public:
	//! ��ʼ�� log �ļ������������������� ASP ����ļ��Ƿ��ڣ������������wrongas ��ʼ��Ϊ 0��
	Evaluate();

	//! �ر� log �ļ���
	~Evaluate();

	//! ���ݳ������������������ͳ������ó�ʼ�������ࡣ�м��д��󷵻� 0.
	int init( const char* etfile, const char* itfile, const char* econfig );

	//! �������������������ַ����õ� vstate.lp���д��� 0.
	int init_et(const char* et, size_t len );

	//! ���������������ַ����õ� vcons.lp �� vtask.lp�������� info ��Ϣ���д��� 0.
	int init_it(const char* it, size_t len );

	//! ��������������ʼһ���µ� team��score���Ϊ0
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
	������ԭ��Ϊ���������ж�д�� vaction.lp������ asp�����ݽ��������������
	*/
	//! ������Ӧ�� bat/bash �ļ������� ASP����� action �� cons��
	void runas_action()
	{
		std::system(VRUNACT);
	}
	//! ������Ӧ�� bat/bash �ļ������� ASP����� task��
	void runas_task()
	{
		std::system(VRUNTASK);
	}

	//! �� ASP ���еĽ���ж�ȡ�� action ���к�Ľ����
	bool read_as_act(std::string & result);

	//! �� ASP ���к���ת��Ϊ���µ� vstate.lp��
	int asp_to_state(const std::string & result);

	//! ���������� as��������ܷ֡�
	int as_to_score(const std::string& result);

	//! ����������� Move �ж���ķ�����
	bool EvaluateMove(unsigned int x);
	//! ����������� PickUp �ж���ķ�����
	bool EvaluatePickUp(unsigned int a);
	//! ����������� PutDown �ж���ķ�����
	bool EvaluatePutDown(unsigned int a);
	//! ����������� ToPlate �ж���ķ�����
	bool EvaluateToPlate(unsigned int a);
	//! ����������� FromPlate �ж���ķ�����
	bool EvaluateFromPlate(unsigned int a);
	//! ����������� Open �ж���ķ�����
	bool EvaluateOpen(unsigned int a);
	//! ����������� Close �ж���ķ�����
	bool EvaluateClose(unsigned int a);
	//! ����������� PutIn �ж���ķ�����
	bool EvaluatePutIn(unsigned int a, unsigned int b);
	//! ����������� TakeOut �ж���ķ�����
	bool EvaluateTakeOut(unsigned int a, unsigned int b);

	//! ����������� askloc �ж���ƽ̨������λ����Ϣ��
	std::string EvaluateAskLoc(unsigned int a);

	//! �����ʻش�����λ�ý����
	std::string wrong_as(unsigned int a, const std::string& right);

	//! ����������� sense �ж���ƽ̨����λ�����������塣
	void EvaluateSense(std::vector<unsigned int>& A_);

	//! ��������滮��������Ҫ�������յ÷֡�
	int EndEvaluation(double time);

	/*!
	����������Ϣ��Ŀǰ�� cerr ���������Ϊ char *��
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
	����������Ϣ��Ŀǰ�� cerr ���������Ϊ string��
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
	�� flog �У�д����� log ��Ϣ��
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
