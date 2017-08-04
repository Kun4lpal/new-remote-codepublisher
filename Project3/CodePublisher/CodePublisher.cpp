/////////////////////////////////////////////////////////////////////
// Code Publisher.cpp - This package publishes source code files as//
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
*
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
#include "CodePublisher.h"
#include "../Project_2/TypeAnalysis/TypeAnalysis.h"
#include "../Project_2/DependencyAnalysis/DependencyAnalysis.h"
#include "../Project_2/CodeAnalyzer/FileMgr/FileMgr.h"
#include "../Project_2/CodeAnalyzer/FileMgr/IFileMgr.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

//<-----------------------------------Using Statements---------------------------------->

using namespace CPublish;
using namespace Universal;

//<-----------------------------------Primary Constructor---------------------------------->

CodePublisher::CodePublisher()
{
}

//<---------------------------------Secondary constructor---------------------------------->

CPublish::CodePublisher::CodePublisher(std::vector<std::string> files)
{
	this->dataSet = files;
}

//<-----------------------------------Destructor---------------------------------->

CodePublisher::~CodePublisher()
{
}

//<-----------------------------String manipulation helper function---------------------------------->

std::string CodePublisher::replaceStrChar(std::string src, const std::string& target, const std::string& repl)
{
	// handle error situations/trivial cases
	if (target.length() == 0) {
		// searching for a match to the empty string will result in 
		//  an infinite loop
		//  it might make sense to throw an exception for this case
		return src;
	}
	if (src.length() == 0) {
		return src;  // nothing to match against
	}
	size_t idx = 0;
	for (;;) {
		idx = src.find(target, idx);
		if (idx == std::string::npos)  break;
		src.replace(idx, target.length(), repl);
		idx += repl.length();
	}
	return src;
}

//<-----------------------------------this function is used to fetch file stream---------------------------------->

std::string CodePublisher::getFileString(std::string fileName)
{
	std::string line;
	std::ostringstream ss;
	std::ifstream myReadFile(fileName);
	if (myReadFile.is_open()) {
		while (std::getline(myReadFile, line))
		{
			ss << line << std::endl;
		}
		myReadFile.close();
	}
	else
	{
		std::cout << "Unable to open file\n";
	}
	return ss.str();
}

//<-----------------------------------a function to add JavaScript and CSS to doc element--------------------------------->

void CPublish::CodePublisher::addCSSandScript(Document &doc, std::string scriptName, std::string cssName)
{
	Tag script("script");
	script.SetAttribute("src", scriptName);
	Tag css("link");
	css.SetAttribute("rel", "stylesheet");
	css.SetAttribute("href", cssName);
	doc.AddTagToHead(script);
	doc.AddTagToHead(css);
}

//<----------------this function publishes files using information stored in NoSqlDb and TypeTable------------------------>

void CPublish::CodePublisher::publishFileSet(std::unordered_map<std::string, TypeRecord> ttable, NoSqlDb<std::string> database, std::string filePath)
{
	for (auto it : database.keys()) {Universal::Document doc;
		doc.AddTagToBody(Universal::Tag("h3").SetContent("Dependencies!"));
		std::vector<std::string> dependencies = database.value(it).dependencyList;
		int val = 1;
		for (auto item : dependencies) {
			std::ostringstream ss;ss << val;
			std::string reference = item.substr(item.find_last_of("/\\") + 1);
			Tag header("h4");
			Tag dependency("a.indent");
			dependency.SetAttribute("href", reference + ".htm");
			dependency.SetContent(ss.str() + "- " + reference);
			header.AppendChild(dependency);
			doc.AddTagToBody(header); val++;}
		TypeAnalysis typeAnalysis;
		std::string fullpath = database.value(it).data;
		std::string test = getFileString(fullpath);
		Tag x;   std::vector<std::pair<std::string, std::string >> typeList;
		typeList = getTypeList(ttable, fullpath);
		for (auto values : typeList) {
			if (it.find(".cpp") != std::string::npos && values.first == "MemberFunction") {
				test = x.modifyString(test, ":" + values.second);
			}
			else if (it.find(".h") != std::string::npos && values.first == "class") {
				test = x.modifyString(test, "class " + values.second);
			}
			else if (it.find(".h") != std::string::npos && values.first == "struct") {
				test = x.modifyString(test, "struct " + values.second);
			}}
		test = x.modifyString(test, "int main");
		doc.AddTagToBody(Universal::Tag("pre").SetContent(test));
		addCSSandScript(doc, "../Scripts/Script.js", "../Scripts/CSS-sheet.css");//add path to scripts here
		std::string fName = database.value(it).name;
		if (doc.WriteToFile(filePath + "/" + fName + ".htm", Universal::Readability::MULTILINE)) {
			std::cout << "\n File being built at path: " << FileSystem::Path::getFullFileSpec(filePath + "/" + fName + ".htm");
		}
	}createIndexFile(database,filePath);
}

