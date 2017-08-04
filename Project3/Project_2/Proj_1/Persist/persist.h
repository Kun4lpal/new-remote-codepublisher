#pragma once
/////////////////////////////////////////////////////////////////////
// Persist.h - demo one way to persist NoSqlDb to XML file.        //
// Created by - Kunal Paliwal CSE 687 - OOD, Spring 2017           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* This package builds wrapper classes for strings, doubles, and Widgets.
* They provide the same functionality as the wrapped type, but also
* attach methods save() and restore() to persist the wrapped type.
*
* Package operations:
* -------------------
* This package contains:
* - IPersist interface - defines the methods needed to
*   serialize to XML and deserialize from XML
* - StringWrapper class that persists strings
* - DoubleWrapper class that persists doubles
* - Widget class with int and string properties
* - WidgetWrapper class that persits Widgets
*
* Required Files:
* ---------------
*   PersistHelp.h, PersistHelp.cpp,
*   Convert.h, Convert.cpp,
*   CppProperties.h, CppProperties.cpp,
*   XmlDocument.h, XmlDocument.cpp, XmlElement.h, XmlElement.cpp,
*  ---- required to build XmlDocument from file or string ----
*   XmlParser.h, XmlParser.cpp,
*   XmlElementParts.h, XmlElementParts.cpp,
*   ITokenizer.h, Tokenizer.h, Tokenizer.cpp.
*
* Functions:
* ----------
* The functions inside the Persist template class are save, restore, toXMl and xmlProc.
*
* Build Instructions:
* -------------------
* - Uses XmlDocument, so build XmlDocument project as static library
*   after undefining its test stub
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/

#include <string>
#include "../Proj_1/NoSqlDb.h"
#include "../Convert/Convert.h"

using StrData = std::string;
using Xml = std::string;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;

namespace Persist {

	//--------------< Base class for Persistance of all datatypes >------------------------

	template<typename T>
	struct IPersist
	{
		//using Xml = std::string;
		virtual ~IPersist() {}
		virtual IPersist<T>& operator=(const T& t) = 0;
		virtual operator T() = 0;
		virtual Xml save() = 0;
		virtual void restore(const Xml& xml) = 0;
		std::string id() { return typeid(*this).name(); }
	};

	//--------------< Class for persistance of string datatype >------------------------
	/////////////////////////////////////////////////////////////////////
	// StringWrapper class
	//

	class StringWrapper : public IPersist<std::string>
	{
	public:
		StringWrapper(const std::string& str = "") : str_(str) {}
		StringWrapper& operator=(const std::string& s);  // covariant return type
		Xml save();
		void restore(const Xml& xml);
		operator std::string();
		NoSqlDb<std::string> xmlProc(XmlDocument * pDoc, NoSqlDb<std::string> kd);
		std::string toXml(NoSqlDb<StrData> x);

	private:
		std::string str_;
	};

	//--------------< Assignment operator >------------------------

	StringWrapper& StringWrapper::operator=(const std::string& s)
	{
		str_ = s;
		return *this;
	}
	//----< saves instance state as XML string >-------------------------

	inline Xml StringWrapper::save()
	{
		return "<StringWrapper>" + str_ + "</StringWrapper>";
	}
	//---< restores instance state from XML string >---------------------

	inline void StringWrapper::restore(const Xml& xml)
	{
		size_t posStart = xml.find_first_of(">") + 1;
		size_t posEnd = xml.find_last_of("</");
		str_ = xml.substr(posStart, posEnd - posStart - 1);
	}

	//----< cast operator returns string member >------------------------

	inline StringWrapper::operator std::string()
	{
		return str_;
	}

	//--------------< This function is to AUGMENT DATA FROM PDOC ELEMENT >------------------------

	NoSqlDb<std::string> StringWrapper::xmlProc(XmlDocument *pDoc, NoSqlDb<std::string> kd)
	{
		for (auto pItem : pDoc->elements("root").select()) {        //pItem represent all tag elements of root
			Element<std::string> *ed = new Element<std::string>;
			std::vector<std::string>  strVec;
			for (auto pElem : pDoc->elements(pItem->tag()).select()) {
				if (pElem->tag() == "name") {    // if pItem matches "name" its text element is assigned to name property of element
					ed->name = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "dependency") { //update dependency list
					strVec.push_back(pElem->children()[0]->value());
					ed->dependencyList = strVec;
				}
				else if (pElem->tag() == "category") { //update category
					ed->category = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "timedate") { //update timedate
					ed->timeDate = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "data") {   //update data
					for (auto pItem : pElem->children()) {
						std::string data = pItem->children()[0]->value();
						ed->data = data;
					}
				}
			}
			kd.save(ed->name, *ed); //save key value pair with key as the name of the value item.
		}
		return kd;
	}

