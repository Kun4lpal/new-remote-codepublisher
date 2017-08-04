#pragma once
///////////////////////////////////////////////////////////////////////////
// ServerCommChannel.h - C++\CLI implementation of WPF Application       //
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
*  Dependency analysis.h, StrongComponents.h
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
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
#include "../Sockets/Sockets.h"
#include "../ClientCommChannel/ICommChannel.h"
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include "../Project3/Project_2/DependencyAnalysis/DependencyAnalysis.h"
#include "../Project3/Project_2/StrongComponents/StrongComponents.h"
#include "../Project3/Project_2/CodeAnalyzer/FileMgr/IFileMgr.h"
#include "../Project3/Project_2/CodeAnalyzer/FileMgr/FileMgr.h"

using namespace FileSystem;
using StrongComp = std::vector<std::vector<Vertex<std::string, std::string>*>>;
using SCDataBase = NoSqlDb<std::string>;

class AnalFileMgr : public FileManager::FileMgr
{
public:
	//<-----------------------------------using statements---------------------------------->
	using Path = std::string;
	using File = std::string;
	using Files = std::vector<File>;
	using Pattern = std::string;
	using Ext = std::string;
	using FileMap = std::unordered_map<Pattern, Files>;

	//<---------------------------------function declarations---------------------------------->
	AnalFileMgr(const Path& path, FileMap& fileMap);  //constructor
	virtual void file(const File& f);
	virtual void dir(const Dir& d);
	virtual void done();
	size_t numFiles();
	size_t numDirs();

	//<----------------------------------private data members---------------------------------->
private:
	Path d_;
	FileMap& fileMap_;
	bool display_;
	size_t numFiles_;
	size_t numDirs_;
};

/////////////////////////////////////////////////////////////////////////////
// ClientHandler class
// - handler the msg accepted by server
//
class ClientHandler
{
	using Path = std::string;
	using File = std::string;
	using Files = std::vector<File>;
	using Pattern = std::string;
	using Ext = std::string;
	using FileMap = std::unordered_map<Pattern, Files>;
	using Patterns = std::vector<Pattern>;
	using Options = std::vector<char>;
	using XMLPath = std::string;
	using FileNodes = std::vector<std::pair<File, ASTNode*>>;
	using Slocs = size_t;
	using SlocMap = std::unordered_map<File, Slocs>;

public:
	ClientHandler();
	void setCommandLinepath(std::string path);
	std::string getCommandLinePath();
	void setPort(std::string port);
	std::string getPort();
	void operator()(Socket& socket_);
	void addPatterns(std::string str);
	bool upLoadHandling(Socket& socket_, std::string filespec);
	bool downLoadHandling(Socket& socket_, HttpMessage msg);
	bool LazydownLoadHandling(Socket & socket_, HttpMessage msg);
	bool exploreHandling(Socket& socket_, HttpMessage msg);
	bool searchHtmlHandling(Socket & socket_, HttpMessage msg);
	std::vector<File> fileSet();
	bool publishHandling(Socket & socket_, HttpMessage msg);
	bool testHandling(Socket& socket_, HttpMessage msg);

	XMLPath xpath_;
	std::string Serverport;
	std::vector<std::string> vec;
	Parser* pParser_;
	ConfigParseForCodeAnal configure_;
	Repository* pRepo_;
	FileMap fileMap_;
	Patterns patterns_;
	std::vector<File> cppHeaderFiles_;
	size_t numFiles_;
	size_t numDirs_;
	Options options_;
	FileNodes fileNodes_;
	std::vector<File> cppImplemFiles_;
	std::vector<File> csharpFiles_;
	SlocMap slocMap_;
	SCDataBase strongCompDB;
	void showUsage();
	std::string replaceSubString(std::string replacer, const std::string & replacable, const std::string & replace) const;
	void launch(std::string value);
	bool ProcessCommandLine();
	void showCommandLineArguments(int argc, char * argv[]);
	std::string getAnalysisPath();
	void getSourceFiles(std::string file);
	std::vector<File>& cppHeaderFiles();
	std::vector<File>& cppImplemFiles();
	std::vector<File>& csharpFiles();
	ClientHandler::Slocs fileSLOCs(const File & file);
	std::string systemTime();
	void processSourceCode(bool showProc);
	size_t numFiles();
	size_t numDirs();
	TypeTable getTypeTable();
	NoSqlDb<std::string> database;

private:
	Path pathofSERVER_REPO_;
};

/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads HttpMessages from Sendr and writes HttpMessages to Recvr
//
class ServerMockChannel : public IMockChannel
{
public:
	ServerMockChannel(ISendr* pSendr, IRecvr* pRecvr);
	~ServerMockChannel();
	void start();
	void stop();
	void setPort(int port);
	void setAddr(std::string path);

	std::string port_;
	SocketSystem ss;
	SocketListener* sl;
	ClientHandler ch;
	std::thread thread_;
	ISendr* pISendr_;
	IRecvr* pIRecvr_;
	std::string fpath;
	bool stop_ = false;
};