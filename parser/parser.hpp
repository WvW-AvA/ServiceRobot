#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
using namespace std;

#define S (uint8_t)0  //句
#define NP (uint8_t)1 //名词部分
#define VP (uint8_t)2 //动词部分

#define N (uint8_t)3    //名词
#define ADJ (uint8_t)4  //形容词
#define ART (uint8_t)5  //冠词
#define PREP (uint8_t)6 //介词
#define V (uint8_t)7    //动词

#define THERE (uint8_t)8
#define NOT (uint8_t)9
#define MUST (uint8_t)10
#define WHICH (uint8_t)13

struct token
{
    uint8_t type;
    string value;
    token(uint8_t type, const string &value) : type(type), value(value) {}
    token(uint8_t type) : type(type), value("") {}
    token() {}
};
struct syntax_node
{
    token value;
    vector<shared_ptr<syntax_node>> sons;
    syntax_node(const token &v) : value(v) {}
};

class parser
{
public:
    vector<token> tokens;
    vector<shared_ptr<syntax_node>> stack;
    shared_ptr<syntax_node> root;
    unordered_map<string, uint8_t> words_map;

    void parse(const string &str);
    void to_token(string str);
    void push_down_automata();
    void push_down(token &token);
    bool match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last);
    bool match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last, uint8_t last_last);

    void push_back_np(shared_ptr<syntax_node> &np);
    void push_back_vp(shared_ptr<syntax_node> &vp);

    void words_map_initialize();
    parser(/* args */);
    ~parser();
};