	//--------------< This function is used to convert database to a string format >------------------------

	std::string StringWrapper::toXml(NoSqlDb<StrData> x)
	{
		if (x.noOfKeys() < 2) { return "\n  Number of wrties less than 2, add more data....\n"; }
		std::string xml;
		XmlDocument doc;   // instantiate a doc element.

		Keys ikeys = x.keys();
		SPtr pRoot = makeTaggedElement("test");  // root tag defined as test
		doc.docElement() = pRoot;
		for (Key key : ikeys)
		{
			Element<StrData> ex = x.value(key);
			SPtr pItem = makeTaggedElement(ex.name);
			pRoot->addChild(pItem);
			SPtr pNameElem = makeTaggedElement("name"); // assigning tags name,category, time data and data.
			pItem->addChild(pNameElem);
			SPtr pTextElem = makeTextElement(ex.name);
			pNameElem->addChild(pTextElem);
			SPtr pcategoryElem = makeTaggedElement("category");
			pItem->addChild(pcategoryElem);
			pTextElem = makeTextElement(ex.category);
			pcategoryElem->addChild(pTextElem);
			SPtr ptimedateElem = makeTaggedElement("timedate");
			pItem->addChild(ptimedateElem);
			pTextElem = makeTextElement(ex.timeDate);
			ptimedateElem->addChild(pTextElem);
			std::vector<std::string> Vdep = ex.dependencyList;
			for (std::string x : Vdep) {
				SPtr pDependencyElem = makeTaggedElement("dependency");
				pItem->addChild(pDependencyElem);
				pTextElem = makeTextElement(x);
				pDependencyElem->addChild(pTextElem);
			}
			SPtr pdataElem = makeTaggedElement("data");
			pItem->addChild(pdataElem);
			pTextElem = makeTextElement(ex.data);
			pdataElem->addChild(pTextElem);
		}
	 xml = doc.toString(); return xml;
	}

	//--------------< This class is for persisting and augmenting database of type double >------------
	/////////////////////////////////////////////////////////////////////
	// DoubleWrapper class
	//
	class DoubleWrapper : public IPersist<double>
	{
	public:
		DoubleWrapper(double d = 0) : d_(d) {}
		DoubleWrapper& operator=(const double& d);
		Xml save();
		void restore(const Xml& xml);
		operator double();
		NoSqlDb<double> xmlProc(XmlDocument * pDoc, NoSqlDb<double> kd);
		std::string toXml(NoSqlDb<double> x);
	private:
		double d_;
	};
	//----< assignment overload >----------------------------------------

	DoubleWrapper& DoubleWrapper::operator=(const double& d)
	{
		d_ = d;
		return *this;
	}
	//----< saves instance state as XML string >-------------------------

	inline Xml DoubleWrapper::save()
	{
		return "<DoubleWrapper>" + Convert<double>::toString(d_) + "</DoubleWrapper>";
	}
	//---< restores instance state from XML string >---------------------

	inline void DoubleWrapper::restore(const Xml& xml)
	{
		size_t posStart = xml.find_first_of(">") + 1;
		size_t posEnd = xml.find_last_of("</");
		std::string bodyString = xml.substr(posStart, posEnd - posStart - 1);
		d_ = Convert<double>::fromString(bodyString);
	}
	//----< cast operator returns double member >------------------------

	inline DoubleWrapper::operator double()
	{
		return d_;
	}

	//--------------< This function is to AUGMENT DATA FROM PDOC ELEMENT >------------------------

