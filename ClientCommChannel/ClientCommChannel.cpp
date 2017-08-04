#define IN_DLL
///////////////////////////////////////////////////////////////////////////
// ClientCommChannel.cpp - C++\CLI implementation of WPF Application     //
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
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
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

#include "ICommChannel.h"
#include "../Project3/Project_2/Proj_1/XmlDocument/Utilities/Utilities.h"
#include "../Logger/Logger.h"
#include "../BlockingQueue/BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "../Project3/Project_2/CodeAnalyzer/FileSystem/FileSystem.h"
#include <string>
#include <thread>
#include <iostream>
#include <chrono>

using namespace RemoteLogger;
using Util = Utilities::StringHelper;
template<typename T>
using Conv = Utilities::Converter<T>;
using Show = StaticLogger<1>;

using BQueue = BlockingQueue < HttpMessage >;
using namespace FileSystem;

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts HttpMessages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
	void postMessage(const HttpMessage& msg);
	BQueue& queue();
	bool connect(std::string addr, size_t port);
	bool shutdown();
	SocketConnecter& sc();
private:
	SocketSystem ss;
	SocketConnecter sc_;
	BQueue sendQ_;
};

void Sendr::postMessage(const HttpMessage& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }

SocketConnecter& Sendr::sc() { return sc_; }

// connect method to a addr and port
bool Sendr::connect(std::string addr, size_t port)
{
	while (!sc_.connect(addr, port))
	{
		std::cout << "\nsender waiting to connect\n";
		::Sleep(100);
	}
	std::cout << "\nSender connect success: " << addr << " " << port << "\n";
	return true;
}

// shut down the sender
bool Sendr::shutdown()
{
	std::cout << "\nSender is shutting down.\n";
	return sc_.shutDown();
}

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

std::string Recvr::getMessage()
{
	return recvQ_.deQ();
}

BlockingQueue<std::string>& Recvr::queue()
{
	return recvQ_;
}


/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads HttpMessages from Sendr and writes HttpMessages to Recvr
//
class ClientMockChannel : public IMockChannel
{
public:
	ClientMockChannel(ISendr* pSendr, IRecvr* pRecvr);
	void start();
	void stop();

private:
	void parseMsg(HttpMessage msg);
	void LazyDownLoadMsg(HttpMessage msg, std::string filename);
	void uploadMsg(HttpMessage msg);
	void getHtmlList(HttpMessage msg);
	void dotest(HttpMessage msg);
	void publishFiles(HttpMessage msg);
	void DownLoadMsg(HttpMessage msg, std::string filespec);
	void exploreMsg(HttpMessage msg);
	std::string ItoS(long i);
	std::thread thread_;
	ISendr* pISendr_;
	IRecvr* pIRecvr_;
	bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------

ClientMockChannel::ClientMockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------

void ClientMockChannel::start()
{
	std::cout << "\n  MockChannel starting up";
	thread_ = std::thread(
		[this] {
		Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
		Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
		if (pSendr == nullptr || pRecvr == nullptr)
		{
			std::cout << "\n  failed to start Mock Channel\n\n";
			return;
		}
		BQueue& sendQ = pSendr->queue();
		BlockingQueue<std::string>& recvQ = pRecvr->queue();
		while (!stop_)
		{
			std::cout << "\n  channel deQing HttpMessage";
			HttpMessage msg = sendQ.deQ();  // will block here so send quit HttpMessage when stopping
			parseMsg(msg);
			std::cout << "\n  channel enQing HttpMessage";
		}
		std::cout << "\n  Server stopping\n\n";
	});
}
//----< signal server thread to stop >---------------------------------------

void ClientMockChannel::stop() { stop_ = true; }

//----<helper func >---------------------------------------

std::string ClientMockChannel::ItoS(long i)
{
	std::string s;
	std::ostringstream oss;
	oss << i;
	s = oss.str();
	return s;
}

//----< parsemsg >---------------------------------------

void ClientMockChannel::parseMsg(HttpMessage msg)
{
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);

	std::string cmd = msg.findValue("Command");
	pRecvr->queue().enQ(msg.toString());

	SocketConnecter& sc = pSendr->sc();
	pSendr->connect(msg.findValue("RecvAddr"), std::stoi(msg.findValue("RecvPort")));

