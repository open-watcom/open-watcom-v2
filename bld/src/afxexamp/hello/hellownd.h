#ifndef _HELLOWND_H_INCLUDED
#define _HELLOWND_H_INCLUDED

class CHelloWnd : public CFrameWnd {
public:
    CHelloWnd();

protected:
    afx_msg void    OnPaint();
    DECLARE_MESSAGE_MAP()
};

#endif /* _HELLOWND_H_INCLUDED */

