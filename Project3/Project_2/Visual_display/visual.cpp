///////////////////////////////////////////////////////////////////////////
// Visual.cpp   -  This package provides a class which is used to display//
// ver 1.0        the information inside the TypeTable                   //
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* Uses information in the Analyzer's AST and TypeTable to build an effective display
* of the dependency relationships between all files in the selected file set.
*
//
//* Public Interface (see test stub for details):
* =============================================
* displayHeader()                       // this displays the header of the TABLE
* displayMetricsLine                    // this displays all the information contained inside
*/
/* Build Process
*  =============
*  Required Files: (Both direct and indirect besides implementation file .cpp)
* - TypeTable.h, TypeTable.cpp           // always required
* - TypeRecord.h, TypeRecord.cpp         // always required
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
//
#include "visual.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <unordered_map>

//<----------------------function implementation------------------------------->

// A header display for demonstration purposes
void visual::displayHeader()
{
	std::ostringstream out;
	out << std::right;
	out << "\n ";
	out << std::setw(25) << "TypeName";
	out << std::setw(40) << "Type";
	out << std::setw(40) << "FileName";
	out << std::setw(40) << "Parent namespace";
	out << std::right;
	out << "\n  ";
	out << std::setw(25) << "-----------------------";
	out << std::setw(40) << "----------------------";
	out << std::setw(40) << "-------------------------------";
	out << std::setw(40) << "-----------------------";
	std::cout << out.str();

}
//-----------------< displays the key value pairs stored in typeTable >-------------------

void visual::displayMetricsLine(std::unordered_map<std::string, TypeRecord> typeTable)
{
	std::function<std::string(std::string, size_t)> trunc = [](std::string in, size_t count)
	{
		return in.substr(0, count);
	};
	std::ostringstream out;
	bool value = true;
	for (auto it : typeTable) {
		if (it.second.getTypeName() != "MemberFunction") {
			std::vector<std::pair<std::string, std::string>> output = it.second.getPairs();

			for (auto xd : output) {
				std::string temp = xd.first;
				out << std::right;
				out << "\n ";
				out << std::setw(25) << trunc(it.first, 23);
				out << std::setw(40) << it.second.getTypeName();
				out << std::setw(40) << temp.substr(temp.find_last_of("/\\") + 1);
				out << std::setw(40) << xd.second;

			}
		}
	}
	std::cout << out.str();
}

//<----------------------------TEST STUB for visual------------------------------->

#ifdef TEST_VISUAL

int main() {
	std::cout << "\nTesting visual package:\n";
	TypeTable tt;
	visual v;
	tt.setValue("dothis", "function", "A.cpp", "globalNamespace");
	tt.setValue("Property", "class", "A.h", "globalNamespace");
	tt.setValue("B", "Namespace", "B.cpp", "globalNamespace");
	tt.setValue("dothat", "function", "D.h", "D");
	v.displayHeader();
	v.displayMetricsLine(tt.getTypeTable());
}
#endif // TEST_VISUAL
