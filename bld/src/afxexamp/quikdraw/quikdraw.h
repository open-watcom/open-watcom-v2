#ifndef _QUIKDRAW_H_INCLUDED
#define _QUIKDRAW_H_INCLUDED

#include "resource.h"

class CQuickDrawApp : public CWinApp {
public:
    CQuickDrawApp();

    virtual BOOL InitInstance();

protected:
    afx_msg void    OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

#endif /* _QUIKDRAW_H_INCLUDED */
