#include "stdafx.h"
#include "hello.h"
#include "hellownd.h"

// Instantiate the one and only application object.
CHelloApp g_app;

CHelloApp::CHelloApp()
{
}

BOOL CHelloApp::InitInstance()
{
    // Create the application's main window.
    CHelloWnd *pWnd = new CHelloWnd;
    if( !pWnd->LoadFrame( IDR_MAINFRAME ) ) {
        return( FALSE );
    }
    m_pMainWnd = pWnd;

    // Show and update the window.
    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();
    
    return( TRUE );
}
