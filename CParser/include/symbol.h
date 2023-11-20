#ifndef TAG_H
#define TAG_H

#include "common.h"
#include <string>
#include <map>

using std::string;
using std::map;

//�ķ�����
enum class Symbol
{
	//�ս��
	epsilon = 0,	//��
	id,				//��ʶ��
	num,			//����,����������С��
	the_end,		//#,��ʾ��ֹ

	//�ؼ��� keyword
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

	//�����
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

	//�ֽ��
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

	//�ս���ͷ��ս���ķֽ���
	//��ö��ֵ�����������ս���ͷ��ս��
	vtnboundary = 200,


	//���ս��
	S0,				//�ع��ķ���ʼ����
	program,
	dec_list,		//declaration_list,�����б�
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

//�ؼ��ֱ�,���ؼ����ַ���ת��Ϊ��Ӧ��Tag
extern const std::map<std::string, Symbol> KEYWORD_STR2TAG;
//���ַ���ת��Ϊ��Ӧ��Tag,���ڶ����ķ�����ʽ
extern const std::map<std::string, Symbol> STR2TAG;
#ifdef DEBUG
//����Tagö��ֵ��Ӧ���ַ�����ʾ,���ڵ���
extern const std::map<Symbol, std::string> TAG2STR;
#endif // DEBUG


inline bool isVT(const Symbol t)	//�ж�Tag�Ƿ�Ϊ�ս��
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
