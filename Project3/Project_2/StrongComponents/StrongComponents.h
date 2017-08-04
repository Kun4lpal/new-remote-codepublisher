#ifndef STRONG_COMPONENTS
#define STRONG_COMPONENTS
#pragma once
////////////////////////////////////////////////////////////////////////////
// StrongComponent.h   -  This package finds all the strong components in //
// ver 1.0                a graph instance using tarjan algorithm         //
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
*
* class StrongComponents {.....}
*
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
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>
#include "../CodeAnalyzer/Utilities/Utilities.h"
#include "../Graph/Graph/Graph.h"

//<-----------------------------using STATEMENTS------------------------------>
using namespace GraphLib;
using vertex = Vertex<std::string, std::string>;

//<-----------------------------CLASS DEFINITION------------------------------>

class StrongComponents
{
//<---------------------------------function declaration---------------------->
public:
	StrongComponents(Graph<std::string, std::string> g) : g_(g) {};
	~StrongComponents();
	void findStrongComponents();
	void strongConnect(int id);
	std::vector<std::vector<vertex*>> returnList();

//<--------------------------------private Members------------------------------>
private:
	void getVal();
	int index;
	std::stack<Vertex<std::string, std::string>*> list;
	Graph<std::string, std::string> g_;  
	std::vector<std::vector<Vertex<std::string, std::string>*>> strongCompList_;

};
#endif