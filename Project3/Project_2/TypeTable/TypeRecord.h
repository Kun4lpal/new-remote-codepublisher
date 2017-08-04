#ifndef TYPE_REC
#define TYPE_REC
#pragma once
///////////////////////////////////////////////////////////////////////////
// TypeRecord.h   -  Data structure which is part of TypeTable used to   //
// ver 1.0           store typeName, fileName and parent Namespace       //
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*  A simple data structure which contains typeName information and contains a 
*  vector of pairs which in turn contain the type and Namespace it belongs to.
*
//
//* Public Interface (see test stub for details):
* =============================================
*
* class TypeRecord {.....}
*
* getTypeName();      // this method returns the typeName of the instance.
* setTypeName();      // this method sets the typeRecord instance to a value.
* getPairs();         // this methods returns the vector of pairs which contain type and 
*                        namespace information
* setFilePairs();    // sets the filePairs to an instance.
*
*/
/* Build Process
*  =============
*  This class makes use of standard vector container.
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
//
#include <string>
#include <vector>
#include <unordered_map>

//<-----------------------------class defintion---------------------------------->

class TypeRecord
{
	//<----------------------function declarations------------------------------->
public:
	std::string getTypeName();
	void setTypeName(std::string typeName);
	std::vector<std::pair<std::string, std::string>> getPairs();
	void setFilePairs(std::vector<std::pair<std::string, std::string>> filePairs);

	//<----------------------private data members------------------------------->
private:
	std::string _typeName;
	std::vector<std::pair<std::string, std::string>> _pairs;
};
#endif // TYPE_REC


