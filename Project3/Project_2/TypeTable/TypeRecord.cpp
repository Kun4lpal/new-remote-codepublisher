///////////////////////////////////////////////////////////////////////////
// TypeRecord.cpp   -  This file contains the implementation for all the // 
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
*  A simple data structure which contains typeName information and contains a
*  vector of pairs which in turn contain the type and Namespace it belongs to.
*
//
//* Public Interface (see test stub for details):
* =============================================
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
*  This class makes use of the standard unordered_map data structure.
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
//
#include "TypeRecord.h"

//<----------------------function implementation------------------------------->

//<----------------------function returns typeName stored in typeRecord------------------->

std::string TypeRecord::getTypeName()
{
	return _typeName;
}

//<----------------------function to set the typename in typeRecord-------------------------->

void TypeRecord::setTypeName(std::string typeName)
{
	_typeName = typeName;
}

//<---------function returns vector of pairs which contain filenames and their namespace ---------->

std::vector<std::pair<std::string, std::string>> TypeRecord::getPairs()
{
	return _pairs;
}

//<----------------------function sets the vector of pairs to instance in argument ------------------------->

void TypeRecord::setFilePairs(std::vector<std::pair<std::string, std::string>> filePairs)
{
	_pairs = filePairs;
}
