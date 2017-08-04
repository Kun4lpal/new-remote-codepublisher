#pragma once
/////////////////////////////////////////////////////////////////////
// QueryEngine.h - supports queries on Key-Value Database          //
// Application: CSE687 - Object Oriented Design, Spring 2017,      // 
//              Project 1: Key Value Database                      //
// Author:      Kunal Paliwal, Syracuse University,                //
//              kupaliwa@syr.edu, (315) 876-8002                   //
/////////////////////////////////////////////////////////////////////
/*
*This package supports performing queries on Key-Value Database. It also provides support for
*regular expression matching. Query list: Value of a specified key, children of a specified key,
*set of all keys matching a specified pattern with defaults to all keys, all keys that contain a
*specific string in their item name or category or template data section when it makes sense. Support
*for fetching keys within a specified timedate interval.
*
* Package operations:
* -------------------
* This package contains:
* IQuery : base class for all three below.
* StringQuery : this class is used to perform queries on string type.
* DoubleQuery : this class is used to perform queries on double type.
* WidgetQuery : this class is used to perform queries on Widget type.
*
* Required Files:
* ---------------
*
* NoSqlDb.h : this file is required to perform queries on an instance of NoSqlDb template class.
*
* Persist.h : this file is not required but can be used to increase functionality of this package.
* XmlElement.h : this file is not required but can be used to increase functionality.
* Convert.h : this file is used to perform utility functions.
* XMLDocument.h : this file is not required but used in test stub of this package.
*
* Functions:
* ----------
* The functions inside the QueryEngine template class are: valueofKey, findChildren, ofPettern, 
* subStringinCategory, regex, orOperation, withinTimedate, isSubsequence.
*
* Build Instructions:
* -------------------
* Uses XmlDocument,NoSqlDb,Convert and Persist so build XmlDocument, NoSqlDb, Convert and Persist
* project as static library after undefining their test stub.
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/

#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include "../Persist/Persist.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include "../Proj_1/NoSqlDb.h"
#include "../Convert/Convert.h"
#include "../XmlDocument/XmlDocument/XmlDocument.h"

using namespace XmlProcessing;

using StrData = std::string;
using namespace XmlProcessing;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;

//----< Base class for StringQuery, DoubleQuery, WidgetQuery >----------------------------------------

template<typename T>
struct IQuery
{
	virtual ~IQuery() {}
	virtual NoSqlDb<T>& operator()() = 0;
	virtual Element<T> valueOfKey(Key key, bool value) = 0;  
	virtual Keys findChildren(Key key, bool value) = 0;
	virtual Keys ofPattern(const std::string str, Keys k, bool val) = 0; // returns keys that match a pattern
	virtual Keys subStringInItem(const std::string str, Keys k, bool val) = 0; // returns keys whose name match the specified string
	virtual Keys subStringInCategory(const std::string str, Keys x, bool val) = 0;// returns keys whose category match specifed string
	virtual bool regex(std::string x, std::regex y); // returns if regex y matches string x
	Keys orOperation(Keys a, Keys b); // utility for performing or operation between two sets of keys.
	bool isSubSequence(std::string str1, std::string str2, int m, int n); // returns true if str1 is a subsequence of str2.
	virtual Keys withinTimeDate(const std::string str1, const std::string str2, Keys x, bool val) = 0;
	std::string id() { return typeid(*this).name(); }
};

template<typename T>
inline Keys IQuery<T>::orOperation(Keys a, Keys b)
{
	a.insert(a.end(), b.begin(), b.end());
	std::sort(a.begin(), a.end());
	a.erase(unique(a.begin(), a.end()), a.end());
	return a;
}

//-------------< Regex function that returns true if x matches expression y >--------------------------
template<typename T>
inline bool IQuery<T>::regex(std::string x, std::regex y)
{
	std::string matched(x);
	std::smatch m;
	while (std::regex_search(x, m, y))
	{
		return (x == matched);
	}
	return false;
}

//--------< Checks if str1 is subsequence of str2  where m and n are size of str1 and str2>----------------------

template<typename T>
inline bool IQuery<T>::isSubSequence(std::string str1, std::string str2, int m, int n)
{
	// Base Cases
	if (m == 0) return true;
	if (n == 0) return false;

	// If last characters of two strings are matching
	if (str1[m - 1] == str2[n - 1])
		return isSubSequence(str1, str2, m - 1, n - 1);

	// If last characters are not matching
	return isSubSequence(str1, str2, m, n - 1);
}

/////////////////////////////////////////////////////////////////////
// StringQuery class

class stringQuery : public IQuery<StrData>
{
public:
	stringQuery(NoSqlDb<StrData>& kdb) : db_(kdb) {}
	stringQuery& operator=(const NoSqlDb<StrData>& s);
	Element<StrData> valueOfKey(Key key, bool value);
	Keys findChildren(Key key, bool value);
	NoSqlDb<StrData>& operator()();
	operator std::string();
	Keys ofPattern(const std::string str, Keys x, bool val);
	Keys subStringInItem(const std::string str, Keys c, bool val); // keys whos name match string str.
	Keys subStringInCategory(const std::string str, Keys x, bool val);
	Keys withinTimeDate(const std::string str1, const std::string str2, Keys x, bool val);
	Keys keysWithStringInDataMember(const std::string str, Keys x, bool val);// keys with specified string in data value.

private:
	NoSqlDb<StrData>  db_;  
};
//----< assignment overload >----------------------------------------

stringQuery& stringQuery::operator=(const NoSqlDb<StrData>& s)
{
	db_ = s;
	return *this;
}
//----< returns Keys that match a specific pattern (if value = true we use RegEx)>---------------------

inline Keys stringQuery::ofPattern(const std::string str, Keys x, bool value)
{
	Keys knew;
	if (value) {                     
		std::regex r(str);
		for (Key k : x) {
			if (IQuery::regex(k, r)) {
				knew.push_back(k);
			}
		}
	}
	else {                            // if value == false, we check if str is a subsequence of key.
		for (Key k : x) {
			if (isSubSequence(str, k, str.length(), k.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys whose name member match a string value (if value = true we use RegEx)>---------------------

inline Keys stringQuery::subStringInItem(const std::string str, Keys c, bool value)
{
	Keys knew;
	if (value) {                     
		std::regex r(str);
		for (Key k : c) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.name;
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else {
		for (Key k : c) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.name;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys whose category match a string value (if value = true we use RegEx)>---------------------

inline Keys stringQuery::subStringInCategory(const std::string str, Keys c, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : c) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.category;
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else {
		for (Key k : c) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.category;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys whose TimeDate value falls within str1 and str2 (if value = true we use RegEx)>---------------------

inline Keys stringQuery::withinTimeDate(const std::string str1, const std::string str2, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r1(str1);
		std::regex r2(str2);
		for (Key k : x) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.timeDate;
			if (IQuery::regex(test, r1) || IQuery::regex(test, r2)) {
				knew.push_back(k);
			}
		}
	}
	else {
		for (Key k : x) {
			Element<StrData> *elem = new Element<StrData>();
			*elem = db_.value(k);
			std::string test = elem->timeDate;
			if (test<str2 && test>str1) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys that have a datamember matching with the string str (if value = true we use RegEx)>------------

inline Keys stringQuery::keysWithStringInDataMember(const std::string str, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : x) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.data;
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : x) {
			Element<StrData> elem = db_.value(k);
			std::string test = elem.data;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//---------------------< returns Value of a Key >---------------------

inline Element<StrData> stringQuery::stringQuery::valueOfKey(Key key, bool value)
{
	if (value) {
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {
				return db_.value(k);
			}
		}
	}
	else
	{
		try {
			return db_.value(key);
		}
		catch (std::exception ex) {
			std::cout << ex.what();
		}
	}

	return Element<StrData>();
}
//---< this function returns a list of strings containing the dependencies of key >------------------

inline Keys stringQuery::findChildren(Key key, bool value)
{
	if (value) {                            //if value == true, we use regEx matching with key values.
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {
				return db_.value(k).dependencyList;
			}
		}
	}
	else
	{
		Element<StrData> ele = db_.value(key);
		Keys k = ele.dependencyList;
		return k;
	}
	return Keys();
}

//----< allows us to access the database without making any changes >---------------------

inline NoSqlDb<StrData>& stringQuery::operator()()
{
	return db_;
}

/////////////////////////////////////////////////////////////////////
// DoubleQuery class

class doubleQuery : public IQuery<double>
{
public:
	doubleQuery(NoSqlDb<double> kdb_) : db_(kdb_) {}
	doubleQuery & operator=(const NoSqlDb<double>& s);
	Element<double> doubleQuery::valueOfKey(Key key, bool value);
	Keys findChildren(Key key, bool value);
	NoSqlDb<double>& operator()();
	Keys ofPattern(const std::string str, Keys x, bool value);
	Keys subStringInItem(const std::string str, Keys y, bool value);
	Keys withinTimeDate(const std::string str1, const std::string str2, Keys x, bool value);
	Keys subStringInCategory(const std::string str, Keys c, bool value);

private:
	NoSqlDb<double>  db_;
};

//----< assignment operator >---------------------

doubleQuery& doubleQuery::operator=(const NoSqlDb<double>& s)
{
	db_ = s;
	return *this;
}
//----< returns value of the key specified>---------------------

inline Element<double> doubleQuery::valueOfKey(Key key, bool value)
{
	if (value) {
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {        //returns value of key that matches the regular expression.
				return db_.value(k);
			}
		}
	}
	else
	{
		try {
			return db_.value(key);
		}
		catch (std::exception ex) {
			std::cout << ex.what();
		}
	}
	return Element<double>();
}

//---< returns a list of string which represents the dependency list >---------------------

inline Keys doubleQuery::findChildren(Key key,bool value)
{
	if (value) {
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {
				return db_.value(k).dependencyList;
			}
		}
	}
	else
	{
		Element<double> ele = db_.value(key);
		Keys k = ele.dependencyList;
		return k;
	}
	return Keys();
}

//----< returns the Key Value database instance>---------------------

inline NoSqlDb<double>& doubleQuery::operator()()
{
	return db_;
}

//----< returns Keys that have category which matches the string value (if value = true we use RegEx)>---------------------

inline Keys doubleQuery::subStringInCategory(const std::string str, Keys c, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : c) {
			Element<double> elem = db_.value(k);
			std::string test = elem.category; //checking if category matches the regular experssion.
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : c) {
			Element<double> elem = db_.value(k);
			std::string test = elem.category;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}

	return knew;
}

//----< returns Keys that match a pattern specified(if value = true we use RegEx)>---------------------

inline Keys doubleQuery::ofPattern(const std::string str, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : x) {
			if (IQuery::regex(k, r)) {  //pushing keys with a pattern.
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (auto it : x) {
			if (isSubSequence(str, it, str.length(), it.length())) {
				knew.push_back(it);
			}
		}
	}
	return knew;
}

//----< returns keys whose name member matches the string value specified (if value = true we use RegEx)>---------------------

inline Keys doubleQuery::subStringInItem(const std::string str, Keys y, bool value)
{

	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : y) {
			Element<double> elem = db_.value(k);
			std::string test = elem.name;          
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : y) {
			Element<double> elem = db_.value(k);
			std::string test = elem.name;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys that have timedate which falls between values str1 and str2 (if value = true we use RegEx)>---------------------

inline Keys doubleQuery::withinTimeDate(const std::string str1, const std::string str2, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r1(str1);
		std::regex r2(str2);
		for (Key k : x) {
			Element<double> elem = db_.value(k);         //checks if timedata of a key matches either of str1 and str2.
			std::string test = elem.timeDate;
			if (IQuery::regex(test, r1) || IQuery::regex(test, r2)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : x) {
			Element<double> elem = db_.value(k);
			std::string test = elem.timeDate;            // checks if timedata falls between str1 and str2.
			if (test<str2 && test>str1) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

/////////////////////////////////////////////////////////////////////
// WidgetQuery class

class widgetQuery : public IQuery<Widget>
{
public:
	widgetQuery() {};
	widgetQuery(NoSqlDb<Widget> kdb) : db_(kdb) {}
	widgetQuery& operator= (const NoSqlDb<Widget>& s);
	Element<Widget> widgetQuery::valueOfKey(Key key,bool value);
	Keys findChildren(Key key,bool value);
	NoSqlDb<Widget>& operator()();
	Keys withinTimeDate(const std::string str1, const std::string str2, Keys x, bool value);
	Keys ofPattern(const std::string str, Keys x, bool value);
	Keys subStringInCategory(const std::string str, Keys c, bool value);
	Keys subStringInItem(const std::string str, Keys y, bool value);

private:
	NoSqlDb<Widget>  db_;
};
//----< assignment overload >----------------------------------------
inline widgetQuery& widgetQuery::operator=(const NoSqlDb<Widget>& s)
{
	db_ = s;
	return *this;
}

//----< returns the value of the Key specified>---------------------

inline Element<Widget> widgetQuery::valueOfKey(Key key,bool value)
{
	if (value) {
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {  //returns value of key matching the regular expression.
				return db_.value(k);
			}
		}
	}
	else
	{
		try {
			return db_.value(key);
		}
		catch (std::exception ex) {
			std::cout << ex.what();
		}
	}
	
	return Element<Widget>();
}

//----< returns Keys that match a pattern specified(if value = true we use RegEx)>---------------------

inline Keys widgetQuery::ofPattern(const std::string str, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : x) {
			if (IQuery::regex(k, r)) {        //returns keys matching a pattern.
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : db_.keys()) {
			if (isSubSequence(str, k, str.length(), k.length())) {
				knew.push_back(k);
			}
		}
	}
	return knew;
}

//----< returns Keys whose category match the string specified(if value = true we use RegEx)>---------------------

inline Keys widgetQuery::subStringInCategory(const std::string str, Keys c, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : c) {
			Element<Widget> elem = db_.value(k);
			std::string test = elem.category;    //returns keys whos category matches the regEX.
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : c) {
			Element<Widget> elem = db_.value(k);
			std::string test = elem.category;             
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}

	return knew;
}

//----< returns Keys whose name member match the string specified(if value = true we use RegEx)>---------------------

inline Keys widgetQuery::subStringInItem(const std::string str, Keys y, bool value)
{
	Keys knew;
	if (value) {
		std::regex r(str);
		for (Key k : y) {
			Element<Widget> elem = db_.value(k);          //returns keys whose name match the string specified.
			std::string test = elem.name;
			if (IQuery::regex(test, r)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : db_.keys()) {
			Element<Widget> elem = db_.value(k);
			std::string test = elem.name;
			if (isSubSequence(str, test, str.length(), test.length())) {
				knew.push_back(k);
			}
		}
	}

	return knew;

}

//----< returns a list of strings which represent the dependency of the key specified>---------------------

inline Keys widgetQuery::findChildren(Key key,bool value)
{
	if (value) {
		std::regex r(key);
		for (Key k : db_.keys()) {
			if (IQuery::regex(k, r)) {          //finds children of key which matches the regular expression
				return db_.value(k).dependencyList;
			}
		}
	}
	else
	{
		Element<Widget> ele = db_.value(key);
		Keys k = ele.dependencyList;
		return k;
	}
	return Keys();
}

inline NoSqlDb<Widget>& widgetQuery::operator()()
{
	return db_;
}

//----< returns Keys whose timedate are between str1 and str2(if value = true we use RegEx)>---------------------

inline Keys widgetQuery::withinTimeDate(const std::string str1, const std::string str2, Keys x, bool value)
{
	Keys knew;
	if (value) {
		std::regex r1(str1);
		std::regex r2(str2);
		for (Key k : x) {
			Element<Widget> elem = db_.value(k);
			std::string test = elem.timeDate;
			if (IQuery::regex(test, r1) || IQuery::regex(test, r2)) {
				knew.push_back(k);
			}
		}
	}
	else
	{
		for (Key k : x) {
			Element<Widget> elem = db_.value(k);
			std::string test = elem.timeDate;
			if (test<str2 && test>str1) {               //checks for keys whose timedate value falls between str1 and str2
				knew.push_back(k);
			}
		}
	}
	return knew;
}
