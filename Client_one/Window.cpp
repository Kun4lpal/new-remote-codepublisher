///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
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

#include "Window.h"
#include <string>
#include <sstream>
#include <vector>

using namespace CppCliWindows;

//<-----------------------------------------Set Up Channel------------------------------------->
WPFCppCliDemo::WPFCppCliDemo()
{   // set up channel
	ObjectFactory* pObjFact = new ObjectFactory;
	pSendr_ = pObjFact->createSendr();
	pRecvr_ = pObjFact->createRecvr();
	pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_);
	pChann_->start();
	delete pObjFact;
	recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));recvThread->Start();
	// set event handlers
	this->Loaded += gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
	this->Closing += gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
	hClearButton1->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	hClearButton11->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
	// upload event
	hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::uploadfile);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
	hShowItemsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getItemsFromList);
	uploadListbox->MouseDoubleClick += gcnew MouseButtonEventHandler(this, &WPFCppCliDemo::enableUpload);
	publish->Click += gcnew RoutedEventHandler(this,&WPFCppCliDemo::publishFiles);
	//download event
	hExploreButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::explorefile);
	hDownLButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadfile);
	hHtmlButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getHtmlList);
	downloadListbox->MouseDoubleClick += gcnew MouseButtonEventHandler(this, &WPFCppCliDemo::enableDownload);
	//lazy load event
	hDownLButtonldld->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::downloadfileld);
	hHtmlButtonld->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getHtmlListld);
	lazyDownload->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::lazyDload);
	downloadListboxld->MouseDoubleClick += gcnew MouseButtonEventHandler(this, &WPFCppCliDemo::enableDownload);
	//lazyDload->click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::lazy)
	// set Window properties
	this->Title = "Remote Code Publisher";this->Width = 900;this->Height = 500;
	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);
	// setup Window controls and views
	setUpTabControl();setUpStatusBar();
	setUpUpDownView();setUpDownLoadView();
	setUpLazyDownload();
}

//<-----------------------------------------Destructor------------------------------------->

WPFCppCliDemo::~WPFCppCliDemo()
{
	delete pChann_;
	delete pSendr_;
	delete pRecvr_;
}

//<---------------------------------------Set up status bar------------------------------------->

void WPFCppCliDemo::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "Status Bar messages";
	//status->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

//<-----------------------------------------TAB CONTROL------------------------------------->

void WPFCppCliDemo::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hUploadTab->Header = "UpLoad";
	hDownloadTab->Header = "Download";
	hLazyTab->Header = "LazyDownload";
	hTabControl->Items->Add(hUploadTab);
	hTabControl->Items->Add(hDownloadTab);
	hTabControl->Items->Add(hLazyTab);
}

//<-----------------------------------------Tab view of Upload------------------------------------->
void WPFCppCliDemo::setUpDownLoadView()
{
	Console::Write("\n  setting up upload/download view");
	hDownloadGrid->Margin = Thickness(20);
	hDownloadTab->Content = hDownloadGrid;

	setTextListBlock();
	setAddrInfo();
	setDownLoadButton();
	setUsage();
}


//<-----------------------------------------Tab view of Lazy Download---------------------------------->
void WPFCppCliDemo::setUpLazyDownload()
{
	Console::Write("\n  setting up lazy view");
	hLazyGrd->Margin = Thickness(20);
	hLazyTab->Content = hLazyGrd;
	setTextListBlockld();
	setAddrInfold();
	setDownLoadButtonld();
	setUsageld();
}


//<-----------------------------------------Set up text box in upload view ------------------------------------->
void WPFCppCliDemo::setTextListBlock()
{
	RowDefinition^ hRow0Def = gcnew RowDefinition();
	hDownloadGrid->RowDefinitions->Add(hRow0Def);
	hRow0Def->Height = GridLength(32);
	hfilenamesLabel->Content = "Look at Status Bar for steps!";
	hfilenamesLabel->FontWeight = FontWeights::Bold;
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 150;
	hStackPanel10->Children->Add(hfilenamesLabel);
	hStackPanel10->Children->Add(hSpacer1);
	hStackPanel10->Orientation = Orientation::Horizontal;
	hStackPanel10->HorizontalAlignment = System::Windows::HorizontalAlignment::Right;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hDownloadGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock3;
	hTextBlock3->Padding = Thickness(15);
	hTextBlock3->Text = "";
	hTextBlock3->FontWeight = FontWeights::Bold;
	hTextBlock3->FontSize = 16;
	hScrollViewer3->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer3->Content = hBorder1;
	hScrollViewer3->Width = 350;
	downloadListbox->Width = 350;
	downloadListbox->SelectionMode = SelectionMode::Multiple;
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 30;
	hStackPanel11->Children->Add(hScrollViewer3);
	hStackPanel11->Children->Add(hSpacer);
	hStackPanel11->Children->Add(downloadListbox);
	hStackPanel11->Orientation = Orientation::Horizontal;
	hStackPanel11->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hDownloadGrid->SetRow(hStackPanel11, 1);
	hDownloadGrid->Children->Add(hStackPanel11);
	hDownloadGrid->SetRow(hStackPanel10, 0);
	hDownloadGrid->Children->Add(hStackPanel10);

}