	if (cmd == "Upload") 
	{
		uploadMsg(msg);
	}
	else if (cmd == "Publish") {
		publishFiles(msg);
	}
	else if (cmd == "test") 
	{
		dotest(msg);
	}
	else if (cmd == "Html") {
		getHtmlList(msg);
	}
	else if (cmd == "Download")
	{
		std::string filespec = msg.findValue("FileSpec");
		int pos = filespec.find_last_of("/");   //here create a string value and parse it to get all the names of files
		if (pos>-1) filespec = filespec.substr(pos);
		DownLoadMsg(msg, filespec);
	}
	else if (cmd == "LazyDownload")
	{
		std::string filespec = msg.findValue("FileSpec");
		int pos = filespec.find_last_of("/");   //here create a string value and parse it to get all the names of files
		if (pos>-1) filespec = filespec.substr(pos);
		LazyDownLoadMsg(msg, filespec);
	}
	else if (cmd == "Explore")
		exploreMsg(msg);

	pSendr->shutdown();
}

//----< Explore Msg Handling >---------------------------------------

void ClientMockChannel::exploreMsg(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";

	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);

	SocketConnecter& sc = pSendr->sc();

	sc.sendString(msg.toString());
	msg.addPattern("*.*");
	sc.sendString(msg.patternsString());

	while (true)
	{
		std::string temp1 = sc.recvString();
		/// put information into recv queue
		if (temp1 == "quit") break;
		std::cout << temp1<<std::endl;
		pRecvr->queue().enQ(temp1);
		if (sc == INVALID_SOCKET)
		{
			pRecvr->queue().enQ("Explore failed:  ");
			return;
		}

	}
	pRecvr->queue().enQ("Explore in " + msg.findValue("RecvAddr") + ":" + msg.findValue("RecvPort") + " is done.");
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Explore took " + ItoS(t) + " miliseconds.");
}

//----< Explore Msg Handling >---------------------------------------

void ClientMockChannel::getHtmlList(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	sc.sendString(msg.toString());
	msg.addPattern("*.*");
	sc.sendString(msg.patternsString());
	while (true)
	{
		std::string temp1 = sc.recvString();
		/// put information into recv queue
		if (temp1 == "quit") break;
		std::cout << temp1 << std::endl;
		pRecvr->queue().enQ(temp1);
		if (sc == INVALID_SOCKET)
		{
			pRecvr->queue().enQ("Explore failed:  ");
			return;
		}

	}
	pRecvr->queue().enQ("Search in " + msg.findValue("RecvAddr") + ":" + msg.findValue("RecvPort") + " is done.");
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Search took " + ItoS(t) + " miliseconds.");
}

//----< runs a simple test >---------------------------------------

void ClientMockChannel::dotest(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	std::string command = "test";
	sc.sendString(command);
	/*msg.addPattern("*.*");
	msg.addAttribute(msg.attribute("kappa","pride"));
	sc.sendString(msg.patternsString());*/

	while (true)
	{
		std::string temp1 = sc.recvString();
		std::cout << "\n\n\nResult From Server:  " << temp1 << "\n\n";
		/// put information into recv queue
		if (temp1 == "quit") break;
		pRecvr->queue().enQ(temp1);
		if (sc == INVALID_SOCKET)
		{
			pRecvr->queue().enQ("Test failed:  ");
			return;
		}
	}
	pRecvr->queue().enQ("Explore in " + msg.findValue("RecvAddr") + ":" + msg.findValue("RecvPort") + " is done.");
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Explore took " + ItoS(t) + " miliseconds.");
}

//----< publish message handling >---------------------------------------

void ClientMockChannel::publishFiles(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	sc.sendString(msg.toString());
	while (true)
	{
		std::string temp1 = sc.recvString();
		/// put information into recv queue
		if (temp1 == "quit") break;
		std::cout << temp1 << std::endl;
		pRecvr->queue().enQ(temp1);
		if (sc == INVALID_SOCKET)
		{
			pRecvr->queue().enQ("Explore failed:  ");
			return;
		}

	}
	pRecvr->queue().enQ("Publish in " + msg.findValue("RecvAddr") + ":" + msg.findValue("RecvPort") + " is done.");
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Publish took " + ItoS(t) + " miliseconds.");
}

//----< Download Msg Handling >---------------------------------------

