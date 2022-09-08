/*
 * Simulation@Home Competition
 * File: validator.cpp
 * Author: Jianmin Ji and Jiongkun Xie
 * Affiliation: Multi-Agent Systems Lab.
 *              University of Science and Technology of China
 */

#include "validator.hpp"
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

using namespace _testhome;
using namespace std;


//////////////////////////////////////////////////////////////////////////
Validator::Validator() :
Plug("Validator"),
is_valid(false),
test_list("test.list")
{
	count_ins = 0;

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
	iftest.open(VSHOW);
	if( !iftest.is_open() )
	{
		print_error(string(VSHOW) + " is not existed!");
	}
	iftest.close();
	iftest.open(VRUNTASK);
	if( !iftest.is_open() )
	{
		print_error(string(VRUNTASK) + " is not existed!");
	}
	iftest.close();
}


//////////////////////////////////////////////////////////////////////////
void Validator::Plan()
{
    //cout << "#(Validator): Init" << endl;
    //cout << "#EnvDes:\n" << GetEnvDes() << endl;
    //cout << "#TaskDes:\n" << GetTaskDes() << endl;

    is_valid = false;

	runasp();

}

void Validator::Fini()
{
    //cout << "#(Validator): Fini" << endl;
    system(VRUNKILL);
	if (!flog)
	{
		flog->close();
		delete flog;
	}

    cout << "Test '" << GetTestName() << "' is " << (is_valid ? "valid" : "unvalid") << endl;
    if (isValid())
    {
        test_list << GetTestName() << endl;
    }
}


/*!
�� sexp ��ʽ�� domian �� <state> ת��Ϊ asp �� string ��ʽ���ء�
ʧ�ܷ��ؿ��ַ�����
<state> ��ʽΪ������ (at 3 2)���������Ϊ at(3, 2).
�м����ɵĸ������򣬴��� base �С�
*/
string Validator::state_to_asp(const sexp_t * sx, string & base)
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
			tmp_base += "obj(" + tmp_word + ").";
		}
		tmp_base += "container(" + tmp_word + ").";
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
			tmp_base += "obj(" + tmp_word + ").";
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
			tmp_base += "obj(" + tmp_word + ").";
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
			tmp_base += "obj(" + tmp_num + "). ";
		}
		tmp_base += "container(" + tmp_num + ").";
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
int Validator::domain_to_asp(const sexp_t * sx)
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
int Validator::init_et(const char* et, size_t len )
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
string Validator::pred_to_asp(const sexp_t * sx)
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
string Validator::tname_to_asp(const sexp_t * sx)
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
string Validator::cond_to_asp(const sexp_t * sx, bool form)
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
int Validator::cti_to_asp(const sexp_t * sx, string & rule)
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
int Validator::ins_to_asp(const sexp_t * sx)
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
int Validator::init_it(const char* it, size_t len )
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
int Validator::init()
{
	at_loc.clear();
	in_loc.clear();
	//��������
	const string set = GetEnvDes();
	
	if( !init_et( (char*)(set.c_str()), set.size()) )
	{
		return 0;
	}

	//��������
	const string set1 = GetTaskDes();
	if( !init_it( (char*) set1.c_str(), set1.size()) )
	{
		return 0;
	}
	return 1;
}

/*!
��ʼ������������ asp ����Ȼ������ asp ����
*/
void Validator::runasp()
{
    init();
	system(VRUNTASK);
	string str;
	read_as_act(str);
	as_to_plate(str);
}

/*!
�� ASP ���еĽ���ж�ȡ�� action ���к�Ľ����
����ж��޷����У����� 0�����Է��� 1.
result ����Ϊ answer set ����ȡ�Ŀո�Ϊ�ָ��Ĵ𰸡�
�������ݷǳ����� iclingo �������Ҫע��汾��
*/
bool Validator::read_as_act(string & result)
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
���� ASP �������ж������ַ���ת��Ϊƽ̨�ϵľ����ж���ִ��
*/
bool Validator::as_to_plate(const std::string& as)
{
	map<int, string > mact;
	map<int, string >::iterator mact_it;
	unsigned int num1, num2, time;
	string tmp_s;
	//ʹ�� boost::regex ����������
	string reg_as = "(\\boccurs\\(\\w*\\([\\d,]*\\))(,)(\\d*)(\\))";
	string reg_as1 = "(\\boccurs\\()(\\w*)(\\()(\\d*)(\\))";
	string reg_as2 = "(\\boccurs\\()(\\w*)(\\()(\\d*)(,)(\\d*)(\\))";

	boost::regex reg(reg_as);
	//regex ����
	boost::sregex_iterator it(as.begin(), as.end(), reg);
	boost::sregex_iterator end;
	for( ; it != end; ++it )
	{
		const boost::smatch& what = *it;
//		print_log(what[1].str() + what[3].str());
		sscanf(what[3].str().c_str(), "%u", &time);
		mact[time] = what[1].str();
	}


	boost::regex reg1(reg_as1);
	boost::regex reg2(reg_as2);
	boost::smatch what;

	for( mact_it = mact.begin(); mact_it != mact.end(); ++mact_it )
	{
//		print_log(mact_it->second);
		if( boost::regex_match(mact_it->second, what, reg1) )
		{
			tmp_s = what[2].str();
			sscanf(what[4].str().c_str(), "%u", &num1);
			if( tmp_s == "move" )
			{
				Move(num1);
                is_valid = true;
			}
			else if( tmp_s == "pickup" )
			{
				PickUp(num1);
                is_valid = true;
			}
			else if( tmp_s == "putdown" )
			{
				PutDown(num1);
                is_valid = true;
			}
			else if( tmp_s == "toplate" )
			{
				ToPlate(num1);
                is_valid = true;
			}
			else if( tmp_s == "fromplate" )
			{
				FromPlate(num1);
                is_valid = true;
			}
			else if( tmp_s == "open" )
			{
				Open(num1);
                is_valid = true;
			}
			else if( tmp_s == "close" )
			{
				Close(num1);
                is_valid = true;
			}
			else
			{
				print_error(tmp_s + " is not known!");
				return 0;
			}
		}//if
		else if( boost::regex_match(mact_it->second, what, reg2) )
		{
			tmp_s = what[2].str();
			sscanf(what[4].str().c_str(), "%u", &num1);
			sscanf(what[6].str().c_str(), "%u", &num2);
			if( tmp_s == "putin" )
			{
				PutIn(num1, num2);
                is_valid = true;
			}
			else if( tmp_s == "takeout" )
			{
				TakeOut(num1, num2);
                is_valid = true;
			}
			else
			{
				print_error(tmp_s + " is not known!");
				return 0;
			}
		}
		else
		{
			print_error(mact_it->second + " is not parsed!");
			return 0;
		}
	}
	return 1;
}
