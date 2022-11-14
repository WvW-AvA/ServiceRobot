#include "parser.hpp"
#include "fstream"
ostream &operator<<(ostream &os, const token &t);
ostream &operator<<(ostream &os, const shared_ptr<syntax_node> &p);

void parser::words_map_initialize()
{
    ifstream f("../words.txt");
    if (!f.is_open())
    {
        cout << "words.txt miss.";
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
        else
        {
            words_map.emplace(str, mode);
            // cout << (int)mode << ":" << str << endl;
        }
    }
    f.close();
    words_map.emplace("there", THERE);
    words_map.emplace("not", NOT);
    words_map.emplace("must", MUST);
    words_map.emplace("which", WHICH);
}

parser::parser(/* args */)
{
    // map initialize
    words_map_initialize();
}

parser::~parser()
{
}

void parser::to_token(string str)
{
    size_t last;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] -= ('A' - 'a');
        if (str[i] == ' ' || str[i] == '.')
        {
            string temp = str.substr(last, i - last);
            if (words_map.find(temp) == words_map.end())
            {
                cout << temp + " is unknown type word\n";
                throw("Abort");
            }
            tokens.push_back(token(words_map[temp], temp));
            // cout << tokens.back();
            last = i + 1;
        }
    }
}

void parser::push_down_automata()
{
    for (int i = 0; i < tokens.size(); i++)
    {
        push_down(tokens[i]);

        for (int tem = 0; tem < stack.size(); tem++)
        {
            cout << stack[tem] << "|";
        }
        cout << endl;
    }
}
void parser::push_down(token &token)
{
    auto p = make_shared<syntax_node>(token);
    if (p->value.type == ART)
    {
        stack.push_back(p);
    }
    else if (p->value.type == WHICH)
    {
        stack.push_back(p);
    }
    else if (p->value.type == THERE)
    {
        stack.push_back(p);
    }
    else if (p->value.type == MUST)
    {
        stack.push_back(p);
    }
    else if (p->value.type == NOT)
    {
        stack.push_back(p);
    }
    else if (p->value.type == V)
    {
        stack.push_back(p);
    }
    else if (p->value.type == N)
    {
        if (!match_rule(p, NP, ART) && !match_rule(p, NP, ADJ, ART))
        {
            auto np = make_shared<syntax_node>(NP);
            np->sons.push_back(p);
            stack.push_back(np);
        }
    }
    else if (p->value.type == PREP)
    {
        if (!match_rule(p, VP, VP))
        {
            stack.push_back(p);
        }
    }
    else if (p->value.type == ADJ)
    {
        if (!match_rule(p, VP, V))
        {
            stack.push_back(p);
        }
    }
}

void parser::push_back_np(shared_ptr<syntax_node> &np)
{
    if (!match_rule(np, NP, PREP, NP) && !match_rule(np, VP, V) && !match_rule(np, VP, VP))
    {
        stack.push_back(np);
    }
}
void parser::push_back_vp(shared_ptr<syntax_node> &vp)
{
    if (!match_rule(vp, NP, WHICH, NP) && !match_rule(vp, VP, NOT) && !match_rule(vp, VP, MUST) && !match_rule(vp, S, THERE) && !match_rule(vp, S, NP))
    {
        stack.push_back(vp);
    }
}

bool parser::match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last)
{
    int i = stack.size();
    if (i > 0 && stack[i - 1]->value.type == last)
    {
        auto temp = make_shared<syntax_node>(match_to);
        temp->sons.push_back(stack[i - 1]);
        temp->sons.push_back(p);
        stack.pop_back();
        if (match_to == NP)
            push_back_np(temp);
        else if (match_to == VP)
            push_back_vp(temp);
        else if (match_to == S)
        {
            stack.push_back(temp);
            root = temp;
        }

        return true;
    }
    return false;
}
bool parser::match_rule(shared_ptr<syntax_node> &p, uint8_t match_to, uint8_t last, uint8_t last_last)
{
    int i = stack.size();
    if (i > 1 && stack[i - 1]->value.type == last && stack[i - 2]->value.type == last_last)
    {
        auto temp = make_shared<syntax_node>(match_to);
        temp->sons.push_back(stack[i - 2]);
        temp->sons.push_back(stack[i - 1]);
        temp->sons.push_back(p);
        stack.pop_back();
        stack.pop_back();
        if (match_to == NP)
            push_back_np(temp);
        else if (match_to == VP)
            push_back_vp(temp);
        else if (match_to == S)
        {
            stack.push_back(temp);
            root = temp;
        }
        return true;
    }
    return false;
}

void parser::parse(const string &str)
{
    to_token(str);
    push_down_automata();
}

int main()
{
    parser p;
    p.parse("The door of the refrigerator is closed.");
}

ostream &operator<<(ostream &os, const token &t)
{
    return os << "<" << (int)t.type << ":" << t.value << ">";
}

ostream &operator<<(ostream &os, const shared_ptr<syntax_node> &p)
{
    os << p->value << '(';
    for (int a = 0; a < p->sons.size(); a++)
        os << p->sons[a] << (a == (p->sons.size() - 1) ? "" : ",");
    return os << ')';
}