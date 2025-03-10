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

ostream& operator<<(ostream& o, Production p)
{
	o<<convSymbol2Str(p.left)<<"->";
	for(auto i = p.right.begin();i!=p.right.end();++i)
	{
		o<<convSymbol2Str(*i)<<' ';
	}
	return o;
}

void LR1_Parser::print_LR_table(ofstream& of)
{
	char s[1024];
	of << "<table>\n<thead>\n<tr>";
	of<<"<th>state</th>";
	for(auto i = TAG2STR.begin();i!=TAG2STR.end();++i)
	{
		of<<"<th>"<<(*i).second<<"</th>";
	}
	of<<"</tr></thead>\n<tbody>";
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

State LR1_Parser::readProductionsFile(ifstream& in)	//读入文法产生式
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
		productions_list[0].right.push_back(productions_list[1].left);	//构造拓广文法
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
		//打印LR分析表
		print_LR_table(of);
	}
	return ret;
}

State LR1_Parser::lex(ifstream& source_file, ofstream&of, ofstream& debug)
{
	int ret;
	if(ret=lexer.start_analysis(source_file,debug))
	{
		debug<<"analysis ret returned "<<ret<<endl;
		return State::ERROR;
	}
	if(ret=lexer.output_analysis(of))
	{
		debug<<"output ret returned "<<ret<<endl;
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
		first_list[Symbol(i)] = { Symbol(i) };	//终结符First集为自身

	vector<int> grammar_pointer;	//记录产生式右部第一个符号为非终结符的文法
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
					//考虑A->A..的特殊情况
					if (productions_list[i].left == elem_A) {
						if (first_list[elem_A].count(Symbol::epsilon))
							continue;
						else
							break;
					}
					//若出现A->B...,则将B的first集全部加到A中
					for (const auto& elem_B : first_list[elem_A]) {
						if (elem_B == Symbol::epsilon) {
							have_epsilon = true;
							continue;	//epsilon不加入
						}
						int before = first_list[productions_list[i].left].size();
						first_list[productions_list[i].left].insert(elem_B);
						int after = first_list[productions_list[i].left].size();
						if (before < after)
							flag = true;
					}
					if (!have_epsilon)
						break;	//若不含空,则后续不用继续加入
				}
				else
					break;
			}
			if (have_epsilon)	//如果产生式最后一个符号也含空,则将空加入First集
				first_list[productions_list[i].left].insert(Symbol::epsilon);
		}
		if (!flag)	//如果first集不再增加,则返回
			break;
	}
}

