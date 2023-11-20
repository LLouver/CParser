#include "include/compiler.h"

#include "include/lexer_tools.h"

using namespace std;

void Compiler::init()
{
    grammar_productions_file.open("grammar.txt",ios::in);
    sources_file.open("source1.c",ios::in);
    parse_tree_file.open("parse_tree.txt",ios::out);
    lexical_table_file.open("lexical_table.txt",ios::out);
    parser_log_file.open("parser_log.txt",ios::out);
    LR_table_file.open("LR_table.txt",ios::out);
}

State Compiler::compile()
{
    if(parser.init(grammar_productions_file,LR_table_file)!=State::OK)
        cerr<<"init error\n";
    // parser.debug();
    if(parser.lex(sources_file,lexical_table_file)!= State::OK)
        cerr<<"lexer error\n";
    // parser.lex()
    Token r_token;
    if(parser.parse(r_token,parser_log_file)!=State::OK)
    {
        cerr<<"error at "<<r_token<<"\n";
        cerr<<"parse aborted\n";
    }
    cerr<<"parse finished\n";
    return State::OK;
}

void Compiler::output()
{
    parser.printTree(parse_tree_file);
}


