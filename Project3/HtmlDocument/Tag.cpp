/////////////////////////////////////////////////////////////////////
// Tag.cpp - This package is responsible for creating tag and      //
// ver 1.0            adding content and attributes to it.         //
//-----------------------------------------------------------------//
// Author:      Kunal Paliwal (c) copyright 2017                   // 
//              kupaliwa @syr.edu, (315) 876 8002                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Project #3, CSE687 - Object Oriented Design, S2017 //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  ===================
*  This package contains a tag class which has functions to append tags, set attributes, content and type.
*
* //* Public Interface (see test stub for details):
* =============================================
*
* Tag(const std::string& name); //constructor to add create a tag with name specifed in the argument.
* Tag(const std::string& name, const std::string& content); // constructor to create a tag with name and content.
* ToString(Readability readability, int indentLevel) const; // converts a tag to string format depending on readability type.
* SetName(const std::string& name); // sets Name of the tag.
* GetAttribute(const std::string& name) const;  // gets all the attributes of the tag element.
* GetSelector() const; // gets the name and id of the tag, for testing purposes.
* SetAttribute(std::string name, std::string value); // sets an attribute with the name and value specifed.
* SetType(TagType type);  //sets the type of tag element.
* SetContent(const std::string& text);   // sets the content of the tag element.
* ToggleClass(const std::string& className);   //toggle class which hides and unhides tag element.
* AppendChild(Tag child); Appends a child tag to the parent tag.
* UseClosingTag(bool close);      // creates a closing tag if bool close tag was false.
* modifyString(std::string read, std::string token)const; // modifies a string value with the token value. (Add Button)

//<------------------------------------PRIVATE DECLARATIONS---------------------------------->
* formatContent(Readability readability, const std::string& indent) const; // gets formatted content of a tag element.
* replaceSubString(std::string replacer, const std::string& replacable, const std::string& replace) const; // a helper function to replace occurences inside a string.
* findAllOccurences(std::string finder, const std::string& findable) const; // returns a count of total occurences of the findable string value.
* parseAndSetClassIdAttributes(std::string classesAndIDs); // this function parses the string containing the id and classes and sets these values
*                                                   // to the tag element.
*
*  Required Files (Direct and indirect):
*  ---------------
*  For testing add TEST_TAG in pre-processor tag.
*
*  Maintanence History:
*  --------------------
*  ver 1.0 : 30 March 2017
*  - first release
*/
//
#include "Universal.h"
#include "Tag.h"
#include <iostream>
#include <sstream>

//<-----------------------------------Using Statements---------------------------------->

using namespace Universal;

//<-----------------------------------Constructor---------------------------------->

Tag::Tag() {
}

//<-----------------------------------Constructor with tag name as argument---------------------------------->r

Tag::Tag(const std::string& name) {
	this->m_type = TagType::ELEMENT;
	this->SetName(name);
}

//<-----------------------------------Constructor with tag name and content as argument---------------------------------->

Tag::Tag(const std::string& name, const std::string& content) {
	this->m_type = TagType::ELEMENT;
	this->SetName(name);
	this->m_content = content;
}

//<-----------------------------------Convert a tag to string format---------------------------------->

std::string Tag::ToString(Readability readability, int indentLevel) const {
	std::string elem = "";
	std::string indent = "  ";
	std::string indentContent = "  ";
	bool isMultiline = (readability == Readability::MULTILINE || readability == Readability::MULTILINE_BR);
	if (this->m_type == TagType::ELEMENT) {
		elem = ((isMultiline) ? indent : "") + "<" + m_name + "";
		if (!m_classes.empty()) {
			std::string classTag = "class=\"";
			elem += " " + classTag + m_classes + "\"";
		}
		if (!m_id.empty()) {
			std::string idTag = "id=\"";
			elem += " " + idTag + m_id + "\"";
		}
		for (const auto& attr : m_attributes) {
			elem += " " + attr.first + "=\"" + attr.second + "\"";
		}
		elem += ">";
		if (m_closeTag)
		{
			if (isMultiline && (!m_content.empty() || !m_children.empty()))
				elem += "\n";
			if (!m_content.empty()) {
				elem += formatContent(readability, indentContent); 
			}
			for (std::size_t i = 0; i < m_children.size(); ++i) {
				const auto& childTag = m_children[i];
				elem += childTag.ToString(readability, indentLevel + 1) + ((i != m_children.size() - 1 && isMultiline) ? "\n" : "");
			}
			elem += ((isMultiline && (!m_content.empty() || !m_children.empty())) ? "\n" + indent : "") + "</" + m_name + ">";
		}
	}
	else if (this->m_type == TagType::DOCUMENT_TYPE) {
		elem += "<!DOCTYPE " + m_content + ">" + ((isMultiline) ? "\n" : "");
	}
	return elem;
}

