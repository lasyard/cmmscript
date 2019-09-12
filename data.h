/* data.h */

/* lasy */
/* 2004.12.12 */

#ifndef _DATA_H_
#define _DATA_H_

enum data_type { UNDEF, INT, STR, VEC, MAP };

class data {
public:
	data();
	data(int v);
	data(const string &s);
	data(const data &obj);
	virtual ~data();

	operator int() const;
	operator const string &() const;
	const data &operator =(const data &obj);
	data &operator [](int i);
	data &operator [](const string &name);

	void new_vec();
	void new_map();
	bool is_true() const;
	bool exists(const string &name) const;

	template<enum data_type t> bool is() const {
		return type == t;
	}
protected:
	static const char *const type_error[];

	enum data_type type;
	union {
		int value;
		string *pstr;
		vector<data> *pvec;
		map<string, data> *pmap;
	};

	void assert_not_undef() const;

	template<enum data_type t> void assert() const {
		assert_not_undef();
		if (type != t) throw type_error[t];
	}

	void copy(const data &obj);
	void release();

	friend ostream& operator <<(ostream &os, const data &obj);
};

#endif
