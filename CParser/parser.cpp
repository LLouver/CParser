#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <fstream>
#include <sstream>
#include <list>
#include <queue>

#include "include/parser.h"

#include "include/lexer_tools.h"
#include "include/Symbol.h"

using namespace std;

/********************************************************
* @Author : LeeQueue & Gao
*
* zcgg��˧!
*
********************************************************/
ostream& operator<<(ostream& o, Production p)
{
	o<<convSymbol2Str(p.left)<<"->";
	for(auto i = p.right.begin();i!=p.right.end();++i)
	{
		o<<convSymbol2Str(*i)<<' ';
	}
	return o;
}

LR1_Parser::LR1_Parser()
{
	//grammar_list = {
	//	/*{Tag::E,{Tag::T,Tag::E1}},
	//	{Tag::E1,{Tag::sb_add,Tag::T,Tag::E1}},
	//	{Tag::E1,{Tag::epsilon}},
	//	{Tag::T,{Tag::F,Tag::T1}},
	//	{Tag::T1,{Tag::sb_time,Tag::F,Tag::T1}},
	//	{Tag::T1,{Tag::epsilon}},
	//	{Tag::F,{Tag::sb_llb,Tag::E,Tag::sb_rlb}},
	//	{Tag::F,{Tag::id}},*/


	//	////��������: S'->S,S->BB,B->aB,B->b
	//	//{Tag::S0,{Tag::S}},
	//	//{Tag::S,{Tag::B,Tag::B}},
	//	//{Tag::B,{Tag::a,Tag::B}},
	//	//{Tag::B,{Tag::b}},

	//	//��������: S'->S,S->BB,B->b,B->��
	//	{Tag::S0,{Tag::S}},
	//	{Tag::S,{Tag::B,Tag::B}},
	//	{Tag::B,{Tag::b}},
	//	{Tag::B,{}},
	//};
}

LR1_Parser::~LR1_Parser()
{
}

void LR1_Parser::print_LR_table(ofstream& of)
{
	char s[1024];
	of << "<table>\n<thead>\n<td>";
	of<<"<th>state</th>";
	for(auto i = TAG2STR.begin();i!=TAG2STR.end();++i)
	{
		of<<"<th>"<<(*i).second<<"</th>";
	}
	of<<"</td></thead>\n<tbody>";
	for(auto i = action_go_map.begin(); i != action_go_map.end(); ++i)
	{
		of<<"<tr>";
		of<<"<td>"<< i->first <<"</td>";
		for(auto j = TAG2STR.begin();j!=TAG2STR.end();++j)
		{
			auto k = i->second.find(j->first);
			of<<"<td>";
			if(k != i->second.end())
			{
				const Action a = k->second;
				switch (a.action_type)
				{
				case ActionType::shift_in:
					of<<"s"<<a.go;
					break;
				case ActionType::reduction:
					of<<"r"<<a.go;
					break;
				case ActionType::accept:
					of<<"acc";
					break;
				}
			}
			of<<"</td>";
		}
		of<<"</tr>\n";
	}
	of<<"</tbody></table>\n";
}

State LR1_Parser::readProductionsFile(ifstream& in)	//�����ķ�����ʽ
{
	if (!in.is_open()){
		std::cerr<<"grammar_productions_file open failed!"<<endl; 
		return State::ERROR;
	}

	productions_list.push_back({ Symbol::S0,{} });
	char buffer[1024];
	string temp;
	try {
		while (in.getline(buffer, 1024, '\n')) {
			istringstream ss(buffer);
			Production new_grammar;
			ss >> temp;
			//cerr<<"read:"<<temp<<endl;
			new_grammar.left = convStr2Symbol(temp);
			ss >> temp;
			//cerr<<"read:"<<temp<<endl;
			while (ss >> temp)
				new_grammar.right.push_back(convStr2Symbol(temp));
			productions_list.push_back(new_grammar);
			//cerr<<(new_grammar)<<endl;
		}
		productions_list[0].right.push_back(productions_list[1].left);	//�����ع��ķ�
		//cerr<<(*(productions_list.end()-1))<<endl;
	}
	catch (const std::exception& e) {
		return State::ERROR;
	}
	return State::OK;
}

