#define IN_DLL
///////////////////////////////////////////////////////////////////////////
// ServerCommChannel.cpp - C++\CLI implementation of WPF Application     //
//                                                                       //
// ver 1.0                                                               //
// Author : Kunal Paliwal -  Object Oriented Design, Spring 2017         //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
* Public Interface
*  ---------------
* void parseMsg(HttpMessage msg);        //does message parsing to find command.
* void LazyDownLoadMsg(HttpMessage msg, std::string filename);  // sends lazy download message using sockets
* void uploadMsg(HttpMessage msg);          // sends an upload message using sockets.
* void getHtmlList(HttpMessage msg);      //gets the list of html files available in server.
* void dotest(HttpMessage msg);     // a test message - this is where it all started
* void publishFiles(HttpMessage msg); // a message that restarts publishing and code analysis.
* void DownLoadMsg(HttpMessage msg, std::string filespec); // downloads file specified in the message.
* void exploreMsg(HttpMessage msg);
*
*  Required Files:
*  ---------------
*  ICommChannel.h,
*  Dependency analysis.h, StrongComponents.h, XMldocument.h, Utilites.h
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp, Logger.h
*
*  Build Command:
*  --------------
*  devenv Project4.sln
*  - this builds C++\CLI client application and native mock channel DLL
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 5 May 2017
*  - add final touches.
*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
#include "ServerCommChannel.h"
#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include "windows.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../Project3/Project_2/Proj_1/XmlDocument/Utilities/Utilities.h"
#include "../Project3/Project_2/CodeAnalyzer/Logger/Logger.h"
#include "../Project3/Project_2/CodeAnalyzer/FileMgr/IFileMgr.h"
#include "../Project3/Project_2/Proj_1/XmlDocument/XmlDocument/XmlDocument.h"
#include "../Project3/Project_2/CodeAnalyzer/FileSystem/FileSystem.h"
#include "../Project3/Project_2/Proj_1/Persist/Persist.h"
#include "../Project3/CodePublisher/CodePublisher.h"

//<-----------------------------------Using Statements---------------------------------->
using namespace XmlProcessing;
using namespace FileSystem;
using namespace RemoteLogger;
using namespace CodeAnalysis;
using namespace Universal;
using sPtr = std::shared_ptr<AbstractXmlElement>;
using BQueue = BlockingQueue <HttpMessage>;
using Show = StaticLogger<1>;
using namespace Utilities;
template<typename T>
using Conv = Utilities::Converter<T>;
using StrData = std::string;
using SPtr = std::shared_ptr<AbstractXmlElement>;
using Key = std::string;
using Keys = std::vector<std::string>;
using Utils = Utilities::StringHelper;
using namespace FileManager;
using namespace Persist;
using namespace CPublish;

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts HttpMessages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
	void postMessage(const HttpMessage& msg);
	BQueue& queue();
private:
	BQueue sendQ_;
};

//<-----------------------------------sends message to sendQ---------------------------------->

void Sendr::postMessage(const HttpMessage& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts HttpMessages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
	std::string getMessage();
	BlockingQueue<std::string>& queue();
private:
	BlockingQueue<std::string> recvQ_;
};

//<----------------------------------dequeues message from recieve queue---------------------------------->

std::string Recvr::getMessage()
{
	return recvQ_.deQ();
}

//<-----------------------------------returns queue---------------------------------->

BlockingQueue<std::string>& Recvr::queue()
{
	return recvQ_;
}

//<-----------------------------------does processing of recieve child thread---------------------------------->

void ClientHandler::operator()(Socket& socket_)
{
	while (true)
	{
		HttpMessage msg;
		std::string line = socket_.recvString();
		if (line.size() == 0)
			break;
		std::cout << line;
		msg = HttpMessage::parseMessage(line);

		std::string cmd = msg.findValue("Command");
		if (cmd == "Upload")
		{
			upLoadHandling(socket_, msg.findValue("FileSpec"));
			continue;
		}
		if (cmd == "Publish") {
			publishHandling(socket_,msg);
			continue;
		}
		if (cmd == "Html") {
			searchHtmlHandling(socket_, msg);
			continue;
		}
		if (cmd == "test") {
			testHandling(socket_, msg);
			continue;
		}
		if (cmd == "Download")
		{
			downLoadHandling(socket_, msg);
			continue;
		}
		if (cmd == "Explore")
		{
			exploreHandling(socket_, msg);
			continue;
		}
		if (cmd == "LazyDownload") {
			
			LazydownLoadHandling(socket_, msg);
			continue;
		}
	}
	socket_.shutDown();
	socket_.close();
}

