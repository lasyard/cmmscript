/* expression.cpp */

/* lasy */
/* 2004.12.12 */

#include <string>
#include <vector>
#include <map>
#include <stack>

using namespace std;

#include "error.h"
#include "data.h"
#include "operand.h"
#include "funs.h"
#include "expression.h"

const struct expression::op_info expression::ops[] = {
	{   0, 0, L2R, 0                      },
	{   1, 1, R2L, &expression::op_inc    },
	{   1, 1, R2L, &expression::op_dec    },
	{   1, 1, R2L, &expression::op_binv   },
	{   1, 1, R2L, &expression::op_pos    },
	{   1, 1, R2L, &expression::op_neg    },
	{   1, 1, R2L, &expression::op_not    },
	{   2, 2, L2R, &expression::op_mul    },
	{   2, 2, L2R, &expression::op_div    },
	{   2, 2, L2R, &expression::op_mod    },
	{   3, 2, L2R, &expression::op_add    },
	{   3, 2, L2R, &expression::op_sub    },
	{   4, 2, L2R, &expression::op_lshf   },
	{   4, 2, L2R, &expression::op_rshf   },
	{   5, 2, L2R, &expression::op_lt     },
	{   5, 2, L2R, &expression::op_gt     },
	{   5, 2, L2R, &expression::op_le     },
	{   5, 2, L2R, &expression::op_ge     },
	{   6, 2, L2R, &expression::op_eq     },
	{   6, 2, L2R, &expression::op_ne     },
	{   7, 2, L2R, &expression::op_band   },
	{   8, 2, L2R, &expression::op_bxor   },
	{   9, 2, L2R, &expression::op_bor    },
	{  10, 2, L2R, &expression::op_and    },
	{  11, 2, L2R, &expression::op_or     },
	{  12, 2, R2L, &expression::op_assign },
	{  12, 2, R2L, &expression::op_mulas  },
	{  12, 2, R2L, &expression::op_divas  },
	{  12, 2, R2L, &expression::op_modas  },
	{  12, 2, R2L, &expression::op_addas  },
	{  12, 2, R2L, &expression::op_subas  },
	{  12, 2, R2L, &expression::op_lshfas },
	{  12, 2, R2L, &expression::op_rshfas },
	{  12, 2, R2L, &expression::op_bandas },
	{  12, 2, R2L, &expression::op_bxoras },
	{  12, 2, R2L, &expression::op_boras  },
	{  12, 2, R2L, &expression::op_andas  },
	{  12, 2, R2L, &expression::op_oras   },
	{  99, 0, R2L, &expression::op_lpar   },
	{  99, 0, R2L, 0                      },
	{  99, 2, R2L, &expression::op_lbrk   },
	{  99, 0, R2L, 0                      },
	{  99, 0, R2L, &expression::op_comm   },
	{  99, 0, R2L, 0                      },
	{  99, 0, R2L, 0                      },
	{ 100, 0, R2L, 0                      }, // END
};

expression::expression()
{
	clear();
}

expression::~expression()
{
}

void expression::bind(script *sc)
{
	m_script = sc;
}

void expression::clear()
{
	while (!ostk.empty()) ostk.pop();
	while (!nstk.empty()) nstk.pop();
	while (!fstk.empty()) fstk.pop();
}

void expression::push(const operand &o)
{
	nstk.push(o);
}

void expression::push_op(enum op_code op)
{
	if (op == LPAR || op == LBRK) {
		ostk.push(op);
	} else if (op == FUNE) {
		pop_fun();
	} else {
		while (!ostk.empty() && (ops[ostk.top()].pri < ops[op].pri ||
		  (ops[ostk.top()].pri == ops[op].pri && ops[op].dir == L2R)))
		{
			pop_op();
		}
		if (op == COMM) {
			if (!ostk.empty() && (ostk.top() == FUN || ostk.top() == COMM)) {
				ostk.push(op);
			} else {
				do_lbrk();
			}
		} else if (op == RBRK) {
			do_lbrk();
			ostk.pop();
		} else if (op == RPAR) {
			if (!ostk.empty() && (ostk.top() == FUN || ostk.top() == COMM)) {
				pop_fun(true);
			} else {
				do_lpar();
				ostk.pop();
			}
		} else if (op != END) {
			ostk.push(op);
		}
	}
}

void expression::push_fun(fun_type *f)
{
	ostk.push(FUN);
	fstk.push(f);
}

const operand &expression::value()
{
	return nstk.top();
}

bool expression::empty() const
{
	return nstk.empty() && ostk.empty();
}

