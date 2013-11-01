/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Declaration of CFileNewDialog.
*
****************************************************************************/


#ifndef __FILENEW_H__
#define __FILENEW_H__

class CFileNewDialog : public CDialog {
public:
    CDocTemplate    *m_pDocTemplate;
private:
    CDocManager     *m_pDocManager;
    CListBox        m_wndListBox;
    
public:
    CFileNewDialog( CDocManager *pDocManager );

    virtual void    DoDataExchange( CDataExchange *pDX );
    virtual void    OnCancel();
    virtual BOOL    OnInitDialog();
    virtual void    OnOK();

protected:
    afx_msg void    OnDblClkListBox();
    DECLARE_MESSAGE_MAP()
};

#endif
