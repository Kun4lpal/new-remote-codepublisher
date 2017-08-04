#ifndef TEST
#define TEST
#pragma once
/////////////////////////////////////////////////////////////////////
// Test Executive.h - Organizes and Dependency Analysis            //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Author: Kunal Paliwal (c) copyright 2016                        // 
//                        kupaliwa @syr.edu, 315 886 8002          //
// All rights granted provided this copyright notice is retained   //
// Source:      Jim Fawcett, Syracuse University, CST 687          //
//              jfawcett@twcny.rr.com                              //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Project #2, CSE687 - Object Oriented Design, S2017 //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  ===================
*  Executive uses Parser, ScopeStack, and AbstrSynTree packages to
*  provide Dependency analysis for C++ and C# files contained in a
*  specified directory tree.
*
*  It finds files for analysis using FileMgr and, for each file found,
*  uses parser rules to detect:
*  - namespace scopes
*  - class scopes
*  - struct scopes
*  - control scopes
*  - function definition scopes
*  - data declarations
*  and uses rule actions to build an Abstract Sytax Tree (AST)
*  representing the entire code set's static structure.  Each detected
*  scope becomes a node in the AST which spans all of the files analyzed,
*  rooted in a top-level Global Namespace scope.
*
*  Executive supports displays for:
*  - a list of all files processed with their source lines of code count.
*  - an indented representation of the AST.
*  - a list of all functions that are defined, organized by package, along
*    with their starting lines, size in lines of code, and complexity as
*    measured by the number of their descendent nodes.
*  - a list of all functions which exceed specified function size and/or
*    complexity.
*
* //* Public Interface (see test stub for details):
* =============================================
*
* class AnalFileMgr {.....}
* dir()                                   // override of FileMgr::file(...) to store found files
* files()                                 // override of FileMgr::dir(...) to save current dir
* numFiles()                              // returns number of files
* numDir()                                // returns number of directories
*
* class TestExecutive {.....}
* showUsage()                             // when arguments are less than 3.
* processCommandLine()                    // this method processes the commandLine arguments and
*                                            makes use of FileManager package to obtain path info.
* showCommandLineArguments();             // displays the commandLine arguments entered.
* getAnalysisPath();                      // returns the path entered.
* getSourceFiles();                       // gets all files from the path and saves them in FileMap
* cppHeaderFIles();                       // cpp header files processing
* cppImplemFiles();                       // cpp implem files processing
* csharpFiles();                          // csharp files processing
* fileSLOCs();                            // retuns number of source lines of code.
* systemTime();                           // returns time
* processSourceCode();                    // where set of Files are parsed and processed
* getTypeTable();                         // this obtains the TypeTable of all the files obtained
* getStrongComponents();                  // this returns a list of strongComponents
*
*  Required Files (Direct and indirect):
*  ---------------
*  - Executive.h, Executive.cpp
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - TypeTable.h, TypeTable.cpp           // always required
*  - TypeRecord.h, TypeRecord.cpp         // always required
*  - TypeAnalysis.h, TypeAnalysis.cpp     // always required
*  - Graph.h, Graph.cpp                   // always required
*  - NoSqlDb.h, NoSqlDb.cpp               // always required
*
*  Maintanence History:
*  --------------------
*  ver 1.0 : 03 March 2017
*  - first release
*/
//
#include "../DependencyAnalysis/DependencyAnalysis.h"
#include "../StrongComponents/StrongComponents.h"
#include "../CodeAnalyzer/FileMgr/FileMgr.h"
#include "../CodeAnalyzer/FileMgr/IFileMgr.h"

//<-----------------------------------Using Statements---------------------------------->

using namespace FileSystem;
using StrongComp = std::vector<std::vector<Vertex<std::string, std::string>*>>;
using SCDataBase = NoSqlDb<std::string>;

//<-----------------------This class derives from FileManager--------------------------------->

class AnalFileMgr : public FileManager::FileMgr
{
public:
	//<-----------------------------------using statements---------------------------------->
	using Path = std::string;
	using File = std::string;
	using Files = std::vector<File>;
	using Pattern = std::string;
	using Ext = std::string;
	using FileMap = std::unordered_map<Pattern, Files>;

	//<---------------------------------function declarations---------------------------------->
	AnalFileMgr(const Path& path, FileMap& fileMap);  //constructor
	virtual void file(const File& f);
	virtual void dir(const Dir& d);
	virtual void done();
	size_t numFiles();
	size_t numDirs();

	//<----------------------------------private data members---------------------------------->
private:
	Path d_;
	FileMap& fileMap_;
	bool display_;
	size_t numFiles_;
	size_t numDirs_;
};

//<-----------------------------THIS is the main executive class---------------------------------->

class TestExecutive
{
	//<-----------------------------------Using statements---------------------------------->
	using Path = std::string;
	using Options = std::vector<char>;
	using XMLPath = std::string;
	using Pattern = std::string;
	using Patterns = std::vector<Pattern>;
	using File = std::string;
	using Files = std::vector<File>;
	using FileMap = std::unordered_map<Pattern, Files>;
	using FileNodes = std::vector<std::pair<File, ASTNode*>>;
	using Slocs = size_t;
	using SlocMap = std::unordered_map<File, Slocs>;

public:
	//<-----------------------------------public data members---------------------------------->
	XMLPath xpath_;
	Path path_;
	std::vector<std::string> vec;
	Parser* pParser_;
	ConfigParseForCodeAnal configure_;
	Repository* pRepo_;
	FileMap fileMap_;
	Patterns patterns_;
	std::vector<File> cppHeaderFiles_;
	size_t numFiles_;
	size_t numDirs_;
	Options options_;
	FileNodes fileNodes_;
	std::vector<File> cppImplemFiles_;
	std::vector<File> csharpFiles_;
	SlocMap slocMap_;
	SCDataBase strongCompDB;

	//<-----------------------------------Function declarations---------------------------------->

	TestExecutive();
	~TestExecutive();
	void showUsage();
	bool ProcessCommandLine(int argc, char* argv[]);
	void showCommandLineArguments(int argc, char * argv[]);
	std::string getAnalysisPath();
	void getSourceFiles();
	std::vector<File>& cppHeaderFiles();
	std::vector<File>& cppImplemFiles();
	std::vector<File>& csharpFiles();
	TestExecutive::Slocs fileSLOCs(const File & file);
	std::string systemTime();
	void processSourceCode(bool showProc);
	size_t numFiles();
	size_t numDirs();
	TypeTable getTypeTable(int argc, char* argv[]);
	SCDataBase getStrongComponents(Graph<std::string, std::string> g);
};

#endif // !TEST