//<-----------------------------------add pattern to list---------------------------------->

void ClientHandler::addPatterns(std::string str)
{
	patterns_.push_back(str);
}

//<-----------------------------------handles explore command from client---------------------------------->

bool ClientHandler::exploreHandling(Socket& socket_, HttpMessage msg)
{
	std::string patternsString = socket_.recvString();
	msg.parsePatternString(patternsString);
	std::vector<std::string> patternV = msg.getTypeVectr();//use the pattern in pattern V later for now we set it as all types
	addPatterns("*.h");
	addPatterns("*.cpp");
	getSourceFiles("/CodeFiles");
	std::vector<std::string> fileVec = fileSet();
	for (auto elem : fileVec)
	{
		socket_.sendString("<filefordownload>" + elem);
	}
	fileMap_.clear();
	patterns_.clear();
	socket_.sendString("quit");
	return true;
}

//<-----------------------------------handles lazydownload command from client---------------------------------->

bool ClientHandler::LazydownLoadHandling(Socket& socket_, HttpMessage msg)
{
	std::string filename = msg.findValue("FileSpec");
	filename = FileSystem::Path::getFullFileSpec(getCommandLinePath() + "//CodeFiles//"+filename);
	//NoSqlDb<std::string> database;
	std::vector<std::string> dependencies;
	try {
		std::string currentTime = systemTime();
		std::cout << "Finding Dependency Information...........................................\n";
		dependencies = database.value(filename).dependencyList;
	}
	catch (std::exception ex) {
		std::cout << ex.what();
	}
	for (auto elem : dependencies)
	{
		std::string relPath = elem.substr(elem.find_last_of('/\\'));
		std::string fpath = FileSystem::Path::getFullFileSpec(getCommandLinePath() + "//HtmlFiles//" + relPath + ".htm");
		socket_.sendString("<filefordownload>" + fpath);
	}
	fileMap_.clear();
	patterns_.clear();
	socket_.sendString("<download>");
	socket_.sendString("quit");
	return true;
}

//<-----------------------------------handles get HTMLFILE list command from client---------------------------------->

bool ClientHandler::searchHtmlHandling(Socket& socket_, HttpMessage msg)
{
	fileMap_.clear();
	patterns_.clear();
	std::string patternsString = socket_.recvString();
	msg.parsePatternString(patternsString);
	std::vector<std::string> patternV = msg.getTypeVectr();//use the pattern in pattern V later for now we set it as all types
	addPatterns("*.html");
	addPatterns("*.htm");
	addPatterns("*.js");
	addPatterns("*.css");
	getSourceFiles("/HtmlFiles");
	std::vector<std::string> fileVec = fileSet();
	for (auto elem : fileVec)
	{
		socket_.sendString("<filefordownload>" + elem);
	}
	fileMap_.clear();
	patterns_.clear();
	socket_.sendString("quit");
	return true;
}

//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}

//<-----------------------------------fileSet contains the list of files---------------------------------->

std::vector<ClientHandler::File> ClientHandler::fileSet()
{
	std::vector<std::string> fileVec;
	for (auto item : fileMap_)
	{
		for (auto file : item.second) {
			std::string filecpy = file;
			fileVec.push_back(filecpy);
		}
	}
	return fileVec;
}

//<-----------------------------------handles publish command from client---------------------------------->