void ClientMockChannel::DownLoadMsg(HttpMessage msg, std::string filename)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n" << msg.toString() << "\n";
	std::string fileSpec = "";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	sc.sendString(msg.toString());
	const size_t BufLen = 512; Socket::byte buffer[BufLen]; bool ok;
	std::string fileNameSub = filename.substr(filename.find_last_of('/\\')+1);
	std:: string path = "../ClientRepository/" + fileNameSub;
	FileSystem::File bufferOut(path);
	bufferOut.open(FileSystem::File::out, FileSystem::File::binary);
	if (!bufferOut.isGood())
	{
		std::cout << "\n  could not open \"" << filename << "\" for writing\n\n";
		pRecvr->queue().enQ("Could not open:  " + filename); return;
	}
	else
		std::cout << "\n  opening: \"" << filename << "\" for writing";
	while (true)
	{
		std::string temp1 = sc.recvString();
		if (temp1 == "quit") break;
		msg = HttpMessage::parseMessage(temp1);
		//fileSpec = msg.findValue("FileSpec");
		size_t numBody = Conv<size_t>::toValue(msg.findValue("NumBody"));
		ok = sc.recv(numBody, buffer);
		if (sc == INVALID_SOCKET || !ok){
			pRecvr->queue().enQ("Download failed:  " + filename);
			return;
		}
		std::string temp;
		for (size_t i = 0; i < numBody; ++i)
			temp += buffer[i];
		std::cout << "\nserver recvd : " + temp;
		bufferOut.putBuffer(numBody, buffer);
		if (!bufferOut.isGood()) break;
		std::cout << "\n";
	}
	std::cout << "\n  Closing: \"" << filename << "\" for writing";
	bufferOut.close();
	std::string result = "File " + fileSpec +" has been download.";
	pRecvr->queue().enQ("Result From Server:  " + result);
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Download took " + ItoS(t) + " miliseconds.");
}

//----------------------------< LazyDownload Msg Handling >---------------------------------------

void ClientMockChannel::LazyDownLoadMsg(HttpMessage msg, std::string filename)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	msg.removeAttribute("FileSpec");
	std::string fileSpc = filename.substr(filename.find_last_of('/\\')+1);
	fileSpc = fileSpc.substr(0,fileSpc.length()-4);
	msg.addAttribute(msg.attribute("FileSpec",fileSpc));
	sc.sendString(msg.toString());
	while (true)
	{
		std::string temp1 = sc.recvString();
		/// put information into recv queue
		if (temp1 == "quit") break;
		std::cout << temp1 << std::endl;
		pRecvr->queue().enQ(temp1);
		if (sc == INVALID_SOCKET)
		{
			pRecvr->queue().enQ("LazyDownload failed:  ");
			return;
		}
	}
	pRecvr->queue().enQ("LazyDownload in " + msg.findValue("RecvAddr") + ":" + msg.findValue("RecvPort") + " is done.");
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("LazyDownload took " + ItoS(t) + " miliseconds.");
}
//----< Upload Msg Handling >---------------------------------------

void ClientMockChannel::uploadMsg(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	std::string value = msg.findValue("FileSpec");
	//value = value.substr(value.find_last_of('/\\'));
	FileSystem::File bufferIn(value);
	bufferIn.open(FileSystem::File::in, FileSystem::File::binary);
	if (!bufferIn.isGood())
	{
		std::cout << "\ncould not open \"" << FileSystem::Path::getFullFileSpec(msg.findValue("FileSpec")) << "\" for uploading";
		pRecvr->queue().enQ("Could not open file:  " + msg.findValue("FileSpec"));
		return;
	}
	else
		std::cout << "\nopening: \"" << msg.findValue("FileSpec") << "\" for uploading";
	std::cout << "\n\nStart sending.";
	sc.sendString(msg.toString());
	const size_t bufLen = 512;
	FileSystem::File::byte buffer[bufLen];
	while (true)
	{
		size_t resultSize = bufferIn.getBuffer(bufLen, buffer);
		std::string temp1(buffer, resultSize);
		std::cout << "\nsend body:" << temp1;
		if (temp1.size() == 0)
			break;
		msg.addAttribute(msg.attribute("NumBody", Conv<size_t>::toString(resultSize)));//conver this to string
		sc.sendString(msg.toString());
		msg.removeAttribute("NumBody");
		sc.send(resultSize, buffer);
		if (resultSize < bufLen || !bufferIn.isGood())
		{
			std::cout << "\nclose file.";
			bufferIn.close();
			break;
		}
	}
	sc.sendString("quit");
	std::string result = sc.recvString();
	std::cout << "\n\n\nResult From Server:  " << result << "\n\n";
	pRecvr->queue().enQ("Result From Server:  " + result);
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Upload took " + ItoS(t) + " miliseconds.");
}



//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr)
{
	return new ClientMockChannel(pISendr, pIRecvr);
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
	pMockChannel->start();
	pSendr->postHttpMessage("Hello World");
	pSendr->postHttpMessage("CSE687 - Object Oriented Design");
	HttpMessage msg = pRecvr->getHttpMessage();
	std::cout << "\n  received HttpMessage = \"" << msg << "\"";
	msg = pRecvr->getHttpMessage();
	std::cout << "\n  received HttpMessage = \"" << msg << "\"";
	pSendr->postHttpMessage("stopping");
	msg = pRecvr->getHttpMessage();
	std::cout << "\n  received HttpMessage = \"" << msg << "\"";
	pMockChannel->stop();
	pSendr->postHttpMessage("quit");
	std::cin.get();
}
#endif
