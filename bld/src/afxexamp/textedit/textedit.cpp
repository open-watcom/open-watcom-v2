#include "stdafx.h"
#include "textedit.h"
#include "editfrm.h"
#include "textdoc.h"
#include "textview.h"

// Instantiate the one and only application object
CTextEditApp g_app;

BEGIN_MESSAGE_MAP( CTextEditApp, CWinApp )
    ON_COMMAND( ID_APP_ABOUT, OnAppAbout )
    
    // Handle the following commands with the framework-provided handlers.
    ON_COMMAND( ID_FILE_NEW, OnFileNew )
    ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
    ON_COMMAND( ID_FILE_PRINT_SETUP, OnFilePrintSetup )
END_MESSAGE_MAP()

CTextEditApp::CTextEditApp()
{
}

BOOL CTextEditApp::InitInstance()
{
    // Create a document template object to represent the application's document
    // type and register it with the framework.
    CDocTemplate *pTemplate = new CMultiDocTemplate( IDR_DOCUMENTTYPE,
                                                     RUNTIME_CLASS( CTextDoc ),
                                                     RUNTIME_CLASS( CMDIChildWnd ),
                                                     RUNTIME_CLASS( CTextView ) );
    AddDocTemplate( pTemplate );
    
    // Create the application's main window.
    CTextEditFrameWnd *pWnd = new CTextEditFrameWnd;
    if( !pWnd->LoadFrame( IDR_MAINFRAME ) ) {
        return( FALSE );
    }
    m_pMainWnd = pWnd;

    // Enable support for opening files via drag/drop.
    m_pMainWnd->DragAcceptFiles();
    
    // Enable support for opening files via DDE.
    EnableShellOpen();
    
    // Perform file associations.
    RegisterShellFileTypes();
    
    // Parse the command line.
    CCommandLineInfo ci;
    ParseCommandLine( ci );

    // Process any commands specified on the command line.  This open any file specified
    // or otherwise create a new document.
    if( !ProcessShellCommand( ci ) ) {
        return( FALSE );
    }
    
    // Show and update the window.
    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();

    return( TRUE );
}

void CTextEditApp::OnAppAbout()
{
    // Display the about box.
    CDialog dlg( IDD_ABOUTBOX );
    dlg.DoModal();
}
