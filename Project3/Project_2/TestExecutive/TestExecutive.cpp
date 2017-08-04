/////////////////////////////////////////////////////////////////////
// Test Executive.cpp - Dependency Analysis main execution package //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Author:      Kunal Paliwal (c) copyright 2017                   // 
//              kupaliwa @syr.edu, (315) 876 8002                  //
// All rights granted provided this copyright notice is retained   //
// Source:      Jim Fawcett, Syracuse University, CSE 687          //
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
#include "TestExecutive.h"
#include "../Proj_1/Persist/Persist.h"
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

//<-----------------------------------File Manager instance---------------------------------->

IFileMgr* FileMgr::pInstance_;  //global

IFileMgr* IFileMgr::getInstance()
{
	return FileMgr::getInstance();
}

//<-----------------------------------Constructor of test executive--------------------------->

TestExecutive::TestExecutive(){
	xpath_ = "../XmlFiles";  //default xml path.
 	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
}

//<-----------------------------------Destructor---------------------------------->

TestExecutive::~TestExecutive()
{
}

//<------------------------------Displayed when arguments are < 3------------------------->

void TestExecutive::showUsage()
{
	std::ostringstream out;
	out << "\n  Usage:";
	out << "\n  Command Line Arguments are:";
	out << "\n  - 1st: path to subdirectory containing files to analyze";
	out << "\n  - 2nd: path to subdirectory where xml will be created";
	out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
	out << "\n  - must have at least one file pattern to specify what to process";
	out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
	out << "\n  A summary is always shown, independent of any options used or not used";
	out << "\n\n";
	std::cout << out.str();
}
//----------------------< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
*/
bool TestExecutive::ProcessCommandLine(int argc, char* argv[])
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

void TestExecutive::showCommandLineArguments(int argc, char* argv[])
{
	std::ostringstream out;
	std::ostringstream xpath;
	out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
	for (int i = 2; i < argc - 1; ++i) {
		out << argv[i] << ", "; 
	}
	xpath << argv[2];
	out << argv[argc - 1];
	std::cout << out.str() << std::endl;
	xpath_ = xpath.str();
}

//--------------------------------< returns path entered on command line >-------------------

std::string TestExecutive::getAnalysisPath()
{
	return path_;
}

//----< searches path for files matching specified patterns >----
/*
* - Searches entire diretory tree rooted at path_, evaluated
*   from a command line argument.
* - Saves found files in FileMap.
*/
void TestExecutive::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numFiles_ = fm.numFiles();
	numDirs_ = fm.numDirs();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}

//<-----------------------------------returns cppheaderFile *.h types-------------------------------->

std::vector<TestExecutive::File>& TestExecutive::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}

//-----------------------------< retrieve from fileMap all files matching *.cpp >---------

std::vector<TestExecutive::File>& TestExecutive::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}
//--------------------< retrieve from fileMap all files matching *.cs >-------------------------

std::vector<TestExecutive::File>& TestExecutive::csharpFiles()
{
	csharpFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cs"))
		{
			for (auto file : item.second)
				csharpFiles_.push_back(file);
		}
	}
	return csharpFiles_;
}
//-------------------------< report number of Source Lines Of Code (SLOCs) >---------------------

TestExecutive::Slocs TestExecutive::fileSLOCs(const File& file)
{
	return slocMap_[file];
}
//-----------------------------< returns current time >----------------------------

std::string TestExecutive::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	temp.erase(8,1);
	return temp;
}

//----------< This is where the set of files are parsed and ASTNodes are created and added >--------

void TestExecutive::processSourceCode(bool showProc)
{
	for (auto file : cppHeaderFiles()){
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);        if (!configure_.Attach(file)){
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
		{pParser_->parse();}
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;}
	for (auto file : cppImplemFiles()){
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);
		if (!configure_.Attach(file)){
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";continue;}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();slocMap_[pRepo_->package()] = slocs;}
	for (auto file : csharpFiles()){
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file)){
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;}
		pRepo_->language() = Language::CSharp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;}
	std::ostringstream out; out << std::left << "\r  " << std::setw(77) << " ";
}

//-----------------------------< returns number of files >----------------------------

size_t TestExecutive::numFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t TestExecutive::numDirs()
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

size_t AnalFileMgr::numFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t AnalFileMgr::numDirs()
{
	return numDirs_;;
}

void comments() {
	Util::Title("Requirement #1: Shall use Visual Studio 2015 and its C++ Windows Console Projects, as provided in the ECS computer labs.");
	putline();
	Util::Title("Requirement #2: Shall use the C++ standard library's streams for all I/O and new and delete for all heap-based memory management.");
	putline();
	Util::Title("Requirement #3: Enforce the \"Single Responsiblity Principle\".");
	putline();
	std::cout << "Package structure: ";
	putline();
	std::cout << "   1) DependencyAnalysis\n   2) TypeAnalysis\n   3) Visual_display\n   4) TypeTable\n   5) StrongComponents\n   6) NoSqlDb\n   4) TestExecutive";
	putline();
	Util::Title("Requirement #8: Shall process the command line, accepting:");
	putline();
	std::cout << "1) Path to the directory tree containing files to analyze";
	putline();
	std::cout << "2) List of file patterns to match for selection of files to analyze.";
	putline();
	std::cout << "3) Specification of the XML results file, supplying a default if no specification is provided";
	putline();
}


