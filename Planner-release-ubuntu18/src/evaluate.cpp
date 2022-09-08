/*!
======================================================
 Simulation@Home Competition
 evaluate.cpp: For evaluate.h: Evaluate class for evaluateing and responsing a competitor
 Written by Jianmin JI (jianmin@ustc.edu.cn)
======================================================
2012-10-24
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <boost/regex.hpp>
#include "sfsexp/sexp.h"
#include "sfsexp/sexp_ops.h"
#include "evaluate.h"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::stringstream;

/*!
��ʼ�� log �ļ������������������� ASP ����ļ��Ƿ��ڣ������������
wrongas ��ʼ��Ϊ 0����һ������ȷ�𰸡�
count_ins ��ʼ��Ϊ 0.
mislead ��ʼ�� 0.
score ��ʼ�� 0.
*/
Evaluate::Evaluate()
{
	//�����ʱ��
	std::srand((unsigned) std::time(0));

	count_ins = 0;
	mislead = 0;
	score = 0;
	flog = new ofstream(VLOGFILE);
	if( !flog->is_open() )
	{
		print_error(string(VLOGFILE) + " can not be created!");
	}
	ifstream iftest(VEVALUE);
	if( !iftest.is_open() )
	{
		print_error(string(VEVALUE) + " is not existed!");
	}
	iftest.close();
	iftest.open(VCONS);
	if( !iftest.is_open() )
	{
		print_error(string(VCONS) + " is not existed!");
	}
	iftest.close();
	iftest.open(VTASK);
	if( !iftest.is_open() )
	{
		print_error(string(VTASK) + " is not existed!");
	}
	iftest.close();
	iftest.open(VSHOW);
	if( !iftest.is_open() )
	{
		print_error(string(VSHOW) + " is not existed!");
	}
	iftest.close();
	iftest.open(VRUNACT);
	if( !iftest.is_open() )
	{
		print_error(string(VRUNACT) + " is not existed!");
	}
	iftest.close();
	iftest.open(VRUNTASK);
	if( !iftest.is_open() )
	{
		print_error(string(VRUNTASK) + " is not existed!");
	}
	iftest.close();
}

/*!
�ر� log �ļ���
*/
Evaluate::~Evaluate()
{
	if (!flog)
	{
		flog->close();
		delete flog;
	}
}

