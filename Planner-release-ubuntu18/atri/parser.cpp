#include "parser.hpp"
#include "fstream"
#include "debuglog.hpp"
#include "atri.hpp"

ostream &operator<<(ostream &os, const token &t);
ostream &operator<<(ostream &os, const shared_ptr<syntax_node> &p);
static unordered_map<uint8_t, string> define_map;

void parser::words_map_initialize(const string &words_map_path)
{
    ifstream f(words_map_path);
    if (!f.is_open())
    {
        LOG_ERROR("%s not found.", words_map_path);
        throw("Abort");
    }
    string str;
    uint8_t mode;
    while (getline(f, str))
    {
        if (str == "")
            continue;
        if (str == "n:")
            mode = N;
        else if (str == "v:")
            mode = V;
        else if (str == "adj:")
            mode = ADJ;
        else if (str == "art:")
            mode = ART;
        else if (str == "prep1:")
            mode = PREP1;
        else if (str == "prep2:")
            mode = PREP2;
        else if (str == "be:")
            mode = BE;
        else
        {
            words_map.emplace(str, mode);
        }
    }
    f.close();
    words_map.emplace("there", THERE);
    words_map.emplace("not", NOT);
    words_map.emplace("no", NOT);
    words_map.emplace("must", MUST);
    words_map.emplace("which", WHICH);
    words_map.emplace("of", OF);

    define_map.emplace(S, "S");
    define_map.emplace(NP, "NP");
    define_map.emplace(VP, "VP");
    define_map.emplace(N, "N");
    define_map.emplace(ADJ, "ADJ");
    define_map.emplace(ART, "ART");
    define_map.emplace(PREP1, "PREP1");
    define_map.emplace(PREP2, "PREP2");
    define_map.emplace(V, "V");
    define_map.emplace(BE, "BE");
    define_map.emplace(THERE, "THERE");
    define_map.emplace(NOT, "NOT");
    define_map.emplace(MUST, "MUST");
    define_map.emplace(WHICH, "WHICH");
    define_map.emplace(OF, "OF");
}

parser::parser()
{
}

parser::~parser()
{
}
bool parser::parse(const string &str)
{

    tokens.clear();
    stack.clear();
    root = nullptr;
    is_must = false;
    is_not = false;
    is_last_token = false;
    to_token(str);
    push_down_automata();
    if (root)
    {
        cout << root;
        return true;
    }
    else
    {
        LOG_ERROR("The garmmar of sentence:(%s) is incorrect!", str.c_str());
        return false;
    }
}

void parser::to_token(string str)
{
    size_t last = 0;
    string temp = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == ' ' || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] == '.')
            temp.push_back(str[i]);
    }
    str = temp;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] -= ('A' - 'a');
        if (str[i] == ' ' || str[i] == '.')
        {
            string temp = str.substr(last, i - last);
            if (temp == "to" && tokens.back()->token.value == "next")
            {
                temp = tokens.back()->token.value + temp;
                tokens.pop_back();
            }
            if (words_map.find(temp) == words_map.end())
            {
                LOG_ERROR("%s is unknown type word\n", temp.c_str());
                throw("Abort");
            }
            tokens.push_back(make_shared<syntax_node>(token(words_map[temp], temp)));
            // cout << tokens.back();
            last = i + 1;
        }
    }
}

void parser::push_down_automata()
{
    for (int i = 0; i < tokens.size(); i++)
    {
        if (i == tokens.size() - 1)
            is_last_token = true;
        push_down(tokens[i]);
        /*
        for (int tem = 0; tem < stack.size(); tem++)
        {
            cout << stack[tem] << "----------------------------------------------\n";
        }
        cout << "=================================================\n";
        */
    }
    if (stack.size() == 1 && stack.back()->token.type == VP)
    {
        root = make_shared<syntax_node>(S);
        root->sons.push_back(stack.back());
        stack.back()->father = root;
        stack.pop_back();
    }
    else if (stack.size() == 2 && (stack[0]->token.type == THERE || stack[0]->token.type == NP) && stack.back()->token.type == VP)
    {
        root = make_shared<syntax_node>(S);
        for (int i = 0; i < 2; i++)
        {
            root->sons.push_back(stack.back());
            stack.back()->father = root;
            stack.pop_back();
        }
    }
    else if (stack.size() == 3 && stack[0]->token.type == THERE && stack[1]->token.type == VP && stack.back()->token.type == VP)
    {
        root = make_shared<syntax_node>(S);
        for (int i = 0; i < 3; i++)
        {
            root->sons.push_back(stack.back());
            stack.back()->father = root;
            stack.pop_back();
        }
    }
}

