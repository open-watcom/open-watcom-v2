#include "stdafx.h"
#include "quikdraw.h"
#include "drawdoc.h"
#include "drawview.h"
#include "shapes.h"

IMPLEMENT_DYNCREATE( CDrawingView, CScrollView )

BEGIN_MESSAGE_MAP( CDrawingView, CScrollView )
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_COMMAND( ID_DRAW_ARROW, OnDrawArrow )
    ON_COMMAND( ID_DRAW_LINE, OnDrawLine )
    ON_COMMAND( ID_DRAW_RECTANGLE, OnDrawRectangle )
    ON_COMMAND( ID_DRAW_ELLIPSE, OnDrawEllipse )
    ON_COMMAND( ID_DRAW_LINE_COLOR, OnDrawLineColor )
    ON_COMMAND( ID_DRAW_FILL_COLOR, OnDrawFillColor )
    ON_UPDATE_COMMAND_UI( ID_DRAW_ARROW, OnUpdateDrawArrow )
    ON_UPDATE_COMMAND_UI( ID_DRAW_LINE, OnUpdateDrawLine )
    ON_UPDATE_COMMAND_UI( ID_DRAW_RECTANGLE, OnUpdateDrawRectangle )
    ON_UPDATE_COMMAND_UI( ID_DRAW_ELLIPSE, OnUpdateDrawEllipse )
    
    // Handle the following commands with the framework-provided handlers.
    ON_COMMAND( ID_FILE_PRINT, OnFilePrint )
    ON_COMMAND( ID_FILE_PRINT_DIRECT, OnFilePrint )
    ON_COMMAND( ID_FILE_PRINT_PREVIEW, OnFilePrintPreview )
END_MESSAGE_MAP()

CDrawingView::CDrawingView()
{
    m_nDrawTool = DRAWTOOL_ARROW;
    m_crLine = RGB( 0, 0, 0 );
    m_crFill = RGB( 255, 255, 255 );
    m_pCurShape = NULL;

    // Load standard cursors used by the view.
    CWinApp *pApp = AfxGetApp();
    ASSERT( pApp != NULL );
    m_hcurArrow = pApp->LoadStandardCursor( IDC_ARROW );
    m_hcurCross = pApp->LoadStandardCursor( IDC_CROSS );
}

void CDrawingView::OnDraw( CDC *pDC )
{
    CDrawingDoc *pDoc = GetDocument();
    ASSERT_VALID( pDoc );

    // Enumerate through the shapes and draw each one.
    POSITION position = pDoc->GetFirstShapePosition();
    while( position != NULL ) {
        CShape *pShape = pDoc->GetNextShape( position );
        pShape->Draw( pDC );
    }
}

void CDrawingView::OnInitialUpdate()
{
    // Set the scroll bars to match the size of the drawing.
    SetScrollSizes( MM_LOENGLISH, GetDocument()->GetDrawingSize() );
}

BOOL CDrawingView::OnPreparePrinting( CPrintInfo *pInfo )
{
    // Do the default preparation.
    return( DoPreparePrinting( pInfo ) );
}

void CDrawingView::OnLButtonDown( UINT nFlags, CPoint point )
{
    CScrollView::OnLButtonDown( nFlags, point );

    // Initialize a device context and convert the point to logical coordinates.
    CClientDC dc( this );
    OnPrepareDC( &dc );
    dc.DPtoLP( &point );

    if( m_nDrawTool == DRAWTOOL_ARROW ) {
        CDrawingDoc *pDoc = GetDocument();
        ASSERT_VALID( pDoc );

        // If there's a shape under the cursor, prepare to move it.
        m_pCurShape = pDoc->HitTest( point );
        if( m_pCurShape != NULL ) {
            m_lastPoint = point;

            // Capture the mouse so that mouse messages are sent even if the cursor
            // leaves the window.
            SetCapture();
        }
    } else {
        // Initialize a shape of type corresponding to the selected tool.
        switch( m_nDrawTool ) {
        case DRAWTOOL_LINE:
            m_pCurShape = new CLine;
            break;
        case DRAWTOOL_RECTANGLE:
            m_pCurShape = new CRectangle;
            break;
        case DRAWTOOL_ELLIPSE:
            m_pCurShape = new CEllipse;
            break;
        }
        m_pCurShape->SetPoint1( point );
        m_pCurShape->SetPoint2( point );
        m_pCurShape->SetLineColor( m_crLine );
        m_pCurShape->SetFillColor( m_crFill );
        
        // Draw the shape in the window.  Use R2_NOTXORPEN so that it can be easily
        // erased.
        int nOldROP = dc.SetROP2( R2_NOTXORPEN );
        m_pCurShape->Draw( &dc );
        dc.SetROP2( nOldROP );

        // Capture the mouse so that mouse messages are sent even if the cursor
        // leaves the window.
        SetCapture();
    }
}