bool ClientHandler::publishHandling(Socket & socket_, HttpMessage msg)
{
	//std::string recvString = socket_.recvString();
	std::cout << "Recieved : " + msg.findValue("Command") << std::endl;
	socket_.sendString("Message was recieved!");
	addPatterns("*.h");
	addPatterns("*.cpp");
	processSourceCode(true);
	try{
	TypeTable TT = getTypeTable();
	std::unordered_map<std::string, TypeRecord> ttable = TT.getTypeTable();
	DependencyAnalysis dp(vec);
	//int i = std::remove("C:\\Kunal\\Course\\OOD\\OOD projects FINAL\\Project3final\\Project3\\Project3\\TestFiles\\GrandParent.h");
	std::string currentTime = systemTime();
	std::cout << "Finding Dependency Information...........................................\n";
	database = dp.storeDependencyInfo(ttable, currentTime); //NoSQLdb containing dependency analysis information
	CodePublisher ex;
	std::string val = getCommandLinePath() + "//HtmlFiles";
	if (xpath_.find("*") == std::string::npos) {
		if (xpath_.find("launch") == std::string::npos) {
			
			ex.publishFileSet(ttable, database,val ); // here i need to send port 
			launch(val + "/index.htm");
		}
		else {
			ex.publishFileSet(ttable, database, val); // here i need to send port 
			std::string path = xpath_.substr(7,xpath_.length());
			launch(path);
		}
	}
	else {
		ex.publishFileSet(ttable, database, "../HtmlFiles");
		launch("HtmlFiles/index.htm");}
	}
	catch (std::exception ex) {
	std::cout << "\n\n  Caught exception in Executive::main: " + std::string(ex.what()) + "\n\n";
	}
	std::vector<std::string> fileVec = fileSet();
	patterns_.clear();
	fileMap_.clear();
	socket_.sendString("quit");
	return true;
}

//<-----------------------------------handles test command from client---------------------------------->

bool ClientHandler::testHandling(Socket & socket_, HttpMessage msg)
{
	//std::string recvString = socket_.recvString();
	std::cout << "Recieved : " + msg.findValue("Command") << std::endl;
	socket_.sendString("Message was recieved!");

	socket_.sendString("quit");
	return true;
}

//<-----------------------------------handles download command from client---------------------------------->

bool ClientHandler::downLoadHandling(Socket& socket_, HttpMessage msg)
{
	//std::string filename = "../ServerRepository/" + msg.findValue("FileSpec");
	std::string filename = msg.findValue("FileSpec");
	FileSystem::File bufferIn(filename);
	bufferIn.open(FileSystem::File::in, FileSystem::File::binary);
	if (!bufferIn.isGood())
	{
		std::cout << "\ncould not open \"" << filename << "\" for downloading";
		return false;
	}
	else
		std::cout << "\nopening: \"" << filename << "\" for uploading";
	std::cout << "\n\nStart sending.";

	const size_t bufLen = 512;
	FileSystem::File::byte buffer[bufLen];
	while (true)
	{
		size_t resultSize = bufferIn.getBuffer(bufLen, buffer);
		std::string temp1(buffer, resultSize);
		std::cout << "\nsend body:" << temp1;
		msg.addAttribute(msg.attribute("NumBody", Conv<size_t>::toString(resultSize)));
		socket_.sendString(msg.toString());
		msg.removeAttribute("NumBody");
		socket_.send(resultSize, buffer);
		if (resultSize < bufLen || !bufferIn.isGood())
		{
			std::cout << "\nclose file.";
			bufferIn.close();
			break;
		}
	}
	patterns_.clear();
	fileMap_.clear();
	socket_.sendString("quit");
	std::string succ = "File " + msg.findValue("FileSpec") + " has been download.";
	return true;
}

//<-----------------------------------handles upload command from client---------------------------------->

bool ClientHandler::upLoadHandling(Socket& socket_, std::string filespec)
{
	HttpMessage msg;
	const size_t BufLen = 512;
	Socket::byte buffer[BufLen];
	bool ok;
	std::string fileNameSub = filespec.substr(filespec.find_last_of('/\\') + 1);
	filespec = getCommandLinePath() + "//CodeFiles//" + fileNameSub;
	FileSystem::File bufferOut(filespec);
	bufferOut.open(FileSystem::File::out, FileSystem::File::binary);
	if (!bufferOut.isGood())
	{
		std::cout << "\n  could not open \"" << filespec << "\" for writing\n\n";
		return false;
	}
	else
	{
		std::cout << "\n  opening: \"" << filespec << "\" for writing";
	}
	std::cout << "\n";

	while (true) {
		std::string line = socket_.recvString();
		if (line == "quit")
			break;
		msg = HttpMessage::parseMessage(line);
		size_t numBody = Conv<size_t>::toValue(msg.findValue("NumBody"));
		ok = socket_.recv(numBody, buffer);
		if (socket_ == INVALID_SOCKET || !ok)
			return false;
		std::string temp;
		for (size_t i = 0; i < numBody; ++i)
			temp += buffer[i];
		std::cout << "\nServer Received :\n" + temp;
		bufferOut.putBuffer(numBody, buffer);
		if (!bufferOut.isGood())
			break;
		std::cout << "\n";
	}
	std::string succ = "File " + msg.findValue("FileSpec") + " has been uploaded.";
	std::cout << "\n  Closing: \"" << filespec << "\" for writing";
	bufferOut.close();
	std::cout << "\n  Reply to sender: " << succ << "\n";
	socket_.sendString(succ);
	return true;
}

