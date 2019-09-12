/* script.h */

/* lasy */
/* 2004.12.12 */

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#define IS_LINE_END(c)  ((c) == '\r' || (c) == '\n')
#define IS_SPACE(c)     ((c) == ' ' || (c) == '\t')
#define IS_NUM(c)       ('0' <= (c) && (c) <= '9')
#define IS_HEX_NUM(c)   (('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))
#define IS_ALPHA(c)     (('a' <= (c) && (c) <= 'z') || ('A' <= (c) && (c) <= 'Z'))
#define IS_NAME_INIT(c) (IS_ALPHA(c) || (c) == '_' || (c) == '@' || c == '\\')
#define IS_NAME(c)      (IS_NAME_INIT(c) || IS_NUM(c))

class script {
public:
	script();
	virtual ~script();

	void exec(const char *src);
	void reset();
	void print_ids();
	void print_funs();
protected:
	enum exe_state { CLEAR, AFT_OPEE, AFT_OP, AFT_DOT, AFT_FUN, AFT_FUNS };
	struct sigil_proc_map {
		char ch;
		int si;
		int ei;
		void (script::*proc)();
	};
	struct fun_map {
		const char *name;
		fun_type *pfun;
	};

	static const struct sigil_proc_map sigil_table[];
	static const struct fun_map fun_table[];

	const char *pc;
	enum exe_state state;
	stack<const char *> pcstk;
	data ids;
	int sigil_index;
	expression exp;
	map<string, fun_type *> funs;

	void clear();
	void clear_ids();
	int parse_hex_int();
	int parse_bin_int();
	int parse_dec_int();
	void parse_const();
	void parse_name();
	void parse_sigil();

	bool state_opee();
	void proc_int(int v);
	void proc_name(const string &name);
	void proc_sigil();
	void proc_end();
	void proc_str(const string &str);

	template<enum op_code op> void proc_op1() {
		if (state_opee()) {
			exp.push_op(op);
		} else {
			throw ERR_NXP_OP;
		}
		state = AFT_OP;
	}

	template<enum op_code op> void proc_op2() {
		if (state == AFT_OPEE) {
			exp.push_op(op);
		} else {
			throw ERR_NXP_OP;
		}
		state = AFT_OP;
	}

	template<enum op_code op1, enum op_code op2> void proc_op2() {
		if (state == AFT_OPEE) {
			exp.push_op(op2);
		} else if (state_opee()) {
			exp.push_op(op1);
		} else {
			throw ERR_NXP_OP;
		}
		state = AFT_OP;
	}

	void proc_lpar();
	void proc_rpar();
	void proc_lbrc();
	void proc_rbrc();
	void proc_rbrk();
	void proc_sharp();
	void proc_ques();
	void proc_colo();
	void proc_quot();
	void proc_dot();
	void proc_note();

	void bypass_branch();
	void exit_loop();
	void add_spec_vars();
	void add_funs();
};

#endif
