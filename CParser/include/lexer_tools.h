#ifndef LEXER_TOOLS_H
#define LEXER_TOOLS_H
#include "lexer.h"

extern char character;
extern int ptr;
extern string buffer;
extern string token;
extern int state;
extern map<string, int> Id;    //标识符集
extern map<string, int> Number;     //常数集

extern const vector<string> Keyword;
extern const vector<string> Operator;
extern const vector<char> Bound;

using namespace std;

bool is_letter(char ch);  //判断是否为字母

bool is_digit(char ch);   //判断是否为数字

int is_keyword(string str);  //判断是否是Ｃ语言保留字

bool is_operator(char ch);   //判断是否为运算符

bool is_bound(char ch);      //判断是否为界符

int get_op(string str);
int get_op(char ch);          //获得运算符类别编码

int get_bound(string str);      //获得界符类别编码

ifstream input_process();              //输入处理
void get_char();        //每调用一次，向前指针forward从buffer中读取一个字符，并把它放到character中，然后移动forward，指向下一个字符
void get_nbc();         //每次调用时检查character是否为空格，若是则反复调用get_char()，直到character是一个非空字符位置
void retract();         //向前指针后退一个字符

string insert_id();        //查找标识符表，添加新项
string insert_num();            //查找常数表，添加新项



#endif // !LEXER_TOOLS_H