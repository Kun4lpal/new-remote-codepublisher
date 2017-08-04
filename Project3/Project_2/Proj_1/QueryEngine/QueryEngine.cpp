/////////////////////////////////////////////////////////////////////
// QueryHelp.cpp - demo one way to persist objects to XML file     //
//                                                                 //
// Kunal Palwial, CSE687 - Object Oriented Design, Spring 2017     //
/////////////////////////////////////////////////////////////////////
/*
* Description: Test stub for QueryEngine Package.
*
* Package operations:
* -------------------
* This package performs tests on QueryEngine package. Note: tests for RegEx are demonstrated in
* the test executive.
*
* Required Files:
* ---------------
* QueryEngine.h : this file has all the function defintions on which tests are performed.
*
* Build Instructions:
* -------------------
* For testing change test stub definition and set follow the build instructions for QueryEngine.h.
* Set all other packages as static library.
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/
#include "QueryEngine.h"

using Key = std::string;
using Keys = std::vector<std::string>;

//--------------< Test stub for QueryEngine >------------------------

#ifdef TEST_QUERY

NoSqlDb<std::string> generateDb() {
	NoSqlDb<std::string> dbString;
	Element<std::string> elem1;
	std::vector<std::string> vec1;
	elem1.name = "Jim";
	elem1.category = "Professor";
	elem1.timeDate = "2017-02-21";
	elem1.data = "red";
	dbString.save("key1", elem1);

	Element<std::string> elem2;
	std::vector<std::string> vec2;
	elem2.createElement("Nub", "Cat", "2015-03-21", vec2, "megaGtron");
	dbString.save("key2", elem2);

	return dbString;
}

//--------------< main for TestStub of QueryEngine >------------------------

int main() {
	title("StringQuery Test");
	NoSqlDb<std::string> dbString = generateDb();
	std::cout << "\n Displaying database :\n";
	dbString.showDataBase();
	stringQuery sq(dbString);
	std::cout << "\n Displaying key with pattern ke\n";
	Keys k = sq.ofPattern("ke", dbString.keys(), false);
	dbString.displayKeys(k);
	title("DoubleQuery Test");
	std::cout << "\n";
	NoSqlDb<double> dbDouble;
	Element<double> dou1;
	std::vector<std::string> vecd1;
	dou1.name = "Jim";
	dou1.category = "Professor";
	dou1.timeDate = "2017-02-21";
	dou1.data = 2.32;
	dbDouble.save("key1", dou1);
	Element<double> dou2;
	std::vector<std::string> vecd2;
	dou2.createElement("Nub", "Cat", "2015-03-21", vecd2, 2.41);
	dbDouble.save("key2", dou2);
	std::cout << "\n Displaying database :\n";
	dbString.showDataBase();
	doubleQuery dq(dbDouble);
	std::cout << "\n Displaying keys of pattern ke:\n";
	Keys kdouble = dq.ofPattern("ke", dbDouble.keys(), false);
	dbDouble.displayKeys(kdouble);
	title("WidgetQuery Test");
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
	std::cout << "\n Current database :\n";
	nwid.showDataBase();
	widgetQuery wquery(nwid);
	std::cout << "\n Displaying key with value key1:\n";
	Element<Widget> D = wquery.valueOfKey("key1",false);
	std::cout << D.show();
	std::cout << "\n Displaying keys of pattern (2):\n";
	Keys ok2 = wquery.ofPattern("2", nwid.keys(),false);
	nwid.displayKeys(ok2);
}
#endif // TEST_QUERY
