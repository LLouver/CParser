#include "include/lexer_tools.h"

map<string, int>::iterator temp;

map<string, int> Id;    //标识符集
map<string, int> Number;     //常数集

const vector<string> Keyword = { "auto", "break", "case", "char", "const", "continue",
								"default", "do", "double", "else", "enum", "extern",
								"float", "for", "goto", "if", "int", "long",
								"register", "return", "short", "signed", "sizeof", "static",
								"struct", "switch", "typedef", "union", "unsigned", "void",
								"volatile", "while"
};   //C语言保留字[32]

const vector<string> Operator = { "+", "-", "*", "/", "%", "++", "--", //算术运算符[7]
								 "<", "<=", ">", ">=", "!=", "==",  //关系运算符[6]
								 "&&", "||", "!",      //逻辑运算符[3]
								 "&", "|", "^", "~", "<<", ">>",    //位运算符[6]
								 "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",  //赋值运算符[9]
								 ".", "->"//    其他运算符[2]
};  //运算符[33]

const vector<char> Bound = { '(', ')', '{', '}', '[', ']',
							';', ',', ':', '\?', '\"', '\'', '#','\\'
};     //分界符[14]

int state = 0;   //当前状态
string buffer = "";   //输入缓冲区
char character;      //当前读到的字符
string token;       //字符数组，存放当前正在识别的单词字符串
int ptr;        //字符指针，指向当前读取的字符

int count_col = 0;//当前字符串的起始字符位置（报错用）

/*所有合法的符合(串)分为５类：标识符，常数，保留字，运算符，界符
 * 注释标记"//","/ *"，词法分析不需要处理这些(指内容被直接忽略)
 * 约定:　　(类别编码唯一)
 * 标识符类别编码为１，
 * 常数类别编码为２，
 * 保留字类别编码为i+10,其中i为Keyword序列中保留字对应的下标，如"auto"的类别编码为10，直到"while"的类别编码为41
 * 运算符类别编码为i+50，其中i为Operator序列中运算符对应的下标，如"+"的类别编码为50等等
 * 界符类别编码为i+100，其中i为Bound序列中界符对应的下标，如"("的类别编码为100，以此类推
 */

bool Lexer::set_result(string s) {
	result_filename = s;
	return true;
}
bool Lexer::set_statistics(string s) {
	statistics_filename = s;
	return true;
}
bool Lexer::set_table(string s) {
	table_filename = s;
	return true;
}
bool Lexer::set_errors(string s) {
	errors_filename = s;
	return true;
}

/*vector<Token>* lexical_analysis::get_table()
{
	return &table;
}*/
Statistics Lexer::get_sta()
{
	return sta;
}
State Lexer::getNextLexical(Token& next)
{
	if (count >= table.size())
		return State::ERROR;
	// .. s=(Symbol)0;
	next = table[count];
	count++;
	cerr<<"put a token"<< next;
	return State::OK;
}