State LR1_Parser::init(ifstream& grammar_productions_file, ofstream&of)
{
	if (readProductionsFile(grammar_productions_file) != State::OK)
	{
		std::cerr<<"grammar_productions_file read failed!"<<endl; 
		return State::ERROR;
	}
	initFirstList();
	State ret = initActionGotoMap();
	if(ret == State::OK)
	{
		//��ӡLR������
		print_LR_table(of);
	}
	return ret;
}

State LR1_Parser::lex(ifstream& source_file, ofstream&of)
{
	int ret;
	ofstream debug("debug.txt",ios::out);
	if(ret=lexer.start_analysis(source_file,debug))
	{
		cerr<<"analysis ret returned "<<ret<<endl;
		return State::ERROR;
	}
	if(ret=lexer.output_analysis(of))
	{
		cerr<<"output ret returned "<<ret<<endl;
		return State::ERROR;
	}
	return State::OK;
}

void LR1_Parser::print_parse_state(ofstream& of, const int& state_top, const Symbol& symbol_top,
	const Token& token, const Action& action)
{
	of<<"<tr>";
	of<<"<td>"<<state_top<<"</td>";
	of<<"<td>"<<convSymbol2Str(symbol_top)<<"</td>";
	of<<"<td>"<<convSymbol2Str(token.symbol_id)<<"</td>";
	switch (action.action_type) {
	case ActionType::shift_in:
		of<<"<td>"<<"shift_in and move to " << action.go<<"</td>";
		break;
	case ActionType::reduction:
		of<<"<td>"<<"reduce with production "<< action.go<<"</td>";
		break;
	case  ActionType::accept:
		of<<"<td>"<<"accept"<<"</td>";
	}
	of<<"</tr>\n";
}

void LR1_Parser::initFirstList()
{
	for (int i = int(Symbol::epsilon); isVT(Symbol(i)); ++i)
		first_list[Symbol(i)] = { Symbol(i) };	//�ս��First��Ϊ����

	vector<int> grammar_pointer;	//��¼����ʽ�Ҳ���һ������Ϊ���ս�����ķ�
	for (int i = 0; i < productions_list.size(); i++) {
		if (productions_list[i].right.size() == 0)
			first_list[productions_list[i].left].insert(Symbol::epsilon);
		else {
			auto first_elem = productions_list[i].right.front();
			if (isVT(first_elem))
				first_list[productions_list[i].left].insert(first_elem);
			else
				grammar_pointer.emplace_back(i);
		}
	}

	bool flag;
	while (true) {
		flag = false;
		for (const auto& i : grammar_pointer) {
			bool have_epsilon = false;
			for (const auto& elem_A : productions_list[i].right) {
				have_epsilon = false;
				if (isVN(elem_A)) {
					//����A->A..���������
					if (productions_list[i].left == elem_A) {
						if (first_list[elem_A].count(Symbol::epsilon))
							continue;
						else
							break;
					}
					//������A->B...,��B��first��ȫ���ӵ�A��
					for (const auto& elem_B : first_list[elem_A]) {
						if (elem_B == Symbol::epsilon) {
							have_epsilon = true;
							continue;	//epsilon������
						}
						int before = first_list[productions_list[i].left].size();
						first_list[productions_list[i].left].insert(elem_B);
						int after = first_list[productions_list[i].left].size();
						if (before < after)
							flag = true;
					}
					if (!have_epsilon)
						break;	//��������,��������ü�������
				}
				else
					break;
			}
			if (have_epsilon)	//�������ʽ���һ������Ҳ����,�򽫿ռ���First��
				first_list[productions_list[i].left].insert(Symbol::epsilon);
		}
		if (!flag)	//���first����������,�򷵻�
			break;
	}
}

