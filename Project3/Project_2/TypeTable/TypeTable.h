#pragma once
///////////////////////////////////////////////////////////////////////////
// TypeTable.h   -   Data structure used to store typeName, type and     // 
// ver 1.0           parent Namespace and fileName.                      //           
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
* class TypeTable {.....}
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
#include "TypeRecord.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "../TypeTable/TypeRecord.h"
#include "../CodeAnalyzer/Parser/ConfigureParser.h"
#include "../CodeAnalyzer/Parser/ActionsAndRules.h"
#include "../CodeAnalyzer/Parser/Parser.h"

//<--------------------------------------using statements------------------------------->

using namespace CodeAnalysis;

class TypeTable
{
	//<----------------------function defintions------------------------------->
public:
	TypeTable();
	void setValue(std::string key, std::string type, std::string filename, std::string nameSpace);
	void setFromInstance(std::unordered_map<std::string, TypeRecord> TT);
	std::unordered_map<std::string, TypeRecord> getTypeTable();
	
	//<----------------------private data members------------------------------->
private:
	std::unordered_map<std::string, TypeRecord> typeTable;
};