//-----------------------------< Constructor >----------------------------

AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}

//-----------------------------< returns number of files >----------------------------

size_t AnalFileMgr::numFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t AnalFileMgr::numDirs()
{
	return numDirs_;;
}

//----< pass pointers to Sender and Receiver >-------------------------------

ServerMockChannel::ServerMockChannel(ISendr* pSendr, IRecvr* pRecvr){ 
	this->pIRecvr_ = pRecvr;
	this->pISendr_ = pSendr;
}

ServerMockChannel::~ServerMockChannel()
{
}

void ServerMockChannel::setPort(int port)
{
	SocketListener* sl_ = new SocketListener(port);
	this->port_ = port;
	sl = sl_;
}

void ServerMockChannel::setAddr(std::string path)
{
	fpath = path;
}


//----< creates thread to read from sendQ and echo back to the recvQ >-------

void ServerMockChannel::start()
{
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	std::cout << "\n  MockChannel starting up";
	FileSystem::Directory::create(fpath + "//HtmlFiles");
	FileSystem::Directory::create(fpath + "//CodeFiles");
	FileSystem::Directory::create(fpath + "//Scripts");
	ch.setCommandLinepath(fpath);
	ch.setPort(port_);
	bool issucc = sl->start(ch);   //client handler here
	if (!issucc)	std::cout << "\n  Start failed";
	//if (issucc)	std::cout << "\n  Start Success";
}
//----< signal server thread to stop >---------------------------------------

void ServerMockChannel::stop() { stop_ = true; }



ClientHandler::ClientHandler()
{
	xpath_ = "../ServerRepository/HtmlFiles";  //default html path.
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
}

void ClientHandler::setCommandLinepath(std::string path)
{
	this->pathofSERVER_REPO_ = path;
}

std::string ClientHandler::getCommandLinePath()
{
	return pathofSERVER_REPO_;
}

void ClientHandler::setPort(std::string port) {
	this->Serverport = port;
}

std::string ClientHandler::getPort() {
	return Serverport;
}

//<------------------------------Displayed when arguments are < 3------------------------->

void ClientHandler::showUsage()
{
	std::ostringstream out;
	out << "\n  Usage:";
	out << "\n  Command Line Arguments are:";
	out << "\n  - 1st: path to subdirectory containing files to analyze";
	out << "\n  - 2nd: path to subdirectory where html file will be created";
	out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
	out << "\n  - must have at least one file pattern to specify what to process";
	out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
	out << "\n  A summary is always shown, independent of any options used or not used";
	out << "\n\n";
	std::cout << out.str();
}

//----------------------< helper Function to replace string>------------------------------

std::string ClientHandler::replaceSubString(std::string replacer, const std::string& replacable, const std::string& replace) const {
	size_t start = 0;
	while ((start = replacer.find(replacable, start)) != std::string::npos) {
		replacer.replace(start, replacable.length(), replace);
		start += replace.length();
	}
	return replacer;
}

//----------------------< handle launch event >------------------------------

