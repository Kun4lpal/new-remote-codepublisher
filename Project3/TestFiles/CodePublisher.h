/////////////////////////////////////////////////////////////////////
// Code Publisher.h - This package publishes source code files as  //
// ver 1.0              HTML documents                             //
//-----------------------------------------------------------------//
// Author:      Kunal Paliwal (c) copyright 2017                   // 
//              kupaliwa @syr.edu, (315) 876 8002                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Project #3, CSE687 - Object Oriented Design, S2017 //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  ===================
* This package makes use of HtmlDocument package that constructs html style
* documents. Html file constructed contains dependency infomation as well as
* the source code.
* It also contains a test publishing function which publishes a default html file
*  for testing.
*
* //* Public Interface (see test stub for details):
* =============================================
* replaceStrChar();   // this function is a string manipulation function that replaces a substring with another string
* getFileString();     // retrieves filestream of the file specified.
* testPublishing();   // test function for publishing document.
* publishFileSet();   // publishes a fileSet using information stored in NOSQLDB and typetable
* getTypeList();      // getType information for a specific file using typeTable.
*
*  Required Files (Direct and indirect):
*  ---------------
*  HtmlDocument:
*  Universal.h - A header file which allows use of both Tag and Document header files.
*  Project 2 :
*  DependencyAnalysis.h - This file is needed for fetching dependency information.
*  FileManager :
*  This package manages file handling.
*
*  Maintanence History:
*  --------------------
*  ver 1.0 : 30 March 2017
*  - first release
*/
//
#pragma once
#include <string>
#include <vector>
#include "../HtmlDocument/Universal.h"
#include "../Project_2/TypeTable/TypeTable.h"
#include "../Project_2/Proj_1/Proj_1/NoSqlDb.h"

namespace CPublish {
	//<-----------------------------------Handles publishing of CodeFiles---------------------------------->
	class CodePublisher
	{
		//<-----------------------------------public data members---------------------------------->
	public:
		CodePublisher();
		CodePublisher(std::vector<std::string> files);
		~CodePublisher();
		std::string replaceStrChar(std::string str, const std::string & replace, const std::string &def);
		std::string getFileString(std::string st);
		void testPublishing(std::string location, NoSqlDb<std::string> database);
		void publishFileSet(std::unordered_map<std::string, TypeRecord> ttable, NoSqlDb<std::string> database, std::string filePath);
		std::vector<std::pair<std::string, std::string >> getTypeList(std::unordered_map<std::string, TypeRecord> ttable, std::string name);
		void addCSSandScript(Universal::Document &doc, std::string scriptName, std::string cssName);

		//<-----------------------------------private data members--------------------------------->
	private:
		std::vector<std::string> dataSet;
	};
}