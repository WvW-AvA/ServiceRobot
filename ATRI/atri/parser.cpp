#include "parser.hpp"
#include "fstream"
#include "debuglog.hpp"
#include "atri.hpp"
#include "sstream"

ostream &operator<<(ostream &os, const token &t);
ostream &operator<<(ostream &os, const shared_ptr<syntax_node> &p);
static unordered_map<uint8_t, string> define_map;

void parser::words_map_initialize(const string &words_map_path)
{
    ifstream f(words_map_path);
    if (!f.is_open())
    {
        LOG_ERROR("%s not found.", words_map_path.c_str());
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
        else if (str == "prep:")
            mode = PREP;
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
    define_map.emplace(PREP, "PREP");
    define_map.emplace(V, "V");
    define_map.emplace(BE, "BE");
    define_map.emplace(THERE, "THERE");
    define_map.emplace(NOT, "NOT");
    define_map.emplace(MUST, "MUST");
    define_map.emplace(WHICH, "WHICH");
    define_map.emplace(OF, "OF");

    color_set.emplace("white");
    color_set.emplace("black");
    color_set.emplace("red");
    color_set.emplace("green");
    color_set.emplace("yellow");
    color_set.emplace("blue");
    color_set.emplace("bule");
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
    is_every = false;

    to_token(str);
    push_down_automata();
    if (root)
    {
        return true;
    }
    else
    {
        stringstream ss;
        for (auto &a : stack)
            ss << a;
        LOG_ERROR("The garmmar of sentence:(%s) is incorrect!\n %s", str.c_str(), ss.str().c_str());
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

        //     for (int tem = 0; tem < stack.size(); tem++)
        //     {
        //         cout << stack[tem] << "----------------------------------------------\n";
        //     }
        //     cout << "=================================================\n";
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
            root->sons.push_back(stack[i]);
            stack[i]->father = root;
        }
        stack.clear();
    }
    else if (stack.size() == 3 && stack[0]->token.type == THERE && stack[1]->token.type == VP &&
             (stack.back()->token.type == ADJ || (stack.back()->token.type == VP && is_match_rule(stack.back(), BE, ADJ))))
    {
        root = make_shared<syntax_node>(S);
        for (int i = 0; i < 3; i++)
        {
            root->sons.push_back(stack[i]);
            stack[i]->father = root;
        }
        stack.clear();
    }
}

