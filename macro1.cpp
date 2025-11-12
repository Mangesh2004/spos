//INPUT :
// source.txt
/*
MACRO
INCR &A, &B
ADD &A, &B
MEND
START 100
INCR AREG, BREG
END
*/

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
	ifstream fin("source_2.txt");
	ofstream mntFile("mnt.txt"), mdtFile("mdt.txt"), interFile("intermediate.txt");

	if (!fin)
	{
		cout << "Error: cannot open input file.\n";
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
			macroName = word;
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

	// Write to MNT file
	mntFile << "MNT\n----------------------------\n";
	for (auto &m : MNT)
		mntFile << "Macro: " << m.name << " -> MDT Index: " << m.mdtIndex << "\n";

	// Write to MDT file
	mdtFile << "MDT\n----------------------------\n";
	for (int i = 0; i < (int)MDT.size(); i++)
		mdtFile << i << ": " << MDT[i] << "\n";

	// Write Intermediate file
	interFile << "Intermediate Code\n----------------------------\n";
	for (auto &line : INTERMEDIATE)
		interFile << line << "\n";

	cout << "\nPass 1 completed successfully!\n";
}

int main()
{
	cout << "------ PASS 1: MACRO PROCESSOR ------\n";
	pass1();
	return 0;
}


// OUTPUT :
// intermediate.txt
/*
Intermediate Code
----------------------------

START 100
INCR AREG, BREG
END
*/

// mdt.txt
/*
MDT
----------------------------
0: ADD &A, &B
1: MEND
*/

// mnt.txt
/*
MNT
----------------------------
Macro: INCR -> MDT Index: 0
*/