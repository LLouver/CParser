#ifndef COMPILER_H
#define COMPILER_H
#include "common.h"
#include "parser.h"
#include <iostream>
class Compiler
{
private:
    //LR(1)�﷨������
    LR1_Parser parser;
    //�ķ�����ʽ�ļ�
    std::ifstream grammar_productions_file;
    //Դ�ļ�
    std::ifstream sources_file;
    //�﷨�������ļ�
    std::ofstream parse_tree_file;
public:
    void init();
    State compile();
    void output();
};
#endif // COMPILER_H
