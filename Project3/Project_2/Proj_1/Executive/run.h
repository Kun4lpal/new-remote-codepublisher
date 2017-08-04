#pragma once
/////////////////////////////////////////////////////////////////////
// RUN.h - This is the main executive package.                     //
//                                                                 //
// Kunal Palwial, CSE687 - Object Oriented Design, Spring 2017     //
/////////////////////////////////////////////////////////////////////
/*
* This package has some basic utilites which will help in demonstrating all the requirements 
* for this project.
*
* Package operations:
* -------------------
* This file contains putline - skips a line, putlines - skips mulitple lines, Title - underlines a string.
*
* Required Files:
* ---------------
* This file makes use of standard C++ libraries.
*
* Build Instructions:
* -------------------
* Make sure all the other packages are of set to type static library after undefining their teststubs.
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/
#include <functional>
#include <string>
#include <iostream>
#include <stdio.h>

std::function<void()> putLine = []() { std::wcout << "\n"; }; //skips a line.

std::function<void(size_t)> putLines = [](size_t n) {  // skips mutliple lines.
	for (size_t i = 0; i < n; ++i)
		putLine();
};

std::function<void(const std::string&, char)> titleCore = [](const std::string& msg, char underline = '-') {
	std::cout << "\n  " << msg.c_str();
	std::wcout << "\n " << std::string(msg.size() + 2, underline).c_str();
};

std::function<void(const std::string&)> Title = [](const std::string& msg) { titleCore(msg, '='); }; //Underlines text

std::function<void(const std::string&)> title = [](const std::string& msg) { titleCore(msg, '-'); };

