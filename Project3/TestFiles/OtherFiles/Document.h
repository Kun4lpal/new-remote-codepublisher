#ifndef DOCUMENT_H
/////////////////////////////////////////////////////////////////////
// Document.h - This package is repsonsible for HTML construction. //
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
#pragma once
#include "Tag.h"
#include <string>
#include <fstream>

namespace Universal {
	class Document {
		Tag docType; // the doctype of this document
		Tag html_head; // the head tag of this document
		Tag html_body; // the body tag of this document
	public:
		Document();
		void AddTagToHead(Tag Tag);
		void AddTagToBody(Tag Tag);
		std::string ToString(const Readability& readability) const;
		bool WriteToFile(const std::string& filePath, const Readability& readability) const;
	};
}
#endif // !DOCUMENT_H