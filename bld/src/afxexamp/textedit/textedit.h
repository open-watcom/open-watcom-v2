#ifndef _TEXTEDIT_H_INCLUDED
#define _TEXTEDIT_H_INCLUDED

#include "resource.h"

class CTextEditApp : public CWinApp {
public:
    CTextEditApp();

    virtual BOOL InitInstance();

protected:
    afx_msg void    OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

#endif /* _TEXTEDIT_H_INCLUDED */
