#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application                  //
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
* void uploadfile(Object^ obj, RoutedEventArgs^ args);   //file upload message sent to queue
* void browseForFolder(Object ^ sender, RoutedEventArgs ^ args); // Event handler for browse window
* void getItemsFromList(Object ^ sender, RoutedEventArgs ^ args); //Get only items which were selected from list
* void publishFiles(Object ^ obj, RoutedEventArgs ^ args); // publish command to kick code analysis and publishing
* void enableUpload(Object^ sender, MouseButtonEventArgs^ e); //command to enable upload
*
* void explorefile(Object^ obj, RoutedEventArgs^ args); // command to perform code file search
* void getHtmlList(Object ^ obj, RoutedEventArgs ^ args); // command to perform html file search
* void getHtmlListld(Object ^ obj, RoutedEventArgs ^ args); // command to perform html file search for lazy load view
* void downloadfile(Object^ obj, RoutedEventArgs^ args); // command to download a file
* void downloadfileld(Object ^ obj, RoutedEventArgs ^ args); // command to download a file in lazy load view
* void lazyDload(Object ^ obj, RoutedEventArgs ^ args);  // command to perform lazy download
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

using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;
using namespace System::Windows::Controls;
using namespace System::Windows::Controls::Primitives;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Collections;

#include "../ClientCommChannel/ICommChannel.h"
#include "../Message/HttpMessage.h"
#include <iostream>

