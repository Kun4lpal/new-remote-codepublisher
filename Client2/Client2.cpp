///////////////////////////////////////////////////////////////////////////
// Client2.cpp - C++\CLI implementation of WPF Application               //
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
void parseMsg(HttpMessage msg);        //does message parsing to find command.
void LazyDownLoadMsg(HttpMessage msg, std::string filename);  // sends lazy download message using sockets
void uploadMsg(HttpMessage msg);          // sends an upload message using sockets.
void getHtmlList(HttpMessage msg);      //gets the list of html files available in server.
void dotest(HttpMessage msg);     // a test message - this is where it all started
void publishFiles(HttpMessage msg); // a message that restarts publishing and code analysis.
void DownLoadMsg(HttpMessage msg, std::string filespec); // downloads file specified in the message.
void exploreMsg(HttpMessage msg);
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

#include "Client2.h"
#include <string>

//-------------------------------------------< using statements >--------------------------------

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

//-----------------------------------------< posts message to queue >--------------------------------

void Sendr::postMessage(const HttpMessage& msg)
{
	sendQ_.enQ(msg);
}

//-----------------------------------------< gets queue reference >--------------------------------

BQueue& Sendr::queue() { return sendQ_; }

//-----------------------------------------< gets socket reference >--------------------------------

SocketConnecter& Sendr::sc() { return sc_; }

//-------------------------------< connects to the port and address specifed >------------------

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

//-------------------------------< shot down sender >--------------------------------

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

//-----------------------------------------< performs get message >--------------------------------

std::string Recvr::getMessage()
{
	return recvQ_.deQ();
}

//-----------------------------------------< returns recieve queue reference >--------------------------------

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

//--------------------------------< pass pointers to Sender and Receiver >-------------------------------

ClientMockChannel::ClientMockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//--------------------< creates thread to read from sendQ and echo back to the recvQ >-------------------

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
//-----------------------------< signal server thread to stop >---------------------------------------

void ClientMockChannel::stop() { stop_ = true; }

//--------------------------------<helper func >---------------------------------------

std::string ClientMockChannel::ItoS(long i)
{
	std::string s;
	std::ostringstream oss;
	oss << i;
	s = oss.str();
	return s;
}

//-----------------------------------------< parsemsg >---------------------------------------

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

//-----------------------------------< Explore Msg Handling of code files >---------------------------------------

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
		std::cout << temp1 << std::endl;
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

//------------------------------< Explore Msg Handling of Html files >---------------------------------------

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

//-----------------------------------------< a test function >--------------------------------

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

//--------------------------------< sends publish command message to server >--------------------------------

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

//-----------------------------< Download Msg Handling >---------------------------------------

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
	std::string fileNameSub = filename.substr(filename.find_last_of('/\\') + 1);
	std::string path = "../ClientRepository/" + fileNameSub;
	FileSystem::File bufferOut(path);
	bufferOut.open(FileSystem::File::out, FileSystem::File::binary);
	if (!bufferOut.isGood())
	{
		std::cout << "\n  could not open \"" << filename << "\" for writing\n\n";
		pRecvr->queue().enQ("Could not open:  " + filename); return;}
	else
		std::cout << "\n  opening: \"" << filename << "\" for writing";
	while (true)
	{   std::string temp1 = sc.recvString();
		if (temp1 == "quit") break;
		msg = HttpMessage::parseMessage(temp1);
		//fileSpec = msg.findValue("FileSpec");
		size_t numBody = Conv<size_t>::toValue(msg.findValue("NumBody"));
		ok = sc.recv(numBody, buffer);
		if (sc == INVALID_SOCKET || !ok) {
			pRecvr->queue().enQ("Download failed:  " + filename);
			return;
		}
		std::string temp;
		for (size_t i = 0; i < numBody; ++i)
			temp += buffer[i];
		std::cout << "\nserver recvd : " + temp;
		bufferOut.putBuffer(numBody, buffer);
		if (!bufferOut.isGood()) break;
		std::cout << "\n";}
	std::cout << "\n  Closing: \"" << filename << "\" for writing";
	bufferOut.close();
	std::string result = "File " + fileSpec + " has been download.";
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

//---------------------------------< Upload Msg Handling >---------------------------------------

