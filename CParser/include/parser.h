#ifndef PARSER_H
#define PARSER_H
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include "common.h"
#include "lexer.h"
#include "symbol.h"


using std::vector;
using std::set;
using std::map;
using std::list;
using std::stack;
using std::ostream;

struct TNode	//�����
{
	Symbol tag;	//tagֵ
	int p;		//ָ��
	list<int> childs;	//���ӽ�㼯
};

struct PTree	//�﷨��
{
	vector<TNode> TNode_List;	//��㼯��
	int RootNode = -1;				//�����ָ��
};

/**
 * Production(����ʽ):��ʾ�ķ���һ������ʽ
 * left:�󲿷���
 * right:�Ҳ����Ŵ�
 */
struct Production
{
	Symbol left;
	vector<Symbol> right;
};
ostream& operator<<(ostream& o, Production p);

enum class ActionType		//LR1����ʱ�Ķ���ö��ֵ
{
	shift_in,	//�ƽ�
	reduction,	//��Լ
	accept,		//����
	//����ʹ��map�洢����,�ʳ���״̬���ٴ洢
};

struct Action		//LR�������е�һ���
{
	ActionType action_type;
	int go;		//��actionΪsʱ,��ʾת����״̬;��actionΪrʱ,��ʾʹ�õڼ�������ʽ��Լ
};

struct GrammarProject	//LR(1)��Ŀ
{
	int id_production;			//����Ŀ�Ĳ���ʽָ��,�洢����ʽ��vector�ж�Ӧ���±�
	int point;				//���λ��
	/* S->.E  point=0 */
	/* S->E.  point=1 */

	set<Symbol> follows;		//��Ŀ������Ը�����ս��
	/* S->.E,#/a/b	follows={# a b} */


	bool operator<(const GrammarProject& gp) const {
		//����ȽϺ���ʹ��GrammarProject���Է���set��map��.ע�����ʹ������̶�
		if (this->id_production < gp.id_production)
			return true;
		else if (this->id_production > gp.id_production)
			return false;
		else if (this->point < gp.point)
			return true;
		else if (this->point > gp.point)
			return false;
		else if (this->follows.size() < gp.follows.size())
			return true;
		else if (this->follows.size() > gp.follows.size())
			return false;
		else {
			for (auto it1 = this->follows.cbegin(), it2 = gp.follows.cbegin();
				it1 != this->follows.cend();
				++it1, ++it2) {
				if (*it1 < *it2)
					return true;
				else if (*it1 > * it2)
					return false;
			}
			return false;
		}
	}
	bool operator==(const GrammarProject& gp) const {
		//�����������������Ŀ��Ƚ�
		return this->id_production == gp.id_production && this->point == gp.point && this->follows == gp.follows;
	}
	bool operator!=(const GrammarProject& gp) const {
		//�����������������Ŀ��Ƚ�
		return !(*this == gp);
	}
};

class LR1_Parser
{
private:
private:
    Lexer lexer;						//�ʷ�������

    vector<Production> productions_list;		//�ķ�����
    map<Symbol, set<Symbol>> first_list;		//���ս��first��
    //map<Tag, set<Tag>> follow_list;	//���ս��follow��
    vector<set<GrammarProject>> project_set_list;	//��Ŀ��
    //map<int, map<Tag, int>> state_trans_map;		//��Ŀ֮���ת�ƹ�ϵ(int�洢��Ŀ�����±�)
    map<int, map<Symbol, Action>> action_go_map;		//action���goto��,�洢��һ��

    PTree pTree;						//�﷨��

private:
    State readProductionsFile(ifstream& in);	//�����ķ�����ʽ
    set<GrammarProject> getClosure(const set<GrammarProject>&);	//��CLOSURE��
    int findSameProjectSet(const set<GrammarProject>&);			//������ͬ��CLOSURE����ʧ�ܷ���-1
    void initFirstList();		//��ʼ��First��
    State initActionGotoMap();	//��ʶ���ǰ׺��DFA

public:		//�ǵø�Ϊprivate
    LR1_Parser();
    ~LR1_Parser();

    void print_LR_table(ofstream& of);
    State init(ifstream& grammar_productions_file,ofstream& output_file);			//�﷨��������ʼ��
	State lex(ifstream& source_file,ofstream& output_file);
    void print_parse_state(ofstream&of, const int& stack, const Symbol& t_stack, const Token& token, const Action& action);
    State parse(Token&, ofstream& output_file);	//�﷨����
    void printTree(ostream& out);		//��ӡ��
    void clear_all();                   //�����һ������
};

#endif // !PARSER_H
