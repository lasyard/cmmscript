/* cmm.cpp */

/* lasy */
/* 2004.12.12 */

#include <iostream>
#include <fstream>
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

int main(void)
{
	script sc;
	string cmd;
	while (1) {
		cout << ": ";
		getline(cin, cmd);
		if (cmd == "exit") {
			break;
		} else if (cmd == "reset") {
			sc.reset();
		} else if (cmd == "ids") {
			sc.print_ids();
		} else if (cmd == "funs") {
			sc.print_funs();
		} else if (cmd.substr(0, 4) == "run ") {
			ifstream file((cmd.substr(4)+".cmm").c_str());
			if (file.is_open()) {
				file.seekg(0, ios::end);
				int len = file.tellg();
				file.seekg(0, ios::beg);
				char *buf = new char[len+1];
				file.read(buf, len);
				buf[file.gcount()] = '\0';
				file.close();
				sc.exec(buf);
				delete[] buf;
			} else {
				cerr << "Cannot open file " << cmd.substr(4)+".cmm" << endl;
			}
		} else {
			sc.exec(cmd.c_str());
		}
	}
	return 0;
}