//<-------------------Allows to setName of a tag (used when i have not initialzed tag name in constructor)--------------------->

Tag& Tag::SetName(const std::string& name) {
	const auto periodIndex = name.find('.');
	const auto poundIndex = name.find('#');
	if (periodIndex != std::string::npos || poundIndex != std::string::npos) {
		bool poundBefore = (poundIndex != std::string::npos && poundIndex < periodIndex);
		const auto ind = ((poundBefore || (periodIndex == std::string::npos && poundIndex != std::string::npos)) ? poundIndex : periodIndex);
		std::string elemName = name.substr(0, ind);
		parseAndSetClassIdAttributes(name.substr(ind));
		this->m_name = elemName;
	}
	else {
		this->m_name = name;
	}
	return *this;
}

//<-----------------------this function returns the attributes of the specifed tag----------------------->

std::string Tag::GetAttribute(const std::string& name) const {
	if (name != "class" && name != "id" && m_attributes.count(name) > 0)
		return m_attributes.at(name);
	else if (name == "class")
		return m_classes;
	else if (name == "id")
		return m_id;
	else
		return "";
}

//<-----------------------------------test function not used in this project---------------------------------->

std::string Tag::GetSelector() const {
	std::string classesPeriod = replaceSubString(m_classes, " ", ".");
	return m_name + classesPeriod + "#" + m_id;
}

//<-----------------------------------Adds an attribute or replaces the id of class---------------------------------->

Tag& Tag::SetAttribute(std::string name, std::string value) {
	if (name != "class" && name != "id")
		m_attributes[name] = value;
	else if (name == "class")
		m_classes = value;
	else if (name == "id")
		m_id = value;
	return *this;
}

//<-----------------------------------Sets the type of the tag---------------------------------->

Tag& Tag::SetType(TagType type) {
	this->m_type = type;
	return *this;
}

//<-----------------------------------Adds content to the tag---------------------------------->

Tag& Tag::SetContent(const std::string& text) {
	this->m_content = text;
	return *this;
}

//<-----------------------------------Append or remove tag from the HTMLDoc---------------------------------->

Tag& Tag::ToggleClass(const std::string& className) {
	size_t findIndex = m_classes.find(className);
	if (findIndex == std::string::npos) {
		m_classes += ((!m_classes.empty()) ? " " : "") + className;
	}
	else {
		m_classes.erase(findIndex, className.size());
	}
	return *this;
}

//<-----------------------------------Add child the document---------------------------------->

Tag& Tag::AppendChild(Tag child) {
	m_children.push_back(child);
	return *this;
}

//<---------------------------------------Used to close tag---------------------------------->

Tag& Tag::UseClosingTag(bool close) {
	this->m_closeTag = close;
	return *this;
}

//<-----------------------------------Adds a button to the string at the token location------------------->

