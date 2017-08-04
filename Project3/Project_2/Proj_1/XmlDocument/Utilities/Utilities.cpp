///////////////////////////////////////////////////////////////////////
// Utilities.cpp - small, generally usefule, helper classes          //
// ver 1.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include <cctype>
#include <iostream>
#include <locale>
#include <iomanip>
#include "Utilities.h"

using namespace Utilities;
bool Verbose::showState_ = false;
const bool always = true;
std::mutex mtx;
std::unique_lock<std::mutex> iolock(mtx, std::defer_lock);

std::function<void(const std::string&)> Title =
[](auto src) { StringHelper::Title(src, '='); };

std::function<void(const std::string&)> title =
[](auto src) { StringHelper::Title(src, '-'); };


void Utilities::putline()
{
	std::cout << "\n";
}


void StringHelper::title(const std::string& src)
{
	iolock.lock();
	std::cout << "\n  " << src;
	std::cout << "\n " << std::string(src.size() + 2, '-');
	iolock.unlock();
}

void StringHelper::Title(const std::string& src, char underline)
{
	iolock.lock();
	std::cout << "\n  " << src;
	std::cout << "\n " << std::string(src.size() + 2, underline);
	iolock.unlock();
}

std::vector<std::string> StringHelper::split(const std::string& src)
{
	iolock.lock();
	std::vector<std::string> accum;
	std::string temp;
	size_t index = 0;
	do
	{
		while ((isspace(src[index]) || src[index] == ',') && src[index] != '\n')
		{
			++index;
			if (temp.size() > 0)
			{
				accum.push_back(temp);
				temp.clear();
			}
		}
		if (src[index] != '\0')
			temp += src[index];
	} while (index++ < src.size());
	if (temp.size() > 0)
		accum.push_back(temp);
	return accum;
	iolock.unlock();
}

//----< remove leading and trailing whitespace >---------------------

std::string StringHelper::trim(const std::string& src)
{
	iolock.lock();
	std::locale loc;
	std::string trimmed = src;
	size_t first = 0;
	while (true)
	{
		if (std::isspace(trimmed[first], loc))
			++first;
		else
			break;
	}
	size_t last = trimmed.size() - 1;
	while (true)
	{
		if (std::isspace(trimmed[last], loc) && last > 0)
			--last;
		else
			break;

	}
	return trimmed.substr(first, last - first + 1);
	iolock.unlock();
}
//----< wrap string in lines >---------------------------------------

std::string StringHelper::addHeaderAndFooterLines(const std::string& src)
{
	iolock.lock();
	std::string line = "------------------------------";
	return line + "\n" + src + "\n" + line + "\n";
	iolock.unlock();
}

void StringHelper::sTitle(std::string src, size_t offset, size_t width, std::ostream& out, char underline)
{
	if (width < src.size())
		width = src.size();
	size_t lwidth = (width - src.size()) / 2;
	out << "\n  " << std::setw(offset) << "" << std::setw(lwidth) << " " << src;
	out << "\n " << std::setw(offset) << "" << std::string(width + 2, underline);
}

void StringHelper::sTitle(std::string src, size_t offset, size_t width, std::ostringstream& out, char underline)
{
	out.str("");
	if (width < src.size())
		width = src.size();
	size_t lwidth = (width - src.size()) / 2;
	out << "\n  " << std::setw(offset) << "" << std::setw(lwidth) << " " << src;
	out << "\n " << std::setw(offset) << "" << std::string(width + 2, underline);
}

//----< takes any pointer type and displays as a dec string >--------

std::string Utilities::ToDecAddressString(size_t address)
{
	iolock.lock();
	std::ostringstream oss;
	oss << std::uppercase << std::dec << address;
	return oss.str();
	iolock.unlock();
}

void Utilities::putline(const std::string& msg)
{
	iolock.lock();
	std::cout << "\n" + msg;
	iolock.unlock();
}

//----< takes any pointer type and displays as a hex string >--------

std::string Utilities::ToHexAddressString(size_t address)
{
	iolock.lock();
	std::ostringstream oss;
	oss << std::uppercase << " (0x" << std::hex << address << ")";
	return oss.str();
	iolock.lock();
}

Verbose::Verbose(bool showState)
{
	showState_ = showState;
	if (showState)
		Utilities::putline("Verbose mode ON!");
}

void Verbose::show(const std::string& msg, bool always)
{
	if (always == true || showState_ == true)
	{
		std::string temp = std::string("\n  ") + msg;
		iolock.lock();
		std::cout << temp;
		std::cout.flush();
		iolock.unlock();
	}
}

#ifdef TEST_UTILITIES

int main()
{
	Title("Testing Utilities Package");
	putline();

	title("test StringHelper::split(std::string)");

	std::string test = "a, \n, bc, de, efg, i, j k lm nopq rst";
	std::cout << "\n  test string = " << test;

	std::vector<std::string> result = StringHelper::split(test);

	std::cout << "\n";
	for (auto item : result)
	{
		if (item == "\n")
			std::cout << "\n  " << "newline";
		else
			std::cout << "\n  " << item;
	}
	std::cout << "\n";

	title("test std::string Converter<T>::toString(T)");

	std::string conv1 = Converter<double>::toString(3.1415927);
	std::string conv2 = Converter<int>::toString(73);
	std::string conv3 = Converter<std::string>::toString("a_test_string plus more");

	std::cout << "\n  Converting from values to strings: ";
	std::cout << conv1 << ", " << conv2 << ", " << conv3;
	putline();

	title("test T Converter<T>::toValue(std::string)");

	std::cout << "\n  Converting from strings to values: ";
	std::cout << Converter<double>::toValue(conv1) << ", ";
	std::cout << Converter<int>::toValue(conv2) << ", ";
	std::cout << Converter<std::string>::toValue(conv3);

	std::cout << "\n\n";
	return 0;
}
#endif
