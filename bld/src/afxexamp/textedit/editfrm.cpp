#include "stdafx.h"
#include "textedit.h"
#include "editfrm.h"

IMPLEMENT_DYNAMIC( CTextEditFrameWnd, CMDIFrameWnd )

BEGIN_MESSAGE_MAP( CTextEditFrameWnd, CMDIFrameWnd )
    ON_WM_CREATE()
END_MESSAGE_MAP()

// Declare an array with the indicators for the status bar.
#define NUM_INDICATORS  4
static const UINT g_indicators[NUM_INDICATORS] = {
    ID_SEPARATOR,
    ID_INDICATOR_NUM,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_SCRL
};

CTextEditFrameWnd::CTextEditFrameWnd()
{
}

int CTextEditFrameWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    // Create a toolbar control.
    if( !m_wndToolBar.CreateEx( this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
                                      CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_GRIPPER ) ) {
        return( -1 );
    }

    // Load the toolbar and bitmap resources.
    if( !m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) ) {
        return( -1 );
    }

    // Create a status bar control.
    if( !m_wndStatusBar.Create( this ) ) {
        return( -1 );
    }

    // Set the status bar's indicators.
    if( !m_wndStatusBar.SetIndicators( g_indicators, NUM_INDICATORS ) ) {
        return( -1 );
    }

    // Enable docking of the toolbar.
    EnableDocking( CBRS_ALIGN_ANY );
    m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
    DockControlBar( &m_wndToolBar );
    
    return( 0 );
}
