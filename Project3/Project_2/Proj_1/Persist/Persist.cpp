/////////////////////////////////////////////////////////////////////
// PersistHelp.cpp - demo one way to persist objects to XML file   //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

/* Description:
*  This package defines the test stub for Persist.h
*
* - You can define and undefine using
*   project > Properties > C/C++ > Preprocessor > Processor Definitions
* - or using command line
*   cl /TEST_PERSIST persist.cpp [plus all the other cpp files needed.
*
* Version history:
* Version 1.0, Released 2/5/2017.
*/

#include "Persist.h"
#include <iostream>

using StrData = std::string;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;

//--------------< Test stub for Persist package >------------------------

#ifdef TEST_PERSIST

int main() {
	std::cout << "\nPersistTest.cpp: \n";
	StringWrapper sw("TextElement");
	std::string ans = sw.save();
	std::cout << ans<<std::endl;


	Widget wd;
	wd.wint = 2;
	wd.wstr = "Mage";
	Element<Widget> elem1;
	std::vector<std::string> v1;
	elem1.createElement("James","Student","2017-09-21",v1,wd);
	std::cout<<elem1.show();
	
	WidgetWrapper swiget(wd);
	std::string ansW = swiget.save();
	std::cout << ansW << std::endl;
}
#endif // 