	NoSqlDb<double> DoubleWrapper::xmlProc(XmlDocument *pDoc, NoSqlDb<double> kd)
	{
		for (auto pItem : pDoc->elements("root").select()) {      //pItem represent all tag elements of root
			Element<double> *ed = new Element<double>;
			std::vector<std::string>  strVec;
			for (auto pElem : pDoc->elements(pItem->tag()).select())
			{
				if (pElem->tag() == "name") { // if pItem matches "name" its text element is assigned to name property of element
					ed->name = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "dependency") {//update dependency
					strVec.push_back(pElem->children()[0]->value());
					ed->dependencyList = strVec;
				}
				else if (pElem->tag() == "category") { //update category
					ed->category = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "timedate") {
					ed->timeDate = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "data") {
					for (auto pItem : pElem->children()) {
						std::string data = pItem->children()[0]->value();
						ed->data = Convert<double>::fromString(data);
					}
				}
			}
			kd.save(ed->name, *ed);
		}
		return kd;
	}

	//--------------< This function is used to convert database to a string format >------------------------

	std::string DoubleWrapper::toXml(NoSqlDb<double> x)
	{
		if (x.noOfKeys() < 2) { return "\n  Number of wrties less than 2, add more data....\n"; }
		std::string xml;
		XmlDocument doc;       //Create XmlDocument instance
		Keys ikeys = x.keys();
		SPtr pRoot = makeTaggedElement("test"); //root element defined as test.
		doc.docElement() = pRoot;
		for (Key key : ikeys)
		{
			Element<double> ex = x.value(key);
			SPtr pItem = makeTaggedElement(ex.name);
			pRoot->addChild(pItem);
			SPtr pNameElem = makeTaggedElement("name"); // assigning name, tag, category and timedate.
			pItem->addChild(pNameElem);
			SPtr pTextElem = makeTextElement(ex.name);
			pNameElem->addChild(pTextElem);
			SPtr pcategoryElem = makeTaggedElement("category");
			pItem->addChild(pcategoryElem);
			pTextElem = makeTextElement(ex.category);
			pcategoryElem->addChild(pTextElem);
			SPtr ptimedateElem = makeTaggedElement("timedate");
			pItem->addChild(ptimedateElem);
			pTextElem = makeTextElement(ex.timeDate);
			ptimedateElem->addChild(pTextElem);
			std::vector<std::string> Vdep = ex.dependencyList;
			for (std::string x : Vdep) {
				SPtr pDependencyElem = makeTaggedElement("dependency");
				pItem->addChild(pDependencyElem);
				pTextElem = makeTextElement(x);
				pDependencyElem->addChild(pTextElem);
			}
			SPtr pdataElem = makeTaggedElement("data");
			pItem->addChild(pdataElem);
			DoubleWrapper d(ex.data);
			pTextElem = makeTextElement(d.save());
			pdataElem->addChild(pTextElem);
		}std::cout << "\n\n"; xml = doc.toString(); return xml;
	}

	//--------------< Class for persistance of Widget datatype >------------------------
	/////////////////////////////////////////////////////////////////////
	// WidgetWrapper classes

	class WidgetWrapper : public IPersist<Widget>
	{
	public:
		WidgetWrapper() {};
		WidgetWrapper(Widget widget) : widget_(widget) {}
		WidgetWrapper& operator=(const Widget& widget);
		Xml save();
		void restore(const Xml& xml);
		operator Widget();
		NoSqlDb<Widget> xmlProc(XmlDocument * pDoc, NoSqlDb<Widget> kd);
		std::string toXml(NoSqlDb<Widget> x);
	private:
		Widget widget_;
	};
	//----< assignment operator overload >-------------------------------

	WidgetWrapper& WidgetWrapper::operator=(const Widget& widget)
	{
		widget_ = widget;
		return *this;
	}



	//----< saves instance state as XML string >-------------------------

	inline Xml WidgetWrapper::save()
	{
		XmlDocument doc;

		// make and add root element to doc
		SPtr root = makeTaggedElement("WidgetWrapper");
		doc.docElement() = root;

		// make Property<int> wint element and add to root
		int wintValue = static_cast<Widget>(*this).wint;
		std::string wintStr = Convert<int>::toString(wintValue);
		SPtr pWintElem = makeTaggedElement("wint");
		pWintElem->addChild(makeTextElement(wintStr));
		root->addChild(pWintElem);

		// make Property<std::string> wstr element and add to root
		std::string wStr = static_cast<Widget>(*this).wstr;
		SPtr pWstrElem = makeTaggedElement("wstr");
		pWstrElem->addChild(makeTextElement(wStr));
		root->addChild(pWstrElem);
		return doc.toString();
	}
	//---< restores instance state from XML string >---------------------

