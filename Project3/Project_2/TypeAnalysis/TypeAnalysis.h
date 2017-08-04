#ifndef TYPE_ANAL
#define TYPE_ANAL
#pragma once
///////////////////////////////////////////////////////////////////////////
// TypeAnalysis.h   -  Analyzes files and fills the typeTable with the   //
// ver 1.0             information obtained by walking through the AST   //
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
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
*
* class TypeAnalysis {.....}
*
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
#include "../TypeTable/TypeTable.h"
#include "../Visual_display/visual.h"
#include <unordered_map>
#include <string>
#include <vector>

//<--------------------------------Class definition-------------------------------------->

class TypeAnalysis
{
	//<------------------------------function declarations------------------------------->
public:
	void filltypeTable(Repository * repo);
	void DFS(ASTNode* pNode,std::string namespace_, std::string parentType);
	void checkDeclarations(ASTNode * pNode, std::string namespc);
	TypeTable getMainTable();
	Repository* doTypeAnalysis(std::vector<std::string> files);

private:
	//<-----------------------------private Data members---------------------------------->
	std::unordered_map<std::string, TypeRecord> createTypeTable;
	ScopeStack<ASTNode*> stack;
	TypeTable TT;
	visual _v;
};

#endif // !TYPE_ANAL


