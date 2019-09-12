/* operand.h */

/* lasy */
/* 2004.12.12 */

#ifndef _OPERAND_H_
#define _OPERAND_H_

class data;

enum operand_type { NO, LV, RV };

class operand {
public:
	operand();
	operand(const operand &obj);
	operand(const data &d);
	operand(data *d);
	virtual ~operand();

	operator int();
	operator const data &();
	const operand &operator =(const operand &obj);
	const operand &operator =(const data &d);
	const operand &operator =(data *d);
	data &operator [](const operand &o);

	const data &set_lv(const data &d);
	bool is_true() const;
protected:
	enum operand_type type;
	data *pdata;

	void copy(const operand &obj);
	void release();

	friend ostream &operator <<(ostream &os, const operand &obj);
};

#endif