set<GrammarProject> LR1_Parser::getClosure(const set<GrammarProject>& project_set)
{
	set<GrammarProject> ret(project_set);			//project_set�����������Ŀ���ڱհ���
	set<GrammarProject> old_project(project_set);	//��������
	set<GrammarProject> new_project;

	bool flag;
	while (true) {
		flag = false;
		for (const auto& i : old_project) {	//ɨ����һ�β�����������Ŀ
			if (productions_list[i.id_production].right.size() > i.point && isVN(productions_list[i.id_production].right[i.point])) {
				//A->��.B����
				Symbol vn = productions_list[i.id_production].right[i.point];

				//���first(��a)
				set<Symbol> firstba;
				if (i.point + 1 < productions_list[i.id_production].right.size()) {
					firstba = first_list[productions_list[i.id_production].right[i.point + 1]];
					auto p = firstba.find(Symbol::epsilon);
					if (p != firstba.cend()) {
						//�������epsilon,��ɾ��epsilon����ԭ��Ŀ��follows����
						firstba.erase(p);
						for (const auto& follow : i.follows)
							firstba.insert(follow);
					}
				}
				else {
					for (const auto& follow : i.follows)
						firstba.insert(follow);
				}

				for (int gp = 0; gp < productions_list.size(); gp++) {
					//ɨ������B->���͵Ĳ���ʽ
					if (productions_list[gp].left == vn) {
						//��CLOSURE�в�����{B->��,firstba},�����
						bool have = false;
						for (auto it = ret.begin(); it != ret.end(); ++it) {
							if (it->id_production == gp && it->point == 0) {
								//��Ŀ�ڼ���
								have = true;
								if (it->follows != firstba) {
									//��follows������,������µ�follows
									flag = true;
									//���ڼ���Ԫ�ص�ֵ�޷��޸�,��ֻ�ܸ���֮
									auto ngp = *it;
									for (Symbol firstba_elem : firstba)
										ngp.follows.insert(firstba_elem);
									ret.erase(it);
									ret.insert(ngp);
									new_project.insert(ngp);
								}
								break;
							}
						}
						if (!have) {
							//�����������Ŀ
							flag = true;
							ret.insert({ gp,0,firstba });
							new_project.insert({ gp,0,firstba });
						}
					}
				}
			}
		}
		if (!flag)	//��������,�򷵻�
			break;
		old_project = new_project;	//���������Ŀ������һ��ɨ��
		new_project.clear();
	}

	return ret;
}

int LR1_Parser::findSameProjectSet(const set<GrammarProject>& new_pset)
{
	for (int i = 0; i < project_set_list.size(); i++) {
		if (project_set_list[i].size() != new_pset.size())	//���Ȳ�ͬһ�����ظ�
			continue;
		else {
			bool same = true;
			for (auto it1 = new_pset.cbegin(), it2 = project_set_list[i].cbegin();
				it1 != new_pset.cend();
				++it1, ++it2) {
				if (*it1 != *it2) {
					same = false;
					break;
				}
			}
			if (same)
				return i;
		}
	}
	return -1;	//δ�ҵ�
}

State LR1_Parser::initActionGotoMap()
{
	project_set_list.clear();
	//��ʼ״̬ΪCLOSURE({S0->.program,#})
	project_set_list.emplace_back(getClosure({ { 0,0,{Symbol::the_end} } }));

	int new_index = 0;		//����Ŀ���±�
	while (new_index < project_set_list.size()) {
		set<GrammarProject>& pset_now = project_set_list[new_index];
		map<Symbol, set<GrammarProject>> new_pset_map;		//��ǰ��Ŀ�����Բ���������Ŀ��

		//ɨ��������Ŀ
		for (const auto& i : pset_now) {
			if (i.point < productions_list[i.id_production].right.size()) {
				//���ǹ�Լ��Ŀ
				new_pset_map[productions_list[i.id_production].right[i.point]].insert({ i.id_production,i.point + 1,i.follows });
			}
			else {
				//�ǹ�Լ��Ŀ
				if (i.id_production == 0 && i.point == 1 && i.follows.size() == 1 && *i.follows.cbegin() == Symbol::the_end)
					action_go_map[new_index][Symbol::the_end] = { ActionType::accept,i.id_production };	//�ɽ���״̬
				else {
					for (const auto& follow : i.follows) {
						if (action_go_map[new_index].count(follow))
							return State::ERROR;	//���ת�Ʊ�����Ѿ��ж���,������������,����LR(1)�ķ�,����
						else
							action_go_map[new_index][follow] = { ActionType::reduction,i.id_production };	//�øò���ʽ��Լ
					}
				}
			}
		}

		//������closure������дת�Ʊ�
		for (const auto& i : new_pset_map) {
			set<GrammarProject> NS = getClosure(i.second);	//������closure��
			int it = findSameProjectSet(NS);				//����
			if (it == -1) {
				project_set_list.emplace_back(NS);
				action_go_map[new_index][i.first] = { ActionType::shift_in, int(project_set_list.size()) - 1 };	//�ƽ�
			}
			else {
				action_go_map[new_index][i.first] = { ActionType::shift_in, it };	//�ƽ�
			}
		}

		++new_index;	//������һ����Ŀ����ת�ƹ�ϵ
	}
	return State::OK;
}