void parser::push_down(shared_ptr<syntax_node> &p)
{
    if (p->token.type == ART)
    {
        stack.push_back(p);
    }
    else if (p->token.type == WHICH)
    {
        while (stack.back()->token.type != NP)
        {
            auto p = stack.back();
            stack.pop_back();
            for (auto &s : p->sons)
                stack.push_back(s);
        }
        stack.push_back(p);
    }
    else if (p->token.type == THERE)
    {
        stack.push_back(p);
    }
    else if (p->token.type == MUST)
    {
        stack.push_back(p);
        is_must = true;
    }
    else if (p->token.type == NOT)
    {
        stack.push_back(p);
        is_not = true;
    }
    else if (p->token.type == V)
    {
        stack.push_back(p);
    }
    else if (p->token.type == BE)
    {
        stack.push_back(p);
    }
    else if (p->token.type == N)
    {
        if (!match_rule(p, NP, ART) && !match_rule(p, NP, ADJ, ART) && !match_rule(p, NP, ADJ))
        {
            auto np = make_shared<syntax_node>(NP);
            np->sons.push_back(p);
            p->father = np;
            push_back_np(np);
        }
    }
    else if (p->token.type == PREP1)
    {
        if (!match_rule(p, VP, VP) && !match_rule(p, VP, V))
        {
            stack.push_back(p);
        }
    }
    else if (p->token.type == PREP2)
    {
        if (!match_rule(p, VP, V))
            stack.push_back(p);
    }
    else if (p->token.type == ADJ)
    {
        if (!match_rule(p, VP, BE) && !match_rule(p, VP, NOT, BE))
        {
            stack.push_back(p);
        }
    }
    else if (p->token.type == OF)
    {
        while (stack.back()->token.type != NP)
        {
            auto p = stack.back();
            stack.pop_back();
            for (auto &s : p->sons)
                stack.push_back(s);
        }
        stack.push_back(p);
    }
}
void parser::push_back_np(shared_ptr<syntax_node> &np)
{
    if (!match_rule(np, NP, OF, NP) && !match_rule(np, VP, V) && !match_rule(np, VP, VP) &&
        !match_rule(np, VP, PREP2, VP) && !match_rule(np, VP, PREP2, BE) && !match_rule(np, VP, BE) &&
        !match_rule(np, VP, PREP2, NOT, BE))
    {
        stack.push_back(np);
    }
}
void parser::push_back_vp(shared_ptr<syntax_node> &vp)
{
    if (!match_rule(vp, NP, WHICH, NP) && !match_rule(vp, VP, NOT, V) &&
        !match_rule(vp, VP, MUST) && !match_rule(vp, VP, NOT, MUST))
    {
        stack.push_back(vp);
    }
}

