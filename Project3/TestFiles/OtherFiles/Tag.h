#ifndef TAG_H
/////////////////////////////////////////////////////////////////////
// Tag.h - This package is responsible for creating tag and adding //
// ver 1.0              content and attributes to it.              //
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
#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>

namespace Universal {
	//<---------------------------------DECIDE DOC TYPE ---------------------------------->
	enum class TagType { DOCUMENT_TYPE, ELEMENT };

	//<----------------------------------READABILITY ENUM ---------------------------------->
	// SINGLE_LINE returns the string as one line
	// MULTILINE returns the string as multiple lines.
	// MULTILINE_BR is same as MULTILINE, but the difference is that newlines in the content of the Tag are formatted to use <br> tags.
	enum class Readability { SINGLE_LINE, MULTILINE, MULTILINE_BR };

	//<-------------------------CURRENT STATE OF THE PARSER-------------->
	// NONE means that nothing is being parsed
	enum class TagParser { NONE, CLASS, ID };

	//<----------------------------------TAG CLASS---------------------------------->

	class Tag {
		TagType m_type; 
		std::string m_name; 
		std::string m_classes; 
		std::string m_id; 
		std::string m_content; 
		bool m_closeTag = true; 
		std::vector<Tag> m_children; 
		std::unordered_map<std::string, std::string> m_attributes;

		//<----------------------------------PUBLIC DECLARATIONS---------------------------------->

	public:
		Tag();
		Tag(const std::string& name);
		Tag(const std::string& name, const std::string& content);
		std::string ToString(Readability readability, int indentLevel) const;
		Tag& SetName(const std::string& name);
		std::string GetAttribute(const std::string& name) const;
		std::string Tag::GetSelector() const;
		Tag& SetAttribute(std::string name, std::string value);
		Tag& SetType(TagType type);
		Tag& SetContent(const std::string& text);
		Tag& ToggleClass(const std::string& className);
		Tag& AppendChild(Tag child);
		Tag& UseClosingTag(bool close);
		std::string modifyString(std::string read, std::string token)const;

		//<------------------------------------PRIVATE DECLARATIONS---------------------------------->

	private:
		std::string formatContent(Readability readability, const std::string& indent) const;
		std::string replaceSubString(std::string replacer, const std::string& replacable, const std::string& replace) const;
		int findAllOccurences(std::string finder, const std::string& findable) const;
		void parseAndSetClassIdAttributes(std::string classesAndIDs);
	};
}
#endif // !TAG_H