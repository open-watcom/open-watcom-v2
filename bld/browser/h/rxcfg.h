/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef __RXCFG_H__
#define __RXCFG_H__

#include <wdialog.hpp>

#include "optmgr.h"
#include "dgrxcfg.gh"

class WGroupBox;
class WCheckBox;
class WText;
class WRadioButton;
class WDefPushButton;
class WPushButton;

#define NUMCHECKBOXES 12

class RegExpCfg : public RegExpCfgDlg, public WDialog {
public:
                                RegExpCfg( WWindow * parent,
                                            const RXOptions & def,
                                            RXOptions & curr );
                                ~RegExpCfg();

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                void            okButton( WWindow * );
                void            defaultsButton( WWindow * );
                void            enableAllButton( WWindow * );
                void            clearAllButton( WWindow * );
                void            helpButton( WWindow * );
                bool            contextHelp( bool );

protected:
                void            setValues( const RXOptions & opt );

        const   RXOptions &     _default;
                RXOptions &     _current;

private:

        WGroupBox *             _anchorGroup;
        WRadioButton *          _startsWith;
        WRadioButton *          _contains;

        WGroupBox *             _characterGroup;

        union {
            struct {
                WCheckBox *     _anchorChar;
                WCheckBox *     _eolChar;
                WCheckBox *     _charGroupChar;
                WCheckBox *     _exprGroupChar;
                WCheckBox *     _ignoreCaseChar;
                WCheckBox *     _respectCaseChar;
                WCheckBox *     _escapeChar;
                WCheckBox *     _dotChar;
                WCheckBox *     _alternateChar;
                WCheckBox *     _questionChar;
                WCheckBox *     _plusChar;
                WCheckBox *     _starChar;
            } _chbx;

            WCheckBox *         _chbxArray[ NUMCHECKBOXES ];
        };

        WText *                 _anchorText;
        WText *                 _eolText;
        WText *                 _charGroupText;
        WText *                 _exprGroupText;
        WText *                 _ignoreCaseText;
        WText *                 _respectCaseText;
        WText *                 _escapeText;
        WText *                 _dotText;
        WText *                 _alternateText;
        WText *                 _questionText;
        WText *                 _plusText;
        WText *                 _starText;

        WDefPushButton *        _okButton;
        WPushButton *           _defaultsButton;
        WPushButton *           _cancelButton;
        WPushButton *           _enableAllButton;
        WPushButton *           _clearAllButton;
        WPushButton *           _helpButton;
};

#endif // __RXCFG_H__
