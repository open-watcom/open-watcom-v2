#include "stdafx.h"
#include "explore.h"
#include "explfrm.h"
#include "expldoc.h"
#include "rightvw.h"

// Instantiate the one and only application object.
CExploreApp g_app;

BEGIN_MESSAGE_MAP( CExploreApp, CWinApp )
    ON_COMMAND( ID_APP_ABOUT, OnAppAbout )

    // Handle the following commands with the framework-provided handlers.
    ON_COMMAND( ID_FILE_NEW, OnFileNew )
    ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
    ON_COMMAND( ID_FILE_PRINT_SETUP, OnFilePrintSetup )
END_MESSAGE_MAP()

CExploreApp::CExploreApp()
{
}

BOOL CExploreApp::InitInstance()
{
    // Create a document template object to represent the application's document
    // type and register it with the framework.
    CDocTemplate *pTemplate = new CSingleDocTemplate( IDR_MAINFRAME,
                                                      RUNTIME_CLASS( CExploreDoc ),
                                                      RUNTIME_CLASS( CExploreFrameWnd ),
                                                      RUNTIME_CLASS( CRightView ) );
    AddDocTemplate( pTemplate );

    // Enable support for opening files via DDE.
    EnableShellOpen();

    // Perform file associations.
    RegisterShellFileTypes();

    // Parse the command line.
    CCommandLineInfo ci;
    ParseCommandLine( ci );

    // Process any commands specified on the command line.  This will open any file
    // specified or otherwise create a new document.  In an SDI application, it will also
    // create the main window.
    if( !ProcessShellCommand( ci ) ) {
        return( FALSE );
    }

    // Enable support for opening files via drag/drop.
    m_pMainWnd->DragAcceptFiles();

    // Show and update the window.
    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();

    return( TRUE );
}

void CExploreApp::OnAppAbout()
{
    // Display the about box.
    CDialog dlg( IDD_ABOUTBOX );
    dlg.DoModal();
}
