/* operand.cpp */

/* lasy */
/* 2004.12.12 */

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "error.h"
#include "data.h"
#include "operand.h"

operand::operand(): type(NO), pdata(0)
{
}

operand::operand(const operand &obj)
{
	copy(obj);
}

operand::operand(const data &d): type(RV), pdata(new data(d))
{
}

operand::operand(data *d): type(LV), pdata(d)
{
}

operand::~operand()
{
	release();
}

operand::operator int()
{
	return int(*pdata);
}

operand::operator const data &()
{
	return (const data &)(*pdata);
}

const operand &operand::operator =(const operand &obj)
{
	release();
	copy(obj);
	return *this;
}

const operand &operand::operator =(const data &d)
{
	release();
	type = RV;
	pdata = new data(d);
	return *this;
}

const operand &operand::operator =(data *d)
{
	release();
	type = LV;
	pdata = d;
	return *this;
}

data &operand::operator [](const operand &o)
{
	if (o.pdata->is<INT>()) {
		return (*pdata)[int(*o.pdata)];
	} else if (o.pdata->is<STR>()) {
		return (*pdata)[(const string &)(*o.pdata)];
	} else {
		throw ERR_BAD_INDEX;
	}
}

const data &operand::set_lv(const data &d)
{
	if (type != LV) throw ERR_NOT_LV;
	*pdata = d;
	return *pdata;
}

bool operand::is_true() const
{
	return pdata->is_true();
}

void operand::copy(const operand &obj)
{
	type = obj.type;
	if (type == LV) {
		pdata = obj.pdata;
	} else if (type == RV) {
		pdata = new data(*obj.pdata);
	}
}

void operand::release()
{
	if (type == RV) delete pdata;
}

ostream &operator <<(ostream &os, const operand &obj)
{
	os << *obj.pdata;
	return os;
}