int Lexer::start_analysis(ifstream& source_file,ofstream& debug_file)
{
	int ret = 0;
	Id.clear();
	Number.clear();
	character = ' ';          //进行必要的初始化

	//source_file.open(input_filename);
	if (!source_file.is_open())
		return 1;

	// ofstream outfile_errors;
	// outfile_errors.open(errors_filename, ios::out);
	if (!debug_file.is_open())
		return 2;

	while (getline(source_file, buffer))    //从文件按行读入缓冲区
	{
		sta.add_row();         //行数+1
		int category = -1;      //类别编码
		ptr = 0;
		token = "";
		string value;
		if (buffer.length() == 0)		//空行不进行分析
			continue;
		else
		{
			while (buffer[ptr] != '\0')
			{
				get_char();
				get_nbc();
				if (character == '\0')
					break;

				count_col = ptr;    //记录首字符位置

				/*************************标识符识别*************************/

				if (is_letter(character) && state == 0) //开头为字母
				{
					state = 1;
					token += character;           //收集
					sta.add_ch();          //字符总数+1,每收集一个token字符就重复此步骤
					get_char();
					while (is_letter(character) || is_digit(character)) //后跟字母或数字
					{
						token += character;    //收集
						sta.add_ch();
						get_char();
					}
					retract();

					category = is_keyword(token); //判断token是否为保留字
					if (category != -1)          //如果是保留字
					{
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						sta.add_key();
					}
					else
					{
						value = insert_id();
						table.push_back(Token(1, Id[value], sta.get_row(), count_col, value)); //将识别到的标识符添加到table中
						sta.add_id();
					}

					state = 0;      //标识符识别完成，复位状态
					token = "";
				}

				/*************************无符号数识别*************************/

				else if (is_digit(character) && state == 0) //开头为数字
				{
					state = 2;
					token += character;
					sta.add_ch();
					get_char();
					while (is_digit(character))      //继续读入数字
					{
						token += character;
						sta.add_ch();
						get_char();
					}
					retract();
				}
				else if (character == '.' && state == 2)            //已经读入数字又读到小数点
				{
					state = 3;
					token += character;           //将小数点加入token
					sta.add_ch();
				}
				else if (is_digit(character) && state == 3)
				{
					state = 4;
					while (is_digit(character))      //此时token中应是xx.的形式，若接下读到数字
					{
						token += character;
						sta.add_ch();
						get_char();
					}
					retract();
				}
				else if (state == 2 || state == 4) //已经识别到整数/小数，判断之后是否为数字结束的标志，如果后面是字母，则是错误
				{
					if (is_bound(character) || is_operator(character) || character == ' ')
					{
						sta.add_num();
						value = insert_num();
						table.push_back(Token(2, Number[value], sta.get_row(), count_col, value));
						retract();      //回退
					}
					else
					{
						while (!(is_bound(character) || is_operator(character) || character == ' '))
						{
							sta.add_ch();
							token += character;
							get_char();
						}
						retract();
						debug_file << "Error (line " << sta.get_row() << ") : there is a illegal string " << "\"" << token << "\"" << endl;
						ret = 3;
					}
					token = "";
					state = 0;
				}

				/*************************注释处理********************************=*/

				else if (character == '/')		//注释可以在任何状态下出现，故不必判断状态
				{
					get_char();
					if (character == '/')			//识别到"//"，进入单行注释
						break;				//直接忽略这一行之后的所有内容
					else if (character == '*')			//识别到"/*"，进入多行注释
						state = 5;		//状态转移
					else
						retract();			//未识别到注释标志，回退指针
				}
				else if (state == 5)
				{
					if (character == '*')		//在注释状态(state=5)时，读到'*'
					{
						get_char();			//超前扫描
						if (character == '/')		//识别到"*/"，注释结束
							state = 0;
						else
							retract();
					}
				}

				/*************************运算符识别*************************/

				else if (is_operator(character) && state == 0) //识别到运算符
				{
					state = 6;
					token += character;
					sta.add_ch();
					char chtemp = character;
					switch (chtemp)    //判断是否是二目运算符
					{
					case '+':
						get_char();
						if (character == '+' || character == '=')    //识别到"++"||"+="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '-':
						get_char();
						if (character == '-' || character == '=' || character == '>') //识别到"--"||"-="||"->"
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '*':
						get_char();
						if (character == '=')        //识别到"*="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '/':
						get_char();
						if (character == '=')        //识别到"/="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '%':
						get_char();
						if (character == '=')        //识别到"%="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '<':
						get_char();
						if (character == '=' || character == '<') //识别到"<="||"<<"
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '>':
						get_char();
						if (character == '=' || character == '>') //识别到">="||">>"
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '!':
						get_char();
						if (character == '=') //识别到"!="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '=':
						get_char();
						if (character == '=') //识别到"=="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
						{
							string tmp;
							tmp+=chtemp;
							category = get_op(tmp);
						}
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '&':
						get_char();
						if (character == '&' || character == '=') //识别到"&&"||"&="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '|':
						get_char();
						if (character == '|' || character == '=') //识别到"||"||"|="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '^':
						get_char();
						if (character == '=') //识别到"^="
						{
							token += character;
							sta.add_ch();
							category = get_op(token);
							get_char();
						}
						else
							category = get_op(chtemp);
						retract();
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					case '~':
						category = get_op(chtemp);
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						break;

					case '.':
						category = get_op(chtemp);
						sta.add_op();
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						token = "";
						state = 0;
						break;

					default:
						break;
					}
				}

				/*************************界符识别*************************/

				else if (is_bound(character) && state == 0)
				{
					token += character;
					sta.add_ch();
					sta.add_bound();
					category = get_bound(token);
					table.push_back(Token(category, 0, sta.get_row(), count_col));
					token = "";
					state = 0;
				}
				/*************************错误处理(无法识别的符号)*************************/

				else if (character != '\t' && character != '\0')
				{
					token += character;
					sta.add_ch();
					debug_file << "Error (line " << sta.get_row() << ") : there is a unrecognizable character. " << "\"" << token << "\"" << endl;
					ret = 3;
					token = "";
					state = 0;
				}
			}
		}
	}
	debug_file.close();
	return ret;
}

