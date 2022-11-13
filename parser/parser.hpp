#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
using namespace std;

#define S 0  //句
#define NP 1 //名词部分
#define VP 2 //动词部分

#define N 3    //名词
#define ADJ 4  //形容词
#define ART 5  //冠词
#define PREP 6 //介词
#define V 7    //动词

#define THERE 8
#define NOT 9
#define MUST 10
#define ME 11

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
};

class parser
{
public:
    vector<token> tokens;
    shared_ptr<syntax_node> root;
    unordered_map<string, uint8_t> words_map;

    void parse(const string &str);
    void to_token(string str);
    void words_map_initialize();
    parser(/* args */);
    ~parser();
};
