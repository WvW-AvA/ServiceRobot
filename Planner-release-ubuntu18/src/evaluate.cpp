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
初始化 log 文件，并测试推理中所有 ASP 相关文件是否都在，否则给出错误。
wrongas 初始化为 0，即一定给正确答案。
count_ins 初始化为 0.
mislead 初始化 0.
score 初始化 0.
*/
Evaluate::Evaluate()
{
	//随机数时间
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
关闭 log 文件。
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
将 sexp 形式的 domian 中 <state> 转换为 asp 的 string 形式返回。
失败返回空字符串。
<state> 形式为：例如 (at 3 2)，输出规则为 at(3, 2).
中间生成的辅助规则，存在 base 中。
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <state> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<state> 格式为 (X x) 或 (X x x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <state> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<sort> 格式为 (sort <num> <sname>)
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
将 sexp 形式的场景描述解读，并转换到 vstate.lp。
并将辅助的规则，加入到 vbase.lp。
sexp 形式为：(:domain <state>* )
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A domain should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	const sexp_t * tmp_s = sx->list;
	//一开始格式为 (:domain <state>*)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || string(tmp_s->val) != string(D_DOMAIN) )
	{
		print_error("A domain should be in the form (:domain <state>*) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	//处理后续的 <state>
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

	//将结果打印到 vstate.lp
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

	//将结果打印到 vbase.lp
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
根据完整场景描述得到 vstate.lp，有错返回 0.
et 为括号表达式形式的字符串。
*/
int Evaluate::init_et(const char* et, size_t len )
{
	//通过 sfexp 解析
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
将 sexp 形式的 <predicate> 转换为 P(X, Y) 形式。
<predicate> 类似 state，只不过不是 num 而是 <variable>。
失败返回 ""
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <predicate> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<predicate> 格式为 (X x) 或 (X x x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <predicate> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<sort> 格式为 (sort <variable> <sname>)
	if( tmp_type == string(D_SORT) )
	{
		tmp_var = string(tmp_s->val);
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
		//非大写字母开头
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
			//非大写字母开头
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
			//非大写字母开头
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
将 sexp 形式的 <tname> 转换为 P(X, Y) 形式。
<tname> 例如: (puton <variable> <variable>)，直接对应为 puton(<variable> <variable>).
失败返回 ""
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <tname> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}

	sexp_t * tmp_s = sx->list;

	//<tname> 格式为 (X x x) 或 (X x)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_VALUE )
	{
		print_error("A <tname> should be the form of (X x) or (X x x) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return "";
	}
	tmp_rule = string(tmp_s->val) + "(";
	tmp_var = string(tmp_s->next->val);
	//非大写字母开头
	if( (tmp_var[0] > 'Z' || tmp_var[0] < 'A') && tmp_var != string("human") )
	{
		print_error("A <variable> should be started with an uppercase letter in:");
		print_error(tmp_var);
		return "";
	}
	tmp_rule += tmp_var;

	//单层
	if( tmp_s->next->next == NULL )
	{
		tmp_rule += ")";
	}//if
	//双层
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
		//非大写字母开头
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
将 sexp 形式的 <cond> 转换为相应的 , 形式或 : 形式，通过输入的参数控制。
form == 0 表示 ：形式，form == 1 表示 , 形式。
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
	//<cond> 格式为：(:cond <predicate>*)
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
将 sexp 形式的 ins 中 <task> (return 1) 和 <cons> (return 2) 转换为 asp 的 string 形式，通过参数反馈。
将其中的 <info> (return 3)，忽略。有问题 return 0.
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A <task>|<info>|<cons> should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	sexp_t * tmp_s = sx->list;

	//<task>|<info>|<cons> 格式为 (X <>) 或 (X <> <>)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || tmp_s->next->ty != SEXP_LIST )
	{
		print_error("A <task>|<info>|<cons> should be the form of (X <>) or (X <> <>) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	string tmp_type(tmp_s->val);
	tmp_s = tmp_s->next;

	//<task> 格式为 (:task <tname> <condition>)
	if( tmp_type == string(I_TASK) )
	{
		//<tname>
		tmp_name = tname_to_asp(tmp_s);
		if( tmp_name == "" )
		{
			return 0;
		}

		//<condition> form 0, : 形式
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

		//<task> 输出为 1{ task(X, <tname>) : <cond> }1.
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

			//<condition> form 1, , 形式
			tmp_s = tmp_s->next;
			tmp_cond = cond_to_asp(tmp_s, 1);
			if( tmp_cond == "" )
			{
				return 0;
			}

			//(:cons_not <task>) 输出为 task(X, cons(<tname>)) :- <condition>.
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
			//<condition> form 1, , 形式
			tmp_s = tmp_s->next;
			tmp_cond = cond_to_asp(tmp_s, 1);
			if( tmp_cond == "" )
			{
				return 0;
			}

			//(:cons_not <info>) 输出为 task(X, cons(<predicate>)) :- <condition>.
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
		//<condition> form 1, , 形式
		tmp_s = tmp_s->next;
		tmp_cond = cond_to_asp(tmp_s, 1);
		if( tmp_cond == "" )
		{
			return 0;
		}

		//(:cons_not <info>) 输出为 task(X, ncons(<predicate>)) :- <condition>.
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
将 sexp 形式的任务描述解读，并转换为 vcons.lp 和 vtask.lp。
sexp 形式为：(:ins (<task> | <info> | <cons>)*)
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

	//根据格式，每个 s-exp 一定是 list
	if( sx->ty != SEXP_LIST )
	{
		print_error("A ins should be a list in the s-expression:");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	const sexp_t * tmp_s = sx->list;
	//一开始格式为 (:ins (<task> | <info> | <cons>)*)
	if( tmp_s->ty != SEXP_VALUE || tmp_s->next == NULL || string(tmp_s->val) != string(I_INS) )
	{
		print_error("A ins should be in the form (:ins (<task> | <info> | <cons>)*) in the s-expression");
		print_sexp(tmp_buffer, SEXPSIZE, sx);
		print_error(tmp_buffer);
		return 0;
	}

	//处理后续的 (<task> | <info> | <cons>)
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

	//将结果打印到 vtask.lp 和 vcons.lp
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
将 sexp 形式的任务描述解读，并转换为 vcons.lp 和 vtask.lp。
有错返回 0.
*/
int Evaluate::init_it(const char* it, size_t len )
{
	//通过 sfexp 解析
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
根据场景描述 etfile，任务描述 itfile 和场景配置 econfig，初始化整个类。
etfile, itfile 内为括号表达式形式，要生成相应的 vstate.lp, vcons.lp 和 vtask.lp。
econfig 为 xml 形式，要配置是否给出人机交互的错误回答。
中途有错误则返回 0.
*/
int Evaluate::init(const char* etfile, const char* itfile, const char* econfig )
{
	at_loc.clear();
	in_loc.clear();
	//场景描述
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

	//任务描述
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
从 ASP 运行的结果中读取出 action 运行后的结果。
如果行动无法运行，返回 0，可以返回 1.
result 里面为 answer set 中提取的空格为分隔的答案。
具体内容非常依赖 iclingo 的输出，要注意版本。
*/
bool Evaluate::read_as_act(string & result)
{
	ifstream f_result(VRESULT);
	if( !f_result.is_open() )
	{
		print_error(string(VRESULT) + " can not be opened!");
		return 0;
	}

	// 从 vanswerset.txt 中读取答案
	for( string line; getline(f_result, line); )
	{
		//出现 UNSATISFIABLE
		if( line.find( "UNSATISFIABLE" ) != string::npos )
		{
			print_log("Execution failed!");
			f_result.close();
			return 0;
		}
		//出现 Answer:
		if( line.find( "Answer:" ) == 0 )
		{
			//下一行就是 answer set.
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
将 ASP 运行后结果转换为到新的 vstate.lp。
解析正确返回 1，否则为 0.
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
	
	//使用 boost::regex 来处理结果。
	//新状态。
	string reg_as = "(\\bh\\()((\\w|,|\\(|\\))*)(,1\\))";
	//约束满足情况。
	string reg_cons = "(\\bfalse\\((\\w|,)*\\))";
	//替换后结果。
	string rep_form = "$1$2,0).";

	boost::regex reg(reg_as);
	boost::regex reg1(reg_cons);

	//regex 查找 h(*, 1)
	boost::sregex_iterator it(result.begin(), result.end(), reg);
	boost::sregex_iterator end;

	for( ; it != end; ++it )
	{
		const boost::smatch& what = *it;
		tmp_str = what.str();
		tmp_str[tmp_str.size() - 2] = '0';
		tmp_file.push_back(tmp_str + ".");
	}

	//regex 查找 false(*）
	boost::sregex_iterator it1(result.begin(), result.end(), reg1);
	boost::sregex_iterator end1;
	
	for( ; it1 != end1; ++ it1 )
	{
		const boost::smatch& what1 = * it1;
		tmp_str = what1.str();
		tmp_file.push_back(tmp_str + "." );
	}

	//写入到 vstate.lp
	for( size_t i = 0; i < tmp_file.size(); i ++ )
	{
		f_state << tmp_file[i] << std::endl;
	}
	f_state.close();
	return 1;
}

/*!
参赛程序调用 Move 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateMove(unsigned int x)
{
	//评分记录
	score -= 4;
	stringstream ss;
	stringstream ss_score;
	ss << "move(" << x << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -4");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 PickUp 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluatePickUp(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "pickup(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 PutDown 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluatePutDown(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "putdown(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 ToPlate 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateToPlate(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "toplate(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 FromPlate 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateFromPlate(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "fromplate(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 Open 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateOpen(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "open(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 Close 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateClose(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "close(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 PutIn 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluatePutIn(unsigned int a, unsigned int b)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "putin(" << a << ", " << b << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 TakeOut 行动后的反馈。
失败返回 0.
*/
bool Evaluate::EvaluateTakeOut(unsigned int a, unsigned int b)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "takeout(" << a << ", " << b << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
参赛程序调用 askloc 行动，平台返回其位置信息。
*/
string Evaluate::EvaluateAskLoc(unsigned int a)
{
	//评分记录
	score -= 2;
	stringstream ss;
	stringstream ss_score;
	ss << "askloc(" << a << ")";
	print_log(string("Executing the action: ") + ss.str() + " score -2");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
		//使用 boost::regex 来处理结果。
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

		//正确回答
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
按概率回答错误的位置结果。
60% 正确，30% 错误，10% 不知道
*/
string Evaluate::wrong_as(unsigned int a, const string& right)
{
	int t = std::rand() % 10;
	//正确情况
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
	//错误位置情况
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
参赛程序调用 sense 行动，平台返回位置上所有物体（除去机器人自身）。
*/
void Evaluate::EvaluateSense(vector<unsigned int>& A_)
{
	A_.clear();
	//评分记录
	score -= 1;
	stringstream ss;
	stringstream ss_score;
	ss << "sense";
	print_log(string("Executing the action: ") + ss.str() + " score -1");
	ss_score << score;
	print_log(string("Current socre is: ") + ss_score.str() );

	//将行动写入 vaction.lp
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
		//使用 boost::regex 来处理结果。
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
分析读出的 as，计算出总分。
*/
int Evaluate::as_to_score(const string& result)
{
	int tmp_score = 0;
	int tmp_n;
	//使用 boost::regex 来处理结果。
	string reg_as = "(\\bvalue\\()(\\d*,)(\\d*)(\\))";
	boost::regex reg(reg_as);

	//regex 查找
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
参赛程序规划结束，需要给出最终得分。
time 输入单位为 s.
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

	//运行 task 相关的 asp 程序
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