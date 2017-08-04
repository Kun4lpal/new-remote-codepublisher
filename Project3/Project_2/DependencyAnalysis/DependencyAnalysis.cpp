///////////////////////////////////////////////////////////////////////////
// DependencyAnalysis.cpp   -  Contains implementation of methods        //
// ver 1.0                     declared in DependencyAnalysis.h          //
//                                                                       //
// Language:      Visual C++.                                            //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design   //
// Author:        Kunal Paliwal, Syracyse University, CST 687            //
//                kupaliwa@syr.edu, (315) 876 8002                       //
///////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module does analysis of a set of files and gives back their dependency
* information. Finds, for each file in a specified file collection, all other
* files from the file collection on which they depend. File A depends on file B,
* if and only if, it uses the name of any type or global function or global data
* defined in file B. It might do that by calling a function or method of a type
* or by inheriting the type. Note that this intentionally does not record dependencies
* of a file on files outside the file set, e.g., language and platform libraries.
*
*   Namespaces:
*   GraphLibrary belongs to Graph package which contains the Graph class
*   used to create a graph instance from the dependency information obtained.
*
*   Scanner belongs to Parse package which contains the CodeAnalyzer classes
*   from which we obtain AST.
*
*   Utilties has been included for display purposes in the test stub of dependency
*   analysis.
*
//
//* Public Interface (see test stub for details):
* =============================================
* intitializeDependencyAnalysis();      // this method kickstarts the analysis.
* storeDependencyInformation();         // this method stores the dependency information in a NOSQLDB
* createGraph();                        // this method is used to create a graph from the NOSQLDB
* getGraph();                           // returns graph containing the dependency information
*
*/
/* Build Process
*  =============
*  Required Files: (Both direct and indirect)
* - TypeTable.h, TypeTable.cpp           // always required
* - TypeRecord.h, TypeRecord.cpp         // always required
* - TypeAnalysis.h, TypeAnalysis.cpp     // always required
* - Graph.h, Graph.cpp                   // always required
* - Tokenizer.h, Tokenizer.cpp           // always required
* - NoSqlDb.h, NoSqlDb.c;;               // always required
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
#include "DependencyAnalysis.h"
#include "../StrongComponents/StrongComponents.h"

//-----------< this function stores the dependency information in NOSQLDB from project #1 >-----------------------

NoSqlDb<std::string> DependencyAnalysis::storeDependencyInfo(std::unordered_map<std::string, TypeRecord> _typeTable, std::string currentTime)
{
	NoSqlDb<std::string> db;
	for (auto it : vec_) {
		std::vector<std::string> depVec = intializeDepAnalysis(it, _typeTable);
		Element<std::string> elem;
		elem.dependencyList = depVec;
		elem.name = it.substr(it.find_last_of("/\\") + 1);
		elem.timeDate = currentTime;
		elem.category = "fileName";
		elem.data = it;
		db.save(it, elem);
	}
	return db;
}

//--------------------< this functions kicks of the depAnalysis for file fs >-----------------------------------

std::vector<std::string> DependencyAnalysis::intializeDepAnalysis(std::string fs, std::unordered_map<std::string, TypeRecord> _typeTable)
{
	std::unordered_map<std::string, bool> dependencies;
	for (auto file : vec_) {
		auto it = dependencies.find(file);
		if (it != dependencies.end())
			it->second = false;
		else
			dependencies.emplace(file, false);
	}
	std::ifstream in(fs);
	toker.returnComments(false);
	toker.attach(&in);
	Util::Title("FileName:" + fs + " depends on: ");
	std::vector<std::string> depVector;
	do
	{
		std::string tok = toker.getTok();
		std::unordered_map<std::string, TypeRecord> _typeTableR = _typeTable;
		
		if (_typeTableR.find(tok) == _typeTableR.end()) {
			continue;
		}

		for (auto it : _typeTableR) {
			if (tok == it.first) { 
				std::vector<std::pair<std::string, std::string>> dependents = it.second.getPairs();
				addUsings(depVector,_typeTableR,fs,dependencies);
				for (unsigned int i = 0; i < dependents.size(); i++)
				{
					if (dependents[i].first == "") { continue; }
					if (fs != dependents[i].first && !dependencies.find(dependents[i].first)->second) {
						depVector.push_back(dependents[i].first);
						dependencies.find(dependents[i].first)->second = true;
					}
				}}}
	} while (in.good());
	if (depVector.size() == 0) std::cout << "\n No Dependencies!!" << std::endl;
	for (std::string dep : depVector) {
		std::cout << std::endl << dep;
	}
	return depVector;
}

//--------------------< this functions is a helper function for depAnalysis which takes care of the using declarations >-----------------------------------

void DependencyAnalysis::addUsings(std::vector<std::string> dep, std::unordered_map<std::string, TypeRecord> &_typeTableR, std::string fs, std::unordered_map<std::string, bool> dependencies) {
	for (auto it : _typeTableR) {
		if (it.second.getTypeName() == "Using Namespace") {
			std::vector<std::pair<std::string, std::string>> fileName = it.second.getPairs();
			for (unsigned int i = 0; i < fileName.size(); i++)
			{
				if (fs == fileName[i].first) {
					std::vector<std::pair<std::string, std::string>> dependents = _typeTableR[it.first.substr(6)].getPairs();
					for (unsigned int i = 0; i < dependents.size(); i++)
					{
						if (fs != dependents[i].first && !dependencies.find(dependents[i].first)->second) {
							dep.push_back(dependents[i].first);
							dependencies.find(dependents[i].first)->second = true;
						}
					}
				}
			}
		}
	}
}

//<----------Creates a graph by filling it with dependency data from NoSqlDb----------------->

void DependencyAnalysis::createGraph(NoSqlDb<std::string> db)
{
	Keys keys = db.keys();
	int i = 0;
	for (Key k : keys) {
		Vertex<std::string, std::string> v1(k);
		g_.addVertex(v1);
	}

	for (auto it : g_) {
		std::vector<std::string> dep = db.value(it.value()).dependencyList;
		int edgeName = 0;
		std::string eN = "edge";
		for (auto item : dep) {
			for (auto vertex : g_) {
				std::ostringstream out;
				out << eN << " " << edgeName;
				if (vertex.value() == item) {// use dynamic programming here instead????
					g_.addEdge(out.str(), it, vertex);
					edgeName++;
				}
			}
		}
	}
}

//<----------------------------returns graph created using NSQLDB------------------------------>

Graph<std::string, std::string> DependencyAnalysis::getGraph()
{
	return g_;
}

//<----------------------------TEST STUB for DependencyAnalysis------------------------------>

#ifdef TEST_DEPANAL
int main() {
	std::cout << "\nDepAnalyserTest\n" << std::endl;
	TypeAnalysis demo;
	std::vector<std::string> vec;
	std::vector<std::string> fullPathVec;
	vec.push_back("../TestFiles/A.h");
	vec.push_back("../TestFiles/B.h");
	vec.push_back("../TestFiles/A.cpp");
	vec.push_back("../TestFiles/B.cpp");

	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	Repository* pRepo = Repository::getInstance();
	for (auto it : vec) {
		std::string path = FileSystem::Path::getFullFileSpec(it);
		pRepo->package() = FileSystem::Path::getName(path);
		fullPathVec.push_back(path);
		pRepo->currentPath() = path;
		try{if (pParser) {if (!configure.Attach(path)) {
					std::cout << "\n  could not open file " << path << std::endl;continue;}}
			else {
				std::cout << "\n\n  Parser not built\n\n";return 1;}
			while (pParser->next())
				pParser->parse();
			std::cout << "\n";}
		catch (std::exception& ex) {
			std::cout << "\n\n    " << ex.what() << "\n\n";}
	}
	demo.filltypeTable(pRepo);
	TypeTable tt = demo.getMainTable();
	visual V;
	V.displayHeader();
	V.displayMetricsLine(tt.getTypeTable());
	DependencyAnalysis dp(fullPathVec);
	NoSqlDb<std::string> database = dp.storeDependencyInfo(tt.getTypeTable(),"3-6-2017");
	dp.createGraph(database);
	Graph<std::string, std::string> g = dp.getGraph();
	database.showDataBase();
	std::cout << "\n  Analysis completed\n";
	putline();
}
#endif // TEST_DEPANAL
