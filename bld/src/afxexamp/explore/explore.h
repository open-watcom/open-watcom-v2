#ifndef _EXPLORE_H_INCLUDED
#define _EXPLORE_H_INCLUDED

#include "resource.h"

class CExploreApp : public CWinApp {
public:
    CExploreApp();

    virtual BOOL    InitInstance();

protected:
    afx_msg void    OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

#endif /* _EXPLORE_H_INCLUDED */