void ClientHandler::launch(std::string valu)
{
	std::cout << "\n Launching file at path: " << valu;
	std::cout << "\n";
	std::string htm = valu;
	std::string browser("start \"\" \"" + htm + "\"");
	std::system(browser.c_str());
}
//----------------------< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
*/
bool ClientHandler::ProcessCommandLine()
{
	try {
		if (!FileSystem::Directory::exists(pathofSERVER_REPO_))
		{
			std::cout << "\n\n  path \"" << pathofSERVER_REPO_ << "\" does not exist\n\n";
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

//<------------------------------Displays command line arguments------------------------------>

void ClientHandler::showCommandLineArguments(int argc, char* argv[])
{
	/*std::ostringstream out;
	std::ostringstream xpath;
	out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
	for (int i = 2; i < argc - 1; ++i) {
		out << argv[i] << ", ";
	}
	xpath << argv[2];
	out << argv[argc - 1];
	std::cout << out.str();
	xpath_ = xpath.str();*/
}

//--------------------------------< returns path entered on command line >-------------------

std::string ClientHandler::getAnalysisPath()
{
	return pathofSERVER_REPO_;
}

//----< searches path for files matching specified patterns >----
/*
* - Searches entire diretory tree rooted at path_, evaluated
*   from a command line argument.
* - Saves found files in FileMap.
*/
void ClientHandler::getSourceFiles(std::string file)
{
	AnalFileMgr fm(pathofSERVER_REPO_ +file, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numFiles_ = fm.numFiles();
	numDirs_ = fm.numDirs();
}
//<-----------------------------------returns cppheaderFile *.h types-------------------------------->

std::vector<ClientHandler::File>& ClientHandler::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}

//-----------------------------< retrieve from fileMap all files matching *.cpp >---------

std::vector<ClientHandler::File>& ClientHandler::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}
//--------------------< retrieve from fileMap all files matching *.cs >-------------------------

std::vector<ClientHandler::File>& ClientHandler::csharpFiles()
{
	csharpFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cs") && !contains(item.first, "*.htm"))
		{
			for (auto file : item.second)
				csharpFiles_.push_back(file);
		}
	}
	return csharpFiles_;
}
//-------------------------< report number of Source Lines Of Code (SLOCs) >---------------------

ClientHandler::Slocs ClientHandler::fileSLOCs(const File& file)
{
	return slocMap_[file];
}
//-----------------------------< returns current time >----------------------------

std::string ClientHandler::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	temp.erase(8, 1);
	return temp;
}

//----------< This is where the set of files are parsed and ASTNodes are created and added >--------

void ClientHandler::processSourceCode(bool showProc)
{
	for (auto file : cppHeaderFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);        if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next()){
			pParser_->parse();}
		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : cppImplemFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		vec.push_back(file);
		if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n"; continue;
		}
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();
		Slocs slocs = pRepo_->Toker()->currentLineCount(); slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : csharpFiles()) {
		pRepo_->package() = FileSystem::Path::getName(file);
		if (!configure_.Attach(file)) {
			std::ostringstream out;
			out << "\n  could not open file " << file << "\n";
			continue;}
		pRepo_->language() = Language::CSharp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	std::ostringstream out; out << std::left << "\r  " << std::setw(77) << " ";
}

//-----------------------------< returns number of files >----------------------------

size_t ClientHandler::numFiles()
{
	return numFiles_;
}

//-----------------------------< returns number of directories >----------------------------

size_t ClientHandler::numDirs()
{
	return numDirs_;;
}

//-----------------------------<populates type table >----------------------------

TypeTable ClientHandler::getTypeTable()
{
	bool succeeded = ProcessCommandLine();
	std::ostringstream tOut("Dependency Analysis");
	Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
	std::cout << "\n--------------------------------------------------------------------------------------------------------------------------------\n";
	getSourceFiles("/CodeFiles");
	processSourceCode(true);
	TypeTable TT;
	TypeAnalysis demo;
	demo.filltypeTable(pRepo_);
	TypeTable x = demo.getMainTable();
	std::unordered_map<std::string, TypeRecord> trialTable = x.getTypeTable();
	TT.setFromInstance(trialTable);
	return TT;
}

//---------------------------< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr)
{
	return new ServerMockChannel(pISendr, pIRecvr);
}

//-----------------------------< Main >----------------------------

int main(int argc, char** argv)
{
	std::cout << "\n==========Server Port: " << argv[1] << " =============\n\n";
	Show::attach(&std::cout);
	Show::start();
	Show::title("Testing Socket Server", '=');
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	ServerMockChannel* pMockChannel = dynamic_cast<ServerMockChannel*>(objFact.createMockChannel(pSendr, pRecvr));
	pMockChannel->setPort(std::stoi(argv[1]));
	pMockChannel->port_ = argv[1];
	FileSystem::Directory::create(argv[2]);
	pMockChannel->setAddr(argv[2]);
	pMockChannel->start();
	//pMockChannel->stop();
	std::cin.get();
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "quit"));
	pSendr->postMessage(msg);
}

//-----------------------------< End >----------------------------