set<GrammarProject> LR1_Parser::getClosure(const set<GrammarProject>& project_set)
{
	set<GrammarProject> ret(project_set);			//project_set自身的所有项目都在闭包中
	set<GrammarProject> old_project(project_set);	//辅助集合
	set<GrammarProject> new_project;

	bool flag;
	while (true) {
		flag = false;
		for (const auto& i : old_project) {	//扫描上一次产生的所有项目
			if (productions_list[i.id_production].right.size() > i.point && isVN(productions_list[i.id_production].right[i.point])) {
				//A->α.Bβ型
				Symbol vn = productions_list[i.id_production].right[i.point];

				//求出first(βa)
				set<Symbol> firstba;
				if (i.point + 1 < productions_list[i.id_production].right.size()) {
					firstba = first_list[productions_list[i.id_production].right[i.point + 1]];
					auto p = firstba.find(Symbol::epsilon);
					if (p != firstba.cend()) {
						//如果含有epsilon,则删除epsilon并把原项目的follows加入
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
					//扫描所有B->γ型的产生式
					if (productions_list[gp].left == vn) {
						//若CLOSURE中不存在{B->γ,firstba},则加入
						bool have = false;
						for (auto it = ret.begin(); it != ret.end(); ++it) {
							if (it->id_production == gp && it->point == 0) {
								//项目在集合
								have = true;
								if (it->follows != firstba) {
									//若follows不完整,则插入新的follows
									flag = true;
									//由于集合元素的值无法修改,故只能覆盖之
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
							//否则插入新项目
							flag = true;
							ret.insert({ gp,0,firstba });
							new_project.insert({ gp,0,firstba });
						}
					}
				}
			}
		}
		if (!flag)	//不再增加,则返回
			break;
		old_project = new_project;	//对新添加项目进行下一轮扫描
		new_project.clear();
	}

	return ret;
}

int LR1_Parser::findSameProjectSet(const set<GrammarProject>& new_pset)
{
	for (int i = 0; i < project_set_list.size(); i++) {
		if (project_set_list[i].size() != new_pset.size())	//长度不同一定不重复
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
	return -1;	//未找到
}

State LR1_Parser::initActionGotoMap()
{
	project_set_list.clear();
	//初始状态为CLOSURE({S0->.program,#})
	project_set_list.emplace_back(getClosure({ { 0,0,{Symbol::the_end} } }));

	int new_index = 0;		//新项目集下标
	while (new_index < project_set_list.size()) {
		set<GrammarProject>& pset_now = project_set_list[new_index];
		map<Symbol, set<GrammarProject>> new_pset_map;		//当前项目集可以产生的新项目集

		//扫描所有项目
		for (const auto& i : pset_now) {
			if (i.point < productions_list[i.id_production].right.size()) {
				//不是归约项目
				new_pset_map[productions_list[i.id_production].right[i.point]].insert({ i.id_production,i.point + 1,i.follows });
			}
			else {
				//是归约项目
				if (i.id_production == 0 && i.point == 1 && i.follows.size() == 1 && *i.follows.cbegin() == Symbol::the_end)
					action_go_map[new_index][Symbol::the_end] = { ActionType::accept,i.id_production };	//可接受状态
				else {
					for (const auto& follow : i.follows) {
						if (action_go_map[new_index].count(follow))
							return State::ERROR;	//如果转移表该项已经有动作,则产生多重入口,不是LR(1)文法,报错
						else
							action_go_map[new_index][follow] = { ActionType::reduction,i.id_production };	//用该产生式归约
					}
				}
			}
		}

		//生成新closure集，填写转移表
		for (const auto& i : new_pset_map) {
			set<GrammarProject> NS = getClosure(i.second);	//生成新closure集
			int it = findSameProjectSet(NS);				//查重
			if (it == -1) {
				project_set_list.emplace_back(NS);
				action_go_map[new_index][i.first] = { ActionType::shift_in, int(project_set_list.size()) - 1 };	//移进
			}
			else {
				action_go_map[new_index][i.first] = { ActionType::shift_in, it };	//移进
			}
		}

		++new_index;	//处理下一个项目集的转移关系
	}
	return State::OK;
}

State LR1_Parser::parse(Token& err_token, ofstream&of)
{
    err_token.line = err_token.col = 0;
    stack<int> SStack;	//状态栈
    stack<Symbol> TStack;	//输入栈
    stack<int> NStack;  //树结点栈，存放树节点下标

    SStack.push(0);		//初始化
    TStack.push(Symbol::the_end);	//初始化
    //NStack.push(-1);			//初始化
    bool use_lastToken = false;	//判断是否使用上次的token
    Token t_now;	//当前token
    int s_now;		//当前state
    Action m_now;	//当前动作

	of << "<table>\n<thead>\n<tr>";
	of<<"<th>state Top</th><th>Symbol Top</th><th>input</th><th>action</th>";
	of<<"</tr></thead>\n<tbody>";
	
	
    while (true) {
        //需要新获取一个token
        if (!use_lastToken) {
            State ret = this->lexer.getNextLexical(t_now);
            if (ret == State::ERROR)
                return ret;
        }
        s_now = SStack.top();						//获取当前状态
        if (action_go_map.count(s_now) == 0 || action_go_map[s_now].count(t_now.symbol_id) == 0) {
            //若对应表格项为空,则出错
            err_token = t_now;
            return State::ERROR;
        }
        m_now = action_go_map[s_now][t_now.symbol_id];	//获取当前动作
            //移进
    	print_parse_state(of,SStack.top(),TStack.top(),t_now,m_now);
        if (m_now.action_type == ActionType::shift_in) {
            SStack.push(m_now.go);
            TStack.push(t_now.symbol_id);

            TNode node_in;	//移进的树结点
            node_in.symbol = t_now.symbol_id;	//初始化tag值
            node_in.p = pTree.TNode_List.size();	//指定树节点在TNode_List中的下标
            pTree.TNode_List.push_back(node_in);	//移进树结点
            NStack.push(node_in.p);					//将树节点下标移进树栈（保证栈内结点和TNode_List中的结点一一对应）

            use_lastToken = false;
        }	//归约
        else if (m_now.action_type == ActionType::reduction) {
            int len = productions_list[m_now.go].right.size();	//产生式右部长度

            TNode node_left;								//产生式左部
            node_left.symbol = productions_list[m_now.go].left;	//产生式左部tag
            node_left.p = pTree.TNode_List.size();			//移进树结点

            //移出栈
            while (len-- > 0) {
                SStack.pop();
                TStack.pop();

                node_left.childs.push_front(NStack.top());	//创建子结点链表
                NStack.pop();
            }

            pTree.TNode_List.push_back(node_left);			//移进树栈

            s_now = SStack.top();	//更新当前状态
            if (action_go_map.count(s_now) == 0 ||
                action_go_map[s_now].count(node_left.symbol) == 0) {
                //若对应表格项为空,则出错
                err_token = t_now;
                return State::ERROR;
            }

            m_now = action_go_map[s_now][node_left.symbol];	//更新当前动作
            //入栈操作
            SStack.push(m_now.go);
            TStack.push(node_left.symbol);
            NStack.push(node_left.p);

            use_lastToken = true;
        }
        else //接受
        {
            pTree.RootNode = pTree.TNode_List.size() - 1;	//根结点即为最后一个移进树结点集的结点
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
		//没有根节点，树都不存在，没得画咯
		cerr<<"nothing to output!"<<endl;
		return;
	}
	stack<int> Q;
	//out << "digraph parser_tree{" << endl;
	// out << "rankdir=TB;" << endl;
	//初始化结点
	of<<"{\n";
	printTreeNode(of, pTree.RootNode);
	of<<"}";
	return;
}

void LR1_Parser::clear_all()
{
    //清空树
    this->pTree.TNode_List.clear();
    this->pTree.RootNode = -1;
    //清空grammar_list、first_list等
    this->productions_list.clear();
    this->first_list.clear();
    this->project_set_list.clear();
    this->action_go_map.clear();
    //清空lexer的数据
    // this->lexer.clear_data();
}
