#include "include/compiler.h"

#include "include/lexer_tools.h"

using namespace std;
void Compiler::init()
{
    grammar_productions_file.open("grammar.txt",ios::in);
    sources_file.open("source.c",ios::in);
}

State Compiler::compile()
{
    parser.init(grammar_productions_file);
    parser.lex(sources_file);
    Token r_token;
    parser.parse(r_token);
    return State::OK;
}

void Compiler::output()
{
    parse_tree_file.open("parse_tree.txt",ios::out);
    parser.printTree(parse_tree_file);
}


