#include "stdafx.h"
#include "quikdraw.h"
#include "drawfrm.h"

IMPLEMENT_DYNCREATE( CDrawingFrameWnd, CMDIChildWnd )

CDrawingFrameWnd::CDrawingFrameWnd()
{
}

BOOL CDrawingFrameWnd::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext )
{
    return( m_wndSplitter.Create( this, 2, 2, CSize( 10, 10 ), pContext ) );
}