//<--------------------------------------------create index file--------------------------------------------->

void CPublish::CodePublisher::createIndexFile(NoSqlDb<std::string> database,std::string filePath) {
	Universal::Document doc;
	doc.AddTagToBody(Universal::Tag("h3").SetContent("INDEX!")); int val = 1;
	for (auto it : database.keys()) {
		std::ostringstream ss;
		ss << val;
		std::string reference = it.substr(it.find_last_of("/\\") + 1);
		Tag header("h4");
		Tag dependency("a.indent");
		dependency.SetAttribute("href", reference + ".htm");
		dependency.SetContent(ss.str() + "- " + reference);
		header.AppendChild(dependency);
		doc.AddTagToBody(header); val++;
	}
	addCSSandScript(doc, "../Scripts/Script.js", "../Scripts/CSS-sheet.css");//add path to scripts here
	doc.WriteToFile(filePath + "/" + "index.htm", Universal::Readability::MULTILINE);
}

//<-----------------------Obtains type information for a specific file using typetable-------------------------->

std::vector<std::pair<std::string, std::string >> CPublish::CodePublisher::getTypeList(std::unordered_map<std::string, TypeRecord> ttable, std::string name)
{
	std::vector<std::pair<std::string, std::string >> list;

	for (auto it : ttable) {
		std::vector<std::pair<std::string, std::string>> example = it.second.getPairs();
		for (auto item : example) {
			if (item.first == name) {
				std::string val = it.second.getTypeName();
				std::pair<std::string, std::string > testPair = std::make_pair(val, it.first);
				list.push_back(testPair);
			}
		}
	}
	return list;
}

//<-----------------------------------a publishing function for tesing purposes---------------------------------->

void CPublish::CodePublisher::testPublishing(std::string location, NoSqlDb<std::string> database)
{
	try
	{
		for (auto it : database.keys()) {
			Universal::Document doc;
			doc.AddTagToBody(Universal::Tag("h3").SetContent("Dependencies!"));
			std::vector<std::string> dependencies = database.value(it).dependencyList;
			for (auto item : dependencies) {
				std::string reference = item.substr(item.find_last_of("/\\") + 1);
				Tag header("h4");
				Tag dependency("a.indent");
				dependency.SetAttribute("href", reference + ".html");
				dependency.SetContent(reference);
				header.AppendChild(dependency);
				doc.AddTagToBody(header);
			}
			std::string test = getFileString(it);
			std::cout << test;
			Tag x;
			test = x.modifyString(test, "int main");
			doc.AddTagToBody(Universal::Tag("pre").SetContent(test));
			std::string JavaScript = getFileString("../Scripts/Script.js");
			std::string script = getFileString("../Scripts/CSS-sheet.css");
			doc.AddTagToHead(Universal::Tag("script").SetContent(script));
			doc.AddTagToHead(Universal::Tag("style").SetContent(JavaScript));
			std::string itach = "Name";
			doc.WriteToFile("../PublishedHtmlFiles/itach.html", Universal::Readability::MULTILINE);
		}
	}
	catch (const std::exception& ex) { std::cout << ex.what(); }
}

//<----------------------------------------TEST STUB FOR CODE PUBLISHER---------------------------------->

#ifdef TESTPUBLISH
int main(char argc, char* argv[])
{
	try {
		std::vector<std::string> fileset;
		NoSqlDb<std::string> db;
		Element<std::string> elem;
		elem.name = "TEST";
		elem.category = "test";
		elem.timeDate = "Testdate";
		std::vector<std::string> dependencies;
		dependencies.push_back("Cpp11-BlockingQueue.cpp");
		elem.dependencyList = dependencies;
		db.save("../Executive/Executive.cpp", elem);
		//fileset.push_back("CodePublisher.cpp");
		//fileset.push_back("CodePublisher.cpp");
		CodePublisher ex;
		ex.testPublishing("CodePublisher.cpp", db);
	}
	catch (std::exception ex) {
		std::cout << ex.what();
	}

}
#endif // TESTPUBLISH