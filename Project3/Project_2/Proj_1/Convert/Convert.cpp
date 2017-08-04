/////////////////////////////////////////////////////////////////////
// TemplatesIntro.cpp - Demonstrate Template Syntax                //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "Convert.h"
#include <string>
#include <iostream>
#include <functional>

/////////////////////////////////////////////////////////////////////
//Version history :
//Version 1.0, Released 2 / 5 / 2017.
// ConvertWidget class - shows user defined types can be template arguments

class ConvertWidget
{
public:
	ConvertWidget(const std::string& str = "") : state(str) {}
	std::string& value() { return state; }
private:
	std::string state;
};

std::ostream& operator<<(std::ostream& out, ConvertWidget& widget)
{
	out << widget.value();
	return out;
}

std::istream& operator >> (std::istream& in, ConvertWidget& widget)
{
	std::string temp;
	while (in.good())  // extract all the words from widget's string state
	{
		in >> temp;
		widget.value() += temp + " ";
	}
	return in;
}

/////////////////////////////////////////////////////////////////////
// lambdas that provide mildly useful local processing

std::function<void()> putLine = []() { std::wcout << "\n"; };

std::function<void(size_t)> putLines = [](size_t n) {
	for (size_t i = 0; i < n; ++i)
		putLine();
};

std::function<void(const std::string&, char)> titleCore = [](const std::string& msg, char underline = '-') {
	std::cout << "\n  " << msg.c_str();
	std::wcout << "\n " << std::string(msg.size() + 2, underline).c_str();
};

std::function<void(const std::string&)> Title = [](const std::string& msg) { titleCore(msg, '='); };

std::function<void(const std::string&)> title = [](const std::string& msg) { titleCore(msg, '-'); };

/////////////////////////////////////////////////////////////////////
// Demo code

int main()
{
	Title("Demonstrating Templates");
	putLine();

	title("Demonstrating Conversion of numerical types");
	std::cout << "\n  conversion of integer: " << Convert<int>::toString(42);
	std::cout << "\n  conversion of double:  " << Convert<double>::toString(3.1415927);
	putLine();

	title("Demonstrating Conversion of Widget type");
	ConvertWidget widget("Joe Widget");
	std::string widgetStore = Convert<ConvertWidget>::toString(widget);
	std::cout << "\n  conversion of Widget: " << widgetStore;

	ConvertWidget newWidget = Convert<ConvertWidget>::fromString(widgetStore);
	std::cout << "\n  newWidget state = " << newWidget.value();

	putLines(2);
}