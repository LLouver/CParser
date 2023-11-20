#include "include/lexer_tools.h"

map<string, int>::iterator temp;

map<string, int> Id;    //��ʶ����
map<string, int> Number;     //������

const vector<string> Keyword = { "auto", "break", "case", "char", "const", "continue",
								"default", "do", "double", "else", "enum", "extern",
								"float", "for", "goto", "if", "int", "long",
								"register", "return", "short", "signed", "sizeof", "static",
								"struct", "switch", "typedef", "union", "unsigned", "void",
								"volatile", "while"
};   //C���Ա�����[32]

const vector<string> Operator = { "+", "-", "*", "/", "%", "++", "--", //���������[7]
								 "<", "<=", ">", ">=", "!=", "==",  //��ϵ�����[6]
								 "&&", "||", "!",      //�߼������[3]
								 "&", "|", "^", "~", "<<", ">>",    //λ�����[6]
								 "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",  //��ֵ�����[9]
								 ".", "->"//    ���������[2]
};  //�����[33]

const vector<char> Bound = { '(', ')', '{', '}', '[', ']',
							';', ',', ':', '\?', '\"', '\'', '#','\\'
};     //�ֽ��[14]

int state = 0;   //��ǰ״̬
string buffer = "";   //���뻺����
char character;      //��ǰ�������ַ�
string token;       //�ַ����飬��ŵ�ǰ����ʶ��ĵ����ַ���
int ptr;        //�ַ�ָ�룬ָ��ǰ��ȡ���ַ�

int count_col = 0;//��ǰ�ַ�������ʼ�ַ�λ�ã������ã�

