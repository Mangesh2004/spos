// INPUT
// source.txt
/*
MACRO
INCR
MOVER AREG, DATA
ADD AREG, ONE
MOVEM AREG, DATA
MEND

START 100
READ DATA
INCR
PRINT DATA
STOP
DATA DS 1
ONE DC 1
END
*/

// macro_combined.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct MNTEntry
{
	string name;
	int mdtIndex;
};

vector<MNTEntry> MNT;
vector<string> MDT;
vector<string> INTERMEDIATE;

void pass1()
{
	ifstream fin("source.txt");
	ofstream mntFile("mnt.txt"), mdtFile("mdt.txt"), interFile("intermediate.txt");

	if (!fin)
	{
		cout << "Error: cannot open source.txt file\n";
		return;
	}

	string line, word;
	bool inMacro = false;
	string macroName;

	while (getline(fin, line))
	{
		stringstream ss(line);
		ss >> word;

		if (word == "MACRO")
		{
			inMacro = true;
			continue;
		}

		if (inMacro && macroName.empty())
		{
			macroName = word; // macro name
			MNT.push_back({macroName, (int)MDT.size()});
			continue;
		}

		if (inMacro)
		{
			if (word == "MEND")
			{
				MDT.push_back("MEND");
				macroName.clear();
				inMacro = false;
			}
			else
			{
				MDT.push_back(line);
			}
		}
		else
		{
			INTERMEDIATE.push_back(line);
		}
	}

	// Write MNT
	mntFile << "MNT\n----------------------------\n";
	for (auto &m : MNT)
		mntFile << "Macro: " << m.name << " -> MDT Index: " << m.mdtIndex << "\n";

	// Write MDT
	mdtFile << "MDT\n------------------------------\n";
	for (int i = 0; i < (int)MDT.size(); i++)
		mdtFile << i << ": " << MDT[i] << "\n";

	// Write Intermediate file
	interFile << "Intermediate code\n---------------------------\n";
	for (auto &line : INTERMEDIATE)
		interFile << line << "\n";

	cout << "\nPass 1 completed successfully!\n";
}

void pass2()
{
	ifstream interFile("intermediate.txt");
	ofstream expandedFile("expanded.txt");

	if (!interFile)
	{
		cout << "Error: cannot open intermediate.txt\n";
		return;
	}

	cout << "\nPass 2: Expanded Code\n----------------------\n";
	expandedFile << "Expanded Code\n----------------------\n";

	string line, word;

	// Skip header lines in intermediate.txt
	getline(interFile, line); // skip "Intermediate code"
	getline(interFile, line); // skip dashed line

	while (getline(interFile, line))
	{
		stringstream ss(line);
		ss >> word;

		bool expanded = false;
		for (auto &m : MNT)
		{
			if (word == m.name)
			{
				expanded = true;
				int i = m.mdtIndex;
				while (MDT[i] != "MEND")
				{
					cout << MDT[i] << "\n";
					expandedFile << MDT[i] << "\n";
					i++;
				}
				break;
			}
		}

		if (!expanded)
		{
			cout << line << "\n";
			expandedFile << line << "\n";
		}
	}

	cout << "\nPass 2 completed successfully! Expanded code written to expanded.txt\n";
}

int main()
{
	cout << "------ TWO PASS MACRO PROCESSOR ------\n";
	pass1();
	pass2();
	return 0;
}

// OUTPUT :

// mnt.txt

/*
MNT
----------------------------
Macro: INCR -> MDT Index: 0
*/

// mdt.txt

/*
MDT
------------------------------
0: MOVER AREG, DATA
1: ADD AREG, ONE
2: MOVEM AREG, DATA
3: MEND
*/

//intermediate.txt

/*
Intermediate code
---------------------------

START 100
READ DATA
INCR
PRINT DATA
STOP
DATA DS 1
ONE DC 1
END
*/


// expanded.txt

/*
Expanded Code
----------------------

START 100
READ DATA
MOVER AREG, DATA
ADD AREG, ONE
MOVEM AREG, DATA
PRINT DATA
STOP
DATA DS 1
ONE DC 1
END

*/