int Lexer::output_analysis(ofstream& debug_file)
{
	//ofstream outfile_statistics;
	//outfile_statistics.open(statistics_filename, ios::out);
	if (!debug_file.is_open())
		return 1;

	/*ofstream outfile_result;
	outfile_result.open(result_filename, ios::out);
	if (!outfile_result.is_open())
		return 2;

	ofstream outfile_table;
	outfile_table.open(table_filename, ios::out);
	if (!outfile_table.is_open())
		return 3;
*/
	//how_statistics(debug_file);
	show_result(debug_file);
	//show_table(outfile_table);
	cerr<<"debug info output finished\n";
	return 0;
}

void Lexer::show_statistics(ofstream& outfile)
{
	outfile << "----------------------------------统计结果----------------------------------" << endl << endl
		<< "语句行数 : " << sta.get_row() << endl
		<< "字符总数 : " << sta.get_ch() << endl
		<< "保留字 : " << sta.get_key() << endl
		<< "运算符 : " << sta.get_op() << endl
		<< "界符 : " << sta.get_bound() << endl
		<< "标识符 : " << sta.get_id() << endl
		<< "常数 : " << sta.get_num() << endl;
}

void Lexer::show_result(ofstream& outfile)
{

	outfile << "---------------------------------"
			"识别结果---------------------------------" << endl;
	outfile << "记号\t\t属性" << endl;

	for (int i = 0; i < int(table.size()); i++)
	{
		outfile << table[i] << "\t\t" << table[i].value << endl;
	}

	outfile.close();
}

void Lexer::show_table(ofstream& outfile)
{

	outfile << "----------------------------------" << "符号－编码对照表" << "----------------------------------" << endl << endl;

	outfile << "==================" << "保留字" << "==================" << endl;

	outfile << "保留字符\t\t类别编码" << endl;
	for (int i = 0; i < int(Keyword.size()); i++)
	{
		if (Keyword[i].size() >= 8)
		{
			outfile << Keyword[i] << "\t" << i + 10 << endl;
		}
		else
		{
			outfile << Keyword[i] << "\t\t" << i + 10 << endl;
		}
	}

	outfile << endl << "==================" << "运算符" << "==================" << endl;
	outfile << "运算符\t\t类别编码" << endl;
	for (int i = 0; i < int(Operator.size()); i++)
	{
		outfile << Operator[i] << "\t\t" << i + 50 << endl;
	}

	outfile << endl << "==================" << "界符" << "==================" << endl;
	outfile << "界符\t\t类别编码" << endl;
	for (int i = 0; i < int(Bound.size()); i++)
	{
		outfile << Bound[i] << "\t\t" << i + 100 << endl;
	}

	outfile << endl << "==================" << "标识符" << "==================" << endl;
	outfile << "标识符\t\t类别编码\t\t表中位置" << endl;
	for (temp = Id.begin(); temp != Id.end(); temp++)
	{
		if (temp->first.size() >= 8)
		{
			outfile << temp->first << "\t1\t\t" << temp->second << endl;
		}
		else
		{
			outfile << temp->first << "\t\t1\t\t" << temp->second << endl;
		}
	}
	outfile << endl << "==================" << "常数表" << "==================" << endl;
	outfile << "常量值\t\t类别编码\t\t表中位置" << endl;
	for (temp = Number.begin(); temp != Number.end(); temp++)
	{
		outfile << temp->first << "\t\t2\t\t" << temp->second << endl;
	}
}

ostream& operator<<(ostream&o, Token r_token)
{
	o<<"$"<<r_token.line<<":"<<r_token.col<<","<<(int)r_token.symbol_id<<":"<<convTag2Str((Symbol)r_token.symbol_id)<<"$\n";
	return o;
}