/*********************************************************************************************************************
* parser����ֱ�ӵ�����getNextLexical����������lexer��ʱ��û��Ϊ��ָ���ļ�·��file_in��������parser�Ĺ��캯����ָ��һ��
* ��Լ��ʱ����Թ����﷨���������ȿ��ˣ��������������ټӽ�ȥ����
**********************************************************************************************************************/
State LR1_Parser::parse(Token& err_token, ofstream&of)
{
    err_token.line = err_token.col = 0;
    stack<int> SStack;	//״̬ջ
    stack<Symbol> TStack;	//����ջ
    stack<int> NStack;  //�����ջ��������ڵ��±�

    SStack.push(0);		//��ʼ��
    TStack.push(Symbol::the_end);	//��ʼ��
    //NStack.push(-1);			//��ʼ��
    bool use_lastToken = false;	//�ж��Ƿ�ʹ���ϴε�token
    Token t_now;	//��ǰtoken
    int s_now;		//��ǰstate
    Action m_now;	//��ǰ����

	of << "<table>\n<thead>\n<tr>";
	of<<"<th>state Top</th><th>Symbol Top</th><th>input</th><th>action</th>";
	of<<"</tr></thead>\n<tbody>";
	
	
    while (true) {
        //��Ҫ�»�ȡһ��token
        if (!use_lastToken) {
            State ret = this->lexer.getNextLexical(t_now);
            if (ret == State::ERROR)
                return ret;
        }
        s_now = SStack.top();						//��ȡ��ǰ״̬
        if (action_go_map.count(s_now) == 0 || action_go_map[s_now].count(t_now.symbol_id) == 0) {
            //����Ӧ�����Ϊ��,�����
            err_token = t_now;
            return State::ERROR;
        }
        m_now = action_go_map[s_now][t_now.symbol_id];	//��ȡ��ǰ����
            //�ƽ�
    	print_parse_state(of,SStack.top(),TStack.top(),t_now,m_now);
        if (m_now.action_type == ActionType::shift_in) {
            SStack.push(m_now.go);
            TStack.push(t_now.symbol_id);

            TNode node_in;	//�ƽ��������
            node_in.symbol = t_now.symbol_id;	//��ʼ��tagֵ
            node_in.p = pTree.TNode_List.size();	//ָ�����ڵ���TNode_List�е��±�
            pTree.TNode_List.push_back(node_in);	//�ƽ������
            NStack.push(node_in.p);					//�����ڵ��±��ƽ���ջ����֤ջ�ڽ���TNode_List�еĽ��һһ��Ӧ��

            use_lastToken = false;
        }	//��Լ
        else if (m_now.action_type == ActionType::reduction) {
            int len = productions_list[m_now.go].right.size();	//����ʽ�Ҳ�����

            TNode node_left;								//����ʽ��
            node_left.symbol = productions_list[m_now.go].left;	//����ʽ��tag
            node_left.p = pTree.TNode_List.size();			//�ƽ������

            //�Ƴ�ջ
            while (len-- > 0) {
                SStack.pop();
                TStack.pop();

                node_left.childs.push_front(NStack.top());	//�����ӽ������
                NStack.pop();
            }

            pTree.TNode_List.push_back(node_left);			//�ƽ���ջ

            s_now = SStack.top();	//���µ�ǰ״̬
            if (action_go_map.count(s_now) == 0 ||
                action_go_map[s_now].count(node_left.symbol) == 0) {
                //����Ӧ�����Ϊ��,�����
                err_token = t_now;
                return State::ERROR;
            }

            m_now = action_go_map[s_now][node_left.symbol];	//���µ�ǰ����
            //��ջ����
            SStack.push(m_now.go);
            TStack.push(node_left.symbol);
            NStack.push(node_left.p);

            use_lastToken = true;
        }
        else //����
        {
            pTree.RootNode = pTree.TNode_List.size() - 1;	//����㼴Ϊ���һ���ƽ�����㼯�Ľ��
        	of<<"</tbody></table>\n";
        	return State::OK;		//accept
        }
    }
}