void ClientMockChannel::uploadMsg(HttpMessage msg)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "\n\n==========MSG INFO==========\n";
	std::cout << msg.toString();
	std::cout << "\n";
	Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	SocketConnecter& sc = pSendr->sc();
	FileSystem::File bufferIn(msg.findValue("FileSpec"));
	bufferIn.open(FileSystem::File::in, FileSystem::File::binary);
	if (!bufferIn.isGood()){
		std::cout << "\ncould not open \"" << msg.findValue("FileSpec") << "\" for uploading";
		pRecvr->queue().enQ("Could not open file:  " + msg.findValue("FileSpec"));
		return;}
	else
		std::cout << "\nopening: \"" << msg.findValue("FileSpec") << "\" for uploading";
	std::cout << "\n\nStart sending.";
	sc.sendString(msg.toString());
	const size_t bufLen = 512;
	FileSystem::File::byte buffer[bufLen];
	while (true){
		size_t resultSize = bufferIn.getBuffer(bufLen, buffer);
		std::string temp1(buffer, resultSize);
		std::cout << "\nsend body:" << temp1;
		if (temp1.size() == 0)
			break;
		msg.addAttribute(msg.attribute("NumBody", Conv<size_t>::toString(resultSize)));//conver this to string
		sc.sendString(msg.toString());
		msg.removeAttribute("NumBody");
		sc.send(resultSize, buffer);
		if (resultSize < bufLen || !bufferIn.isGood()){
			std::cout << "\nclose file.";
			bufferIn.close();
			break;}}
	sc.sendString("quit");
	std::string result = sc.recvString();
	std::cout << "\n\n\nResult From Server:  " << result << "\n\n";
	pRecvr->queue().enQ("Result From Server:  " + result);
	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	pRecvr->queue().enQ("Upload took " + ItoS(t) + " miliseconds.");
}

//------------------------< factory functions >----------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr)
{
	return new ClientMockChannel(pISendr, pIRecvr);
}

//--------------------------< Client 2 constructor >--------------------------------

Client2::Client2()
{
}

//--------------------------< Client 2 destructor >--------------------------------

Client2::~Client2()
{
}

//--------------------------< A function to create a message with command upload >----------------------------

HttpMessage createUploadMessage(std::string addr, std::string recvp, std::string sendp,std::string fileSpec) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Upload"));
	msg.addAttribute(msg.attribute("FileSpec", fileSpec));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", recvp));
	return msg;
}

//--------------------------< A function to create a message with command download >----------------------------

HttpMessage createDownloadMessage(std::string addr, std::string recvp, std::string sendp,std::string filename) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Download"));
	msg.addAttribute(msg.attribute("FileSpec", filename));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", recvp));
	return msg;
}

//--------------------------< A function to create a message with commad explore >----------------------------

HttpMessage creteFileSearchMessage(std::string addr, std::string recvp, std::string sendp) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Explore"));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", recvp));
	return msg;
}

//--------------------------< A function to create a message with command publish >----------------------------

HttpMessage createpublishFiles(std::string addr, std::string recvp, std::string sendp) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Publish"));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", recvp));
	return msg;
}

//---------------------< A function to create a message with command getHtmlFileList >------------------

HttpMessage getHtmlFileList(std::string addr, std::string recvp, std::string sendp) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Html"));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", recvp));
	return msg;
}

//---------------------< A function to create a message with command - lazy download >---------------------

HttpMessage doLazyDownload(std::string addr, std::string rcvp, std::string sendp) {
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "LazyDownload"));
	msg.addAttribute(msg.attribute("SendAddr", addr));
	msg.addAttribute(msg.attribute("SendPort", sendp));
	msg.addAttribute(msg.attribute("RecvAddr", addr));
	msg.addAttribute(msg.attribute("RecvPort", rcvp));
	return msg;
}

#ifdef TEST_MOCKCHANNEL

//----------------------< test stub >----------------------------------------------------------

int main(int argc, char** argv)
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	ClientMockChannel cm (pSendr,pRecvr);
	cm.start();
	std::string arg1 = argv[1];
	std::string arg2 = argv[2];
	std::vector<std::string> files;
	std::string fullpath = FileSystem::Path::getFullFileSpec( arg1 + "AbstrSynTree.cpp");
	files.push_back(fullpath);
	for (auto it : files) {
		HttpMessage msg = createUploadMessage("localhost", "8081", "9000", it);
		pSendr->postMessage(msg);
	}
	pSendr->postMessage(getHtmlFileList("localhost", "8081", "9000"));
	HttpMessage msgf = creteFileSearchMessage("localhost", "8081", "8080");
	pSendr->postMessage(msgf);
	std::string strmsg = pRecvr->getMessage();
	std::vector<std::string> filetoDownload;
	
	filetoDownload.push_back( arg2 +  "CodeFiles/CppProperties.cpp");
	filetoDownload.push_back( arg2 +  "CodeFiles/CppProperties.h");
	for (auto it : filetoDownload) {
		HttpMessage msg2 = createDownloadMessage("localhost", "8081", "8080", it);
		pSendr->postMessage(msg2);
	}
	pRecvr->getMessage();
	std::cin.get();
}
#endif
