#ifndef TAG_H
#define TAG_H

#include "common.h"
#include <string>
#include <map>

using std::string;
using std::map;

//文法符号
enum class Symbol
{
	//终结符
	epsilon = 0,	//空
	id,				//标识符
	num,			//数字,包括整数和小数
	the_end,		//#,表示终止

	//关键字 keyword
	kw_auto = 10,
	kw_break,
	kw_case,
	kw_char,
	kw_const,
	kw_continue,
	kw_default,
	kw_do,
	kw_double,
	kw_else,
	kw_enum,
	kw_extern,
	kw_float,
	kw_for,
	kw_goto,
	kw_if,
	kw_int,
	kw_long,
	kw_register,
	kw_return,
	kw_short,
	kw_signed,
	kw_sizeof,
	kw_static,
	kw_struct,
	kw_switch,
	kw_typedef,
	kw_union,
	kw_unsigned,
	kw_void,
	kw_volatile,
	kw_while,

	//运算符
	sb_add = 50,
	sb_sub,
	sb_mult,
	sb_div,
	sb_mod,
	sb_double_add,
	sb_double_sub,
	sb_less,
	sb_less_equal,
	sb_more,
	sb_more_equal,
	sb_not_equal,
	sb_equal,
	sb_and,
	sb_or,
	sb_not,
	sb_b_and,
	sb_b_or,
	sb_xor,
	sb_b_not,
	sb_b_left,
	sb_b_right,
	sb_assign,
	sb_add_assign,
	sb_sub_assign,
	sb_mult_assign,
	sb_div_assign,
	sb_mod_assign,
	sb_and_assign,
	sb_or_assign,
	sb_not_assign,
	sb_point,
	sb_for,

	//分界符
	sb_left_parenthesis = 100,
	sb_right_parenthesis,
	sb_left_brace,
	sb_right_brace,
	sb_left_bracket,
	sb_right_bracket,
	sb_semicolon,
	sb_comma,
	sb_colon,
	sb_question,
	sb_double_quotation,
	sb_single_quotation,
	sb_pound,
	sb_backslash,

	//终结符和非终结符的分界线
	//此枚举值仅用作区分终结符和非终结符
	vtnboundary = 200,


	//非终结符
	S0,				//拓广文法开始符号
	program,
	dec_list,		//declaration_list,声明列表
	dec,
	dec_type,
	func_dec,
	var_dec,
	params,
	stat_block,
	param_list,
	param,
	pre_param,
	in_dec,
	stat_str,
	in_var_dec,
	pre_in_var_dec,
	stat,
	if_stat,
	while_stat,
	return_stat,
	assign_stat,
	expr,
	add_expr,
	comp_add_expr,
	comp_operator,
	term,
	pre_term,
	factor,
	pre_factor,
	func_type,
	call,
	argus,
	argu_list,
	pre_expr,
};

//关键字表,将关键字字符串转换为对应的Tag
extern const std::map<std::string, Symbol> KEYWORD_STR2TAG;
//将字符串转换为相应的Tag,用于读入文法产生式
extern const std::map<std::string, Symbol> STR2TAG;
#ifdef DEBUG
//返回Tag枚举值对应的字符串表示,便于调试
extern const std::map<Symbol, std::string> TAG2STR;
#endif // DEBUG


inline bool isVT(const Symbol t)	//判断Tag是否为终结符
{
	return t < Symbol::vtnboundary;
}
inline bool isVN(const Symbol t)
{
	return t > Symbol::vtnboundary;
}

inline Symbol convStr2Tag(const string& s)
{
	return STR2TAG.at(s);
}

inline string convTag2Str(const Symbol& t)
{
	return TAG2STR.at(t);
}

#endif // !TAG_H
