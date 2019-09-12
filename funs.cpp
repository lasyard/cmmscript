/* funs.cpp */

/* lasy */
/* 2004.12.12 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <stack>

using namespace std;

#include "error.h"
#include "data.h"
#include "operand.h"
#include "funs.h"
#include "expression.h"
#include "script.h"

void f__p(script *sc, operand &r, deque<operand> &p)
{
	deque<operand>::const_iterator i;
	for (i = p.begin(); i != p.end(); ++i) {
		cout << *i << " ";
	}
}

void f__pw(script *sc, operand &r, deque<operand> &p)
{
	deque<operand>::const_iterator i;
	int w = p.front();
	p.pop_front();
	for (i = p.begin(); i != p.end(); ++i) {
		cout << setw(w) << *i;
	}
}

void f__pln(script *sc, operand &r, deque<operand> &p)
{
	cout << endl;
}
