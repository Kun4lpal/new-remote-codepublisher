/////////////////////////////////////////////////////////////////////
// Document.cpp - This package is repsonsible for HTML construction//
// ver 1.0                                                         //
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
* This package makes use of Tag package to construct HTML style documents.
* Html file constructed contains dependency infomation as well as
* the source code.
*
* //* Public Interface (see test stub for details):
* =============================================
* AddTagToHead();  //this function adds a tag with its contents to Head Tag.
* AddTagToBody();  //this functions adds a tag with its content to Body Tag.
* ToString() const; //this function converts HTML doc element to string format.
* WriteToFile() const; //this function writes HTML string to a file.
*
*  Required Files (Direct and indirect):
*  ---------------
*  HtmlDocument:
*  Tag.h - this file contains functions create tags and enable adding attributes and content.
*
*  Maintanence History:
*  --------------------
*  ver 1.0 : 30 March 2017
*  - first release
*/
//
#include "Universal.h"
#include "Tag.h"
#include "Document.h"
#include <iostream>

using namespace Universal;

//<-----------------------------------Document constructor---------------------------------->

Document::Document() {
	this->docType = Tag("", "html");
	this->docType.SetType(TagType::DOCUMENT_TYPE);
	this->html_head = Tag("head");
	this->html_body = Tag("body");
}

//<-----------------------------------Appends tag to the HEAD TAG---------------------------------->

void Document::AddTagToHead(Tag Tag) {
	this->html_head.AppendChild(Tag);
}

//<-----------------------------------Appends tag to the BODY TAG---------------------------------->

void Document::AddTagToBody(Tag Tag) {
	this->html_body.AppendChild(Tag);
}

//<------------------------displays the document type in string format---------------------------------->

std::string Document::ToString(const Readability& readability) const {
	bool isMultiline = (readability == Readability::MULTILINE || readability == Readability::MULTILINE_BR);
	std::string doc = "";
	doc += docType.ToString(readability, 0);
	doc += "<html>";
	doc += ((isMultiline) ? "\n" : "");
	doc += html_head.ToString(readability, 1) + ((isMultiline) ? "\n" : "");
	doc += html_body.ToString(readability, 1) + ((isMultiline) ? "\n" : "");
	doc += "</html>";
	return doc;
}

//<----------------------------------a simple write function to the specified path------------------------------>

bool Document::WriteToFile(const std::string& filePath, const Readability& readability) const {
	std::ofstream file(filePath.c_str());
	if (file.is_open()) {
		file << this->ToString(readability);
		file.close();
		return true;
	}
	return false;
}

//<-----------------------------------TEST STUB OF DOCUMENT PACKAGE---------------------------------->

#ifdef  TEST_DOCUMENT
int main() {
	Document pDoc;
	Tag t("test");
	t.SetContent("This is content!");
	pDoc.AddTagToBody(t);
	std::cout << pDoc.ToString(Readability::MULTILINE) << std::endl;
}
#endif //  TEST_DOCUMENT
