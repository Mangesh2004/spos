// input.txt
/*
START 100
** MOVER AREG, A
** ADD BREG, B
A DC 2
B DS 5
C DC 1
END
*/

// pass1st.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct Symbol
{
	string name;
	int addr;
};

vector<Symbol> symtab;

// Instruction set table
map<string, string> MOT = {
	{"STOP", "00"},
	{"ADD", "01"},
	{"SUB", "02"},
	{"MULT", "03"},
	{"MOVER", "04"},
	{"MOVEM", "05"},
	{"COMP", "06"},
	{"BC", "07"},
	{"DIV", "08"},
	{"READ", "09"},
	{"PRINT", "10"}};

void addSymbol(string label, int lc)
{
	for (auto &s : symtab)
		if (s.name == label)
			return; // avoid duplicates
	symtab.push_back({label, lc});
}

int main()
{
	ifstream fin("input.txt");
	ofstream fout("intermediate.txt");
	ofstream fsym("symtab.txt");

	if (!fin)
	{
		cout << "Error: input.txt not found!\n";
		return 0;
	}

	string label, opcode, op1, op2;
	int lc = 0, startAddr = 0;

	fin >> opcode;
	if (opcode == "START")
	{
		fin >> startAddr;
		lc = startAddr;
		fout << lc << "\tSTART\t" << startAddr << "\n";
	}

	while (fin >> label)
	{
		if (label == "END")
		{
			fout << lc << "\tEND\n";
			break;
		}

		fin >> opcode;

		if (opcode == "DS")
		{
			fin >> op1;
			addSymbol(label, lc);
			fout << lc << "\t(DL,02)\t(C," << op1 << ")\n";
			lc += stoi(op1);
		}
		else if (opcode == "DC")
		{
			fin >> op1;
			addSymbol(label, lc);
			fout << lc << "\t(DL,01)\t(C," << op1 << ")\n";
			lc++;
		}
		else
		{
			// IS Instructions (imperative statements)
			fin >> op1 >> op2;
			if (label != "")
				addSymbol(label, lc);

			string code = MOT[opcode];
			fout << lc << "\t(IS," << code << ")\t" << op1 << ",\t" << op2 << "\n";
			lc++;
		}
	}

	// Write symbol table
	for (auto &s : symtab)
		fsym << s.name << "\t" << s.addr << "\n";

	cout << "\nPASS 1 completed successfully.\n";
	cout << "Generated: intermediate.txt and symtab.txt\n";

	fin.close();
	fout.close();
	fsym.close();
	return 0;
}

// OUTPUT :

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