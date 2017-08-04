#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally useful, helper classes             //
// ver 1.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  This class will be extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Utilities.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.2 : 22 Feb 2015
* - changed default underline char in Title(const std::string&, Char)
* ver 1.1 : 06 Feb 2015
* - fixed bug in split which turns a comma separated string into
*   a vector of tokens.
* - added comments
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <mutex>
#include <functional>

namespace Utilities
{
	class StringHelper
	{
	public:
		static std::vector<std::string> split(const std::string& src);
		static void Title(const std::string& src, char underline = '=');
		static void title(const std::string& src);
		static std::string trim(const std::string& src);
		static std::string addHeaderAndFooterLines(const std::string& src);
		static void sTitle(
			std::string src, size_t offset, size_t width, std::ostream& out = std::cout, char underline = '-'
		);
		static void sTitle(
			std::string src, size_t offset, size_t width, std::ostringstream& out, char underline = '-'
		);
	};

	std::string ToDecAddressString(size_t address);
	std::string ToHexAddressString(size_t address);
	void putline(const std::string& msg);
	void putline();


	template <typename T>
	class Converter
	{
	public:
		static std::string toString(const T& t);
		static T toValue(const std::string& src);
	};

	template <typename T>
	std::string Converter<T>::toString(const T& t)
	{
		std::ostringstream out;
		out << t;
		return out.str();
	}

	template<typename T>
	T Converter<T>::toValue(const std::string& src)
	{
		std::istringstream in(src);
		T t;
		in >> t;
		return t;
	}

	class Verbose
	{
	public:
		Verbose(bool showState = true);
		void static show(const std::string& msg, bool always = false);
	private:
		static bool showState_;
	};

	struct Cosmetic
	{
		~Cosmetic()
		{
			std::cout << "\n\n  Press Enter to exit: ";
			std::cin.get();
			std::cout << "\n\n";
		}
	};
}
#endif