//<-----------------------------------------Text list box in lazy load view------------------------------------->
void WPFCppCliDemo::setTextListBlockld()
{
	RowDefinition^ hRow0Def = gcnew RowDefinition();
	hLazyGrd->RowDefinitions->Add(hRow0Def);
	hRow0Def->Height = GridLength(32);
	hfilenamesLabelld->Content = "Look at Status Bar for steps!";
	hfilenamesLabelld->FontWeight = FontWeights::Bold;
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 150;
	hStackPanel10ld->Children->Add(hfilenamesLabelld);
	hStackPanel10ld->Children->Add(hSpacer1);
	hStackPanel10ld->Orientation = Orientation::Horizontal;
	hStackPanel10ld ->HorizontalAlignment = System::Windows::HorizontalAlignment::Right;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hLazyGrd->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlockld3;
	hTextBlockld3->Padding = Thickness(15);
	hTextBlockld3->Text = "";
	//hTextBlock3->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlockld3->FontWeight = FontWeights::Bold;
	hTextBlockld3->FontSize = 16;
	hScrollViewerld->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewerld->Content = hBorder1;
	hScrollViewerld->Width = 350;
	downloadListboxld->Width = 350;
	downloadListboxld->SelectionMode = SelectionMode::Multiple;
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 30;
	hStackPanel11ld->Children->Add(hScrollViewerld);
	hStackPanel11ld->Children->Add(hSpacer);
	hStackPanel11ld->Children->Add(downloadListboxld);
	hStackPanel11ld->Orientation = Orientation::Horizontal;
	hStackPanel11ld->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLazyGrd->SetRow(hStackPanel11ld, 1);
	hLazyGrd->Children->Add(hStackPanel11ld);
	hLazyGrd->SetRow(hStackPanel10ld, 0);
	hLazyGrd->Children->Add(hStackPanel10ld);
}


//<---------------------------------------Set up addr info in upload view------------------------------------->
void WPFCppCliDemo::setAddrInfo()
{
	RowDefinition^ hRowDef = gcnew RowDefinition();
	hDownloadGrid->RowDefinitions->Add(hRowDef);
	hRowDef->Height = GridLength(20);
	RowDefinition^ hRow0Def = gcnew RowDefinition();
	hDownloadGrid->RowDefinitions->Add(hRow0Def);
	hRow0Def->Height = GridLength(30);
	hClientPortdLabel->Content = "ClientPort:";
	hServerAddrdLAbel->Content = "ServerAddr:";
	hServerPortdLabel->Content = "ServerPort:";
	hClientPortdTB->Text = "8080";
	hServerAddrdTB->Text = "localhost";
	hServerPortdTB->Text = "8081";
	Border^ hBorder0 = gcnew Border();
	hBorder0->Width = 120;
	hBorder0->Height = 30;
	hBorder0->BorderThickness = Thickness(1);
	hBorder0->BorderBrush = Brushes::Black;
	hBorder0->Child = hClientPortdTB;
	Border^ hBorder1 = gcnew Border();
	hBorder1->Width = 120;
	hBorder1->Height = 30;
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hServerAddrdTB;
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hServerPortdTB;
	hStackPanel12->Children->Add(hClientPortdLabel);hStackPanel12->Children->Add(hBorder0);
	hStackPanel12->Children->Add(hServerAddrdLAbel);hStackPanel12->Children->Add(hBorder1);
	hStackPanel12->Children->Add(hServerPortdLabel);hStackPanel12->Children->Add(hBorder2);
	hStackPanel12->Orientation = Orientation::Horizontal;
	hStackPanel12->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hDownloadGrid->SetRow(hStackPanel12, 3);
	hDownloadGrid->Children->Add(hStackPanel12);
}

