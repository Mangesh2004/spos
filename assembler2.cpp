// Input :
// intermediate.txt
/*
100	START	100
100	(IS,04)	AREG,,	A
101	(IS,01)	BREG,,	B
102	(DL,01)	(C,2)
103	(DL,02)	(C,5)
108	(DL,01)	(C,1)
109	END
*/

// symtab.txt
/*
A	102
B	103
C	108
*/

// pass2.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
using namespace std;

int main()
{
	ifstream fin1("intermediate.txt");
	ifstream fin2("symtab.txt");
	ofstream fout("machinecode.txt");

	if (!fin1 || !fin2)
	{
		cout << "Error: intermediate.txt or symtab.txt not found!\n";
		return 0;
	}

	// --- Step 1: Load Symbol Table ---
	map<string, int> symtab;
	string sym;
	int addr;
	while (fin2 >> sym >> addr)
	{
		symtab[sym] = addr;
	}

	// --- Step 2: Process Intermediate Code ---
	string line;
	while (getline(fin1, line))
	{
		if (line.empty())
			continue;

		string lc; // location counter
		stringstream ss(line);
		ss >> lc; // first token is LC

		// Skip START and END lines (don’t write them in output)
		if (line.find("START") != string::npos || line.find("END") != string::npos)
			continue;

		// --- Declarative statements (DL) ---
		if (line.find("(DL,") != string::npos)
		{
			size_t pos = line.find("(C,");
			if (pos != string::npos)
			{
				string val = line.substr(pos + 3);
				if (!val.empty() && val.back() == ')')
					val.pop_back();
				fout << lc << "\t00 00 " << val << endl;
			}
			continue;
		}

		// --- Imperative statements (IS) ---
		if (line.find("(IS,") != string::npos)
		{
			string opcodePart, reg, symb;
			ss >> opcodePart >> reg >> symb;

			// Extract opcode number from (IS,XX)
			string opcodeNum = opcodePart.substr(4, 2);

			// Register codes
			string regcode;
			if (reg.find("AREG") != string::npos)
				regcode = "01";
			else if (reg.find("BREG") != string::npos)
				regcode = "02";
			else if (reg.find("CREG") != string::npos)
				regcode = "03";
			else
				regcode = "00";

			// Get symbol address
			int symaddr = 0;
			if (symtab.find(symb) != symtab.end())
				symaddr = symtab[symb];

			fout << lc << "\t" << opcodeNum << " " << regcode << " " << symaddr << endl;
		}
	}

	cout << "\nPASS 2 completed successfully.\n";
	cout << "Check 'machinecode.txt' for final machine code.\n";

	fin1.close();
	fin2.close();
	fout.close();
	return 0;
}
// OUTPUT :

// machinecode.txt
/*
100	04 01 102
101	01 02 103
102	00 00 2
103	00 00 5
108	00 00 1
*/