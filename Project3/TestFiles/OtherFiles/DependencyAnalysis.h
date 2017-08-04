#ifndef DEP_ANAL
#define DEP_ANAL
///////////////////////////////////////////////////////////////////////////
// DependencyAnalysis.h   -  Analyzes files and returns their dependency //
// ver 1.0                    information                                //
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module does analysis of a set of files and gives back their dependency 
* information. Finds, for each file in a specified file collection, all other
* files from the file collection on which they depend. File A depends on file B,
* if and only if, it uses the name of any type or global function or global data
* defined in file B. It might do that by calling a function or method of a type 
* or by inheriting the type. Note that this intentionally does not record dependencies
* of a file on files outside the file set, e.g., language and platform libraries.
*
*   Namespaces: 
*   GraphLibrary belongs to Graph package which contains the Graph class
*   used to create a graph instance from the dependency information obtained.
*   
*   Scanner belongs to Parse package which contains the CodeAnalyzer classes
*   from which we obtain AST.
*   
*   Utilties has been included for display purposes in the test stub of dependency
*   analysis.
*
//
//* Public Interface (see test stub for details):
* =============================================
*
* class DependencyAnalysis {.....}
*
* intitializeDependencyAnalysis();      // this method kickstarts the analysis.
* storeDependencyInformation();         // this method stores the dependency information in a NOSQLDB
* createGraph();                        // this method is used to create a graph from the NOSQLDB
* getGraph();                           // returns graph containing the dependency information
* 
*/
/* Build Process
*  =============
*  Required Files: (Both direct and indirect besides implementation file .cpp)
* - TypeTable.h, TypeTable.cpp           // always required
* - TypeRecord.h, TypeRecord.cpp         // always required
* - TypeAnalysis.h, TypeAnalysis.cpp     // always required
* - Graph.h, Graph.cpp                   // always required
* - Tokenizer.h, Tokenizer.cpp           // always required
* - NoSqlDb.h, NoSqlDb.cpp               // always required
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
//
#pragma once
#include<conio.h>
#include<iomanip>
#include<iostream>
#include <unordered_map>
#include<vector>
#include "../TypeTable/TypeTable.h"
#include "../TypeTable/TypeRecord.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../Graph/Graph/Graph.h"
#include "../CodeAnalyzer/Tokenizer/Tokenizer.h"
#include "../Proj_1/Proj_1/NoSqlDb.h"

using namespace GraphLib;
using namespace Scanner;
using namespace Utilities;
using Util = StringHelper;
using Keys = std::vector<std::string>;
using Key = std::string;

class DependencyAnalysis
{
	//---------------------------------------< Member functions >-----------------------------------
public:
	DependencyAnalysis(std::vector<std::string> vec) : vec_(vec) {};  // constructor definition
	std::vector<std::string> intializeDepAnalysis(std::string fs, std::unordered_map<std::string, TypeRecord> _typeTable);
	void addUsings(std::vector<std::string> dep, std::unordered_map<std::string, TypeRecord> &_typeTableR, std::string fs, std::unordered_map<std::string, bool> dependencies);
	NoSqlDb<std::string> storeDependencyInfo(std::unordered_map<std::string, TypeRecord> _typeTable,std::string currentTime);
	void createGraph(NoSqlDb<std::string> db);// or we can use NoSqlDb
	Graph<std::string, std::string> getGraph();

private:
	//---------------------------------------< Private Data Members >-------------------------------
	std::vector<std::string> vec_;
	Graph<std::string, std::string> g_;
	Toker toker;
};
#endif // !DEP_ANAL


