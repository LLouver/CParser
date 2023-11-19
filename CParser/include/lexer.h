#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

struct Token       //符号表
{
	int symbol = 0;       //类别编码
	int index = 0;          //表中位置
	string value = "-";             //属性值(标识符，常数的属性是其本身，其他三类符号唯一)

	int line;                //行
	int col;                //列

	Token(int c, int i, int r, int co, string v = "-")
	{
		symbol = c;
		index = i;
		value = v;
		line = r;
		col = co;
	}
};

class Statistics
{
private:
	int ch;         //字符总数(只统计非空且有意义的字符)
	int line;        //行数
	int id, num, key, op, bound;    //各类单词的个数
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
	string input_filename;//输入文件路径
	string result_filename;//具体结果输出文件路径
	string statistics_filename;//统计结果输出文件路径
	string table_filename;//记号表输出文件路径
	string errors_filename;//错误记录输出文件路径

	vector<Token> table;        //存储所有识别到的合法符号(串)
	Statistics sta;             //存储统计数据

	int count;     //已经返回到了哪个table

	void show_statistics(ofstream& outfile);      //输出统计结果
	void show_result(ofstream& outfile);          //输出识别结果
	void show_table(ofstream& outfile);           //输出记号表
public:
	Lexer()
	{
		count = 0;
	}
	bool set_input(string s);
	bool set_result(string s);
	bool set_statistics(string s);
	bool set_table(string s);
	bool set_errors(string s);

	int start_analysis();
	/*
	0：正常运行
	1：输入文件打开失败
	2：错误输出文件打开失败
	3：词法分析出错
	*/
	int output_analysis();
	/*
	0：输出正常
	1：统计结果文件打开失败
	2：具体文件打开失败
	3：记号表输出文件打开失败
	*/

	int getNextLexical(Token& next);
	/*
	0：读取成功
	1：返回失败（越界）
	*/
	Statistics get_sta();
	//vector<Table>* get_table();
};
#endif // !LEXER_H
