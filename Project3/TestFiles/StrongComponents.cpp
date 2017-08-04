////////////////////////////////////////////////////////////////////////////
// StrongComponent.cpp  - This file contains all the implementations of   //
// ver 1.0               the methods defined in the StrongComponent.h file//
//                                                                        //
// Language:      Visual C++.                                             //
// Application:   Dependency Analyzer, CSE687 - Object Oriented Design    //
// Author:        Kunal Paliwal, Syracyse University, CST 687             //
//                kupaliwa@syr.edu, (315) 876 8002                        //
////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* A strong component is the largest set of files that are all mutually dependent.
* That is, all the files which can be reached from any other file in the set by
* following direct or transitive dependency links. The term "Strong Component"
* comes from the theory of directed graphs. There are a number of algorithms
* for finding strong components in graphs. We make use of Tarjan Algorithm,
* suggested by professor. The pseudocode for this algorithm can be found from
* its wiki page.
*
*  Namespaces:
*  GraphLibrary belongs to Graph package which contains the Graph class
*  used to create a graph instance from the dependency information obtained.
*
//
//* Public Interface (see test stub for details):
* =============================================
* findStrongComponents();  // this method kickstarts the analysis of the graph and
*                             fills the strongComponent list with the results.
* strongConnect();         // this is a helper method called by findStrongComponents()
*                             It's a recursive function which is called if the index of a vertex
*                             is undefined.
* returnList();            // this method returns the list which was filled with strongComp info.
*
*/
/* Build Process
*  =============
*  Required Files: (Both direct and indirect)
* - Utilties.h                 // required for display purposes
* - Graph.h, Graph.cpp         // always required
*
* Maintenance History:
* ====================
* ver 1.0 : 05 March'17
* Final version.
*/
#include "StrongComponents.h"
using namespace GraphLib;

//<-------------------------------using STATEMENTS------------------------------>
using vertex = Vertex<std::string, std::string>;

//<-----------------------------------Destructor---------------------------------->

StrongComponents::~StrongComponents()
{
}

//<-----this function finds the strong components and stores them in a LIST----------->

void StrongComponents::findStrongComponents()
{
	index = 0;
	for (auto &it : g_) {
		try {
			if (it.index == NULL) {
				strongConnect(it.id());   // DEFINED BELOW !
			}
			else {				
				 //break;
			}
		}
		catch (std::exception ex) {
			std::cout << ex.what() << std::endl;
		}
	}
}

//<------------------ helper function to find strong components---------------------------------->

void StrongComponents::strongConnect(int id)  //NOTE: the input here is index value of a graph vertex
{
	g_[id].index = index;
	g_[id].lowLink = index;
	index++;
	g_[id].onStack = true;
	list.push(&g_[id]);

	for (auto &it : g_[id]) {   // v is the list of edges of vertex V
		if (g_[it.first].index == NULL) {
			strongConnect(it.first);
			g_[id].lowLink = std::min(g_[id].lowLink, g_[it.first].lowLink);
		}
		else if (g_[it.first].onStack) {
			g_[id].lowLink = std::min(g_[id].lowLink, g_[it.first].index);
		}
	}

	if (g_[id].lowLink == g_[id].index) {
		std::vector<vertex*> strongComponent;
		size_t value;
		do {
			value = list.top()->id();
			list.pop();
			g_[value].onStack = false;
			if (!g_[value].mark()) {
				strongComponent.push_back(&g_[value]);
			}
			g_[value].mark() = true;
		} while (value != g_[id].id());
		if (strongComponent.size() != 0) {
			strongCompList_.push_back(strongComponent);
		}
	}
}

//<---------------this function returs the strong component List---------------------->

std::vector<std::vector<vertex*>> StrongComponents::returnList()
{
	return strongCompList_;
}

//<------------------------TEST STUB for strong components---------------------------->

#ifdef STRONG_COMP
//<-------------------------this is a test function----------------------------------->

void StrongComponents::getVal()
{
	std::cout << "Successful!" << std::endl;
}

int main() {
	try
	{
		std::cout << "\n  Constructing Graph instance";
		std::cout << "\n -----------------------------";
		Graph<std::string, std::string> g;
		vertex v1("v1",0);vertex v2("v2",1);vertex v3("v3",2);vertex v4("v4",3);vertex v5("v5",4);
		g.addVertex(v2);
		g.addVertex(v1);
		g.addVertex(v3);
		g.addVertex(v4);
		g.addVertex(v5); g.addEdge("e1", v1, v2); g.addEdge("e2", v2, v1); g.addEdge("e3", v2, v3);
		g.addEdge("e4", v3, v4); g.addEdge("e5", v4, v5);
		Display<std::string, std::string> dp;
		dp.show(g);
		StrongComponents sc(g);
		sc.findStrongComponents();
		std::vector<std::vector<Vertex<std::string,std::string>*>> &vec = sc.returnList();
		int value = 1;
		std::cout << std::endl;
		for (auto it : vec) {
			std::ostringstream out;
			out << value;
			std::cout << "StrongComponent " << out.str() << std::endl;
			for (auto item : it) {
				std::cout << item->value()<<std::endl;
			}
	value++;
		}
	}catch(std::exception ex){
		std::cout << ex.what();}	
}
#endif // STRONG_COMP
