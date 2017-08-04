/////////////////////////////////////////////////////////////////////
// Executive.cpp - Organizes, does dependency analysis and         //
// ver 1.0         publishing                                      //
//-----------------------------------------------------------------//
// Author:      Kunal Paliwal (c) copyright 2017                   // 
//              kupaliwa @syr.edu, (315) 876 8002                  //
// All rights granted provided this copyright notice is retained   //
// Source:      Jim Fawcett, Syracuse University, CSE 687          //
//              jfawcett@twcny.rr.com                              //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Project #3, CSE687 - Object Oriented Design, S2017 //
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
* class Executive {.....}
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
* launch()                                // this function launches the html file specified in the commandLine.
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
*  - CodePublisher.h, CodePublisher.cpp   // always required
*
*  Maintanence History:
*  --------------------
*  ver 1.1 : 30 March 2017
*  - second release
*
*  ver 1.0 : 03 March 2017
*  - first release
*/
//
#include "Executive.h"
#include "../Project_2/Proj_1/Persist/Persist.h"
#include "../CodePublisher/CodePublisher.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

//<-----------------------------------Using Statements---------------------------------->

using StrData = std::string;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;
using Utils = Utilities::StringHelper;
using namespace FileManager;
using namespace Persist;
using namespace CPublish;

//<-----------------------------------File Manager instance---------------------------------->

IFileMgr* FileMgr::pInstance_;  //global

IFileMgr* IFileMgr::getInstance()
{
	return FileMgr::getInstance();
}

//<-----------------------------------Constructor of test executive--------------------------->

Executive::Executive() {
	xpath_ = "../HtmlFiles";  //default html path.
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
}

//<-----------------------------------Destructor---------------------------------->

Executive::~Executive()
{
}

//<------------------------------Displayed when arguments are < 3------------------------->

void Executive::showUsage()
{
	std::ostringstream out;
	out << "\n  Usage:";
	out << "\n  Command Line Arguments are:";
	out << "\n  - 1st: path to subdirectory containing files to analyze";
	out << "\n  - 2nd: path to subdirectory where html file will be created";
	out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
	out << "\n  - must have at least one file pattern to specify what to process";
	out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
	out << "\n  A summary is always shown, independent of any options used or not used";
	out << "\n\n";
	std::cout << out.str();
}

//----------------------< helper Function to replace string>------------------------------

std::string Executive::replaceSubString(std::string replacer, const std::string& replacable, const std::string& replace) const {
	size_t start = 0;
	while ((start = replacer.find(replacable, start)) != std::string::npos) {
		replacer.replace(start, replacable.length(), replace);
		start += replace.length();
	}
	return replacer;
}

//----------------------< handle launch event >------------------------------

void Executive::launch(std::string valu)
{
	std::cout << "\n Launching file at path: " << valu;
	putline();
	std::string htm = valu;
	std::string browser("start \"\" \"" + htm + "\"");
	std::system(browser.c_str());
}
//----------------------< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
*/
bool Executive::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 3)
	{
		showUsage();
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/') {
				options_.push_back(argv[i][1]);
			}
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			showUsage();
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

//<------------------------------Displays command line arguments------------------------------>

void Executive::showCommandLineArguments(int argc, char* argv[])
{
	std::ostringstream out;
	std::ostringstream xpath;
	out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
	for (int i = 2; i < argc - 1; ++i) {
		out << argv[i] << ", ";
	}
	xpath << argv[2];
	out << argv[argc - 1];
	std::cout << out.str();
	xpath_ = xpath.str();
}

//--------------------------------< returns path entered on command line >-------------------

std::string Executive::getAnalysisPath()
{
	return path_;
}

//----< searches path for files matching specified patterns >----
/*
* - Searches entire diretory tree rooted at path_, evaluated
*   from a command line argument.
* - Saves found files in FileMap.
*/
void Executive::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numFiles_ = fm.numAFiles();
	numDirs_ = fm.numADirs();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}

//<-----------------------------------returns cppheaderFile *.h types-------------------------------->

std::vector<Executive::File>& Executive::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}

//-----------------------------< retrieve from fileMap all files matching *.cpp >---------

std::vector<Executive::File>& Executive::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}
//--------------------< retrieve from fileMap all files matching *.cs >-------------------------

std::vector<Executive::File>& Executive::csharpFiles()
{
	csharpFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cs") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				csharpFiles_.push_back(file);
		}
	}
	return csharpFiles_;
}
//-------------------------< report number of Source Lines Of Code (SLOCs) >---------------------

Executive::Slocs Executive::fileSLOCs(const File& file)
{
	return slocMap_[file];
}
//-----------------------------< returns current time >----------------------------

std::string Executive::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	temp.erase(8, 1);
	return temp;
}

//----------< This is where the set of files are parsed and ASTNodes are created and added >--------

void Executive::processSourceCode(bool showProc)
{
	for (auto file : cppHeaderFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);        if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;
		}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
		{
			pParser_->parse();
		}
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : cppImplemFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);
		if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n"; continue;
		}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount(); slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : csharpFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;
		}
		pRepo_->language() = Language::CSharp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	std::ostringstream out; out << std::left << "\r  " << std::setw(77) << " ";
}

//-----------------------------< returns number of files >----------------------------

size_t Executive::numFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t Executive::numDirs()
{
	return numDirs_;;
}

//-----------------------------< Constructor >----------------------------

AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}

//-----------------------------< returns number of files >----------------------------