//<-----------------------------------------Set up addr info in lazy load------------------------------------->
void WPFCppCliDemo::setAddrInfold()
{
	RowDefinition^ hRowDef = gcnew RowDefinition();
	hLazyGrd->RowDefinitions->Add(hRowDef);
	hRowDef->Height = GridLength(20);
	RowDefinition^ hRow0Def = gcnew RowDefinition();
	hLazyGrd->RowDefinitions->Add(hRow0Def);
	hRow0Def->Height = GridLength(30);
	hClientPortdLabelld->Content = "ClientPort:";
	hServerAddrdLAbelld->Content = "ServerAddr:";
	hServerPortdLabelld->Content = "ServerPort:";
	hClientPortdTBld->Text = "8080";
	hServerAddrdTBld->Text = "localhost";
	hServerPortdTBld->Text = "8081";
	Border^ hBorder0 = gcnew Border();
	hBorder0->Width = 120;
	hBorder0->Height = 30;
	hBorder0->BorderThickness = Thickness(1);
	hBorder0->BorderBrush = Brushes::Black;
	hBorder0->Child = hClientPortdTBld;
	Border^ hBorder1 = gcnew Border();
	hBorder1->Width = 120;
	hBorder1->Height = 30;
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hServerAddrdTBld;
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hServerPortdTBld;
	hStackPanel12ld->Children->Add(hClientPortdLabelld);
	hStackPanel12ld->Children->Add(hBorder0);
	hStackPanel12ld->Children->Add(hServerAddrdLAbelld);
	hStackPanel12ld->Children->Add(hBorder1);
	hStackPanel12ld->Children->Add(hServerPortdLabelld);
	hStackPanel12ld->Children->Add(hBorder2);
	hStackPanel12ld->Orientation = Orientation::Horizontal;
	hStackPanel12ld->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLazyGrd->SetRow(hStackPanel12ld, 3);
	hLazyGrd->Children->Add(hStackPanel12ld);
}

//<----------------------------------------set up download view buttons------------------------------------->
void WPFCppCliDemo::setDownLoadButton()
{
	RowDefinition^ Row3Def = gcnew RowDefinition();
	Row3Def->Height = GridLength(10);
	hDownloadGrid->RowDefinitions->Add(Row3Def);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(50);
	hDownloadGrid->RowDefinitions->Add(hRow3Def);
	hExploreButton->Content = "Get CodeFiles";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hDownLButton->Content = "Download";
	hBorder2->Child = hExploreButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hDownLButton;
	TextBlock^ hSpacer = gcnew TextBlock();hSpacer->Width = 30;
	TextBlock^ hSpacer1 = gcnew TextBlock();hSpacer1->Width = 30;
	TextBlock^ hSpacer2 = gcnew TextBlock();hSpacer2->Width = 30;
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hClearButton11->Content = "Clear";
	hBorder4->Child = hClearButton11;
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hHtmlButton->Content = "Get HtmlList";
	hBorder5->Child = hHtmlButton; hStackPanel13->Children->Add(hBorder2);
	hStackPanel13->Children->Add(hSpacer); hStackPanel13->Children->Add(hBorder3);
	hStackPanel13->Children->Add(hSpacer1); hStackPanel13->Children->Add(hBorder4);
	hStackPanel13->Children->Add(hSpacer2); hStackPanel13->Children->Add(hBorder5);
	hStackPanel13->Orientation = Orientation::Horizontal;
	hStackPanel13->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hDownloadGrid->SetRow(hStackPanel13, 5);
	hDownloadGrid->Children->Add(hStackPanel13);
}

