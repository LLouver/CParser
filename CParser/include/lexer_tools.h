#ifndef LEXER_TOOLS_H
#define LEXER_TOOLS_H
#include "lexer.h"

extern char character;
extern int ptr;
extern string buffer;
extern string token;
extern int state;
extern map<string, int> Id;    //��ʶ����
extern map<string, int> Number;     //������

extern const vector<string> Keyword;
extern const vector<string> Operator;
extern const vector<char> Bound;

using namespace std;

bool is_letter(char ch);  //�ж��Ƿ�Ϊ��ĸ

bool is_digit(char ch);   //�ж��Ƿ�Ϊ����

int is_keyword(string str);  //�ж��Ƿ��ǣ����Ա�����

bool is_operator(char ch);   //�ж��Ƿ�Ϊ�����

bool is_bound(char ch);      //�ж��Ƿ�Ϊ���

int get_op(string str);
int get_op(char ch);          //��������������

int get_bound(string str);      //��ý��������

void get_char();        //ÿ����һ�Σ���ǰָ��forward��buffer�ж�ȡһ���ַ����������ŵ�character�У�Ȼ���ƶ�forward��ָ����һ���ַ�
void get_nbc();         //ÿ�ε���ʱ���character�Ƿ�Ϊ�ո������򷴸�����get_char()��ֱ��character��һ���ǿ��ַ�λ��
void retract();         //��ǰָ�����һ���ַ�

string insert_id();        //���ұ�ʶ�����������
string insert_num();            //���ҳ������������


#endif // !LEXER_TOOLS_H