void LR1_Parser::printTreeNode(ofstream &of, const int node_id)
{
	TNode node = pTree.TNode_List[node_id];
	of<<"\"data\":" << "\""<< convSymbol2Str(node.symbol) << "\",";
	of<<"\"children\":[\n";
	auto i = node.childs.cbegin();
	if(i!= node.childs.cend())
	{
		of<<"{\n";
		printTreeNode(of,*i);
		of<<"}";
		for (i++; i != node.childs.cend(); i++)
		{
			of<<",\n{";
			printTreeNode(of,*i);
			of<<"}";
		}
	}
	of<<"]";
}

void LR1_Parser::printTree(ofstream& of)
{
	if (pTree.RootNode == -1)
	{
		//û�и��ڵ㣬���������ڣ�û�û���
		cerr<<"nothing to output!"<<endl;
		return;
	}
	stack<int> Q;
	//out << "digraph parser_tree{" << endl;
	// out << "rankdir=TB;" << endl;
	//��ʼ�����
	of<<"{\n";
	printTreeNode(of, pTree.RootNode);
	of<<"}";
	return;
}
/*
void LR1_Parser::printVP_DFA(ostream& out)
{
	out << "digraph{" << endl;
	out << "rankdir=LR;" << endl;
	//����ÿһ����Ŀ��
	for (int i = 0; i < project_set_list.size(); i++)
	{
		out << "node_" << i << "[label=\"";
		//�����Ŀ���е�ÿһ����Ŀ
		for (const auto& gp : project_set_list[i])
		{
			//�������ʽ
			out << convTag2Str(productions_list[gp.id_production].left) << "->";
            int p;
            for (p = 0; p < productions_list[gp.id_production].right.size(); p++)
			{
				if (gp.point == p)
					out << ".";
				out << convTag2Str(productions_list[gp.id_production].right[p]);
			}
            if (gp.point == p)
                out << ".";
			out << ", ";
			//���follows
			for (auto it = gp.follows.cbegin(); it != gp.follows.cend(); it++)
			{
				if (it != gp.follows.cbegin())
					out << "/";
				out << convTag2Str(*it);
			}
			out << "\n";
		}
		//�����������
		out << "\" shape=\"box\"];" << endl;
	}

	//����ת�ƹ�ϵ
	for (int i = 0; i < project_set_list.size(); i++)
	{
		for (const auto& tag_mov : action_go_map[i])
		{
			//ֻ���ƽ��Ż�ת��
			if (tag_mov.second.action_type != ActionType::shift_in)
				continue;
			else
				out << "node_" << i << "->node_" << tag_mov.second.go << "[label=\"" << convTag2Str(tag_mov.first) << "\"];" << endl;
		}
	}

	out << "}" << endl;
	return;
}
*/
void LR1_Parser::clear_all()
{
    //�����
    this->pTree.TNode_List.clear();
    this->pTree.RootNode = -1;
    //���grammar_list��first_list��
    this->productions_list.clear();
    this->first_list.clear();
    this->project_set_list.clear();
    this->action_go_map.clear();
    //���lexer������
    // this->lexer.clear_data();
}



/*int main()
{
	LR1_Parser lr1;

	//lr1.init(R"(D:\Mydata\homework\������\�γ�\����ԭ��\����ҵ\test\grammar.txt)");

	ofstream T_out;
	ofstream D_out;
    T_out.open(R"(C:\QT_Projects\LR1_parser\graph_Tree.dot)");
    D_out.open(R"(C:\QT_Projects\LR1_parser\graph_DFA.dot)");

    lr1.openGrammarFile(R"(C:\QT_Projects\LR1_parser\grammar.txt)");
	lr1.initFirstList();
	lr1.initActionGotoMap();

	lr1.printVP_DFA(D_out);

    lr1.parser(R"(C:\QT_Projects\LR1_parser\test_program.txt)");
	lr1.printTree(T_out);
	return 0;
}*/