namespace CppCliWindows
{
	ref class WPFCppCliDemo : Window
	{
//<-----------------------------------------MockChannel references------------------------------------->
		ISendr* pSendr_;
		IRecvr* pRecvr_;
		IMockChannel* pChann_;

//<-----------------------------------------Support for Windows------------------------------------->

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hUploadTab = gcnew TabItem(); // upload tab
		TabItem^ hDownloadTab = gcnew TabItem(); // download tab
		TabItem^ hLazyTab = gcnew TabItem(); //lazy download tab
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

//<-----------------------------------------Download control------------------------------------->
		int countDownloadFile;
		Grid^ hDownloadGrid = gcnew Grid();
		ScrollViewer^ hScrollViewer3 = gcnew ScrollViewer();
		StackPanel^ hStackPanel10 = gcnew StackPanel();
		Label^ hfilenamesLabel = gcnew Label();
		StackPanel^ hStackPanel11 = gcnew StackPanel();
		TextBlock^ hTextBlock3 = gcnew TextBlock();
		ListBox^ downloadListbox = gcnew ListBox();
		StackPanel^ hStackPanel12 = gcnew StackPanel();
		Label^ hClientPortdLabel = gcnew Label();
		TextBox^ hClientPortdTB = gcnew TextBox();
		Label^ hServerAddrdLAbel = gcnew Label();
		TextBox^ hServerAddrdTB = gcnew TextBox();
		Label^ hServerPortdLabel = gcnew Label();
		TextBox^ hServerPortdTB = gcnew TextBox();
		StackPanel^ hStackPanel13 = gcnew StackPanel();
		Button^ hExploreButton = gcnew Button();
		Button^ hDownLButton = gcnew Button();
		Button^ hClearButton11 = gcnew Button();
		Button^ hHtmlButton = gcnew Button();
		StackPanel^ hStackPanel14 = gcnew StackPanel();
		Label^ UsageLabel = gcnew Label();

//<-----------------------------------------Upload View------------------------------------->
		Grid^ hUploadGrid = gcnew Grid();
		Button^ hUploadButton = gcnew Button();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hShowItemsButton = gcnew Button();
		Button^ publish = gcnew Button();
		TextBox^ hFileSpec = gcnew TextBox();
		TextBox^ hServerAddr = gcnew TextBox();
		TextBox^ hServerPort = gcnew TextBox();
		TextBox^ hClientPort = gcnew TextBox();
		TextBlock^ hTextBlock0 = gcnew TextBlock();
		ScrollViewer^ hScrollViewer0 = gcnew ScrollViewer();
		ScrollViewer^ hScrollViewer4 = gcnew ScrollViewer();
		StackPanel^ hStackPanel0 = gcnew StackPanel();
		StackPanel^ hStackPanel2 = gcnew StackPanel();
		StackPanel^ hStackPanel3 = gcnew StackPanel();
		StackPanel^ hStackPanelup = gcnew StackPanel();
		
		Label^ hFilespecLable = gcnew Label();
		Label^ hClientPortLable = gcnew Label();
		Label^ hServerAddrLable = gcnew Label();
		Label^ hServerPortLable = gcnew Label();
		Button^ hClearButton1 = gcnew Button();
		ListBox^ hListBox = gcnew ListBox();
		ListBox^ uploadListbox = gcnew ListBox();
		int countUploadFile;
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
	
//<-----------------------------------------Lazy Download view------------------------------------->
		int countDownloadFileld;
		Grid^ hLazyGrd = gcnew Grid();
		Button^ lazyDownload = gcnew Button();
		ScrollViewer^ hScrollViewerld = gcnew ScrollViewer();
		StackPanel^ hStackPanel10ld = gcnew StackPanel();
		Label^ hfilenamesLabelld = gcnew Label();
		StackPanel^ hStackPanel11ld = gcnew StackPanel();
		TextBlock^ hTextBlockld3 = gcnew TextBlock();
		ListBox^ downloadListboxld = gcnew ListBox();
		StackPanel^ hStackPanel12ld = gcnew StackPanel();
		Label^ hClientPortdLabelld = gcnew Label();
		TextBox^ hClientPortdTBld = gcnew TextBox();
		Label^ hServerAddrdLAbelld = gcnew Label();
		TextBox^ hServerAddrdTBld = gcnew TextBox();
		Label^ hServerPortdLabelld = gcnew Label();
		TextBox^ hServerPortdTBld = gcnew TextBox();
		StackPanel^ hStackPanel13ld = gcnew StackPanel();
		Button^ hExploreButtonld = gcnew Button();
		Button^ hDownLButtonldld = gcnew Button();
		Button^ hClearButton11ld = gcnew Button();
		Button^ hHtmlButtonld = gcnew Button();
		StackPanel^ hStackPanel14ld = gcnew StackPanel();
		Label^ UsageLabelld = gcnew Label();
		// receive thread
		Thread^ recvThread;

//<-----------------------------------------public data members------------------------------------>
	public:
		WPFCppCliDemo();
		~WPFCppCliDemo();
		void setUpStatusBar();
		void setUpTabControl();
		void setUpUpDownView();
		void setUpDownLoadView();
		void setUpLazyDownload();

//<-----------------------------------------upload Event------------------------------------>
		void uploadfile(Object^ obj, RoutedEventArgs^ args);
		void browseForFolder(Object ^ sender, RoutedEventArgs ^ args);
		void getItemsFromList(Object ^ sender, RoutedEventArgs ^ args);
		void publishFiles(Object ^ obj, RoutedEventArgs ^ args);
		void enableUpload(Object^ sender, MouseButtonEventArgs^ e);

//<--------------------------------------download and explore event------------------------------------>
		void explorefile(Object^ obj, RoutedEventArgs^ args);
		void getHtmlList(Object ^ obj, RoutedEventArgs ^ args);
		void getHtmlListld(Object ^ obj, RoutedEventArgs ^ args);
		void downloadfile(Object^ obj, RoutedEventArgs^ args);
		void downloadfileld(Object ^ obj, RoutedEventArgs ^ args);
		void lazyDload(Object ^ obj, RoutedEventArgs ^ args);

//<-----------------------------------------Search Event------------------------------------>
		void enableDownload(Object^ sender, MouseButtonEventArgs^ e);
		void addText(String^ msg);
		void addListBox(String^ msg);
		void clickDownload(String ^ msg);
		void getMessage();
		void clear(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);

//<-----------------------------------------private data members------------------------------------>
	private:
		List<String^>^ listServerAddrs = gcnew List<String^>();
		List<String^>^ listWildCards = gcnew List<String^>();
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setTextBlock0();
		void setTextBoxes0();
		void setTextBoxes1();
		void setButtons0();
		void setTextListBlock();
		void setTextListBlockld();
		void setAddrInfo();
		void setAddrInfold();
		void setDownLoadButton();
		void setDownLoadButtonld();
		void setUsage();
		void setUsageld();
	};
}

#endif