/*���кϷ��ķ���(��)��Ϊ���ࣺ��ʶ���������������֣�����������
 * ע�ͱ��"//","/ *"���ʷ���������Ҫ������Щ(ָ���ݱ�ֱ�Ӻ���)
 * Լ��:����(������Ψһ)
 * ��ʶ��������Ϊ����
 * ����������Ϊ����
 * ������������Ϊi+10,����iΪKeyword�����б����ֶ�Ӧ���±꣬��"auto"��������Ϊ10��ֱ��"while"��������Ϊ41
 * �����������Ϊi+50������iΪOperator�������������Ӧ���±꣬��"+"��������Ϊ50�ȵ�
 * ���������Ϊi+100������iΪBound�����н����Ӧ���±꣬��"("��������Ϊ100���Դ�����
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
	character = ' ';          //���б�Ҫ�ĳ�ʼ��

	//source_file.open(input_filename);
	if (!source_file.is_open())
		return 1;

	// ofstream outfile_errors;
	// outfile_errors.open(errors_filename, ios::out);
	if (!debug_file.is_open())
		return 2;

	while (getline(source_file, buffer))    //���ļ����ж��뻺����
	{
		sta.add_row();         //����+1
		int category = -1;      //������
		ptr = 0;
		token = "";
		string value;
		if (buffer.length() == 0)		//���в����з���
			continue;
		else
		{
			while (buffer[ptr] != '\0')
			{
				get_char();
				get_nbc();
				if (character == '\0')
					break;

				count_col = ptr;    //��¼���ַ�λ��

				/*************************��ʶ��ʶ��*************************/

				if (is_letter(character) && state == 0) //��ͷΪ��ĸ
				{
					state = 1;
					token += character;           //�ռ�
					sta.add_ch();          //�ַ�����+1,ÿ�ռ�һ��token�ַ����ظ��˲���
					get_char();
					while (is_letter(character) || is_digit(character)) //�����ĸ������
					{
						token += character;    //�ռ�
						sta.add_ch();
						get_char();
					}
					retract();

					category = is_keyword(token); //�ж�token�Ƿ�Ϊ������
					if (category != -1)          //����Ǳ�����
					{
						table.push_back(Token(category, 0, sta.get_row(), count_col));
						sta.add_key();
					}
					else
					{
						value = insert_id();
						table.push_back(Token(1, Id[value], sta.get_row(), count_col, value)); //��ʶ�𵽵ı�ʶ����ӵ�table��
						sta.add_id();
					}

					state = 0;      //��ʶ��ʶ����ɣ���λ״̬
					token = "";
				}

				/*************************�޷�����ʶ��*************************/

				else if (is_digit(character) && state == 0) //��ͷΪ����
				{
					state = 2;
					token += character;
					sta.add_ch();
					get_char();
					while (is_digit(character))      //������������
					{
						token += character;
						sta.add_ch();
						get_char();
					}
					retract();
				}
				else if (character == '.' && state == 2)            //�Ѿ����������ֶ���С����
				{
					state = 3;
					token += character;           //��С�������token
					sta.add_ch();
				}
				else if (is_digit(character) && state == 3)
				{
					state = 4;
					while (is_digit(character))      //��ʱtoken��Ӧ��xx.����ʽ�������¶�������
					{
						token += character;
						sta.add_ch();
						get_char();
					}
					retract();
				}
				else if (state == 2 || state == 4) //�Ѿ�ʶ������/С�����ж�֮���Ƿ�Ϊ���ֽ����ı�־�������������ĸ�����Ǵ���
				{
					if (is_bound(character) || is_operator(character) || character == ' ')
					{
						sta.add_num();
						value = insert_num();
						table.push_back(Token(2, Number[value], sta.get_row(), count_col, value));
						retract();      //����
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

				/*************************ע�ʹ���********************************=*/

				else if (character == '/')		//ע�Ϳ������κ�״̬�³��֣��ʲ����ж�״̬
				{
					get_char();
					if (character == '/')			//ʶ��"//"�����뵥��ע��
						break;				//ֱ�Ӻ�����һ��֮�����������
					else if (character == '*')			//ʶ��"/*"���������ע��
						state = 5;		//״̬ת��
					else
						retract();			//δʶ��ע�ͱ�־������ָ��
				}
				else if (state == 5)
				{
					if (character == '*')		//��ע��״̬(state=5)ʱ������'*'
					{
						get_char();			//��ǰɨ��
						if (character == '/')		//ʶ��"*/"��ע�ͽ���
							state = 0;
						else
							retract();
					}
				}

				/*************************�����ʶ��*************************/

				else if (is_operator(character) && state == 0) //ʶ�������
				{
					state = 6;
					token += character;
					sta.add_ch();
					char chtemp = character;
					switch (chtemp)    //�ж��Ƿ��Ƕ�Ŀ�����
					{
					case '+':
						get_char();
						if (character == '+' || character == '=')    //ʶ��"++"||"+="
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
						if (character == '-' || character == '=' || character == '>') //ʶ��"--"||"-="||"->"
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
						if (character == '=')        //ʶ��"*="
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
						if (character == '=')        //ʶ��"/="
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
						if (character == '=')        //ʶ��"%="
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
						if (character == '=' || character == '<') //ʶ��"<="||"<<"
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
						if (character == '=' || character == '>') //ʶ��">="||">>"
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
						if (character == '=') //ʶ��"!="
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
						if (character == '=') //ʶ��"=="
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
						if (character == '&' || character == '=') //ʶ��"&&"||"&="
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
						if (character == '|' || character == '=') //ʶ��"||"||"|="
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
						if (character == '=') //ʶ��"^="
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

				/*************************���ʶ��*************************/

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
				/*************************������(�޷�ʶ��ķ���)*************************/

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
	outfile << "----------------------------------ͳ�ƽ��----------------------------------" << endl << endl
		<< "������� : " << sta.get_row() << endl
		<< "�ַ����� : " << sta.get_ch() << endl
		<< "������ : " << sta.get_key() << endl
		<< "����� : " << sta.get_op() << endl
		<< "��� : " << sta.get_bound() << endl
		<< "��ʶ�� : " << sta.get_id() << endl
		<< "���� : " << sta.get_num() << endl;
}

void Lexer::show_result(ofstream& outfile)
{

	outfile << "---------------------------------"
			"ʶ����---------------------------------" << endl;
	outfile << "�Ǻ�\t\t����" << endl;

	for (int i = 0; i < int(table.size()); i++)
	{
		outfile << table[i] << "\t\t" << table[i].value << endl;
	}

	outfile.close();
}

void Lexer::show_table(ofstream& outfile)
{

	outfile << "----------------------------------" << "���ţ�������ձ�" << "----------------------------------" << endl << endl;

	outfile << "==================" << "������" << "==================" << endl;

	outfile << "�����ַ�\t\t������" << endl;
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

	outfile << endl << "==================" << "�����" << "==================" << endl;
	outfile << "�����\t\t������" << endl;
	for (int i = 0; i < int(Operator.size()); i++)
	{
		outfile << Operator[i] << "\t\t" << i + 50 << endl;
	}

	outfile << endl << "==================" << "���" << "==================" << endl;
	outfile << "���\t\t������" << endl;
	for (int i = 0; i < int(Bound.size()); i++)
	{
		outfile << Bound[i] << "\t\t" << i + 100 << endl;
	}

	outfile << endl << "==================" << "��ʶ��" << "==================" << endl;
	outfile << "��ʶ��\t\t������\t\t����λ��" << endl;
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
	outfile << endl << "==================" << "������" << "==================" << endl;
	outfile << "����ֵ\t\t������\t\t����λ��" << endl;
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