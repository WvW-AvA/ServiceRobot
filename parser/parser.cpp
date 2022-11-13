#include "parser.hpp"
#include "fstream"
ostream &operator<<(ostream &os, const token &t);

void parser::words_map_initialize()
{
    ifstream f("../words.txt");
    if (!f.is_open())
    {
        cout << "words.txt miss.";
        exit(1);
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
            cout << (int)mode << ":" << str << endl;
        }
    }
    f.close();
    words_map.emplace("there", THERE);
    words_map.emplace("not", NOT);
    words_map.emplace("must", MUST);
    words_map.emplace("me", ME);
}

parser::parser(/* args */)
{
    root = make_shared<syntax_node>();
    root->value = token(S);

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
            uint8_t type = words_map[temp];
            if (type == 0)
            {
                throw(temp + " is unknown type word\n");
            }
            tokens.push_back(token(type, temp));
            cout << tokens.back();
            last = i + 1;
        }
    }
}
void parser::parse(const string &str)
{
}

int main()
{
    parser p;
    p.to_token("The door of the refrigerator is closed.");
}

ostream &operator<<(ostream &os, const token &t)
{
    return os << "token <" << (int)t.type << ":" << t.value << ">\n";
}