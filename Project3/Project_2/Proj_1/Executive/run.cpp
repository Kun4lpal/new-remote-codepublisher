/////////////////////////////////////////////////////////////////////
// RUN.cpp - This is the main executive package.                   //
//                                                                 //
// Kunal Palwial, CSE687 - Object Oriented Design, Spring 2017     //
/////////////////////////////////////////////////////////////////////
/*
* This package makes use of Persist, QueryEngine, NoSqlDb, Convert, XMLDocument and CppProperties
* to demonstrate all the requirements asked for Project 1.
*
* Package operations:
* -------------------
* This package consists of a template function called startPersisting which performs timed persistance by making use of the timer
* package and making use of the persist package.
*
* Required Files:
* ---------------
* QueryEngine.h : this file is used to perform all queries(with and without regex) on the 
* database of different types.
* NoSqlDb.h : this file is used to create instances of NoSqlDb<Data>.
* Persist.h : this file is used to persist all the items stored in the database.
* Convert : this file is used to convert a value from one type to another type.
* XmlDocument : this file is used to parse, persist and augment data .
* XmlParser : this file is used to parse XML and used with XmlDocument package to meet 
* requirement 5 and 6.
* widget.xml, lect.xml and MainXML are files used to agument database available in XmlFiles directory.
*
* Build Instructions:
* -------------------
* Make sure all the other packages are of set to type static library after undefining their teststubs.
* Note: ALL THE PERSISTED FILES ARE PRESENT IN PERSISTED DATA FOLDER.
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/

#include "run.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <functional>
#include <regex>
#include <fstream>
#include "../Proj_1/NoSqlDb.h"
#include "../Persist/Persist.h"
#include "../QueryEngine/QueryEngine.h"
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlParser/XmlParser.h"

using StrData = std::string;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;
using Sptr = std::shared_ptr<AbstractXmlElement>;

//----< Timed persistance with the NoSqlDb instance as argument (Set to false initially)>---------

template<typename T>
void startPersisting(bool value, NoSqlDb<T> db, std::string xml, unsigned long long time)
{

	while (value)
	{
		using namespace std::chrono_literals;
		std::cout << "Persisting!" << std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		db.buildDocument(xml, "TestNewProject");
		std::this_thread::sleep_for(std::chrono::duration<double>(time));
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = end - start;
		std::cout << "Waited " << elapsed.count() << " ms\n";
	}

}

//-----------------------------< Requirement 1 and 2 >---------------------------------------

void Requirement_1_2(){
	
	Title("Requirement 1 and 2: Shall implement a template class providing Key Value DB implemented in C++: ");
	putLine();
	std::cout << "\n  1) Type String : \n";
	std::cout << " Creating Elements of type string and adding them:\n";
	NoSqlDb<std::string> dbString;
	Element<std::string> elem1;
	std::vector<std::string> vec1;
	vec1.push_back("key2");
	elem1.name = "Jim";
	elem1.category = "Professor";
	elem1.timeDate = "2017-02-12";
	elem1.data = "Data for elem1";
	dbString.save("key1", elem1);

	Element<std::string> elem2;
	std::vector<std::string> vec2;
	elem2.createElement("Nubia", "Cat", "2015-03-21", vec2, "Data for elem2");
	dbString.save("key2", elem2);
	dbString.showDataBase();

	std::cout << "\n  2) Type Widget : \n";

	NoSqlDb<Widget> dbWidget;
	Widget wd1;
	wd1.wint = 2;
	wd1.wstr = "Mage";
	Element<Widget> elem3;
	std::vector<std::string> v3;
	elem3.createElement("Jimmy", "Student", "2017-09-21", v3, wd1);
	dbWidget.save("Key1",elem3);
	
	Widget wd2;
	wd2.wint = 24;
	wd2.wstr = "RedEye";
	Element<Widget> elem4;
	std::vector<std::string> v4;
	elem4.createElement("Hango", "Student", "2016-09-04", v4, wd2);
	dbWidget.save("Key2", elem4);
	dbWidget.showDataBase();

}

//-----------------------------< Requirement 3 >---------------------------------------

void Requirement_3() {
	Title("Requirement 3: Shall support addition and deletion of Key/Value Pairs ");
	putLine();
	NoSqlDb<Widget> dbWidget;
	Widget wd1;
	wd1.wint = 2;
	wd1.wstr = "Mage";
	Element<Widget> elem3;
	std::vector<std::string> v3;
	elem3.createElement("Jimmy", "Student", "2017-02-21", v3, wd1);
	dbWidget.save("Key1", elem3);

	Widget wd2;
	wd2.wint = 24;
	wd2.wstr = "RedEye";
	Element<Widget> elem4;
	std::vector<std::string> v4;
	elem4.createElement("Hango", "Student", "2016-11-15", v4, wd2);
	dbWidget.save("Key2", elem4);

	std::cout << "\n Before Deletion and Addition operations :\n";
	dbWidget.showDataBase();
	Widget wd3;
	wd2.wint = 21;
	wd2.wstr = "Eye";
	Element<Widget> elem5;
	std::vector<std::string> v5;
	v5.push_back("Key1");
	v5.push_back("Key2");
	elem5.createElement("Missa", "Student", "2016-09-09", v4, wd2);
	dbWidget.save("Key3", elem5);
	std::cout << "\nAfter Addition of Key3 :\n";
	dbWidget.showDataBase();
	std::cout << "\nAfter Deletion of Key2:\n";
	dbWidget.deleteElement("Key2");
	dbWidget.showDataBase();

}

//-----------------------------< Requirement 4 >---------------------------------------

void Requirement_4() {
	Title("Requirement 4: Support for Addition Deletion of Relationships and Update Values:");
	putLine();
	NoSqlDb<std::string> dbString;
	Element<std::string> elem1;
	std::vector<std::string> vec1;
	vec1.push_back("key2");
	vec1.push_back("key3");
	elem1.name = "Jim";
	elem1.dependencyList = vec1;
	elem1.category = "Professor";
	elem1.timeDate = "2017-02-21";
	elem1.data = "red";
	dbString.save("key1", elem1);

	Element<std::string> elem2;
	std::vector<std::string> vec2;
	vec2.push_back("key3");
	vec2.push_back("key4");
	elem2.createElement("Nub", "Cat", "2015-03-21", vec2, "megaGtron");
	dbString.save("key2", elem2);

	Element<std::string> elem3;
	std::vector<std::string> vec3;
	vec3.push_back("key4");
	elem3.createElement("Koko", "Dog", "2016-09-21", vec3, "DubFin");
	dbString.save("key3", elem3);

	Element<std::string> elem4;
	std::vector<std::string> vec4;
	elem4.createElement("Hitler", "Human", "2019-09-11", vec4, "German");
	dbString.save("key4", elem4);
	std::cout << "\n Current Database : \n";
	dbString.showDataBase();

	std::cout << "\n After adding key4 and removing key3 from \"key1's\" dependency list:\n";
	dbString.removeRelationship("key1", "key3");
	dbString.addRelationship("key1", "key4");
	dbString.showDataBase();

	std::cout << "\n Editing Value of key1 to: name = NewValue, category = New, time = 11-09-2012, data = American and removing dependencies:\n";
	Element<std::string> elem5;
	std::vector<std::string> vec5;
	elem5.createElement("NewValue", "New", "2012-09-11", vec5, "American");
	dbString.updateElement("key1",elem5);
	dbString.showDataBase();
}

//-----------------------------< Requirement 5 and 6 with string type DB >------------------------

void Requirement_5_and_6_with_type_string() {
	Title("Shall on command Persist and Augment database to an XML file:");
	putLine();
	std::cout << "\n Current Database :\n";
	NoSqlDb<double> dB;
	Element<double> elem1;
	std::vector<std::string> vec1;
	vec1.push_back("key1");
	elem1.createElement("Jim", "Professor", "2017-02-21",vec1, 2.12);
	dB.save("key1", elem1);

	Element<double> elem2;
	std::vector<std::string> vec2;
	vec2.push_back("key3");
	vec2.push_back("key4");
	elem2.createElement("Nub", "Cat", "2015-03-21", vec2, 1.45);
	dB.save("key2", elem2);
	dB.showDataBase();
	std::cout << "\n Augmenting Database form xml of type Double: \n";
	DoubleWrapper dw;
	Title("Adding elements from XML");
	putLine();
	std::string src2 = "XmlFiles/lect.xml";    // NOTE TO TA: CHANGE THIS WHEN RUNNING THROUGH VISUAL STUDIO
	XmlParser parser2(src2);
	parser2.verbose();
	std::cout << "Compressed form of formatted XML:" << std::endl;
	std::cout << "\n" << src2 << "\n";
	std::cout << "Parsing compressed XML:" << std::endl;
	XmlDocument *pDoc = parser2.buildDocument();
	dB = dw.xmlProc(pDoc,dB);        // note always have to update db here.
	std::cout << "\nAugmented database for type double from file lect.xml: \n";
	dB.showDataBase();
	std::string str = dw.toXml(dB);
	std::cout << str << std::endl;
	std::cout << "\nNow Persisting this database to a file with name: Requirement5 with Kvdb of type Double\n";
	dB.buildDocument(str,"Requirement5 with Kvdb of type Double");
	Title("Note : Timed saving is OFF by default: GO TO LINE 259 in run.cpp and change it to TRUE instead");
	putLine();
	std::cout << "\nChecks if database has at least two key-value pair and then start persisting with the specified time delay.\n";
	startPersisting<double>(false, dB, str,2); //timed persistance is off in this case.
}

//-----------------------------< Requirement 5 and 6 with Widget type DB >------------------------

void Requirement_5_and_6_with_type_Widget() {
	Title("Requirement 5 and 6 for type Widget: ");
	putLine();
	std::cout << " Current DataBase : \n";
	NoSqlDb<Widget> nwid;
	Widget nw;
	nw.wint = 22;
	nw.wstr = "widgetString";
	Element<Widget> ew1;
	std::vector<std::string> vwid;
	ew1.createElement("Widget1", "Widget", "2017-09-21", vwid, nw);
	nwid.save("key1", ew1);

	Element<Widget> ew2;
	std::vector<std::string> vwid2;
	ew2.createElement("Widget2", "Widget", "2017-09-04", vwid2, nw);
	nwid.save("key2", ew2);
	widgetQuery wquery(nwid);
	Keys ok2 = wquery.ofPattern("key", nwid.keys(),false); //SET TO FALSE since we aren't using regEx
	nwid.displayKeys(ok2);

	std::cout<<"\nAdding elements from XML: \n";
	std::string src2 = "XmlFiles/widget.xml";// NOTE TO TA: CHANGE THIS WHEN RUNNING THROUGH VISUAL STUDIO
	XmlParser parser2(src2);
	parser2.verbose();
	std::cout << "Compressed form of formatted XML:" << std::endl;
	std::cout << "\n" << src2 << "\n";
	std::cout << "Parsing compressed XML:" << std::endl;
	XmlDocument *wDoc = parser2.buildDocument();
	
	WidgetWrapper newWwdgt;
	nwid = newWwdgt.xmlProc(wDoc, nwid);
	std::string xml = newWwdgt.toXml(nwid);
	std::cout << "\n Augmented Databse for type Widget from file widget.xml:\n";
	nwid.showDataBase();
	std::cout << "\n Persisting this database to a file with name: Requirement 5 with type Widget\n";
	nwid.buildDocument(xml, "Requirement 5 with type Widget");
	Title("Note : Timed saving is OFF by default: GO TO LINE 304 in run.cpp and change it to TRUE instead");
	putLine();
	std::cout << "\nChecks if database has at least two key-value pair and then start persisting with a time delay.\n";
	startPersisting<Widget>(false, nwid, xml,2); //timed persistance is off in this case.
}

//--------------< Requirement 7, 8 and 9 with double type DB (Queries without regEx) >------------------------


NoSqlDb<double> generateElements() {
	Title("Requirement 7: Performing Queries on this database of type double:"); putLine();
	NoSqlDb<double> dB; Element<double> elem1; std::vector<std::string> vec1;
	vec1.push_back("key2");
	elem1.createElement("Jim", "Professor", "2017-02-21", vec1, 2.12);
	dB.save("key1", elem1); Element<double> elem2; std::vector<std::string> vec2;
	vec2.push_back("key3");
	elem2.createElement("Nub", "Student", "2015-03-21", vec2, 1.45);
	dB.save("Nub", elem2);
	Element<double> elem3;
	std::vector<std::string> vec3;
	elem3.createElement("Nisha", "Student", "2013-04-29", vec3, 9.99);
	dB.save("key3", elem3);
	return dB;
}

void Requirement_7_8_and_9() {
	NoSqlDb<double> dB = generateElements();
	dB.showDataBase();putLine();
	doubleQuery dw(dB);putLine();

	std::cout << "Query1: Fetch value of Key -> key3\n";
	Element<double> elem = dw.valueOfKey("key3",false);
	std::cout<<elem.show();
	std::cout << "Query2: Dependencies of Key -> Nub \n";
	std::vector<std::string> Keys = dw.findChildren("Nub",false);
	dB.displayKeys(Keys);
	std::cout << "Query3: Set of all keys matching the specified pattern -> \"key\" \n";
	std::vector<std::string> patternKeys = dw.ofPattern("key",dB.keys(),false);
	dB.displayKeys(patternKeys);
	std::cout << "Query4: All keys with matching string in item name -> \"N\"\n";
	std::vector<std::string> itemKeys = dw.subStringInItem("N",dB.keys(), false);
	dB.displayKeys(itemKeys);
	std::cout << "Query 5: All keys with specified string in their category name -> \"Student\"\n";
	std::vector<std::string> catKeys = dw.subStringInCategory("Student", dB.keys(), false);
	dB.displayKeys(catKeys);
	std::cout << "Query 6: Displayed after Requirement 9 - forming a union of keys\n";
	std::cout << "Query 7: All Keys within a specified time period of -> \"2015-01-01 to 2016-12-26\"\n";
	std::vector<std::string> timeKeys = dw.withinTimeDate("2015-01-01", "2016-12-26",dB.keys(), false);
	dB.displayKeys(timeKeys);putLine();
	Title("Requirement 8 - Shall support making the above queries on the set returned by a previous query: ");putLine();
	std::cout << "Fetching results for keys in database matching pattern \"key\" AND of category \"Student\": ";
	std::vector<std::string> newKeys = dw.subStringInCategory("Student",patternKeys,false);
	dB.displayKeys(newKeys); putLine();
	Title("Requirement 9 - Shall support forming a union of Keys:");putLine();
	std::cout << "Fetching results for keys in database within time period \"2015-01-01 to 2016-12-26\" OR of category \"Student\": ";
	std::vector<std::string> unionKeys = dw.orOperation(timeKeys,catKeys);
	dB.displayKeys(unionKeys);putLine();
}

//-----------------------------< Requirement Query number 6 >------------------------

void RequirementQuery_6() {
	std::cout << "Query 6: Specified string in their data section:\n";

	std::cout << " Note this query is on a new Database of type string. Here is the current database :\n";
	NoSqlDb<std::string> dbString;
	Element<std::string> str1;
	std::vector<std::string> vecstr1; vecstr1.push_back("key2");
	str1.name = "Jim";
	str1.category = "Professor";
	str1.timeDate = "2017-02-21";
	str1.data = "red";
	str1.dependencyList = vecstr1;
	dbString.save("key1", str1);

	Element<std::string> str2;
	std::vector<std::string> vecstr2;
	str2.createElement("Mingo", "Cat", "2015-03-21", vecstr2, "megaTron");
	dbString.save("key2", str2);
	dbString.showDataBase();
	putLine();
	std::cout << "\n Displaying query result for match with data containing string \"red\" in the above database\n";
	stringQuery sw(dbString);
	std::vector<std::string> matchedKeys = sw.keysWithStringInDataMember("red", dbString.keys(), false);
	dbString.displayKeys(matchedKeys);
}

//-----------------------------< Requirement 10, file available in XmlFiles folder >-------------------

void Requirement_10() {
	Title("Requirement 10- Shall be submitted with contents in the form of an XML file that describe the project structure");
	putLine();

	NoSqlDb<double> db;
	Title("Adding elements from XML");
	putLine();
	std::string src = "XmlFiles/MainXml.xml";
	XmlParser parser(src);
	parser.verbose();
	std::cout << "Compressed form of formatted XML:" << std::endl;
	std::cout << "\n" << src << "\n";
	std::cout << "Parsing compressed XML:" << std::endl;
	XmlDocument *pDoc = parser.buildDocument();

	DoubleWrapper dw;
	db = dw.xmlProc(pDoc, db);
	Title(" DISPLAYING DATABASE FROM MAINXML.XML");
	putLine();
	db.showDataBase();
	Title(" DISPLAYING XDOC element in STRING:");
	putLine();
	std::string example = dw.toXml(db);
	std::cout << example;

}

//--------------< Requirement 12 with Widget type DB (Queries with regEx matching) >------------------------

void Comments_12() {
	Title("Bonus Requirement: Regular Expression Test");
	putLine();
	Title("Requirement 12 : Performing Queries with regular expression matching:");
	putLine();
}

void Comments_12_1() {
	Title("Requirement 8 with regEx- Shall support making the above queries on the set returned by a previous query: "); putLine();
	std::cout << "Fetching results for keys in db matching pattern \"key\" and of category \"Student\": ";
}

NoSqlDb<Widget> createElements() {
	NoSqlDb<Widget> dB; Element<Widget> elem1; std::vector<std::string> vec1;
	vec1.push_back("key2");
	Widget w1; w1.wint = 2;
	w1.wstr = "Element1";
	elem1.createElement("Jim Wicky", "Teacher", "2017-02-21", vec1, w1);
	dB.save("key1", elem1);

	Element<Widget> elem2; std::vector<std::string> vec2;
	vec2.push_back("key3");
	Widget w2; w2.wint = 22; w2.wstr = "Element2";
	elem2.createElement("RedEye", "Student", "2015-03-07", vec2, w2);
	dB.save("second", elem2);

	Element<Widget> elem3;
	std::vector<std::string> vec3;
	Widget w3; w3.wint = 14; w3.wstr = "Element3";
	elem3.createElement("Nisha", "Student", "2013-09-21", vec3, w3);
	dB.save("key3", elem3);
	return dB;
}

void Requirement_12() {
	Comments_12();
	NoSqlDb<Widget> dB = createElements();
	dB.showDataBase(); putLine();

	widgetQuery dw(dB); putLine();
	std::cout << "Query1: Fetch value of Key -> \\b(key3)([^ ]*)\n";
	Element<Widget> elem = dw.valueOfKey("\\b(key3)([^ ]*)",true);
	std::cout << elem.show();
	std::cout << "Query2: Dependencies of Key -> \\b(second)([^ ]*) \n";
	std::vector<std::string> Keys = dw.findChildren("\\b(second)([^ ]*)",true);
	dB.displayKeys(Keys);
	std::cout << "Query3: Set of all keys matching a specified pattern: \\b(key)([^ ] \n";
	std::vector<std::string> patternKeys = dw.ofPattern("\\b(key)([^ ]*)", dB.keys(), true);
	dB.displayKeys(patternKeys);
	std::cout << "Query4: All keys with matching string in item name: \\b(N)([^ ]*)\n";
	std::vector<std::string> itemKeys = dw.subStringInItem("\\b(N)([^ ]*)", dB.keys(), true);
	dB.displayKeys(itemKeys);
	std::cout << "Query 5:specified string in their category name: \\b(Student)([^ ]*)\n";
	std::vector<std::string> catKeys = dw.subStringInCategory("\\b(Student)([^ ]*)", dB.keys(), true);
	dB.displayKeys(catKeys);
	std::cout << "Query 6: doesnt make sense with widget type.\n";
	std::cout << "Query 7: All Keys within a specified time period \\b(2017-02-21)([^ ]*) , \\b(2020-09-20)([^ ]*)\n";
	std::vector<std::string> timeKeys = dw.withinTimeDate("\\b(2017-02-21)([^ ]*)", "\\b(2020-09-20)([^ ]*)", dB.keys(), true);
	dB.displayKeys(timeKeys); putLine();
	Comments_12_1();
	std::vector<std::string> newKeys = dw.subStringInCategory("\\b(Student)([^ ]*)", patternKeys, true);
	dB.displayKeys(newKeys); putLine();
	Title("Bonus Requirement 9 with RegEx - Shall support forming a union of Keys with regex queries:"); putLine();
	std::vector<std::string> unionKeys = dw.orOperation(timeKeys, catKeys);
	dB.displayKeys(unionKeys); putLine();
}

//--------------< THE MAIN TEXT EXECUTIVE THIS IS WHERE WE TEST ALL OUR PACKAGES >------------------------

int main() {

	Title("Executive run CPP");
	putLine();
	Requirement_1_2();
	Requirement_3();
	Requirement_4();
	Requirement_5_and_6_with_type_string();
	Requirement_5_and_6_with_type_Widget();
	Requirement_7_8_and_9();
	RequirementQuery_6();
	Requirement_10();
	Requirement_12();
	std::cin.get();
}
