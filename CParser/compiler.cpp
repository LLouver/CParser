#include "include/compiler.h"

#include "include/lexer_tools.h"

using namespace std;

State Compiler::init()
{
    //打开错误报告文件
    
    for(int i=0;!debug_log_file.is_open();++i)
    {
        ostringstream os;
        os << "debug";
        os << i;
        os << ".txt";
        debug_log_file.open(os.str());
    }
    grammar_productions_file.open("grammar.txt",ios::in);
    if(!grammar_productions_file.is_open())
    {
        debug_log_file << "file 'grammar.txt' open failed" << endl;
        return State::ERROR;
    }
    sources_file.open("source1.c",ios::in);
    if(!sources_file.is_open())
    {
        debug_log_file << "file 'source1.c' open failed" << endl;
        return State::ERROR;
    }
    parse_tree_file.open("parse_tree.txt",ios::out);
    if(!parse_tree_file.is_open())
    {
        debug_log_file << "file 'parse_tree.txt' open failed" << endl;
        return State::ERROR;
    }
    lexical_table_file.open("lexical_table.txt",ios::out);
    if(!lexical_table_file.is_open())
    {
        debug_log_file << "file 'lexical_table.txt' open failed" << endl;
        return State::ERROR;
    }
    parser_log_file.open("parser_log.txt",ios::out);
    if(!parser_log_file.is_open())
    {
        debug_log_file << "file 'parser_log.txt' open failed" << endl;
        return State::ERROR;
    }
    LR_table_file.open("LR_table.txt",ios::out);
    if(!LR_table_file.is_open())
    {
        debug_log_file << "file 'LR_table.txt' open failed" << endl;
        return State::ERROR;
    }
    return State::OK;
}


State Compiler::compile()
{
    if(parser.init(grammar_productions_file,LR_table_file)!=State::OK)
    {
        debug_log_file<<"compiler init error\n";
        return State::ERROR;
    }
    // parser.debug();
    if(parser.lex(sources_file,lexical_table_file,debug_log_file)!= State::OK)
    {
        debug_log_file<<"lexer error\n";
        return State::ERROR;
    }
    // parser.lex()
    Token r_token;
    /*if(parser.parse(r_token,parser_log_file)!=State::OK)
    {
        debug_log_file<<"parser error at "<<r_token<<"\n";
        debug_log_file<<"parse aborted\n";
        return State::ERROR;
    }*/
    debug_log_file<<"parse finished\n";
    return State::OK;
}

void Compiler::output()
{
    parser.printTree(parse_tree_file);
}