size_t AnalFileMgr::numAFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t AnalFileMgr::numADirs()
{
	return numDirs_;;
}

void comments() {
	std::cout << "-----------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement #1: Shall use Visual Studio 2015 and its C++ Windows Console Projects, as provided in the ECS computer labs.\n";
	std::cout << "------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\nRequirement #2: Shall use the C++ standard library's streams for all I/O and new and delete for all heap-based memory management.\n";
	std::cout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\nRequirement #3: Shall provide a Publisher program that provides for creation of web pages each of which captures\n";
	std::cout << "                  the content of a single C++ source code file, e.g., *.h or *.cpp.\n";
	std::cout << "\n   1) HTMLDocument package is similar to XMLDocument package which makes use of Tag.h and Document.h to create a doc Element.\n";
	std::cout << "   2) Code Publisher make use of this package to meet all the requirements of Project #3:Code Publisher.\n";
	std::cout << "-------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\nRequirement #4: Shall, optionally provide the facility to expand or collapse class bodies, methods, and global functions using JavaScript and CSS properties.\n";
	std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\nRequirement #7: Shall embedd HTML5 links to dependent files with a label, at the top of the web page. Publisher shall use functionality from your\n";
	std::cout << "                  project #2 to discover package dependencies within the published set of source files.\n";
	std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------\n";
	
	std::cout << "\nRequirement #10: Shall include an automated unit test suite that demonstrates you meet all the requirements of this project2.\n\n";
	std::cout << "-------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\nRequirement #8: Shall process the command line, accepting:\n";
	std::cout << "                  1) Path to the directory tree containing files to analyze";
	putline();
	std::cout << "                  2) List of file patterns to match for selection of files to analyze.";
	putline();
	std::cout << "                  3) Specification of the Html results file or launch file, supplying a default if no specification is provided";
	putline();
}


TypeTable Executive::getTypeTable(int argc, char* argv[])
{
	comments();
	bool succeeded = ProcessCommandLine(argc, argv);
	std::ostringstream tOut("Dependency Analysis");
	Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
	showCommandLineArguments(argc, argv);
	std::cout << "\n--------------------------------------------------------------------------------------------------------------------------------\n";
	getSourceFiles();
	processSourceCode(true);
	putline();
	TypeTable TT;
	TypeAnalysis demo;
	demo.filltypeTable(pRepo_);
	TypeTable x = demo.getMainTable();
	std::unordered_map<std::string, TypeRecord> trialTable = x.getTypeTable();
	TT.setFromInstance(trialTable);
	return TT;
}

//<----------------------------------------MAIN----------------------------------------------->

#ifdef TEST_EXECUTIVE
int main(int argc, char* argv[]) {
	Util::Title("CODE PUBLISHER testrun:");
	Executive exec;
	putline();
	std::cout << std::setw(70) << "Current Time: " << exec.systemTime() << std::endl;
	try {
		TypeTable TT = exec.getTypeTable(argc, argv);
		std::unordered_map<std::string, TypeRecord> ttable = TT.getTypeTable();
		DependencyAnalysis dp(exec.vec);
		std::string currentTime = exec.systemTime();
		std::cout << "Finding Dependency Information...........................................\n";
		NoSqlDb<std::string> database = dp.storeDependencyInfo(ttable, currentTime); //NoSQLdb containing dependency analysis information
		dp.createGraph(database);
		CodePublisher ex;putline();
		std::cout << "--------------------------------------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\nRequirement #5:  Shall provide a CSS style sheet that the Publisher uses to style its generated pages and (if you are implementing the previous ";
		std::cout << "\n                 optional requirement) a JavaScript file that provides functionality to hide and unhide sections of code for outlining, using mouse clicks. \n";
		putline();
		std::cout << ex.getFileString("Scripts/CSS-sheet.css") << std::endl;
		std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\nRequirement #6:  Shall embed in each web page's <head> section links to the style sheet and JavaScript file.\n"; putline();
		std::cout << ex.getFileString("Scripts/Script.js") << std::endl; putline();
		std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\nRequirement #9: Shall demonstrate the CodePublisher functionality by publishing all the important packages in your Project #3.\n";
		std::cout << "\nPackage structure: ";putline();
		std::cout << "   1) DependencyAnalysis\n   2) TypeAnalysis\n   3) TypeTable\n   4) StrongComponents\n";
		std::cout << "   5) HTMLDocument\n   6) Code Publisher\n   7) Tag\n";putline();
		Util::Title("Publishing documents:");
		putline();
		if (exec.xpath_.find("*") == std::string::npos) {
			if (exec.xpath_.find("launch") == std::string::npos) {
				ex.publishFileSet(ttable, database, exec.xpath_);
				exec.launch(exec.xpath_ + "/DependencyAnalysis.cpp.htm");} 
			else {
				ex.publishFileSet(ttable, database, "HtmlFiles");
				std::string path = exec.xpath_.substr(7, exec.xpath_.length());
				exec.launch(path);
			}}else {
			ex.publishFileSet(ttable, database, "HtmlFiles");
			exec.launch("HtmlFiles/DependencyAnalysis.cpp.htm");}
		std::cout << "\n--------------------------------------------------------------------End of Line-----------------------------------------------------------------\n";}
	catch (std::exception ex) {
		std::cout << "\n\n  Caught exception in Executive::main: " + std::string(ex.what()) + "\n\n";
		return 1;}
}
//<----------------------------------------EOP----------------------------------------------->
#endif // TEST_EXECUTIVE