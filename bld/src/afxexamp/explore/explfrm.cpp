#include "stdafx.h"
#include "explore.h"
#include "explfrm.h"
#include "expldoc.h"
#include "leftview.h"
#include "rightvw.h"

IMPLEMENT_DYNCREATE( CExploreFrameWnd, CFrameWnd )

BEGIN_MESSAGE_MAP( CExploreFrameWnd, CFrameWnd )
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

CExploreFrameWnd::CExploreFrameWnd()
{
}

BOOL CExploreFrameWnd::OnCmdMsg( UINT nID, int nCode, void *pExtra,
                                 AFX_CMDHANDLERINFO *pInfo )
{
    // Let both views have a chance to handle any commands, regardless of which one is
    // currently active.
    if( GetLeftView()->OnCmdMsg( nID, nCode, pExtra, pInfo ) ) {
        return( TRUE );
    }
    if( GetRightView()->OnCmdMsg( nID, nCode, pExtra, pInfo ) ) {
        return( TRUE );
    }
    return( CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pInfo ) );
}
    
BOOL CExploreFrameWnd::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext )
{
    // Create a static splitter window with one row and two columns.
    if( !m_wndSplitter.CreateStatic( this, 1, 2 ) ) {
        return( FALSE );
    }

    // Create a view for the left pane.
    if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CLeftView ), CSize( 100, 100 ),
                                   pContext ) ) {
        return( FALSE );
    }

    // Create a view for the right pane.
    if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CRightView ), CSize( 100, 100 ),
                                   pContext ) ) {
        return( FALSE );
    }

    return( TRUE );
}

CLeftView *CExploreFrameWnd::GetLeftView()
{
    CLeftView *pLeftView = (CLeftView *)m_wndSplitter.GetPane( 0, 0 );
    ASSERT( pLeftView != NULL );
    ASSERT( pLeftView->IsKindOf( RUNTIME_CLASS( CLeftView ) ) );
    return( pLeftView );
}

CRightView *CExploreFrameWnd::GetRightView()
{
    CRightView *pRightView = (CRightView *)m_wndSplitter.GetPane( 0, 1 );
    ASSERT( pRightView != NULL );
    ASSERT( pRightView->IsKindOf( RUNTIME_CLASS( CRightView ) ) );
    return( pRightView );
}

int CExploreFrameWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 ) {
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
