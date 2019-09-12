/* script.cpp */

/* lasy */
/* 2004.12.12 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using namespace std;

#include "error.h"
#include "data.h"
#include "operand.h"
#include "funs.h"
#include "expression.h"
#include "script.h"

const struct script::sigil_proc_map script::sigil_table[] = {
	{  0 ,  1, 29, 0                           }, // Root (0)
	{ '=', 29, 30, &script::proc_op2<ASSIGN>   }, // Level 1 (1)
	{ '<', 30, 32, &script::proc_op2<LT>       },
	{ '>', 32, 34, &script::proc_op2<GT>       },
	{ '!', 34, 35, &script::proc_op1<NOT>      },
	{ '&', 35, 37, &script::proc_op2<BAND>     },
	{ '|', 37, 39, &script::proc_op2<BOR>      },
	{ '+', 39, 41, &script::proc_op2<POS, ADD> },
	{ '-', 41, 43, &script::proc_op2<NEG, SUB> },
	{ '*', 43, 45, &script::proc_op2<MUL>      },
	{ '/', 45, 47, &script::proc_op2<DIV>      },
	{ '%', 47, 48, &script::proc_op2<MOD>      },
	{ '(',  0,  0, &script::proc_lpar          },
	{ ')',  0,  0, &script::proc_rpar          },
	{ '~',  0,  0, &script::proc_op1<BINV>     },
	{ '^', 48, 49, &script::proc_op2<BXOR>     },
	{ '?',  0,  0, &script::proc_ques          },
	{ '#',  0,  0, &script::proc_sharp         },
	{ '$',  0,  0, &script::proc_end           },
	{ ':',  0,  0, &script::proc_colo          },
	{ '"',  0,  0, &script::proc_quot          },
	{'\'',  0,  0, 0                           },
	{ ',',  0,  0, &script::proc_op2<COMM>     },
	{ '.',  0,  0, &script::proc_dot           },
	{ '[',  0,  0, &script::proc_op2<LBRK>     },
	{ ']',  0,  0, &script::proc_rbrk          },
	{ '{',  0,  0, &script::proc_lbrc          },
	{ '}',  0,  0, &script::proc_rbrc          },
	{ ';',  0,  0, &script::proc_end           },
	{ '=',  0,  0, &script::proc_op2<EQ>       }, // Level 2 (29)
	{ '=',  0,  0, &script::proc_op2<LE>       },
	{ '<', 49, 50, &script::proc_op2<LSHF>     },
	{ '=',  0,  0, &script::proc_op2<GE>       },
	{ '>', 50, 51, &script::proc_op2<RSHF>     },
	{ '=',  0,  0, &script::proc_op2<NE>       },
	{ '&', 51, 52, &script::proc_op2<AND>      },
	{ '=',  0,  0, &script::proc_op2<BANDAS>   },
	{ '|', 52, 53, &script::proc_op2<OR>       },
	{ '=',  0,  0, &script::proc_op2<BORAS>    },
	{ '+',  0,  0, &script::proc_op1<INC>      },
	{ '=',  0,  0, &script::proc_op2<ADDAS>    },
	{ '-',  0,  0, &script::proc_op1<DEC>      },
	{ '=',  0,  0, &script::proc_op2<SUBAS>    },
	{ '=',  0,  0, &script::proc_op2<MULAS>    },
	{ '/',  0,  0, 0                           },
	{ '*',  0,  0, &script::proc_note          },
	{ '=',  0,  0, &script::proc_op2<DIVAS>    },
	{ '=',  0,  0, &script::proc_op2<MODAS>    },
	{ '=',  0,  0, &script::proc_op2<BXORAS>   },
	{ '=',  0,  0, &script::proc_op2<LSHFAS>   }, // Level 3 (49)
	{ '=',  0,  0, &script::proc_op2<RSHFAS>   },
	{ '=',  0,  0, &script::proc_op2<ANDAS>    },
	{ '=',  0,  0, &script::proc_op2<ORAS>     },
};

const struct script::fun_map script::fun_table[] = {
	{ "_p",   f__p   },
	{ "_pw",  f__pw  },
	{ "_pln", f__pln },
};

script::script()
{
	add_funs();
	exp.bind(this);
	reset();
}

script::~script()
{
}

void script::exec(const char *src)
{
	pc = src;
	try {
		while (1) {
			if (pc[0] == '\0') break;
			if (IS_LINE_END(pc[0])) {
				proc_end();
				pc++;
			} else if (IS_SPACE(pc[0])) {
				pc++;
			} else if (IS_NUM(pc[0])) {
				parse_const();
			} else if (IS_NAME_INIT(pc[0])) {
				parse_name();
			} else {
				parse_sigil();
			}
		}
		proc_end();
	} catch (const char *err) {
		cerr << err << ": " << pc << endl;
		clear();
	}
}

void script::reset()
{
	clear();
	clear_ids();
}

void script::print_ids()
{
	cout << ids << endl;
}

void script::print_funs()
{
	map<string, fun_type *>::const_iterator i;
	for (i = funs.begin(); i != funs.end(); ++i) {
		cout << i->first << endl;
	}
}

void script::clear()
{
	while (!pcstk.empty()) pcstk.pop();
	sigil_index = 0;
	exp.clear();
	state = CLEAR;
}

void script::clear_ids()
{
	ids.new_map();
	add_spec_vars();
}

int script::parse_hex_int()
{
	int value = 0;
	bool ok = false;
	while (1) {
		if (IS_NUM(pc[0])) {
			value <<= 4;
			value += pc[0] & 0x0F;
			ok = true;
		} else if (IS_HEX_NUM(pc[0])) {
			value <<= 4;
			value += (pc[0] & 0x0F)+9;
			ok = true;
		} else {
			break;
		}
		pc++;
	}
	if (!ok) throw ERR_BAD_HEX_INT;
	return value;
}

int script::parse_bin_int()
{
	int value = 0;
	bool ok = false;
	while (1) {
		if (pc[0] == '1') {
			value <<= 1;
			value++;
			ok = true;
		} else if (pc[0] == '0') {
			ok = true;
		} else {
			break;
		}
		pc++;
	}
	if (!ok) throw ERR_BAD_BIN_INT;
	return value;
}

int script::parse_dec_int()
{
	int value = 0;
	while (1) {
		if (IS_NUM(pc[0])) {
			value *= 10;
			value += pc[0] & 0x0F;
		} else {
			break;
		}
		pc++;
	}
	return value;
}

void script::parse_const()
{
	if (pc[0] == '0' && (pc[1] == 'x' || pc[1] == 'X')) {
		pc += 2;
		proc_int(parse_hex_int());
	} else if (pc[0] == '0' && (pc[1] == 'b' || pc[1] == 'B')) {
		pc += 2;
		proc_int(parse_bin_int());
	} else {
		proc_int(parse_dec_int());
	}
}

void script::parse_name()
{
	const char *name = pc;
	while (IS_NAME(pc[0])) pc++;
	proc_name(string(name, pc-name));
}

void script::parse_sigil()
{
	while (1) {
		int i;
		for (i = sigil_table[sigil_index].si; i < sigil_table[sigil_index].ei; i++) {
			if (pc[0] == sigil_table[i].ch) break;
		}
		if (i < sigil_table[sigil_index].ei) {
			sigil_index = i;
		} else {
			if (sigil_index != 0) proc_sigil();
			break;
		}
		pc++;
	}
}

bool script::state_opee()
{
	return state == CLEAR || state == AFT_OP || state == AFT_FUNS;
}

void script::proc_int(int v)
{
	if (state_opee()) {
		exp.push(data(v));
	} else {
		throw ERR_NXP_CONST;
	}
	state = AFT_OPEE;
}

void script::proc_name(const string &name)
{
	if (state == AFT_DOT) {
		exp.push_op(LBRK);
		proc_str(name);
		exp.push_op(RBRK);
		state = AFT_OPEE;
	} else if (state_opee()) {
		if (funs.find(name) != funs.end()) {
			exp.push_fun(funs[name]);
			state = AFT_FUN;
		} else {
			if (!ids.exists(name)) ids[name] = data();
			exp.push(&ids[name]);
			state = AFT_OPEE;
		}
	} else {
		throw ERR_NXP_NAME;
	}
}

void script::proc_sigil()
{
	if (sigil_table[sigil_index].proc != 0) {
		(this->*sigil_table[sigil_index].proc)();
	} else {
		throw ERR_NXP_SIGIL;
	}
	sigil_index = 0;
}

void script::proc_end()
{
	if (state == AFT_OPEE) {
		exp.push_op(END);
		exp.clear();
	} else if (state != CLEAR) {
		throw ERR_NOT_END;
	}
	state = CLEAR;
}

void script::proc_str(const string &str)
{
	exp.push(data(str));
	state = AFT_OPEE;
}

void script::proc_lpar()
{
	if (state_opee()) {
		exp.push_op(LPAR);
		state = AFT_OP;
	} else if (state == AFT_FUN) {
		state = AFT_FUNS;
	} else {
		throw ERR_NXP_SIGIL;
	}
}

void script::proc_rpar()
{
	if (state == AFT_OPEE) {
		exp.push_op(RPAR);
	} else if (state == AFT_FUNS) {
		exp.push_op(FUNE);
	} else {
		throw ERR_NXP_SIGIL;
	}
	state = AFT_OPEE;
}

void script::proc_lbrc()
{
	proc_end();
	pcstk.push(pc);
}

void script::proc_rbrc()
{
	proc_end();
	if (pcstk.empty()) throw ERR_BAD_LOOP;
	pc = pcstk.top();
}

void script::proc_rbrk()
{
	if (state == AFT_OPEE) {
		exp.push_op(RBRK);
	} else {
		throw ERR_NXP_SIGIL;
	}
	state = AFT_OPEE;
}

void script::proc_sharp()
{
	proc_end();
	if (pcstk.empty()) throw ERR_BAD_LOOP;
	pcstk.pop();
	exit_loop();
}

void script::proc_ques()
{
	if (state == AFT_OPEE) {
		exp.push_op(END);
		if (!exp.value().is_true()) bypass_branch();
		exp.clear();
	} else {
		throw ERR_NXP_SIGIL;
	}
	state = CLEAR;
}

void script::proc_colo()
{
	proc_end();
	bypass_branch();
}

void script::proc_quot()
{
	const char *str = pc;
	while (pc[0] != '\"' && pc[0] != '\0') pc++;
	if (pc[0] == '\0') throw ERR_STR_NOT_END;
	proc_str(string(str, pc-str));
	pc++;
}

void script::proc_dot()
{
	state = AFT_DOT;
}

void script::proc_note()
{
	while (!(pc[0] == '*' && pc[1] == '/') && pc[0] != '\0') pc++;
	if (pc[0] == '\0') throw ERR_NOTE_NOT_END;
	pc += 2;
}

void script::bypass_branch()
{
	int n;
	for (n = 0; (pc[0] != ':' && pc[0] != '$' && pc[0] != '\0') || n > 0; pc++) {
		if (pc[0] == '?') {
			n++;
		} else if (pc[0] == '$') {
			n--;
		}
	}
	if (pc[0] == '\0') throw ERR_BAD_BRANCH;
	pc++;
}

void script::exit_loop()
{
	int n;
	for (n = 1; pc[0] != '\0'; pc++) {
		if (pc[0] == '{') {
			n++;
		} else if (pc[0] == '}') {
			n--;
			if (n == 0) break;
		}
	}
	if (pc[0] == '\0') throw ERR_BAD_LOOP;
	pc++;
}

void script::add_spec_vars()
{
	string var;
	var = "@";
	ids[var] = data();
	ids[var].new_vec();
	var = "\\";
	ids[var] = data();
	ids[var].new_map();
}

void script::add_funs()
{
	unsigned int i;
	for (i = 0; i < sizeof(fun_table)/sizeof(struct fun_map); i++) {
		funs[fun_table[i].name] = fun_table[i].pfun;
	}
}
