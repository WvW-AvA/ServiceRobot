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
	����������е��� ASP ������ļ��ͳ������ơ�
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
	�� s-expresion ���´�ӡ���������� buffer �� size ��Сֵ��
	*/
	static const int SEXPSIZE = 10000;

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
		//! log �ļ�
		std::ofstream *flog;

		//! ��ǰ�����Լ����������
		int count_ins;

		//! �����п��ܵ� at λ�á�
		std::vector<unsigned int> at_loc;
		//! �����п��ܵ� inside λ�á�
		std::vector<unsigned int> in_loc;

        //! �жϲ����Ƿ����
        bool is_valid;

        //! ���в����б��ļ�
        std::ofstream test_list;

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

		//! ���ݳ������������������ͳ������ó�ʼ�������ࡣ�м��д��󷵻� 0.
		int init();

		//! �������������������ַ����õ� vstate.lp���д��� 0.
		int init_et(const char* et, size_t len );

		//! ���������������ַ����õ� vcons.lp �� vtask.lp�������� info ��Ϣ���д��� 0.
		int init_it(const char* it, size_t len );

		//! �� ASP ���еĽ���ж�ȡ�� action ���к�Ľ����
		bool read_as_act(std::string & result);

		//! �� ASP ����������ж����У�ֱ��ת��Ϊƽ̨���ж�������
		bool as_to_plate(const std::string & as);
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
    };//Plug

}//_home

#endif//__home_validator_HPP__
//end of file