//<-----------------------------------------Tab view of Lazy load view------------------------------------->
void WPFCppCliDemo::setDownLoadButtonld ()
{
	RowDefinition^ Row3Def = gcnew RowDefinition();
	Row3Def->Height = GridLength(10);
	hLazyGrd->RowDefinitions->Add(Row3Def);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(50);
	hLazyGrd->RowDefinitions->Add(hRow3Def);
	hDownLButtonldld->Content = "Lazy Download";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hDownLButtonldld;
	TextBlock^ hSpacer1 = gcnew TextBlock();hSpacer1->Width = 30;
	TextBlock^ hSpacer2 = gcnew TextBlock();hSpacer2->Width = 30;
	TextBlock^ hSpacer34 = gcnew TextBlock();hSpacer34->Width = 30;
	Border^ hBorder4 = gcnew Border(); hBorder4->Width = 120; hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hClearButton11ld->Content = "Clear";
	hBorder4->Child = hClearButton11ld;
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hHtmlButtonld->Content = "Get HtmlFiles";
	hBorder5->Child = hHtmlButtonld;
	Border^ hBorder7 = gcnew Border();hBorder7->Width = 120;hBorder7->Height = 30;
	hBorder7->BorderThickness = Thickness(1);
	hBorder7->BorderBrush = Brushes::Black;
	lazyDownload->Content = "SelectFile";
	hBorder7->Child = lazyDownload;
	hStackPanel13ld->Children->Add(hBorder5);hStackPanel13ld->Children->Add(hSpacer34);
	hStackPanel13ld->Children->Add(hBorder4);hStackPanel13ld->Children->Add(hSpacer1);
	hStackPanel13ld->Children->Add(hBorder7);hStackPanel13ld->Children->Add(hSpacer2);
	hStackPanel13ld->Children->Add(hBorder3);
	hStackPanel13ld->Orientation = Orientation::Horizontal;
	hStackPanel13ld->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLazyGrd->SetRow(hStackPanel13ld, 5);
	hLazyGrd->Children->Add(hStackPanel13ld);
}

//<-----------------------------------------Set up download usage------------------------------------->
void WPFCppCliDemo::setUsage()
{
	RowDefinition^ Row3Def = gcnew RowDefinition();
	Row3Def->Height = GridLength(10);
	hDownloadGrid->RowDefinitions->Add(Row3Def);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(30);
	hDownloadGrid->RowDefinitions->Add(hRow3Def);
	UsageLabel->Content = "Select the files to download";
	UsageLabel->FontWeight = FontWeights::Bold;
	hStackPanel14->Children->Add(UsageLabel);
	hStackPanel14->Orientation = Orientation::Horizontal;
	hStackPanel14->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hDownloadGrid->SetRow(hStackPanel14, 7);
	hDownloadGrid->Children->Add(hStackPanel14);
}

//<-----------------------------------------Set up usage in lazy load------------------------------------->
void WPFCppCliDemo::setUsageld()
{
	RowDefinition^ Row3Def = gcnew RowDefinition();
	Row3Def->Height = GridLength(10);
	hLazyGrd->RowDefinitions->Add(Row3Def);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(30);
	hLazyGrd->RowDefinitions->Add(hRow3Def);
	UsageLabelld->Content = "Select the files to download";
	UsageLabelld->FontWeight = FontWeights::Bold;
	hStackPanel14ld->Children->Add(UsageLabelld);
	hStackPanel14ld->Orientation = Orientation::Horizontal;
	hStackPanel14ld->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLazyGrd->SetRow(hStackPanel14ld, 7);
	hLazyGrd->Children->Add(hStackPanel14ld);
}



//<------------------------------------Set up textblock in upload grid------------------------------------->
void WPFCppCliDemo::setTextBlock0()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hUploadGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->Width = 350;
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock0;
	hTextBlock0->Padding = Thickness(15);
	hTextBlock0->Text = "";
	hTextBlock0->FontFamily = gcnew Windows::Media::FontFamily("Arial");
	hTextBlock0->FontWeight = FontWeights::Bold;
	hTextBlock0->FontSize = 16;
	hScrollViewer0->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
	hScrollViewer0->Content = hBorder1;
	//-----------------------------------------------------------------------------------------------------------
	hScrollViewer4->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer4->Content = hBorder1;
	hScrollViewer4->Width = 350;
	uploadListbox->Width = 350;
	uploadListbox->SelectionMode = SelectionMode::Multiple;
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 30;
	hStackPanelup->Children->Add(hScrollViewer4);
	hStackPanelup->Children->Add(hSpacer);
	hStackPanelup->Children->Add(uploadListbox);
	hStackPanelup->Orientation = Orientation::Horizontal;
	hStackPanelup->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	//-----------------------------------------------------------------------------------------------------------	
	hUploadGrid->SetRow(hStackPanelup, 0);
	hUploadGrid->Children->Add(hStackPanelup);
	hUploadGrid->SetRow(hScrollViewer0, 1);
	hUploadGrid->Children->Add(hScrollViewer0);

	//-----------------------------------------------------------------------------------------------------------

}