void CDrawingView::OnLButtonUp( UINT nFlags, CPoint point )
{
    CScrollView::OnLButtonUp( nFlags, point );

    // Add the shape to the drawing (if it's a new shape) and update all views.
    if( m_pCurShape != NULL ) {
        CDrawingDoc *pDoc = GetDocument();
        ASSERT_VALID( pDoc );
        if( m_nDrawTool != DRAWTOOL_ARROW ) {
            pDoc->AddShape( m_pCurShape );
        }
        m_pCurShape = NULL;
        pDoc->UpdateAllViews( NULL );

        // Release the mouse capture now that the drag is complete.
        ::ReleaseCapture();
    }
}

void CDrawingView::OnMouseMove( UINT nFlags, CPoint point )
{
    CScrollView::OnMouseMove( nFlags, point );

    if( m_pCurShape != NULL ) {
        // Initialize a device context and convert the point to logical coordinates.
        CClientDC dc( this );
        OnPrepareDC( &dc );
        dc.DPtoLP( &point );

        // Erase the shape, move it to its new position, and then redraw it.
        int nOldROP = dc.SetROP2( R2_NOTXORPEN );
        m_pCurShape->Draw( &dc );
        if( m_nDrawTool == DRAWTOOL_ARROW ) {
            m_pCurShape->Offset( point - m_lastPoint );
            m_lastPoint = point;
        } else {
            m_pCurShape->SetPoint2( point );
        }
        m_pCurShape->Draw( &dc );
        dc.SetROP2( nOldROP );
    }
}

BOOL CDrawingView::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message )
{
    if( nHitTest != HTCLIENT ) {
        return( CScrollView::OnSetCursor( pWnd, nHitTest, message ) );
    }

    // Set the cursor appropriately for the current drawing tool.
    if( m_nDrawTool == DRAWTOOL_ARROW ) {
        ::SetCursor( m_hcurArrow );
    } else {
        ::SetCursor( m_hcurCross );
    }
    return( TRUE );
}

void CDrawingView::OnDrawArrow()
{
    m_nDrawTool = DRAWTOOL_ARROW;
}

void CDrawingView::OnDrawLine()
{
    m_nDrawTool = DRAWTOOL_LINE;
}

void CDrawingView::OnDrawRectangle()
{
    m_nDrawTool = DRAWTOOL_RECTANGLE;
}

void CDrawingView::OnDrawEllipse()
{
    m_nDrawTool = DRAWTOOL_ELLIPSE;
}

void CDrawingView::OnDrawLineColor()
{
    CColorDialog dlg( m_crLine );
    if( dlg.DoModal() == IDOK ) {
        m_crLine = dlg.GetColor();
    }
}

void CDrawingView::OnDrawFillColor()
{
    CColorDialog dlg( m_crFill );
    if( dlg.DoModal() == IDOK ) {
        m_crFill = dlg.GetColor();
    }
}

void CDrawingView::OnUpdateDrawArrow( CCmdUI *pCmdUI )
{
    pCmdUI->SetCheck( m_nDrawTool == DRAWTOOL_ARROW );
}

void CDrawingView::OnUpdateDrawLine( CCmdUI *pCmdUI )
{
    pCmdUI->SetCheck( m_nDrawTool == DRAWTOOL_LINE );
}

void CDrawingView::OnUpdateDrawRectangle( CCmdUI *pCmdUI )
{
    pCmdUI->SetCheck( m_nDrawTool == DRAWTOOL_RECTANGLE );
}

void CDrawingView::OnUpdateDrawEllipse( CCmdUI *pCmdUI )
{
    pCmdUI->SetCheck( m_nDrawTool == DRAWTOOL_ELLIPSE );
}
