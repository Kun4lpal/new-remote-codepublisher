#pragma once
///////////////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database                     //
//                                                                   // 
// Created by : Kunal Paliwal CSE 687 - OOD, Spring 2017             //   
// Author : Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017//
///////////////////////////////////////////////////////////////////////
/*
* Description: this package defines the template class for key value pair database. It makes
* use of unordered_map to store key value pairs. 
*
* Package operations:
* -------------------
* This package contains:
* Element<Data> template class which defines the value in the key value pair. This structure 
* is fixed with only changes to datatype allowed.
*
* NoSqlDb<Data> template class which defines functions/utilites that can be performed on
* a database. Some of them are updateValue, showDatabase, add or remove value.
*
* Required Files:
* ---------------
* CppProperties.h : this file is used to make use of Property class which is used to define
* different members of the item value for the key.
* XMLDocument.h : this file is not required but is used for test stub. 
*
//* Public Interface (see test stub for details):
* =============================================
*
* struct Element {....}
* show();                                   //displays an elements and its attributes
* createElement();                          //creates a new element.
*
*  class NoSqlDb {.....}  
*  value();                                 //returns the value corresponding to the key.
*  count()                                  //return the size of the database.
*  noOfKeys()                               //size (similar to above).
*  deleteElement()                          // deletes the corresponding keyvalue pair.
*  updateElement()                          //updates value of they key k.
*  void removeRelationship()                // removes dependency.
*  addRelationship();                       // adds dependency.
*  void showDataBase();                     //display current item in the store.
*  void showList();                         // display current items with less information.
*  void displayKeys();                      // displays values for a set of keys k.
*  void buildDocument();                    //builds an xml file.
*  bool buildDocAtPath                      //builds xmlfile a specifed path.
*
* Functions:
* ----------
* The functions inside the NoSqlDb template class are : deleteElement, updateElement, addRelationship,
*  removeRelationship, showDatabase, buildDocument, displayKeys.
*
* Build Instructions:
* -------------------
* Uses XmlDocument,CppProperties so build XmlDocument and CppProperties project as static 
* library after undefining their test stub.
*/
/*
* Version history:
* Version 1.0, Released 2/5/2017.
*/
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <map>
#include <vector>
#include <iomanip>
#include <utility>
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../CppProperties/Property.h"

using namespace XmlProcessing;

//--------------< User-defined datatype for storage >------------------------

class Widget
{
public:
	Property<int> wint;
	Property<std::string> wstr;
};

//--------------< Element definition with information on the datatype >------------------------

template<typename Data>
class Element
{
public:
	using Name = std::string;
	using Category = std::string;
	using TimeDate = std::string;
	using Dependencies = std::vector<std::string>;

	Property<Name> name;            // metadata
	Property<Category> category;    // metadata
	Property<TimeDate> timeDate;    // metadata
	Property<Data> data;            // data
	Property<Dependencies> dependencyList;      // metadata

	std::string show();
	std::string showList();
	Element<Data> createElement(Name n, Category c, TimeDate t, Dependencies d, Data da);
	
};

//--------------< Used to display contents of the Element >------------------------

template<typename Data>
inline std::string Element<Data>::show()
{
	// show children when you've implemented them
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "category" << " : " << category;
	out << "\n    " << std::setw(8) << "timeDate" << " : " << timeDate;
	std::vector<std::string> VEC = dependencyList;
	for (auto it : VEC)
	{
		out << "\n    " << std::setw(8) << "dependency" << " : " << it;
	}
	out << "\n    " << std::setw(8) << "data" << " : " << data;
	out << "\n";
	return out.str();
}

//--------------< Used to display contents of the Element >------------------------

template<typename Data>
inline std::string Element<Data>::showList()
{
	// show children when you've implemented them
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "category" << " : " << category;
	out << "\n    " << std::setw(8) << "timeDate" << " : " << timeDate;
	out << "\n    " << std::setw(8) << "data" << " : " << data; 
	out << "\n";
	return out.str();
}

//--------------< Used to display contents of Widget type Element >------------------------

inline std::string Element<Widget>::show()
{
	// show children when you've implemented them
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "category" << " : " << category;
	out << "\n    " << std::setw(8) << "timeDate" << " : " << timeDate;
	std::vector<std::string> VEC = dependencyList;
	for (auto it : VEC)
	{
		out << "\n    " << std::setw(8) << "dependency" << " : " << it;
	}
	out << "\n    " << std::setw(8) << "wint" << " : " << static_cast<Widget>(data).wint;
	out << "\n    " << std::setw(8) << "wstr" << " : " << static_cast<Widget>(data).wstr;
	out << "\n";
	return out.str();
}

//--------------< Utility function to fill an element with values >------------------------

template<typename Data>
inline Element<Data> Element<Data>::createElement(Name n, Category c, TimeDate t, Dependencies d, Data da)
{
	name = n;
	category = c;
	timeDate = t;
	dependencyList = d;
	data = da;
	return Element<Data>();
}

//--------------< Template class for the Key-Value Database >------------------------

template<typename Data>
class NoSqlDb
{
public:
	using Key = std::string;
	using Keys = std::vector<std::string>;

