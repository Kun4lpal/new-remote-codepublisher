///////////////////////////////////////////////////////////////////////////
// TypeTable.cpp   -   This file contains the implementation for all the // 
// ver 1.0             methods declard in the header file.               //           
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*  Provides a container class that stores type information needed for dependency analysis.
*
//
//* Public Interface (see test stub for details):
* =============================================
*
* setValue();             // this method adds a key and a value to the TypeTable.
* setFromInstance();      // this method restores the TypeTable instance from an old value.
* getTypeTable();         // this methods returns the instance after completing analysis
*
*/
/* Build Process
*  =============
*  This class makes use of the standard unordered_map data structure.
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
//
#include "TypeTable.h"
#include <string>
#include <vector>

//<----------------------function implementation------------------------------->

TypeTable::TypeTable()
{   //constructor
}

//<----------------------adds a key value pair to typeTable-------------------------->

void TypeTable::setValue(std::string key, std::string type, std::string filename, std::string nameSpace)
{
	if (typeTable.find(key) == typeTable.end())
	{
		TypeRecord *tr = new TypeRecord;
		tr->setTypeName(type);
		std::vector<std::pair<std::string, std::string>> _pair;
		std::pair<std::string, std::string> pair = std::make_pair(filename, nameSpace);
		_pair.push_back(pair);
		tr->setFilePairs(_pair);
		typeTable.emplace(key, *tr);
	}
	else {
		TypeRecord *typeStore = &typeTable[key + " constructor"];
		//TypeRecord *typeStore = &typeTable[key];
		typeStore->setTypeName(type);
		std::vector<std::pair<std::string, std::string>> _pair = typeStore->getPairs();
		std::pair<std::string, std::string> pair = std::make_pair(filename, nameSpace);
		_pair.push_back(pair);
		typeStore->setFilePairs(_pair);
	}
}

//<----------------restores the typetable to instance defined in the argument-------------------->

void TypeTable::setFromInstance(std::unordered_map<std::string, TypeRecord> TT)
{
	typeTable = TT;
}

//<----------------------this funcion returns the typeTable instance------------------------------->

std::unordered_map<std::string, TypeRecord> TypeTable::getTypeTable()
{
	return typeTable;
}

//<----------------------TEST STUB for typeTable------------------------------->

#ifdef TEST_TYPE_TABLE
int main() {
	TypeTable tt;
	tt.setValue("dothis","function","A.cpp","globalNamespace");
	tt.setValue("Property", "class", "A.h", "globalNamespace");
	tt.setValue("B", "Namespace", "B.cpp", "globalNamespace");
	tt.setValue("dothat", "function", "D.h", "D");
}
#endif // TEST_TYPE_TABLE