//<-----------------------------------------Set list boxes in upload------------------------------------>
void WPFCppCliDemo::setTextBoxes0()
{
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(50);
	hUploadGrid->RowDefinitions->Add(hRow4Def);hFileSpec->Text = "*.*";
	hClientPort->Text = "8080"; hClientPortLable->Content = "Client Port:";
	hFilespecLable->Content = "Pattern:"; publish->Content = "Publish";
	Border^ hBorder1 = gcnew Border(); hBorder1->Width = 80;
	hBorder1->Height = 30; hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black; hBorder1->Child = publish;
	Border^ hBorder2 = gcnew Border(); hBorder2->Width = 120;
	hBorder2->Height = 30; hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hFileSpec;
	Border^ hBorder4 = gcnew Border(); hBorder4->Width = 80; hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hClientPortLable;
	Border^ hBorder3 = gcnew Border(); hBorder3->Width = 120; hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hClientPort;
	TextBlock^ hSpacer = gcnew TextBlock();hSpacer->Width = 30;hStackPanel2->Children->Add(hBorder1);
	hStackPanel2->Children->Add(hSpacer);hStackPanel2->Children->Add(hBorder4);
	hStackPanel2->Children->Add(hBorder3);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hUploadGrid->SetRow(hStackPanel2, 1);
	hUploadGrid->Children->Add(hStackPanel2);
}

//<----------------------------------------Set up text boxes in upload------------------------------------->
void WPFCppCliDemo::setTextBoxes1()
{
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(50);
	hUploadGrid->RowDefinitions->Add(hRow4Def);
	hServerAddr->Text = "localhost"; hServerPort->Text = "8081";
	hServerAddrLable->Content = "Server Addr:"; hServerPortLable->Content = "Server Port:";
	Border^ hBorder1 = gcnew Border(); hBorder1->Width = 80; hBorder1->Height = 30;
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hServerAddrLable;
	Border^ hBorder2 = gcnew Border(); hBorder2->Width = 120; hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hServerAddr;
	Border^ hBorder4 = gcnew Border(); hBorder4->Width = 80; hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hServerPortLable;
	Border^ hBorder3 = gcnew Border(); hBorder3->Width = 120; hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hServerPort;
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 30;
	hStackPanel3->Children->Add(hBorder1);
	hStackPanel3->Children->Add(hBorder2);
	hStackPanel3->Children->Add(hSpacer);
	hStackPanel3->Children->Add(hBorder4);
	hStackPanel3->Children->Add(hBorder3);
	hStackPanel3->Orientation = Orientation::Horizontal;
	hStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hUploadGrid->SetRow(hStackPanel3, 2);
	hUploadGrid->Children->Add(hStackPanel3);
}

//<-----------------------------------------Set up button in upload------------------------------------->

void WPFCppCliDemo::setButtons0()
{
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(50);
	hUploadGrid->RowDefinitions->Add(hRow3Def);
	hUploadButton->Content = "Upload File"; hFolderBrowseButton->Content = "Browse"; 
	hShowItemsButton->Content = "ShowItem"; Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120; hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	Border^ hBorder3 = gcnew Border(); hBorder3->Width = 120; hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	Border^ hBorder5 = gcnew Border(); hBorder5->Width = 120; hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	TextBlock^ hSpacer3 = gcnew TextBlock(); hSpacer3->Width = 30;
	TextBlock^ hSpacer2 = gcnew TextBlock(); hSpacer2->Width = 30;
	hBorder2->Child = hUploadButton;
	hBorder3->Child = hFolderBrowseButton;
	hBorder5->Child = hShowItemsButton;
	hStackPanel0->Children->Add(hBorder2);
	hStackPanel0->Children->Add(hSpacer3);
	hStackPanel0->Children->Add(hBorder3);
	hStackPanel0->Children->Add(hSpacer2);
	hStackPanel0->Children->Add(hBorder5);
	TextBlock^ hSpacer = gcnew TextBlock(); hSpacer->Width = 30;
	TextBlock^ hSpacer1 = gcnew TextBlock(); hSpacer1->Width = 30;
	Border^ hBorder4 = gcnew Border(); hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hClearButton1->Content = "Clear";
	hBorder4->Child = hClearButton1;
	hStackPanel0->Children->Add(hSpacer);
	hStackPanel0->Children->Add(hBorder4);
	hStackPanel0->Orientation = Orientation::Horizontal;
	hStackPanel0->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hUploadGrid->SetRow(hStackPanel0, 3);
	hUploadGrid->Children->Add(hStackPanel0);
	hUploadButton->IsEnabled = false;
}

