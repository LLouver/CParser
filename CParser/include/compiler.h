#ifndef COMPILER_H
#define COMPILER_H
#include "common.h"
#include "parser.h"
#include <iostream>
class Compiler
{
private:
    //LR(1)语法分析器
    LR1_Parser parser;
    //文法产生式文件
    std::ifstream grammar_productions_file;
    //源文件
    std::ifstream sources_file;
    //语法分析树文件
    std::ofstream parse_tree_file;
public:
    void init();
    State compile();
    void output();
};
#endif // COMPILER_H