bool parser::match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last)
{
    int i = stack.size();
    if (i > 0 && stack[i - 1]->token.type == last)
    {
        auto temp = make_shared<syntax_node>(match_to);
        temp->sons.push_back(stack[i - 1]);
        stack[i - 1]->father = temp;
        temp->sons.push_back(p);
        p->father = temp;
        stack.pop_back();
        if (match_to == NP)
            push_back_np(temp);
        else if (match_to == VP)
            push_back_vp(temp);

        return true;
    }
    return false;
}
bool parser::match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last, uint8_t last_last)
{
    int i = stack.size();
    if (i > 1 && stack[i - 1]->token.type == last && stack[i - 2]->token.type == last_last)
    {
        auto temp = make_shared<syntax_node>(match_to);
        temp->sons.push_back(stack[i - 2]);
        stack[i - 2]->father = temp;
        temp->sons.push_back(stack[i - 1]);
        stack[i - 1]->father = temp;
        temp->sons.push_back(p);
        p->father = temp;
        stack.pop_back();
        stack.pop_back();
        if (match_to == NP)
            push_back_np(temp);
        else if (match_to == VP)
            push_back_vp(temp);
        return true;
    }
    return false;
}
bool parser::match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last, uint8_t last_last, uint8_t last_last_last)
{
    int i = stack.size();
    if (i > 2 && stack[i - 1]->token.type == last && stack[i - 2]->token.type == last_last && stack[i - 3]->token.type == last_last_last)
    {
        auto temp = make_shared<syntax_node>(match_to);
        temp->sons.push_back(stack[i - 3]);
        stack[i - 3]->father = temp;
        temp->sons.push_back(stack[i - 2]);
        stack[i - 2]->father = temp;
        temp->sons.push_back(stack[i - 1]);
        stack[i - 1]->father = temp;
        temp->sons.push_back(p);
        p->father = temp;
        stack.pop_back();
        stack.pop_back();
        stack.pop_back();
        if (match_to == NP)
            push_back_np(temp);
        else if (match_to == VP)
            push_back_vp(temp);
        return true;
    }
    return false;
}

bool parser::is_task()
{
    auto &sons = root->sons;
    if (sons.size() == 1 && sons[0]->token.type == VP)
        return true;
    else if (sons.size() == 2 && (sons[0]->token.type == THERE || (sons[0]->token.type == NP) && sons[0]->token.type == VP))
        return false;
    else
    {
        LOG_ERROR("parser tree garmmar error");
        throw(1);
    }
}

shared_ptr<syntax_node> parser::find_v(const shared_ptr<syntax_node> &vp)
{
    for (auto &son : vp->sons)
    {
        if ((son->token.type == V || son->token.type == BE) && son->token.value != "do")
            return son;
        else if (son->token.type == VP)
        {
            shared_ptr<syntax_node> temp = find_v(son);
            if (temp != nullptr)
                return temp;
        }
    }
    return nullptr;
}

_home::Instruction parser::get_task_instruction()
{
    _home::Instruction instr;
    auto v = find_v(root->sons[0]);
    if (is_match_rule(v->father, V, NP))
    {
        auto &father = v->father;
        auto &father_2 = father->father;
        if (father_2->token.type == S)
        {
            instr.behave = v->token.value;
            instr.conditionX = get_object_condition(father->sons[1]);
            return instr;
        }
        else if (father_2->token.type == VP && is_match_rule(father_2, VP, PREP2))
        {
            if (v->token.value == "put")
                instr.behave = v->token.value + father_2->sons[1]->token.value;
            else
                instr.behave = v->token.value;
            instr.conditionX = get_object_condition(father->sons[1]);
            if (father_2->father->token.type == VP && is_match_rule(father_2->father, VP, NP))
                instr.conditionY = get_object_condition(father_2->father->sons[1]);
            return instr;
        }
        else
        {
            LOG_ERROR("error");
            throw(1);
        }
    }
    else
    {
        LOG_ERROR("error");
        throw(1);
    }
}
_home::Instruction parser::get_info_instruction()
{
}
_home::Condition parser::get_object_condition(const shared_ptr<syntax_node> &np)
{
}

ostream &operator<<(ostream &os, const token &t)
{
    return os << "<" << define_map[t.type] << ":" << t.value << ">";
}

ostream &operator<<(ostream &os, const shared_ptr<syntax_node> &p)
{
    static int layer = 0;
    for (int o = 0; o < layer; o++)
        os << "  ";
    os << p->token;
    os << '\n';
    layer++;
    for (int a = 0; a < p->sons.size(); a++)
    {
        os << p->sons[a];
    }
    layer--;
    return os;
}