/*!
�� sexp ��ʽ�� domian �� <state> ת��Ϊ asp �� string ��ʽ���ء�
ʧ�ܷ��ؿ��ַ�����
<state> ��ʽΪ������ (at 3 2)���������Ϊ at(3, 2).
�м����ɵĸ������򣬴��� base �С�
*/
string Evaluate::state_to_asp(const sexp_t * sx, string & base)
{
	char tmp_buffer[SEXPSIZE];
	string tmp_word;
	string tmp_num;
	string tmp_rule = "";
	string tmp_base = "";

	unsigned int tmp_loc;

	if( sx == NULL )
	{
		return "";
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <state> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<state> ��ʽΪ (X x) �� (X x x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <state> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<sort> ��ʽΪ (sort <num> <sname>)
	if( tmp_type == string(D_SORT) )
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <sort> should be the form of (sort <num> <sname>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = " ";
		tmp_base = tmp_word + "(" + tmp_num + ").";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
	}//sort
	//<color>: (color <num> <cname>)
	else if( tmp_type == string(D_COLOR) )
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <color> should be the form of (color <num> <cname>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = " ";
		tmp_base = tmp_word + "(" + tmp_num + ").";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
	}//color
	//<size>: (size <num> big/small)
	else if( tmp_type == string(D_SIZE) )
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <size> should be the form of (size <num> big/small) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = " ";
		tmp_base = tmp_word + "(" + tmp_num + ").";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
	}//size
	//<type>: (type <num> container)
	else if (tmp_type == string(D_TYPE))
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <type> should be the form of (type <num> container) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = " ";
		tmp_base = tmp_word + "(" + tmp_num + ").";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}

		sscanf(tmp_num.c_str(), "%u", &tmp_loc);
		in_loc.push_back(tmp_loc);
	}//type
	//<location>: (at <num> <loc>)
	else if (tmp_type == string(D_LOC))
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <location> should be the form of (at <num> <loc>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = "h(at(" + tmp_num + ", " + tmp_word + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		tmp_base += " loc(" + tmp_word + ").";

		sscanf(tmp_word.c_str(), "%u", &tmp_loc);
		at_loc.push_back(tmp_loc);
	}//location
	//<inside>: (inside <num> <num>)
	else if (tmp_type == string(D_IN))
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <inside> should be the form of (inside <num> <num>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = "h(inside(" + tmp_num + ", " + tmp_word + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		if( tmp_word != "0" )
		{
			tmp_base += " obj(" + tmp_word + ").";
		}
		tmp_base += " container(" + tmp_word + ").";
	}//inside
	//<position>: (on <num> <num>)
	else if (tmp_type == string(D_ON))
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <position> should be the form of (on <num> <num>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = "h(on(" + tmp_num + ", " + tmp_word + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		if( tmp_word != "0" )
		{
			tmp_base += " obj(" + tmp_word + ").";
		}
	}//on
	//<position>: (near <num> <num>)
	else if (tmp_type == string(D_NEAR))
	{
		tmp_num = string(tmp_s->val);
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <position> should be the form of (near <num> <num>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = "h(near(" + tmp_num + ", " + tmp_word + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		if( tmp_word != "0" )
		{
			tmp_base += " obj(" + tmp_word + ").";
		}
	}//near
	//<door>: (opened <num>)
	else if( tmp_type == string(D_OPEN) )
	{
		tmp_num = string(tmp_s->val);
		tmp_rule = "h(opened(" + tmp_num + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		tmp_base += " container(" + tmp_num + ").";
	}//opened
	//<door>: (closed <num>)
	else if( tmp_type == string(D_CLOSE) )
	{
		tmp_num = string(tmp_s->val);
		tmp_rule = "h(closed(" + tmp_num + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + ").";
		}
		tmp_base += " container(" + tmp_num + ").";
	}//closed
	//<plate>: (plate <num>)
	else if( tmp_type == string(D_PLATE) )
	{
		tmp_num = string(tmp_s->val);
		tmp_rule = "h(plate(" + tmp_num + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + "). small(" + tmp_num + ").";
		}
	}//plate
	//<hold>: (hold <num>)
	else if( tmp_type == string(D_HOLD) )
	{
		tmp_num = string(tmp_s->val);
		tmp_rule = "h(hold(" + tmp_num + "), 0).";
		tmp_base = "";
		if( tmp_num != "0" )
		{
			tmp_base += "obj(" + tmp_num + "). small(" + tmp_num + ").";
		}
	}//hold
	else
	{
		print_error("Unrecognized s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	base = tmp_base;
	return tmp_rule;
}

/*!
�� sexp ��ʽ�ĳ��������������ת���� vstate.lp��
���������Ĺ��򣬼��뵽 vbase.lp��
sexp ��ʽΪ��(:domain <state>* )
*/
int Evaluate::domain_to_asp(const sexp_t * sx)
{
	vector<string> tmp_vstate;
	vector<string> tmp_vbase;
	string tmp_state;
	string tmp_base;
	
	char tmp_buffer[SEXPSIZE];

	if( sx == NULL )
	{
		return 0;
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A domain should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	const sexp_t * tmp_s = sx->list;
	//һ��ʼ��ʽΪ (:domain <state>*)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || string(tmp_s->val) != string(D_DOMAIN) )
	{
		print_error("A domain should be in the form (:domain <state>*) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	//��������� <state>
	for( tmp_s = tmp_s->next; tmp_s != NULL; tmp_s = tmp_s->next )
	{
		tmp_state = state_to_asp(tmp_s, tmp_base);
		if( tmp_state == "" )
		{
			return 0;
		}
		else
		{
			tmp_vstate.push_back(tmp_state);
			tmp_vbase.push_back(tmp_base);
		}
	}

	//�������ӡ�� vstate.lp
	ofstream f_vstate(VEFILE);
	if( !f_vstate.is_open() )
	{
		print_error( string(VEFILE) + " can not be created!" );
		return 0;
	}
	for( size_t i = 0; i < tmp_vstate.size(); i ++ )
	{
		f_vstate << tmp_vstate[i] << std::endl;
	}
	f_vstate.close();

	//�������ӡ�� vbase.lp
	ofstream f_vbase(VBASE);
	if( !f_vbase.is_open() )
	{
		print_error( string(VBASE) + " can not be created!" );
		return 0;
	}
	for( size_t i = 0; i < tmp_vbase.size(); i ++ )
	{
		f_vbase << tmp_vbase[i] << std::endl;
	}
	f_vbase.close();
	
	return 1;
}

/*!
�����������������õ� vstate.lp���д��� 0.
et Ϊ���ű��ʽ��ʽ���ַ�����
*/
int Evaluate::init_et(const char* et, size_t len )
{
	//ͨ�� sfexp ����
	pcont_t * cc = NULL;
	sexp_t * sx = NULL;

	cc = init_continuation(const_cast<char*>(et));
	sx = (sexp_t *) iparse_sexp(const_cast<char*>(et), len, cc);
	
	while( sx != NULL )
	{
		if( !domain_to_asp(sx) )
		{
			return 0;
		}
		destroy_sexp(sx);
		sx = (sexp_t *) iparse_sexp(const_cast<char*>(et), len, cc);
	}//while

	destroy_continuation(cc);
	return 1;
}

/*!
�� sexp ��ʽ�� <predicate> ת��Ϊ P(X, Y) ��ʽ��
<predicate> ���� state��ֻ�������� num ���� <variable>��
ʧ�ܷ��� ""
*/
string Evaluate::pred_to_asp(const sexp_t * sx)
{
	char tmp_buffer[SEXPSIZE];
	string tmp_word;
	string tmp_var;
	string tmp_rule = "";

	if( sx == NULL )
	{
		return "";
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <predicate> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<predicate> ��ʽΪ (X x) �� (X x x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <predicate> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<sort> ��ʽΪ (sort <variable> <sname>)
	if( tmp_type == string(D_SORT) )
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <sort> should be the form of (sort <variable> <sname>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = tmp_word + "(" + tmp_var + ")";
	}//sort
	//<color>: (color <variable> <cname>)
	else if( tmp_type == string(D_COLOR) )
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <color> should be the form of (color <variable> <cname>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = tmp_word + "(" + tmp_var + ")";
	}//color
	//<size>: (size <variable> big/small)
	else if( tmp_type == string(D_SIZE) )
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <size> should be the form of (size <variable> big/small) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = tmp_word + "(" + tmp_var + ")";
	}//size
	//<type>: (type <variable> container)
	else if (tmp_type == string(D_TYPE))
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <type> should be the form of (type <variable> container) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		tmp_rule = tmp_word + "(" + tmp_var + ")";
	}//type
	//<location>: (at <variable> <variable>)
	else if (tmp_type == string(D_LOC))
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <location> should be the form of (at <variable> <variable>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_word[0] > 'Z' || tmp_word[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_word);
			return "";
		}

		tmp_rule = "at(" + tmp_var + ", " + tmp_word + ")";
	}//location
	//<inside>: (inside <variable> <variable>)
	else if (tmp_type == string(D_IN))
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <inside> should be the form of (inside <variable> <variable>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_word[0] > 'Z' || tmp_word[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_word);
			return "";
		}

		tmp_rule = "inside(" + tmp_var + ", " + tmp_word + ")";
	}//inside
	//<position>: (on <variable> <variable>)
	else if (tmp_type == string(D_ON))
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <position> should be the form of (on <variable> <variable>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_word[0] > 'Z' || tmp_word[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_word);
			return "";
		}

		tmp_rule = "on(" + tmp_var + ", " + tmp_word + ")";
	}//on
	//<position>: (near <variable> <variable>)
	else if (tmp_type == string(D_NEAR))
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE )
		{
			print_error("A <position> should be the form of (near <variable> <variable>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_word = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_word[0] > 'Z' || tmp_word[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_word);
			return "";
		}

		tmp_rule = "near(" + tmp_var + ", " + tmp_word + ")";
	}//near
	//<door>: (opened <variable>)
	else if( tmp_type == string(D_OPEN) )
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_rule = "opened(" + tmp_var + ")";
	}//opened
	//<door>: (closed <variable>)
	else if( tmp_type == string(D_CLOSE) )
	{
		tmp_var = string(tmp_s->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_rule = "closed(" + tmp_var + ")";
	}//closed
	//<plate>: (plate <variable>)
	else if( tmp_type == string(D_PLATE) )
	{
		tmp_var = string(tmp_s->val);
			//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_rule = "plate(" + tmp_var + ")";
	}//plate
	else if( tmp_type == string(D_HOLD) )
	{
		tmp_var = string(tmp_s->val);
			//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}

		tmp_rule = "hold(" + tmp_var + ")";
	}//hold
	else
	{
		print_error("Unrecognized s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}
	return tmp_rule;
}

/*!
�� sexp ��ʽ�� <tname> ת��Ϊ P(X, Y) ��ʽ��
<tname> ����: (puton <variable> <variable>)��ֱ�Ӷ�ӦΪ puton(<variable> <variable>).
ʧ�ܷ��� ""
*/
string Evaluate::tname_to_asp(const sexp_t * sx)
{
	char tmp_buffer[SEXPSIZE];
	string tmp_rule;
	string tmp_var;

	if( sx == NULL )
	{
		return "";
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <tname> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<tname> ��ʽΪ (X x x) �� (X x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <tname> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}
	tmp_rule = string(tmp_s->val) + "(";
	tmp_var = string(tmp_s->next->val);
	//�Ǵ�д��ĸ��ͷ
	if( (tmp_var[0] > 'Z' || tmp_var[0] < 'A') && tmp_var != string("human") )
	{
		print_error("A <variable> should be started with an uppercase letter in:");
		print_error(tmp_var);
		return "";
	}
	tmp_rule += tmp_var;

	//����
	if( tmp_s->next->next == NULL )
	{
		tmp_rule += ")";
	}//if
	//˫��
	else
	{
		if( tmp_s->next->next->ty != SEXP_VALUE )
		{
			print_error("A <tname> should be the form of (X x x) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return "";
		}
		tmp_var = string(tmp_s->next->next->val);
		//�Ǵ�д��ĸ��ͷ
		if( tmp_var[0] > 'Z' || tmp_var[0] < 'A' )
		{
			print_error("A <variable> should be started with an uppercase letter in:");
			print_error(tmp_var);
			return "";
		}
		tmp_rule += ", " + tmp_var + ")";
	}//else
	return tmp_rule;
}

/*!
�� sexp ��ʽ�� <cond> ת��Ϊ��Ӧ�� , ��ʽ�� : ��ʽ��ͨ������Ĳ������ơ�
form == 0 ��ʾ ����ʽ��form == 1 ��ʾ , ��ʽ��
*/
string Evaluate::cond_to_asp(const sexp_t * sx, bool form)
{
	string tmp_rule = "";
	string tmp_pred;
	char tmp_buffer[SEXPSIZE];

	if( sx == NULL )
	{
		return 0;
	}

	sexp_t * tmp_s = sx->list;
	//<cond> ��ʽΪ��(:cond <predicate>*)
	if( sx->ty != SEXP_LIST || tmp_s->ty != SEXP_VALUE || string(tmp_s->val) != string(I_COND) || tmp_s->next == NULL || tmp_s->next->ty != SEXP_LIST)
	{
		print_error("A <condition> should be the form (:cond <predicate>*) in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	tmp_s = tmp_s->next;
	tmp_pred = pred_to_asp(tmp_s);
	if( tmp_pred == "" )
	{
		return "";
	}
	tmp_rule = tmp_pred;
	tmp_s = tmp_s->next;
	while( tmp_s != NULL )
	{
		tmp_pred = pred_to_asp(tmp_s);
		if( tmp_pred == "" )
		{
			return "";
		}
		if( form )
		{
			tmp_rule += ", " + tmp_pred;
		}
		else
		{
			tmp_rule += " : " + tmp_pred;
		}
		tmp_s = tmp_s->next;
	}
	return tmp_rule;
}

/*!
�� sexp ��ʽ�� ins �� <task> (return 1) �� <cons> (return 2) ת��Ϊ asp �� string ��ʽ��ͨ������������
�����е� <info> (return 3)�����ԡ������� return 0.
*/
int Evaluate::cti_to_asp(const sexp_t * sx, string & rule)
{
	char tmp_buffer[SEXPSIZE];
	string tmp_name;
	string tmp_cond;
	string tmp_rule;
	char tmp_num[8];

	if( sx == NULL )
	{
		return 0;
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <task>|<info>|<cons> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	sexp_t * tmp_s = sx->list;

	//<task>|<info>|<cons> ��ʽΪ (X <>) �� (X <> <>)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_LIST )
	{
		print_error("A <task>|<info>|<cons> should be the form of (X <>) or (X <> <>) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<task> ��ʽΪ (:task <tname> <condition>)
	if( tmp_type == string(I_TASK) )
	{
		//<tname>
		tmp_name = tname_to_asp(tmp_s);
		if( tmp_name == "" )
		{
			return 0;
		}

		//<condition> form 0, : ��ʽ
		tmp_s = tmp_s->next;
		if( tmp_s == NULL || tmp_s->ty != SEXP_LIST )
		{
			print_error("A <condition> should be the form of (:cond <predicate>*) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return 0;
		}

		tmp_cond = cond_to_asp(tmp_s, 0);
		if( tmp_cond == "" )
		{
			return 0;
		}

		//<task> ���Ϊ 1{ task(X, <tname>) : <cond> }1.
		num2str(tmp_num, ++count_ins);
		tmp_rule = "1{ task(" + string(tmp_num) + ", " + tmp_name + ") : " + tmp_cond + " }1.";
		rule = tmp_rule;
		return 1;
	}//<task>
	//<info>: (:info <predicate> <condition>)
	else if( tmp_type == string(I_INFO) )
	{
		//ignore.
		rule = "";
		return 3;
	}//<info>
	//<cons> with (:cons_not <info>|<task>)
	else if( tmp_type == string(I_CONS) )
	{
		tmp_s = tmp_s->list;
		//<info>|<task>: (X <> <>)
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_LIST 
			|| tmp_s->next->next == NULL || tmp_s->next->next->ty != SEXP_LIST )
		{
			print_error("A <info>|<task> should be the form of (X <> <>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return 0;
		}

		tmp_type = string(tmp_s->val);
		tmp_s = tmp_s->next;
		//<task>: (:task <tname> <condition>)
		if( tmp_type == string(I_TASK) )
		{
			tmp_name = tname_to_asp(tmp_s);
			if( tmp_name == "" )
			{
				return 0;
			}

			//<condition> form 1, , ��ʽ
			tmp_s = tmp_s->next;
			tmp_cond = cond_to_asp(tmp_s, 1);
			if( tmp_cond == "" )
			{
				return 0;
			}

			//(:cons_not <task>) ���Ϊ task(X, cons(<tname>)) :- <condition>.
			num2str(tmp_num, ++count_ins);
			tmp_rule = "task(" + string(tmp_num) + ", cons(" + tmp_name + ")) :- " + tmp_cond + ".";
			rule = tmp_rule;
			return 2;
		}//<task>
		//<info>: (:info <predicate> <condition>)
		else if( tmp_type == string(I_INFO) )
		{
			tmp_name = pred_to_asp(tmp_s);
			if( tmp_name == "" )
			{
				return 0;
			}
			//<condition> form 1, , ��ʽ
			tmp_s = tmp_s->next;
			tmp_cond = cond_to_asp(tmp_s, 1);
			if( tmp_cond == "" )
			{
				return 0;
			}

			//(:cons_not <info>) ���Ϊ task(X, cons(<predicate>)) :- <condition>.
			num2str(tmp_num, ++count_ins);
			tmp_rule = "task(" + string(tmp_num) + ", cons(" + tmp_name + ")) :- " + tmp_cond + ".";
			rule = tmp_rule;
			return 2;
		}
		else
		{
			print_error("A <task>|<info> is expected in (:cons_not <task>|<info>) of the s-expression: ");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return 0;
		}
	}//<cons_not>
	//<cons> with (:cons_notnot <info>)
	else if (tmp_type == string(I_NCONS))
	{
		tmp_s = tmp_s->list;
		//<info>: (:info <predicate> <condition>)
		if( tmp_s == NULL || tmp_s->ty != SEXP_VALUE || string(tmp_s->val) != string(I_INFO)
			|| tmp_s->next == NULL || tmp_s->next->ty != SEXP_LIST 
			|| tmp_s->next->next == NULL || tmp_s->next->next->ty != SEXP_LIST )
		{
			print_error("A <info> should be the form of (:info <predicate> <condition>) in the s-expression");
			print_sexp(tmp_buffer, SEXPSIZE, sx);
			print_error(tmp_buffer);
			return 0;
		}

		tmp_s = tmp_s->next;
		tmp_name = pred_to_asp(tmp_s);
		if( tmp_name == "" )
		{
			return 0;
		}
		//<condition> form 1, , ��ʽ
		tmp_s = tmp_s->next;
		tmp_cond = cond_to_asp(tmp_s, 1);
		if( tmp_cond == "" )
		{
			return 0;
		}

		//(:cons_not <info>) ���Ϊ task(X, ncons(<predicate>)) :- <condition>.
		num2str(tmp_num, ++count_ins);
		tmp_rule = "task(" + string(tmp_num) + ", ncons(" + tmp_name + ")) :- " + tmp_cond + ".";
		rule = tmp_rule;
		return 2;
		
	}//<cons_notnot>
	else
	{
		print_error("Unrecognized s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}
}

/*!
�� sexp ��ʽ�����������������ת��Ϊ vcons.lp �� vtask.lp��
sexp ��ʽΪ��(:ins (<task> | <info> | <cons>)*)
*/
int Evaluate::ins_to_asp(const sexp_t * sx)
{
	vector<string> tmp_vtask;
	vector<string> tmp_vcons;
	string tmp_ins;
	int tmp;
	
	char tmp_buffer[SEXPSIZE];

	if( sx == NULL )
	{
		return 0;
	}

	//���ݸ�ʽ��ÿ�� s-exp һ���� list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A ins should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	const sexp_t * tmp_s = sx->list;
	//һ��ʼ��ʽΪ (:ins (<task> | <info> | <cons>)*)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || string(tmp_s->val) != string(I_INS) )
	{
		print_error("A ins should be in the form (:ins (<task> | <info> | <cons>)*) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	//��������� (<task> | <info> | <cons>)
	for( tmp_s = tmp_s->next; tmp_s != NULL; tmp_s = tmp_s->next )
	{
		tmp = cti_to_asp(tmp_s, tmp_ins);
		if( tmp == 1 )
		{
			tmp_vtask.push_back(tmp_ins);
		}
		else if( tmp == 2 )
		{
			tmp_vcons.push_back(tmp_ins);
		}
		else if( tmp == 3)
		{
			//ignore.
		}
		else
		{
			return 0;
		}
	}

	//�������ӡ�� vtask.lp �� vcons.lp
	ofstream f_vtask(VTFILE);
	if( !f_vtask.is_open() )
	{
		print_error( string(VTFILE) + " can not be created!" );
		return 0;
	}
	for( size_t i = 0; i < tmp_vtask.size(); i ++ )
	{
		f_vtask << tmp_vtask[i] << std::endl;
	}
	f_vtask.close();

	ofstream f_vcons(VCFILE);
	if( !f_vcons.is_open() )
	{
		print_error( string(VCFILE) + " can not be created!" );
		return 0;
	}
	for( size_t i = 0; i < tmp_vcons.size(); i ++ )
	{
		f_vcons << tmp_vcons[i] << std::endl;
	}
	f_vcons.close();
	
	return 1;
}

/*!
�� sexp ��ʽ�����������������ת��Ϊ vcons.lp �� vtask.lp��
�д��� 0.
*/
int Evaluate::init_it(const char* it, size_t len )
{
	//ͨ�� sfexp ����
	pcont_t * cc = NULL;
	sexp_t * sx = NULL;

	cc = init_continuation(const_cast<char*>(it));
	sx = (sexp_t *) iparse_sexp(const_cast<char*>(it), len, cc);

	while( sx != NULL )
	{
		if( !ins_to_asp(sx) )
		{
			return 0;
		}
		destroy_sexp(sx);
		sx = (sexp_t *) iparse_sexp(const_cast<char*>(it), len, cc);
	}//while

	destroy_continuation(cc);
	return 1;
}

/*!
���ݳ������� etfile���������� itfile �ͳ������� econfig����ʼ�������ࡣ
etfile, itfile ��Ϊ���ű��ʽ��ʽ��Ҫ������Ӧ�� vstate.lp, vcons.lp �� vtask.lp��
econfig Ϊ xml ��ʽ��Ҫ�����Ƿ�����˻������Ĵ���ش�
��;�д����򷵻� 0.
*/
int Evaluate::init(const char* etfile, const char* itfile, const char* econfig )
{
	at_loc.clear();
	in_loc.clear();
	//��������
	ifstream fet(etfile);
	if( !fet.is_open() )
	{
		print_error(string(etfile) + " can not be opened!");
	}
//	print_log(string("Reading environment description ") + string(etfile));
	string set = "";
	for( string line; std::getline(fet, line); )
	{
		set += line;
	}
//	print_log(string("Converting environment description to ") + string(VEFILE));
	if( !init_et( (char*)(set.c_str()), set.size()) )
	{
		return 0;
	}

	//��������
	ifstream fit(itfile);
	if( !fit.is_open() )
	{
		print_error(string(itfile) + " can not be opened!");
	}
//	print_log(string("Reading instruction description ") + string(itfile));
	set.clear();
	for( string line; std::getline(fit, line); )
	{
		set += line;
	}
//	print_log(string("Converting instruction description to ") + string(VIFILE));
	if( !init_it( (char*) set.c_str(), set.size()) )
	{
		return 0;
	}
	return 1;
}

/*!
�� ASP ���еĽ���ж�ȡ�� action ���к�Ľ����
����ж��޷����У����� 0�����Է��� 1.
result ����Ϊ answer set ����ȡ�Ŀո�Ϊ�ָ��Ĵ𰸡�
�������ݷǳ����� iclingo �������Ҫע��汾��
*/
bool Evaluate::read_as_act(string & result)
{
	ifstream f_result(VRESULT);
	if( !f_result.is_open() )
	{
		print_error(string(VRESULT) + " can not be opened!");
		return 0;
	}

	// �� vanswerset.txt �ж�ȡ��
	for( string line; getline(f_result, line); )
	{
		//���� UNSATISFIABLE
		if( line.find( "UNSATISFIABLE" ) != string::npos )
		{
			print_log("Execution failed!");
			f_result.close();
			return 0;
		}
		//���� Answer:
		if( line.find( "Answer:" ) == 0 )
		{
			//��һ�о��� answer set.
			getline(f_result, line);
			result = line;
			print_log("Execution success!");
			print_log(string("Computed answer set is ") + result);
			f_result.close();
			return 1;
		}
	}//for

	f_result.close();
	return 0;
}

/*!
�� ASP ���к���ת��Ϊ���µ� vstate.lp��
������ȷ���� 1������Ϊ 0.
*/
int Evaluate::asp_to_state(const std::string & result)
{
	vector<string> tmp_file;
	string tmp_str;

	ofstream f_state(VEFILE);
	if( !f_state.is_open() )
	{
		print_error(string(VEFILE) + " can not be opened!");
		return 0;
	}
	
	//ʹ�� boost::regex ����������
	//��״̬��
	string reg_as = "(\\bh\\()((\\w|,|\\(|\\))*)(,1\\))";
	//Լ�����������
	string reg_cons = "(\\bfalse\\((\\w|,)*\\))";
	//�滻������
	string rep_form = "$1$2,0).";

	boost::regex reg(reg_as);
	boost::regex reg1(reg_cons);

	//regex ���� h(*, 1)
	boost::sregex_iterator it(result.begin(), result.end(), reg);
	boost::sregex_iterator end;

	for( ; it != end; ++it )
	{
		const boost::smatch& what = *it;
		tmp_str = what.str();
		tmp_str[tmp_str.size() - 2] = '0';
		tmp_file.push_back(tmp_str + ".");
	}

	//regex ���� false(*��
	boost::sregex_iterator it1(result.begin(), result.end(), reg1);
	boost::sregex_iterator end1;
	
	for( ; it1 != end1; ++ it1 )
	{
		const boost::smatch& what1 = * it1;
		tmp_str = what1.str();
		tmp_file.push_back(tmp_str + "." );
	}

	//д�뵽 vstate.lp
	for( size_t i = 0; i < tmp_file.size(); i ++ )
	{
		f_state << tmp_file[i] << std::endl;
	}
	f_state.close();
	return 1;
}

/*!
����������� Move �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateMove(unsigned int x)
{
	//���ּ�¼
	score -= 4;
	stringstream ss;
	stringstream ss_score;
	ss << "move(" << x << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -4");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(move(" << x << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� PickUp �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluatePickUp(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "pickup(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(pickup(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� PutDown �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluatePutDown(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "putdown(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(putdown(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� ToPlate �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateToPlate(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "toplate(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(toplate(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� FromPlate �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateFromPlate(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "fromplate(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(fromplate(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� Open �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateOpen(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "open(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(open(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� Close �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateClose(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "close(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(close(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� PutIn �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluatePutIn(unsigned int a, unsigned int b)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "putin(" << a << ", " << b << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(putin(" << a << ", " << b << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� TakeOut �ж���ķ�����
ʧ�ܷ��� 0.
*/
bool Evaluate::EvaluateTakeOut(unsigned int a, unsigned int b)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "takeout(" << a << ", " << b << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return 0;
	}
	f_action << "occurs(takeout(" << a << ", " << b << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		if( asp_to_state( tmp ) )
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*!
����������� askloc �ж���ƽ̨������λ����Ϣ��
*/
string Evaluate::EvaluateAskLoc(unsigned int a)
{
	//���ּ�¼
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "askloc(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return "";
	}
	f_action << "occurs(askloc(" << a << "), 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	if( read_as_act(tmp) )
	{
		//ʹ�� boost::regex ����������
		string reg_answer = "(\\banswer\\()((\\w|,|\\(|\\))*)(,1\\))";
		boost::regex reg(reg_answer, boost::regex::perl);
		boost::sregex_iterator it(tmp.begin(), tmp.end(), reg);
		boost::sregex_iterator end;
		if( it == end )
		{
			print_error("Can not find answer(*,1) in the answer set.");
			return "";
		}
		const boost::smatch& what = *it;
		print_log(ss.str() + "'s right result is : " + what[2].str());

		//��ȷ�ش�
		if( ! mislead )
		{
			print_log(string("Return the right answer: ") + what[2].str() );
			return what[2].str();	
		}
		else
		{
			tmp = wrong_as(a, what[2].str());
			print_log(string("Return possible wrong answer: ") + tmp );
			return tmp;
		}
	}
	return "";
}

/*!
�����ʻش�����λ�ý����
60% ��ȷ��30% ����10% ��֪��
*/
string Evaluate::wrong_as(unsigned int a, const string& right)
{
	int t = std::rand() % 10;
	//��ȷ���
	if( t < 6 )
	{
		print_log("By possibility return the right answer set.");
		return right;
	}
	else if ( t == 9 )
	{
		print_log("By possibility return not_known.");
		return string(NOTK);
	}
	//����λ�����
	print_log("By possibility return the wrong answer set.");
	stringstream ss;
	int size = at_loc.size() + in_loc.size();
	t = std::rand() % size;
	if( t < at_loc.size() )
	{
		ss << "at(" << a << "," << at_loc[t] << ")";
		return ss.str();
	}
	else
	{
		ss << "inside(" << a << "," << in_loc[t-at_loc.size()] << ")";
		return ss.str();
	}
}

/*!
����������� sense �ж���ƽ̨����λ�����������壨��ȥ������������
*/
void Evaluate::EvaluateSense(vector<unsigned int>& A_)
{
	A_.clear();
	//���ּ�¼
	score -= 1;
	stringstream ss;
	stringstream ss_score;
	ss << "sense";
	print_log(string("Executing the action: ") + ss.str() + " score -1");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//���ж�д�� vaction.lp
	ofstream f_action( VACT );
	if( !f_action.is_open() )
	{
		print_error(string(VACT) + " can not be created!");
		return;
	}
	f_action << "occurs(sense, 1).";
	f_action.close();

	runas_action();
	print_log(string("running the batch for asp: ") + string(VRUNACT) );

	string tmp;
	unsigned int tmp_a;
	if( read_as_act(tmp) )
	{
		//ʹ�� boost::regex ����������
		string reg_answer = "(\\bsensed\\()(\\d*)(,1\\))";
		boost::regex reg(reg_answer, boost::regex::perl);
		boost::sregex_iterator it(tmp.begin(), tmp.end(), reg);
		boost::sregex_iterator end;
		for( ; it != end; ++ it )
		{
			const boost::smatch& what = *it;
			sscanf(what[2].str().c_str(), "%u", &tmp_a);
			A_.push_back(tmp_a);
		}		
	}

	ss.clear();
	for( size_t i = 0; i < A_.size(); i ++ )
	{
		ss << A_[i] + " ";
	}
	print_log(string("Sense results are: ") + ss.str() );
}

/*!
���������� as��������ܷ֡�
*/
int Evaluate::as_to_score(const string& result)
{
	int tmp_score = 0;
	int tmp_n;
	//ʹ�� boost::regex ����������
	string reg_as = "(\\bvalue\\()(\\d*,)(\\d*)(\\))";
	boost::regex reg(reg_as);

	//regex ����
	boost::sregex_iterator it(result.begin(), result.end(), reg);
	boost::sregex_iterator end;

	for( ; it != end; ++it )
	{
		const boost::smatch& what = *it;
		sscanf(what[3].str().c_str(), "%d", &tmp_n);
		tmp_score += tmp_n;
	}
	return tmp_score;
}

/*!
��������滮��������Ҫ�������յ÷֡�
time ���뵥λΪ s.
*/
int Evaluate::EndEvaluation(double time)
{
	stringstream ss;
	if( time > 5 || time < 0 )
	{
		ss << time;
		print_error(string("Running time is not legal for: ") + ss.str());
		return 0;
	}

	//���� task ��ص� asp ����
	runas_task();
	print_log(string("running the batch for asp: ") + string(VRUNTASK) );

	string tmp;
	int tmp_n;
	if( read_as_act(tmp) )
	{
		tmp_n = as_to_score(tmp);
		score += tmp_n;
		tmp_n = int((5-time)*10);
		score += 2 * tmp_n;
		ss << score;
		print_log(teamname + "'s score is : " + ss.str() );
        return score;
	}
    return 0;
}