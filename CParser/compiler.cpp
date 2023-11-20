#include "include/compiler.h"

#include "include/lexer_tools.h"

using namespace std;
void Compiler::init()
{
    grammar_productions_file.open("grammar.txt",ios::in);
    sources_file.open("source1.c",ios::in);
}

State Compiler::compile()
{
    if(parser.init(grammar_productions_file)!=State::OK)
        cerr<<"init error\n";
    // parser.debug();
    if(parser.lex(sources_file)!= State::OK)
        cerr<<"lexer error\n";
    // parser.lex()
    Token r_token;
    if(parser.parse(r_token)!=State::OK)
    {
        cerr<<"error at "<<r_token;
    }
    cerr<<"parse finished\n";
    return State::OK;
}

void Compiler::output()
{
    parse_tree_file.open("parse_tree.txt",ios::out);
    parser.printTree(parse_tree_file);
}