	inline void WidgetWrapper::restore(const Xml& xml)
	{
		XmlDocument doc(xml);
		std::vector<SPtr> elems = doc.element("wint").select();
		std::string temp = elems[0]->children()[0]->value();
		widget_.wint = Convert<int>::fromString(temp);
		elems = doc.element("wstr").select();
		widget_.wstr = elems[0]->children()[0]->value();
	}
	//----< cast operator returns Widget member >------------------------

	inline WidgetWrapper::operator Widget()
	{
		return widget_;
	}

	//--------------< This function is to AUGMENT DATA FROM PDOC ELEMENT >------------------------

	NoSqlDb<Widget> WidgetWrapper::xmlProc(XmlDocument *pDoc, NoSqlDb<Widget> kd)
	{
		for (auto pItem : pDoc->elements("root").select()) { // finds all tag elements of "root"
			Element<Widget> *ed = new Element<Widget>;
			std::vector<std::string>  strVec;
			for (auto pElem : pDoc->elements(pItem->tag()).select())
			{
				if (pElem->tag() == "name") { // searches for tagElement name and assigns its textElement to name
					ed->name = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "dependency")
				{
					strVec.push_back(pElem->children()[0]->value());
					ed->dependencyList = strVec;
				}
				else if (pElem->tag() == "category") { //updates category
					ed->category = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "timedate") {
					ed->timeDate = pElem->children()[0]->value();
				}
				else if (pElem->tag() == "data") {
					Widget wd; // POK VALUE IS WIDGETWRAPPER
					for (auto pl : pDoc->elements(pElem->children()[0]->value()).select()) { //PL VALUE IS WINT AND WSTR
						if (pl->value() == "wint") {
							std::string win = pElem->children()[0]->children()[0]->children()[0]->value();
							wd.wint = Convert<int>::fromString(win);
							continue;
						}
						wd.wstr = pElem->children()[0]->children()[1]->children()[0]->value();
					}
					ed->data = wd;
				}
			}
			kd.save(ed->name, *ed);
		}
		return kd;
	}

	//--------------< This function is used to convert database to a string format >------------------------

	std::string WidgetWrapper::toXml(NoSqlDb<Widget> x)
	{
		if (x.noOfKeys() < 2) { return "\n  Number of wrties less than 2, add more data....\n"; }
		std::string xml;
		XmlDocument doc; //XMlDOC instance
		Keys ikeys = x.keys();
		SPtr pRoot = makeTaggedElement("test"); // test is defined as root element.
		doc.docElement() = pRoot;
		for (Key key : ikeys)
		{
			Element<Widget> ex = x.value(key);
			SPtr pItem = makeTaggedElement(ex.name);
			pRoot->addChild(pItem);
			SPtr pNameElem = makeTaggedElement("name"); // name, category, timedate and data assigned values.
			pItem->addChild(pNameElem);
			SPtr pTextElem = makeTextElement(ex.name);
			pNameElem->addChild(pTextElem);
			SPtr pcategoryElem = makeTaggedElement("category");
			pItem->addChild(pcategoryElem);
			pTextElem = makeTextElement(ex.category);
			pcategoryElem->addChild(pTextElem);
			SPtr ptimedateElem = makeTaggedElement("timedate");
			pItem->addChild(ptimedateElem);
			pTextElem = makeTextElement(ex.timeDate);
			ptimedateElem->addChild(pTextElem);
			std::vector<std::string> Vdep = ex.dependencyList;
			for (std::string x : Vdep) {
				SPtr pDependencyElem = makeTaggedElement("dependency");
				pItem->addChild(pDependencyElem);
				pTextElem = makeTextElement(x);
				pDependencyElem->addChild(pTextElem);
			}
			SPtr pdataElem = makeTaggedElement("data");
			pItem->addChild(pdataElem);
			WidgetWrapper w(ex.data);
			pTextElem = makeTextElement(w.save());
			pdataElem->addChild(pTextElem);
		}std::cout << "\n"; xml = doc.toString(); return xml;
	}


}