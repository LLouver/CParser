#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>

#include "symbol.h"
#include "common.h"
using namespace std;

struct Token       //���ű�
{
	Symbol symbol_id = static_cast<Symbol>(0);       //������
	int index = 0;          //����λ��
	string value = "-";             //����ֵ(��ʶ�����������������䱾�������������Ψһ)

	int line;                //��
	int col;                //��

	Token(int c, int i, int r, int co, string v = "-")
	{
		symbol_id = static_cast<Symbol>(c);
		index = i;
		value = v;
		line = r;
		col = co;
	}
	Token()
	{
		symbol_id = static_cast<Symbol>(0);
		index = 0;
		value = "";
		line = 0;
		col = 0;
	}
};
ostream& operator<<(ostream&o, Token r_token);

class Statistics
{
private:
	int ch;         //�ַ�����(ֻͳ�Ʒǿ�����������ַ�)
	int line;        //����
	int id, num, key, op, bound;    //���൥�ʵĸ���
public:
	Statistics()
	{
		ch = line = id = num = key = op = bound = 0;
	}
	void add_row()
	{
		line++;
	}
	void add_ch()
	{
		ch++;
	}
	void add_id()
	{
		id++;
	}
	void add_num()
	{
		num++;
	}
	void add_key()
	{
		key++;
	}
	void add_op()
	{
		op++;
	}
	void add_bound()
	{
		bound++;
	}

	int get_row()
	{
		return line;
	}
	int get_ch()
	{
		return ch;
	}
	int get_id()
	{
		return id;
	}
	int get_num()
	{
		return num;
	}
	int get_key()
	{
		return key;
	}
	int get_op()
	{
		return op;
	}
	int get_bound()
	{
		return bound;
	}
};


class Lexer {
private:
	string input_filename;//�����ļ�·��
	string result_filename;//����������ļ�·��
	string statistics_filename;//ͳ�ƽ������ļ�·��
	string table_filename;//�Ǻű�����ļ�·��
	string errors_filename;//�����¼����ļ�·��

	vector<Token> table;        //�洢����ʶ�𵽵ĺϷ�����(��)
	Statistics sta;             //�洢ͳ������

	int count;     //�Ѿ����ص����ĸ�table

	void show_statistics(ofstream& outfile);      //���ͳ�ƽ��
	void show_result(ofstream& of);          //���ʶ����
	void show_table(ofstream& outfile);           //����Ǻű�
public:
	Lexer()
	{
		count = 0;
	}
	bool set_result(string s);
	bool set_statistics(string s);
	bool set_table(string s);
	bool set_errors(string s);

	int start_analysis(ifstream& source_file,ofstream& debug_file);
	/*
	0����������
	1�������ļ���ʧ��
	2����������ļ���ʧ��
	3���ʷ���������
	*/
	int output_analysis(ofstream& of);
	/*
	0���������
	1��ͳ�ƽ���ļ���ʧ��
	2�������ļ���ʧ��
	3���Ǻű�����ļ���ʧ��
	*/

	State getNextLexical(Token& next);
	/*
	0����ȡ�ɹ�
	1������ʧ�ܣ�Խ�磩
	*/
	Statistics get_sta();
	//vector<Table>* get_table();
};
#endif // !LEXER_H
