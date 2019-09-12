/* data.cpp */

/* lasy */
/* 2004.12.12 */

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

#include "error.h"
#include "data.h"

const char *const data::type_error[] = {
	0,
	ERR_NOT_INT,
	ERR_NOT_STR,
	ERR_NOT_VEC,
	ERR_NOT_MAP
};

data::data(): type(UNDEF)
{
}

data::data(int v): type(INT)
{
	value = v;
}

data::data(const string &s): type(STR)
{
	pstr = new string(s);
}

data::data(const data &obj)
{
	copy(obj);
}

data::~data()
{
	release();
}

data::operator int() const
{
	assert<INT>();
	return value;
}

data::operator const string &() const
{
	assert<STR>();
	return *pstr;
}

const data &data::operator =(const data &obj)
{
	release();
	copy(obj);
	return *this;
}

data &data::operator [](int i)
{
	assert<VEC>();
	while (pvec->size() <= (vector<data>::size_type)i) pvec->push_back(0);
	return (*pvec)[i];
}

data &data::operator [](const string &name)
{
	assert<MAP>();
	return (*pmap)[name];
}

void data::new_vec()
{
	release();
	type = VEC;
	pvec = new vector<data>;
}

void data::new_map()
{
	release();
	type = MAP;
	pmap = new map<string, data>;
}

bool data::is_true() const
{
	if (type == UNDEF) return false;
	if (type == INT && value == 0) return false;
	return true;
}

bool data::exists(const string &name) const
{
	if (type != MAP) return false;
	if (pmap->find(name) == pmap->end()) return false;
	return true;
}

void data::assert_not_undef() const
{
	if (type == UNDEF) throw ERR_UNDEF;
}

void data::copy(const data &obj)
{
	type = obj.type;
	if (type == INT) {
		value = obj.value;
	} else if (type == STR) {
		pstr = new string(*obj.pstr);
	} else if (type == VEC) {
		pvec = new vector<data>(*obj.pvec);
	} else if (type == MAP) {
		pmap = new map<string, data>(*obj.pmap);
	}
}

void data::release()
{
	if (type == STR) {
		delete pstr;
	} else if (type == VEC) {
		delete pvec;
	} else if (type == MAP) {
		delete pmap;
	}
}

ostream &operator <<(ostream &os, const data &obj)
{
	if (obj.type == UNDEF) {
		os << "UNDEF";
	} else if (obj.type == INT) {
		os << obj.value;
	} else if (obj.type == STR) {
		os << obj.pstr->c_str();
	} else if (obj.type == VEC) {
		vector<data>::const_iterator i;
		os << "VEC(";
		for (i = obj.pvec->begin(); i != obj.pvec->end(); ++i) {
			os << *i << ", ";
		}
		os << ")";
	} else if (obj.type == MAP) {
		map<string, data>::const_iterator i;
		os << "MAP(";
		for (i = obj.pmap->begin(); i != obj.pmap->end(); ++i) {
			os << i->first.c_str() << ": " << i->second << ", ";
		}
		os << ")";
	}
	return os;
}