void parser::push_down(shared_ptr<syntax_node> &p)
{
    if (p->token.type == ART)
    {
        if (p->token.value == "every" || p->token.value == "all" || p->token.value == "each")
            is_every = true;
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
    else if (p->token.type == PREP)
    {
        if (!match_rule(p, VP, V)) //&& !match_rule(p, VP, BE))
        {
            while (!match_rule(p, VP, NP, V) && !match_rule(p, VP, NP, VP) &&
                   !match_rule(p, VP, NP, BE) && stack.back()->token.type == VP)
            {
                auto p = stack.back();
                stack.pop_back();
                for (auto &s : p->sons)
                    stack.push_back(s);
            }
            if (stack.back()->token.type != VP)
            {
                auto temp = stack.back();
                stack.pop_back();
                if (temp->token.type == NP)
                    push_back_np(temp);
                else
                    push_down(temp);
                stack.push_back(p);
            }
        }
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
    if (!match_rule(np, NP, OF, NP) && !match_rule(np, VP, V) &&
        !match_rule(np, VP, PREP, BE) && !match_rule(np, VP, BE) &&
        !match_rule(np, VP, PREP, NOT, BE))
    {
        if (stack.size() == 0)
        {
            stack.push_back(np);
            return;
        }
        while (!match_rule(np, VP, PREP, NP, V) && !match_rule(np, VP, PREP, NP, VP) &&
               !match_rule(np, VP, PREP, NP, BE) && stack.back()->token.type == VP)
        {
            auto p = stack.back();
            stack.pop_back();
            for (auto &s : p->sons)
                stack.push_back(s);
        }
        if (stack.back()->token.type != VP)
        {
            auto temp = stack.back();
            stack.pop_back();
            if (temp->token.type == NP)
                push_back_np(temp);
            else
                push_down(temp);
            if (!match_rule(np, VP, VP))
                stack.push_back(np);
        }
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
shared_ptr<syntax_node> parser::find_n(const shared_ptr<syntax_node> &np)
{
    if (np->sons.size() == 1 && np->sons[0]->token.type == N)
        return np->sons[0];
    else if (is_match_rule(np, ART, N) || is_match_rule(np, ADJ, N))
        return np->sons[1];
    else if (is_match_rule(np, ART, ADJ, N))
        return np->sons[2];
    else if (is_match_rule(np, NP, OF, NP))
        return find_n(np->sons[2]);
    else if (is_match_rule(np, NP, WHICH, VP))
        return find_n(np->sons[0]);
    else
    {
        LOG_ERROR("Find n ERROR");
        throw(1);
    }
}

_home::Instruction parser::get_task_instruction()
{
    _home::Instruction instr;
    auto v = find_v(root->sons[0]);
    if (v == nullptr)
        return instr;
    string behave = "";
    if (is_match_rule(v->father, V, PREP))
    {
        behave = v->token.value + v->father->sons[1]->token.value;
        v = v->father;
    }
    else
        behave = v->token.value;

    auto father = v->father;

    while (father != nullptr && father->father != nullptr)
    {
        if (behave == "give" && is_match_rule(father, V, NP) && is_match_rule(father->father, VP, NP))
        {
            instr.behave = behave;
            instr.conditionY = get_object_condition(father->sons[1]);
            instr.conditionX = get_object_condition(father->father->sons[1]);
            return instr;
        }
        else if (is_match_rule(father, V, NP) || is_match_rule(father, VP, NP) || is_match_rule(father, VP, NP, PREP, NP))
        {
            break;
        }
        else if (is_match_rule(father, V, NP, PREP) || is_match_rule(father, V, NP, PREP, NP))
        {
            if (behave == "put" && (father->sons[2]->token.value == "near" || father->sons[2]->token.value == "nextto"))
                behave = "puton";
            else if (behave != "give")
                behave += father->sons[2]->token.value;
            break;
        }
        else
            father = father->father;
    }
    if (father == nullptr)
    {
        LOG_ERROR("task parse error");
        throw(1);
    }
    instr.behave = behave;
    instr.conditionX = get_object_condition(father->sons[1]);
    if (father->sons.size() == 4)
    {
        instr.isUseY = true;
        instr.conditionY = get_object_condition(father->sons[3]);
    }
    return instr;
}
_home::Instruction parser::get_info_instruction()
{
    _home::Instruction instr;
    if (is_match_rule(root, NP, VP))
    {
        instr.conditionX = get_object_condition(root->sons[0]);
        auto be = find_v(root->sons[1]);
        if (be == nullptr || be->token.type != BE)
        {
            LOG_ERROR("Info should contain be verb");
            throw(1);
            return instr;
        }
        auto father = be->father;
        if (is_match_rule(father, BE, ADJ))
            instr.behave = father->sons[1]->token.value;
        else if (is_match_rule(father, BE, NOT, ADJ))
        {
            instr.behave = father->sons[2]->token.value == "closed" ? "opened" : "closed";
        }
        else if (is_match_rule(father, BE, PREP, NP))
        {
            instr.behave = father->sons[1]->token.value;
            instr.isUseY = true;
            instr.conditionY = get_object_condition(father->sons[2]);
        }
        else if (is_match_rule(father, BE, NOT, PREP, NP))
        {
            instr.behave = father->sons[2]->token.value;
            instr.isUseY = true;
            instr.conditionY = get_object_condition(father->sons[3]);
        }
    }
    else if (is_match_rule(root, THERE, VP) || is_match_rule(root, THERE, VP, ADJ) || is_match_rule(root, THERE, VP, VP))
    {
        auto be = find_v(root->sons[1]);
        if (be == nullptr)
        {
            LOG_ERROR("Info should contain be verb");
            throw(1);
            return instr;
        }
        auto father = be->father;
        if (is_match_rule(father, BE, NP))
        {
            auto cond = get_object_condition(father->sons[1], false);
            if (cond.color == "opened" || cond.color == "closed")
            {
                instr.behave = cond.color;
                cond.color = "";
                instr.conditionX = cond;
            }
        }
        else if (is_match_rule(father, BE, NP, PREP, NP))
        {
            instr.behave = father->sons[2]->token.value;
            instr.conditionX = get_object_condition(father->sons[1]);
            instr.isUseY = true;
            instr.conditionY = get_object_condition(father->sons[3]);
        }
        if (root->sons.size() >= 3)
        {
            auto thr = root->sons[2];
            if (thr->token.type == ADJ)
                instr.behave = thr->token.value;
            else if (thr->token.type == VP && is_match_rule(thr, BE, ADJ))
                instr.behave = thr->sons[1]->token.value;
        }
    }

    if (instr.behave == "on" && instr.isUseY && instr.conditionY.sort == "plate")
    {
        instr.behave = "plate";
        instr.isUseY = false;
        instr.conditionY = _home::Condition();
    }
    else if (instr.behave == "nextto")
        instr.behave = "on";
    return instr;
}
_home::Condition parser::get_object_condition(const shared_ptr<syntax_node> &np, bool is_check_color)
{
    _home::Condition cond;
    string color;
    if (np->sons.size() == 1 && np->sons[0]->token.type == N)
        cond.sort = np->sons[0]->token.value;
    else if (is_match_rule(np, ART, N))
        cond.sort = np->sons[1]->token.value;
    else if (is_match_rule(np, ADJ, N))
    {
        color = np->sons[0]->token.value;
        cond.sort = np->sons[1]->token.value;
    }
    else if (is_match_rule(np, ART, ADJ, N))
    {
        color = np->sons[1]->token.value;
        cond.sort = np->sons[2]->token.value;
    }
    else if (is_match_rule(np, NP, OF, NP))
    {
        auto n = find_n(np->sons[2]);
        return get_object_condition(n->father);
    }
    else if (is_match_rule(np, NP, WHICH, VP))
    {
        auto n = find_n(np->sons[0]);
        auto co = get_object_condition(n->father);
        if (co.color == "" && is_match_rule(np->sons[2], BE, ADJ))
        {
            auto color = np->sons[2]->sons[1]->token.value;
            if (color_set.find(color) != color_set.end())
                co.color = color;
        }
        return co;
    }
    if (is_check_color && color != "" && color_set.find(color) == color_set.end())
    {
        LOG_ERROR("%s color do not regist", color.c_str());
        throw(1);
    }
    cond.color = color;
    if (cond.sort == "me")
        cond.sort = "human";
    return cond;
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