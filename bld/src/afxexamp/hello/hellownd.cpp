#include "stdafx.h"
#include "hello.h"
#include "hellownd.h"

BEGIN_MESSAGE_MAP( CHelloWnd, CFrameWnd )
    ON_WM_PAINT()
END_MESSAGE_MAP()

CHelloWnd::CHelloWnd()
{
}

void CHelloWnd::OnPaint()
{
    // Load the string to be drawn.
    CString str;
    str.LoadString( IDR_MAINFRAME );

    // Get the window's client rectangle in which the string will be drawn.
    CRect rect;
    GetClientRect( &rect );

    // Actually draw the string.
    CPaintDC dc( this );
    dc.SetTextColor( RGB( 255, 0, 0 ) );
    dc.DrawText( str, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
}