void expression::check_opees(enum op_code op)
{
	if (nstk.size() < (stack<operand>::size_type)ops[op].opees) throw ERR_LACK_OPEE;
}

void expression::pop_op()
{
	operand i;
	enum op_code op = ostk.top();
	check_opees(op);
	if (ops[op].opees == 2) {
		i = nstk.top();
		nstk.pop();
	}
	(this->*ops[op].op)(i);
	ostk.pop();
}

void expression::pop_fun(bool has_para)
{
	operand r(data(0));
	deque<operand> p;
	fun_type *f = fstk.top();
	if (has_para) {
		while (ostk.top() == COMM) {
			p.push_front(nstk.top());
			nstk.pop();
			if (ostk.top() == FUN) break;
			ostk.pop();
		}
		p.push_front(nstk.top());
		nstk.pop();
	}
	f(m_script, r, p);
	nstk.push(r);
	ostk.pop();
	fstk.pop();
}

void expression::op_inc(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()+1);
}

void expression::op_dec(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()-1);
}

void expression::op_binv(operand &i)
{
	nstk.top() = ~nstk.top();
}

void expression::op_pos(operand &i)
{
}

void expression::op_neg(operand &i)
{
	nstk.top() = -nstk.top();
}

void expression::op_not(operand &i)
{
	nstk.top() = !nstk.top();
}

void expression::op_mul(operand &i)
{
	nstk.top() = nstk.top()*i;
}

void expression::op_div(operand &i)
{
	if (i == 0) throw ERR_DIV_BY_ZERO;
	nstk.top() = nstk.top()/i;
}

void expression::op_mod(operand &i)
{
	if (i == 0) throw ERR_DIV_BY_ZERO;
	nstk.top() = nstk.top()%i;
}

void expression::op_add(operand &i)
{
	nstk.top() = nstk.top()+i;
}

void expression::op_sub(operand &i)
{
	nstk.top() = nstk.top()-i;
}

void expression::op_lshf(operand &i)
{
	nstk.top() = nstk.top() << i;
}

void expression::op_rshf(operand &i)
{
	nstk.top() = nstk.top() >> i;
}

void expression::op_lt(operand &i)
{
	nstk.top() = nstk.top() < i;
}

void expression::op_gt(operand &i)
{
	nstk.top() = nstk.top() > i;
}

void expression::op_le(operand &i)
{
	nstk.top() = nstk.top() <= i;
}

void expression::op_ge(operand &i)
{
	nstk.top() = nstk.top() >= i;
}

void expression::op_eq(operand &i)
{
	nstk.top() = nstk.top() == i;
}

void expression::op_ne(operand &i)
{
	nstk.top() = nstk.top() != i;
}

void expression::op_band(operand &i)
{
	nstk.top() = nstk.top() & i;
}

void expression::op_bxor(operand &i)
{
	nstk.top() = nstk.top() ^ i;
}

void expression::op_bor(operand &i)
{
	nstk.top() = nstk.top() | i;
}

void expression::op_and(operand &i)
{
	nstk.top() = nstk.top() && i;
}

void expression::op_or(operand &i)
{
	nstk.top() = nstk.top() || i;
}

void expression::op_assign(operand &i)
{
	nstk.top() = nstk.top().set_lv(i);
}

void expression::op_mulas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()*i);
}

void expression::op_divas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()/i);
}

void expression::op_modas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()%i);
}

void expression::op_addas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()+i);
}

void expression::op_subas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top()-i);
}

void expression::op_lshfas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() << i);
}

void expression::op_rshfas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() >> i);
}

void expression::op_bandas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() & i);
}

void expression::op_bxoras(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() ^ i);
}

void expression::op_boras(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() | i);
}

void expression::op_andas(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() && i);
}

void expression::op_oras(operand &i)
{
	nstk.top() = nstk.top().set_lv(nstk.top() || i);
}

void expression::op_lpar(operand &i)
{
	throw ERR_LACK_RPAR;
}

void expression::op_lbrk(operand &i)
{
	throw ERR_LACK_RBRK;
}

void expression::op_comm(operand &i)
{
	throw ERR_LACK_RPAR;
}

void expression::do_lpar()
{
	if (ostk.empty() || ostk.top() != LPAR) throw ERR_LACK_LPAR;
}

void expression::do_lbrk()
{
	if (ostk.empty() || ostk.top() != LBRK) throw ERR_LACK_LBRK;
	check_opees(LBRK);
	operand i = nstk.top();
	nstk.pop();
	nstk.top() = &nstk.top()[i];
}
