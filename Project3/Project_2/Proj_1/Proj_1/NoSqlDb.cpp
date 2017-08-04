/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database                 //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Description : Test stub for NoSqlDb.h. 
*
* Package operations:
* -------------------
* This package contains test stub for the NoSqlDb template class. It populates the database
* using createElement function and saves them in the database. Then we perform few basic
* function to test functions defined in the header file.
*
* Required Files:
* ---------------
* NoSqlDb.h - this file contains defintion for NoSqlDb template class.
* XmlDocument.h - this file is used to build a pdoc Element and to test its working.
* CppProperties.h - Although not used but useful to perform further testing.
*
* Build Instructions:
* -------------------
* Uses XmlDocument, NoSqlDb, CppProperties so set them as static library after 
* undefining their test stubs.
* 
* Version history:
* Version 1.0, Released 2/5/2017.
*/
#include "NoSqlDb.h"
#include <iostream>
#include <stdlib.h>
#include <functional>
#include <string>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../CppProperties/Property.h"

using namespace XmlProcessing;
using Sptr = std::shared_ptr<AbstractXmlElement>;
using Keys = std::vector<std::string>;

//--------------< This function was made for to test for linkage errors >-----------------------

void buildDocument() {
	Sptr root = makeTaggedElement("test");
	XmlDocument pdoc(root);

	Sptr first = makeTaggedElement("check1");
	Sptr firstText = makeTextElement("hahahaha");
	first->addChild(firstText);
	Sptr second = makeTaggedElement("check2");
	Sptr secondText = makeTextElement("reddit92");
	second->addChild(firstText);
	root->addChild(first);
	root->addChild(second);
	std::cout << pdoc.toString();
}

//--------------< Test stub for NoSqlDb >------------------------

#ifdef TEST_NOSQL

int main() {

	title("reddays are coming");
	NoSqlDb<std::string> ok;
	std::cout << ok.test;
	//check();
	buildDocument();

	NoSqlDb<double> dbdouble;
	Element<double> elem1;
	std::vector<std::string> v1;
	elem1.createElement("Jim","Professor","2-03-2017",v1,2.32);
	dbdouble.save("key1",elem1);

	Element<double> elem2;
	std::vector<std::string> v2;
	elem2.createElement("Doogard", "Professor", "12-05-2017", v2, 3.32);
	dbdouble.save("key2", elem2);
	dbdouble.showDataBase();
}

#endif // TEST_NOSQL