//INPUT :

// intermediate.txt
/*
Intermediate Code
----------------------------

START 100
INCR AREG, BREG
END
*/

// mnt.txt
/*
MNT
----------------------------
Macro: INCR -> MDT Index: 0
*/

// mdt.txt
/*
MDT
----------------------------
0: ADD &A, &B
1: MEND
*/


// macro_pass2.cpp

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

void loadMNT()
{
	ifstream mntFile("mnt.txt");
	if (!mntFile)
	{
		cout << "Error: cannot open mnt.txt\n";
		return;
	}

	string line;
	while (getline(mntFile, line))
	{
		if (line.find("Macro:") != string::npos)
		{
			string name;
			int index;
			string temp;
			stringstream ss(line);
			ss >> temp >> name >> temp >> temp >> index; // "Macro: NAME -> MDT Index: INDEX"
			MNT.push_back({name, index});
		}
	}
	mntFile.close();
}

void loadMDT()
{
	ifstream mdtFile("mdt.txt");
	if (!mdtFile)
	{
		cout << "Error: cannot open mdt.txt\n";
		return;
	}

	string line;
	while (getline(mdtFile, line))
	{
		if (line.find(":") != string::npos)
		{
			string instr = line.substr(line.find(":") + 2);
			if (instr != "MDT" && instr != "----------------------------")
				MDT.push_back(instr);
		}
	}
	mdtFile.close();
}

void pass2()
{
	ifstream interFile("intermediate.txt");
	ofstream expanded("expanded.txt"); // <-- Output file changed here

	if (!interFile)
	{
		cout << "Error: cannot open intermediate.txt\n";
		return;
	}

	loadMNT();
	loadMDT();

	cout << "\n------ PASS 2: MACRO EXPANSION ------\n";
	cout << "Expanded Code:\n--------------------------------\n";

	string line, word;
	while (getline(interFile, line))
	{
		// Skip header lines in intermediate.txt
		if (line.find("Intermediate") != string::npos || line.find("-") != string::npos)
			continue;

		stringstream ss(line);
		ss >> word;
		bool expandedFlag = false;

		for (auto &m : MNT)
		{
			if (word == m.name)
			{
				expandedFlag = true;
				int i = m.mdtIndex;
				while (i < (int)MDT.size() && MDT[i] != "MEND")
				{
					cout << MDT[i] << "\n";
					expanded << MDT[i] << "\n";
					i++;
				}
				break;
			}
		}

		if (!expandedFlag && !line.empty())
		{
			cout << line << "\n";
			expanded << line << "\n";
		}
	}

	cout << "\nExpanded code successfully written to expanded.txt\n";

	interFile.close();
	expanded.close();
}

int main()
{
	pass2();
	return 0;
}



//OUTPUT :

//expanded.txt
/*
START 100
ADD &A, &B
END

*/
