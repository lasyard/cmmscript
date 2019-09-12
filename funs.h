/* funs.h */

/* lasy */
/* 2004.12.12 */

#ifndef _FUNS_H_
#define _FUNS_H_

class operand;
class script;

typedef void fun_type(script *sc, operand &r, deque<operand> &p);

void f__p(script *sc, operand &r, deque<operand> &p);
void f__pw(script *sc, operand &r, deque<operand> &p);
void f__pln(script *sc, operand &r, deque<operand> &p);

#endif
