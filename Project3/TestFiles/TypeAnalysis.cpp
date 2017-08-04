////////////////////////////////////////////////////////////////////////////
// TypeAnalysis.cpp   - This file contains the implementation for all the //
// ver 1.0             methods declared in the TypeAnalysis.h file        //
//                                                                        //
// Language:      Visual C++.                                             //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design    //
// Author:        Kunal Paliwal, Syracyse University, CST 687             //
//                kupaliwa@syr.edu, (315) 876 8002                        //
////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*   Finds all the types and global functions defined in each of a collection of C++
*   source files. It does this by building rules to detect:
*
*   1) type definitions - classes, structs, enums, typedefs, and aliases.
*   2) global function definitions
*   3) global data definitions
*      and capture their fully qualified names and the files where they are defined.
*   4) It uses the TypeTable package to store that information.
*
//
//* Public Interface (see test stub for details):
* =============================================
* fillTypeTable();      // this method kickstarts the typeAnalysis and calls DFS.
* DFS();                // this is a simple treewalk algorithm and we fill the typeTable
*                       // with information we need for typeAnalysis as we walk through the tree.
* getMainTable();       // this method returns the TypeTable containing the typeinformation
*
*/
/* Build Process
*  =============
*  Required Files: (Both direct and indirect)
* - TypeTable.h, TypeTable.cpp           // always required
* - TypeRecord.h, TypeRecord.cpp         // always required
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
#include "TypeAnalysis.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "../CodeAnalyzer/Utilities/Utilities.h"
#include <vector>

using namespace CodeAnalysis;
using namespace Utilities;
using Util = StringHelper;

//<------------this function fills the main typetable by using traversal----------------->

void TypeAnalysis::filltypeTable(Repository * repo)
{
	ASTNode* node = repo->getGlobalScope();
	std::string namespace_ = node->name_;
	node->nameSpac = namespace_;
	DFS(node, namespace_, node->type_);
}

//<---------------this is an alternative treewalk alogrithm using recursion----------------->

void TypeAnalysis::DFS(ASTNode* pNode, std::string namespc, std::string parentType) {
	std::string change = namespc;
	std::string type = parentType;
	if (pNode->type_ == "namespace") {
		change = pNode->name_;
	}
	if (pNode->type_ == "class" || pNode->type_ == "enum" || pNode->type_ == "namespace" || pNode->type_ == "struct") {
		type = pNode->type_;
		TT.setValue(pNode->name_, pNode->type_, pNode->path_, namespc);
	}
	if (pNode->type_ == "function" && pNode->name_ != "main" && type == "namespace") {
		TT.setValue(pNode->name_, "globalFunction", pNode->path_, namespc);
	}
	if (pNode->type_ == "function" && type == "class") {
		TT.setValue(pNode->name_, "MemberFunction", pNode->path_, namespc);
	}
	if (pNode->type_ == "function" && type == "struct") {
		TT.setValue(pNode->name_, "MemberFunction", pNode->path_, namespc);
	}
	checkDeclarations(pNode,namespc);
	for (auto pChild : pNode->children_) {
		DFS(pChild, change, type);
	}type = pNode->type_;
}

//<-------------------------this function checks for declarations---------------------->

void TypeAnalysis::checkDeclarations(ASTNode* pNode, std::string namespc) {
	for (auto it : pNode->decl_) {
		if (it.declType_ == 0 && it.pTc->show().substr(0, 7) == "typedef") {
			const Scanner::ITokCollection* pTc = it.pTc;
			std::string name = (*pTc)[pTc->find(";") - 1];
			TT.setValue(name, "TypeDef", it.path, namespc);
		}
		else if (it.declType_ == 3) {
			const Scanner::ITokCollection* pTc = it.pTc;
			if ((*pTc)[pTc->find("using")] == "using") {
				std::string name = (*pTc)[pTc->find(";") - 1];
				TT.setValue("Using " + name, "Using Namespace", it.path, namespc);
			}
		}
	}
}

//<-------------------------this function returns main typetable---------------------->

TypeTable TypeAnalysis::getMainTable()
{
	return TT;
}

//<-------------------------this function performs type analysis for single or multiple files---------------------->

Repository* TypeAnalysis::doTypeAnalysis(std::vector<std::string> files) {
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	for (auto it : files) {
		try
		{
			if (pParser) {
				if (!configure.Attach(it)) {
					std::cout << "\n  could not open file " << it << std::endl;
					continue;
				}
			}
			while (pParser->next())
				pParser->parse();
			std::cout << "\n";
		}
		catch (std::exception& ex) {
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
	Repository* pRepo = Repository::getInstance();
	return pRepo;
}

//<-------------------------------------TEST STUB for type analysis------------------------------>
#ifdef TEST_TYPEANALYSIS


int main() {
	std::cout << "\nDepAnalyserTest\n" << std::endl;
	TypeAnalysis demo;
	std::vector<std::string> vec;
	vec.push_back("../TestFiles/A.h");
	vec.push_back("../TestFiles/B.h");
	vec.push_back("../TestFiles/A.cpp");
	vec.push_back("../TestFiles/B.cpp");
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	Repository* pRepo = Repository::getInstance();
	for (auto it : vec) {
		try
		{if (pParser){
				if (!configure.Attach(it)){
					std::cout << "\n  could not open file " << it << std::endl;
					continue;}}
			else{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;}
			while (pParser->next())
				pParser->parse();
			std::cout << "\n";
			pRepo->package() = FileSystem::Path::getName(it);
			pRepo->currentPath() = it;
		}
		catch (std::exception& ex){
			std::cout << "\n\n    " << ex.what() << "\n\n";}}
	
	demo.filltypeTable(pRepo);
	TypeTable tt = demo.getMainTable();
	visual V;
	V.displayHeader();
	V.displayMetricsLine(tt.getTypeTable());
}
#endif // TEST_TYPEANALYSIS