std::string Tag::modifyString(std::string read, std::string token)const {
	std::string ex = read; size_t occurence = 0; int val = findAllOccurences(read, token);
	while (val) {
		size_t index = ex.find(token, occurence);
		size_t end = 0;
		size_t start = 0;
		size_t fire = 0;
		if (index != std::string::npos  && ex[index -1 ] != '\"' && ex[index-1] != '\'') {
			start = ex.find_first_of("{", index);
			if (20 <index - start < 4) { return ex; }
			fire = start + 1;
			int var = 1;
			try {
				while (var != 0) {
					if (ex[fire] == '}' && ex[fire - 1] != '\'' && ex[fire - 1] != '\"') {
						var--;
					}
					else if (ex[fire] == '{' && ex[fire - 1] != '\'' && ex[fire - 1] != '\"') {
						var++;
					}
					fire++;
				}
			}
			catch (...) {} end = fire - 1;
		}
		else { return ex; }
		if (end - start - 1 < 10) {
			val--;occurence = end;continue;}
		std::string replaceWith = ex.substr(start + 1, end - start - 1);
		std::ostringstream ss;
		ss << "&88999button onclick=\"myFunc('" + token + "22" + "')\" class=\"w3 - btn w3 - hover - black w3 - dark - grey\" style=\"box - shadow:none\"&89999" + "CLICK" + "&88999/button&89999";
		ss << "&88999div id=\"" + token + "22" + "\" &89999";//\" style =\"display:none\"
		ss << replaceWith;
		ss << "&88999/div&89999";
		ex = replaceSubString(ex, replaceWith, ss.str());
		occurence = end;val--; }return ex;
}
//<----------------------------------returns a formatted string by replacing "< and >" ---------------------------------->

std::string Tag::formatContent(Readability readability, const std::string& indent) const {
	std::string result;
	std::istringstream iss(m_content);
	if (readability == Readability::MULTILINE || readability == Readability::MULTILINE_BR) {
		std::string newline = ((readability == Readability::MULTILINE_BR) ? "\n" + indent + "<br>\n" : "\n");
		int curLine = 0;
		for (std::string line; std::getline(iss, line);)
		{
			result += ((curLine > 0) ? newline : "") + indent + line;
			curLine++;
		}}
	else {
		for (std::string line; std::getline(iss, line);)
		{
			result += line;
		}}  result += "\n";
	result = replaceSubString(result, "<", "&lt;");
	result = replaceSubString(result, ">", "&gt;");
	result = replaceSubString(result, "&89999", ">");
	result = replaceSubString(result, "&88999", "<"); 
	return result;
}

//<----------------------------------returns a formatted string by replacing all occurences of "< and >" ---------------------------------->

std::string Tag::replaceSubString(std::string replacer, const std::string& replacable, const std::string& replace) const {
	size_t start = 0;
	while ((start = replacer.find(replacable, start)) != std::string::npos) {
		replacer.replace(start, replacable.length(), replace);
		start += replace.length();
	}
	return replacer;
}

//<--------------------------------returns a count of all occurences of a string---------------------------------->

int Tag::findAllOccurences(std::string finder, const std::string& findable) const {
	int occurrences = 0;
	size_t start = 0;
	while ((start = finder.find(findable, start)) != std::string::npos) {
		occurrences++;
		start += findable.length();
	}
	return occurrences;
}

//<------------------this function processes class and id string and sets all attributes in the Tag class-------------------->

void Tag::parseAndSetClassIdAttributes(std::string classesAndIDs) {
	TagParser currentlyParsing = TagParser::NONE;
	std::string attrString;
	for (unsigned int i = 0; i < classesAndIDs.size(); i++) {
		char curChar = classesAndIDs[i];
		if (currentlyParsing == TagParser::NONE) {
			if (curChar == '.') {
				currentlyParsing = TagParser::CLASS;
			}
			else if (curChar == '#') {
				currentlyParsing = TagParser::ID;
			}}
		else {
			if (curChar == '.' || curChar == '#') {
				if (currentlyParsing == TagParser::CLASS)
					m_classes += attrString + " ";
				else
					m_id = attrString;
				attrString.clear();
				currentlyParsing = ((curChar == '.') ? TagParser::CLASS : TagParser::ID);}
			else {
				attrString += curChar;
			}}
		if (currentlyParsing != TagParser::NONE && i == classesAndIDs.size() - 1) {
			if (currentlyParsing == TagParser::CLASS)
				m_classes += attrString;
			else
				m_id = attrString;
			attrString.clear();
		}}
	if (!m_classes.empty()) {
		if (isspace(m_classes.at(m_classes.size() - 1)))
			m_classes = m_classes.substr(0, m_classes.size() - 1);
	}
}

//<-----------------------TEST STUB FOR TAG PACKAGE------------------------------->

#ifdef TEST_TAG

int main() {
	Tag t("TestTag");
	t.SetContent("This is Content");
	t.SetAttribute("master","blaster");
	std::cout << t.ToString(Readability::MULTILINE, 1);
}
#endif // TEST_TAG
