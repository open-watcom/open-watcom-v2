#ifndef _DRAWFRM_H_INCLUDED
#define _DRAWFRM_H_INCLUDED

class CDrawingFrameWnd : public CMDIChildWnd {
    DECLARE_DYNCREATE( CDrawingFrameWnd )

public:
    CDrawingFrameWnd();

    virtual BOOL    OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext );

protected:
    CSplitterWnd m_wndSplitter;
};

#endif /* _DRAWFRM_H_INCLUDED */
