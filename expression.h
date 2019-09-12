/* expression.h */

/* lasy */
/* 2004.12.12 */

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

enum op_code {
	NOOP = 0,
	INC, DEC, BINV, POS, NEG, NOT,
	MUL, DIV, MOD,
	ADD, SUB,
	LSHF, RSHF,
	LT, GT, LE, GE,
	EQ, NE,
	BAND,
	BXOR,
	BOR,
	AND,
	OR,
	ASSIGN, MULAS, DIVAS, MODAS, ADDAS, SUBAS,
	LSHFAS, RSHFAS, BANDAS, BXORAS, BORAS, ANDAS, ORAS,
	LPAR, RPAR, LBRK, RBRK, COMM, FUN, FUNE, END
};

class operand;
class script;

class expression {
public:
	expression();
	virtual ~expression();

	void bind(script *sc);
	void clear();
	void push(const operand &o);
	void push_op(enum op_code op);
	void push_fun(fun_type *f);
	const operand &value();
	bool empty() const;
protected:
	enum op_dir { L2R, R2L };

	struct op_info {
		int pri;
		int opees;
		enum op_dir dir;
		void (expression::*op)(operand &i);
	};

	static const struct op_info ops[];

	script *m_script;

	stack<enum op_code> ostk;
	stack<operand> nstk;
	stack<fun_type *> fstk;

	void check_opees(enum op_code op);
	void pop_op();
	void pop_fun(bool has_para = false);

	void op_inc(operand &i);
	void op_dec(operand &i);
	void op_binv(operand &i);
	void op_pos(operand &i);
	void op_neg(operand &i);
	void op_not(operand &i);
	void op_mul(operand &i);
	void op_div(operand &i);
	void op_mod(operand &i);
	void op_add(operand &i);
	void op_sub(operand &i);
	void op_lshf(operand &i);
	void op_rshf(operand &i);
	void op_lt(operand &i);
	void op_gt(operand &i);
	void op_le(operand &i);
	void op_ge(operand &i);
	void op_eq(operand &i);
	void op_ne(operand &i);
	void op_band(operand &i);
	void op_bxor(operand &i);
	void op_bor(operand &i);
	void op_and(operand &i);
	void op_or(operand &i);
	void op_assign(operand &i);
	void op_mulas(operand &i);
	void op_divas(operand &i);
	void op_modas(operand &i);
	void op_addas(operand &i);
	void op_subas(operand &i);
	void op_lshfas(operand &i);
	void op_rshfas(operand &i);
	void op_bandas(operand &i);
	void op_bxoras(operand &i);
	void op_boras(operand &i);
	void op_andas(operand &i);
	void op_oras(operand &i);
	void op_lpar(operand &i);
	void op_lbrk(operand &i);
	void op_comm(operand &i);

	void do_lpar();
	void do_lbrk();
};

#endif
