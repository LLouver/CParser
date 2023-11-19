#include "include/compiler.h"

using namespace std;
void Compiler::init()
{
    grammar_productions_file.open("grammar.txt",ios::in);
    sources_file.open("source.c",ios::in);
}

State Compiler::compile()
{
    return State::OK;
}

void Compiler::output()
{
    parse_tree_file.open("parse_tree.txt",ios::out);
    
}