	int test = 2;
	Keys keys();
	bool save(Key key, Element<Data> elem);
	Element<Data> value(Key key);
	size_t count();
	int noOfKeys();
	void deleteElement(Key k);      //deletes the corresponding key-value pair.
	void updateElement(Key k, Element<Data> elem); //updates value of they key k.
	void addRelationship(Key k1, Key k2);      // adds dependency.
	void removeRelationship(Key k1, Key k2);   // removes dependency.
	void showDataBase();               //display current item in the store.
	void showList();
	void displayKeys(Keys k);       // displays values for a set of keys k.
	void buildDocument(std::string xml, std::string filename); //builds a document. 
	bool buildDocAtPath(std::string xml, std::string filename, std::string path); //builds a document at specified path. 
private:
	using Item = std::pair<Key, Element<Data>>;   // this represents a key-value pair.
	std::unordered_map<Key, Element<Data>> store; 

};

//--------------< Returns the list of keys inside the database >------------------------

template<typename Data>
inline typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
{
	Keys keys;
	for (Item item : store)
	{
		keys.push_back(item.first);
	}
	return keys;
}

//--------------< Updates the key with a new value defined by user >------------------------

template<typename Data>
inline bool NoSqlDb<Data>::save(Key key, Element<Data> elem)
{
	if (store.find(key) != store.end())
		return false;
	store[key] = elem;
	return true;
}

//--------------< Returns value of a key >------------------------

template<typename Data>
inline Element<Data> NoSqlDb<Data>::value(Key key)
{
	if (store.find(key) != store.end())
		return store[key];
	return Element<Data>();
}

//--------------< Returns current size of the database >------------------------

template<typename Data>
inline size_t NoSqlDb<Data>::count()
{
	try
	{
		return store.count();
	}
	catch (std::exception ex)
	{
		std::cout << ex.what();
	}

}

//--------------< Returns size of the database (secondary function) >------------------------

template<typename Data>
inline int NoSqlDb<Data>::noOfKeys()
{
	try
	{
		return store.size();
	}
	catch (std::exception ex)
	{
		std::cout << ex.what();
	}
	return 0;
}

//--------------< Removes a key and its value >------------------------

template<typename Data>
inline void NoSqlDb<Data>::deleteElement(Key k) {
	try
	{
		store.erase(k);
		for (std::string st : keys())
		{
			Element<Data> *elem = new Element<Data>();
			*elem = store[st];
			std::vector<std::string> v(elem->dependencyList.getValue());
			if (std::find(v.begin(), v.end(), k) != v.end()) {
				v.erase(std::remove(v.begin(), v.end(), k), v.end());
			}
			elem->dependencyList = v;
			updateElement(st, *elem);
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what();
	}
}

//--------------< Updates value of a key >------------------------

template<typename Data>
inline void NoSqlDb<Data>::updateElement(Key k, Element<Data> elem)
{
	store[k] = elem;
}

//--------------< Adds a dependency to value of a key >------------------------

template<typename Data>
inline void NoSqlDb<Data>::addRelationship(Key k1, Key k2)
{
	Element<Data> elem = store[k1];
	std::vector<std::string> vec(elem.dependencyList.getValue());
	vec.push_back(k2);
	elem.dependencyList = vec;
	updateElement(k1, elem);
}

//--------------< Removes a dependency from the value of a key >------------------------

template<typename Data>
inline void NoSqlDb<Data>::removeRelationship(Key k1, Key k2)
{
	Element<Data> *elem = new Element<Data>();
	*elem = store[k1];
	std::vector<std::string> v(elem->dependencyList.getValue());
	if (std::find(v.begin(), v.end(), k2) != v.end()) {
		v.erase(std::remove(v.begin(), v.end(), k2), v.end());
	}
	elem->dependencyList = v;
	updateElement(k1, *elem);
}

//--------------< Displays keys and values in the current database >------------------------

template<typename Data>
inline void NoSqlDb<Data>::showDataBase()
{
	Keys set = keys();
	for (Key k : set)
	{
		std::cout << " Key   :-  " << k + "\n";
		std::cout << " Value =" + static_cast<Element<Data>>(value(k)).show();
		//putline();
	}
}

template<typename Data>
inline void NoSqlDb<Data>::showList()
{
	Keys set = keys();
	for (Key k : set)
	{
		std::cout << " Key   :-  " << k + "\n";
		std::cout << " Value =" + static_cast<Element<Data>>(value(k)).showList();
	}
}

//--------------< Displays keys and their values from a set of keys >------------------------

template<typename Data>
inline void NoSqlDb<Data>::displayKeys(Keys keys)
{
	for (Key k : keys)
	{
		std::cout << " Key   :-  " << k + "\n";
		std::cout << " Value =" + static_cast<Element<Data>>(value(k)).show();
		//putline
	}
}

//-------< Builds a document in the folder: Persisted Data with the specified filename >---------

template<typename Data>
inline void NoSqlDb<Data>::buildDocument(std::string xml, std::string filename)
{
	std::ofstream myfile("../Folder/" + filename + ".xml");
	if (myfile.is_open())
	{
		myfile << "<?xml version=\"1.0\" encoding=\"utf - 8\"?>\n";
		myfile << "<!--XML test case -->";
		myfile << xml;
		myfile.close();
	}
	else std::cout << "Unable to open file";
}

//-------< Builds a document in the folder: Persisted Data with the specified filename AND PATH >---------

template<typename Data>
inline bool NoSqlDb<Data>::buildDocAtPath(std::string xml, std::string filename, std::string path)
{
	std::ofstream myfile( path + "/" + filename + ".xml");
	if (myfile.is_open())
	{
		myfile << "<?xml version=\"1.0\" encoding=\"utf - 8\"?>\n";
		myfile << "<!--XML test case -->";
		myfile << xml;
		myfile.close();
		return true;
	}
	else std::cout << "\n[ Unable to open file ].";
	return false;
}
//end