//<-----------------------------------------Set up upload view------------------------------------->

void WPFCppCliDemo::setUpUpDownView()
{
	Console::Write("\n  setting up upload/download view");
	hUploadGrid->Margin = Thickness(20);
	hUploadTab->Content = hUploadGrid;
	setTextBlock0();
	setTextBoxes0();
	setTextBoxes1();
	setButtons0();
}

//<-----------------------------------------Upload File event------------------------------------->

void WPFCppCliDemo::uploadfile(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Upload File";
	if (uploadListbox->SelectedItems->Count > 0) {
		countUploadFile = 0;
		for each (String^ var in uploadListbox->SelectedItems)
		{
			countUploadFile++;
			HttpMessage msg;
			msg.addAttribute(msg.attribute("Command", "Upload"));
			msg.addAttribute(msg.attribute("FileSpec", toStdString(var)));
			msg.addAttribute(msg.attribute("SendAddr", "localhost"));
			msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPort->Text)));
			msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddr->Text)));
			msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPort->Text)));
			pSendr_->postMessage(msg);
			hStatus->Text = "Uploading File";
		}
	}
	else {
		hStatus->Text = "Please Select Files to Upload";
	}
}

//<----------------------------------------Set up browse event------------------------------------->

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	uploadListbox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			uploadListbox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			uploadListbox->Items->Add(L"<> " + dirs[i]);
	}
	hUploadButton->IsEnabled = true;
}

//<---------------------------------------Select item from list------------------------------------->

void WPFCppCliDemo::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = uploadListbox->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in uploadListbox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	uploadListbox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			uploadListbox->Items->Add(item);
		}
	}
}

//<-----------------------------------------publish files message------------------------------------->

void WPFCppCliDemo::publishFiles(Object^ obj, RoutedEventArgs^ args) {
	hStatus->Text = "PublishFiles";
	downloadListbox->Items->Clear();
	HttpMessage msg;
	//msg.addAttribute(msg.attribute("Command", "test"));
	msg.addAttribute(msg.attribute("Command", "Publish"));
	msg.addAttribute(msg.attribute("SendAddr", "localhost"));
	msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
	msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
	msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
	pSendr_->postMessage(msg);
}


//<-----------------------------------------Search for list of codefiles in server------------------------------------->

void WPFCppCliDemo::explorefile(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Search for Code Files";
	downloadListbox->Items->Clear();
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Explore"));
	msg.addAttribute(msg.attribute("SendAddr", "localhost"));
	msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
	msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
	msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
	pSendr_->postMessage(msg);
}

//<---------------------------------------Explore html files event------------------------------------->

void WPFCppCliDemo::getHtmlList(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Get Html File list";
	downloadListbox->Items->Clear();
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Html"));
	msg.addAttribute(msg.attribute("SendAddr", "localhost"));
	msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
	msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
	msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
	pSendr_->postMessage(msg);
}

//<-----------------------------------------Explore file event in lazy load view----------------------------->

void WPFCppCliDemo::getHtmlListld(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Get Html File list";
	downloadListboxld->Items->Clear();
	HttpMessage msg;
	msg.addAttribute(msg.attribute("Command", "Html"));
	msg.addAttribute(msg.attribute("SendAddr", "localhost"));
	msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
	msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
	msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
	pSendr_->postMessage(msg);
}


//<-----------------------------------------Download file event------------------------------------->

void WPFCppCliDemo::downloadfile(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Download File";
	if (downloadListbox->SelectedItems->Count > 0) {
		countDownloadFile = 0;
		for each (String^ var in downloadListbox->SelectedItems)
		{
			countDownloadFile++;
			HttpMessage msg;
			msg.addAttribute(msg.attribute("Command", "Download"));
			msg.addAttribute(msg.attribute("SendAddr", "localhost"));
			msg.addAttribute(msg.attribute("FileSpec", toStdString(var)));
			msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
			msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
			msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
			pSendr_->postMessage(msg);
			hStatus->Text = "Uploading File";
		}
	}
	else {
		hStatus->Text = "Please Select Files to Upload";
	}
}

//<---------------------------Download file event in lazy load view------------------------------------->