TypeTable TestExecutive::getTypeTable(int argc, char* argv[])
{
	comments();
	bool succeeded = ProcessCommandLine(argc, argv);
	std::ostringstream tOut("Dependency Analysis");
	Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
	showCommandLineArguments(argc, argv);
	getSourceFiles();
	processSourceCode(true);
	putline();
	Util::Title("Requirement #4: Shall provide a TypeAnalysis package that identifies all of the types defined in a specified file");
	putline();
	Utils::Title("TypeTable obtained after analyzing files/file at the specified location.");
	putline();
	TypeTable TT;
	TypeAnalysis demo;
	demo.filltypeTable(pRepo_);
	TypeTable x = demo.getMainTable();
	std::unordered_map<std::string, TypeRecord> trialTable = x.getTypeTable();
	TT.setFromInstance(trialTable);
	return TT;
}

SCDataBase TestExecutive::getStrongComponents(Graph<std::string, std::string> g) {
	Util::Title("Requirement #6: Shall find strong components in the dependency graph defined by the relationships evaluated in the previous requirement");
	putline();
	StrongComponents ts(g);
	ts.findStrongComponents();
	StrongComp result = ts.returnList();
	putline();
	Utils::Title("FILESET:");
	putline();
	for (auto it : g) {
		std::cout << it.value() << std::endl;
	}
	putline();
	int value = 1;
	Util::Title("StrongComponents:"); putline();

	for (auto it : result) {
		std::ostringstream ss;
		std::ostringstream data;
		ss << "Strong Component " << value;
		std::cout <<"\n Strong Component " << value << ": \n";
		Element<std::string> elem;
		elem.name = ss.str();
		elem.category = "StrongComponent";
		std::vector<std::string> components;
		for (auto item : it) {
			std::cout << item->value()<<std::endl;
			std::string itemName = item->value().substr(item->value().find_last_of("/\\") + 1);
			components.push_back(itemName);
			data <<"["<< itemName<<"] ";
		}
		data << ".";
		putline();
		elem.dependencyList = components;
		elem.data = data.str();
		elem.timeDate = systemTime();
		strongCompDB.save(elem.name,elem);
		value++;
	}

	return strongCompDB;
}

//<----------------------------------------MAIN----------------------------------------------->

#ifdef TEST_EXECUTIVE
int main(int argc, char* argv[]){
	Util::Title("DEPENDENCY ANALYZER testrun:");
	TestExecutive exec;
	putline();
	std::cout <<std::setw(70) <<"Current Time: " << exec.systemTime() << std::endl;
	try {
		TypeTable TT = exec.getTypeTable(argc, argv);
		std::unordered_map<std::string, TypeRecord> ttable = TT.getTypeTable();

		visual v;
		v.displayHeader();
		v.displayMetricsLine(ttable); putline();
		Util::Title("Requirement #5: Shall provide a DependencyAnalysis package that identifies all of the dependencies between files in a specified file collection");
		putline();
		DependencyAnalysis dp(exec.vec);
		std::string currentTime = exec.systemTime();
		NoSqlDb<std::string> database = dp.storeDependencyInfo(ttable, currentTime); //NoSQLdb containing dependency analysis information
		dp.createGraph(database);

		Graph<std::string, std::string> g = dp.getGraph();
		putline();
		Utils::Title("Dependency information filled in a NoSqlDB: ");
		putline();

		database.showDataBase();
		SCDataBase sr =  exec.getStrongComponents(g);
		Util::Title("StrongComponents stored in NoSqlDb:"); putline();
		sr.showList();
		std::cout << "\n  Analysis completed\n";
		putline();
		
		Util::Title("Requirement #7: Shall write the analysis results, in XML format, to a specified file");
		putline();             //this xml file create location has to be given at a certain location
		                                 //using argument given in commandline
		StringWrapper sw;
		std::string xmlFile =  sw.toXml(database);  // instead of this db we need to use strong comp analysis data
		std::string xoFile = sw.toXml(sr);
		if (database.buildDocAtPath(xmlFile, "depAnalysis", exec.xpath_)) {
			std::cout << " DepAnalysis file built at path: " << FileSystem::Path::getFullFileSpec(exec.xpath_);
		}
		if (sr.buildDocAtPath(xoFile, "strongComponents ", exec.xpath_)) {
			std::cout << "\n StrongComponents analysis file built at path: " << FileSystem::Path::getFullFileSpec(exec.xpath_);
		}
	}
	catch (std::exception ex) {
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(ex.what()) + "\n\n";
		return 1;
	}
}
//<----------------------------------------EOP----------------------------------------------->
#endif // TEST_EXECUTIVE