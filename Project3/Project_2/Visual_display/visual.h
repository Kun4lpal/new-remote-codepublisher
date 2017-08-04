#pragma once
///////////////////////////////////////////////////////////////////////////
// Visual.h   -  This package provides a class which is used to display  //
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
*
* class visual {.....}
*
* displayHeader()                       // this displays the header of the TABLE
* displayMetricsLine                    // this displays all the information contained inside
*
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
#include "../TypeTable/TypeTable.h"
#include "../TypeTable/TypeRecord.h"

//<----------------------class declaration------------------------------->

class visual
{
	//<----------------------function declarations------------------------------->
public:
	void displayHeader();
	void displayMetricsLine(std::unordered_map<std::string, TypeRecord> typeTable);
};