void WPFCppCliDemo::downloadfileld(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Lazy downloading";
	if (downloadListboxld->Items->Count > 0) {
		countDownloadFileld = 0;
		for each (String^ var in downloadListboxld->Items)
		{
			countDownloadFileld++;
			HttpMessage msg;
			msg.addAttribute(msg.attribute("Command", "Download"));
			msg.addAttribute(msg.attribute("SendAddr", "localhost"));
			msg.addAttribute(msg.attribute("FileSpec", toStdString(var)));
			msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
			msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
			msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
			pSendr_->postMessage(msg);
			hStatus->Text = "Peforming LazyDownload File";
		}
		downloadListboxld->Items->Clear();
	}
	else {
		hStatus->Text = "Please Select Files to Download";
	}
}

//<-----------------------------------------Lazy download message------------------------------------->

void WPFCppCliDemo::lazyDload(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Lazy downloading";
	if (downloadListboxld->SelectedItems->Count > 0) {
		countDownloadFileld = 0;
		for each (String^ var in downloadListboxld->SelectedItems)
		{
			countDownloadFileld++;
			HttpMessage msg;
			msg.addAttribute(msg.attribute("Command", "LazyDownload"));
			msg.addAttribute(msg.attribute("SendAddr", "localhost"));
			msg.addAttribute(msg.attribute("FileSpec", toStdString(var)));
			msg.addAttribute(msg.attribute("SendPort", toStdString(hClientPortdTB->Text)));
			msg.addAttribute(msg.attribute("RecvAddr", toStdString(hServerAddrdTB->Text)));
			msg.addAttribute(msg.attribute("RecvPort", toStdString(hServerPortdTB->Text)));
			pSendr_->postMessage(msg);
			hStatus->Text = "Peforming LazyDownload File";
		}
		downloadListboxld->Items->Clear();
	}
	else {
		hStatus->Text = "Please Select Files to Download";
	}
}

//<-----------------------------------------Enable upload button------------------------------------->

void WPFCppCliDemo::enableUpload(Object^ sender, MouseButtonEventArgs^ e)
{
	hUploadButton->IsEnabled = true;
}

//<----------------------------------------Enable download button------------------------------------->

void WPFCppCliDemo::enableDownload(Object^ sender, MouseButtonEventArgs^ e)
{
	hDownLButton->IsEnabled = true;
}

//<------------------------------String conversion to system string-------------------------------->

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

//<-------------------------------String conversion to standard string------------------------------------->
String^ WPFCppCliDemo::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

//<-------------------------------------Add information to textblocks------------------------------------->

void WPFCppCliDemo::addText(String^ msg)
{
	hTextBlock0->Text += msg + "\n\n";
	hTextBlock3->Text += msg + "\n\n";

}

//<-----------------------------------------Add list boxes------------------------------------->

void WPFCppCliDemo::addListBox(String^ msg)
{
	downloadListbox->Items->Add(msg);
	downloadListboxld->Items->Add(msg);
}

//<-----------------------------------------perform an event------------------------------------->

void WPFCppCliDemo::clickDownload(String^ msg)
{
	hDownLButtonldld->ClickEvent;
	//lazyDownload->ClickEvent;
	//hDownLButton->ClickEvent;
}

//<-----------------------------------------getMessage Thread------------------------------------->
void WPFCppCliDemo::getMessage()
{
	while (true)
	{ 
		std::cout << "\n  receive thread calling getMessage()";
		std::string msg = pRecvr_->getMessage();
		String^ sMsg = toSystemString(msg);
		array<String^>^ args = gcnew array<String^>(1);
		if (sMsg->Contains("<filefordownload>"))
		{
			sMsg = sMsg->Substring(17);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addListBox);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
		if (sMsg->Contains("<download>"))
		{
			sMsg = sMsg->Substring(10);
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::clickDownload);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
		else
		{
			args[0] = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
			Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		}
	}
}

//<----------------------------------------Clear text box------------------------------------->

void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Cleared message";
	hTextBlock0->Text = "";
	hTextBlock3->Text = "";
}

//<-----------------------------------------Window load notifier------------------------------------->

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}

//<---------------------------------------Window unload notifier------------------------------------->

void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

//<---------------------------------------- MAIN ------------------------------------->

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting RCMF Client GUI");

	Application^ app = gcnew Application();
	app->Run(gcnew WPFCppCliDemo());
	Console::WriteLine(L"\n\n");
}
//<----------------------------------------End------------------------------------->