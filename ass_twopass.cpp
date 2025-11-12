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
#include <sstream>
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

// --- Pass 1 from assembler1.cpp ---
void runPass1()
{
	ifstream fin("input.txt");
	ofstream fout("intermediate.txt");
	ofstream fsym("symtab.txt");

	if (!fin)
	{
		cout << "Error: input.txt not found!\n";
		return;
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
}

// INPUT to Pass 2:
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

// --- Pass 2 from assembler2.cpp ---
void runPass2()
{
	ifstream fin1("intermediate.txt");
	ifstream fin2("symtab.txt");
	ofstream fout("machinecode.txt");

	if (!fin1 || !fin2)
	{
		cout << "Error: intermediate.txt or symtab.txt not found!\n";
		return;
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
			stringstream ss2(line);
			ss2 >> lc >> opcodePart >> reg >> symb;

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
}

int main()
{
	runPass1();
	runPass2();
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

// machinecode.txt
/*
100	04 01 102
101	01 02 103
102	00 00 2
103	00 00 5
108	00 00 1
*/

