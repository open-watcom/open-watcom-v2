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


#ifndef __MEMFILT_H__
#define __MEMFILT_H__

#include <wdialog.hpp>

#include "optmgr.h"
#include "dgmemflt.gh"

class WPushButton;
class WDefPushButton;
class WGroupBox;
class WCheckBox;
class WRadioButton;

class MemberFilterDlg : public MemberFiltDlg, public WDialog {
public:
                                MemberFilterDlg( WWindow * parent,
                                               MemberFilter def,
                                               MemberFilter & curr );
                                ~MemberFilterDlg();

        virtual void            cancelButton( WWindow * );
        virtual void            initialize();

                bool            contextHelp( bool );

                void            okButton( WWindow * );
                void            defaultButton( WWindow * );

                void            setValues( const MemberFilter & filt );
                void            varOrFuncPushed( WWindow * );
private:
        MemberFilter            _default;
        MemberFilter &          _current;

        WGroupBox *     _inheritGroup;
        union {
            struct {
                WRadioButton *  _none;
                WRadioButton *  _visible;
                WRadioButton *  _all;
            } _bttns;
            WRadioButton *      _bttnRay[ 3 ];
        };

        WGroupBox * _accessGroup;
        WCheckBox * _public;
        WCheckBox * _protected;
        WCheckBox * _private;

        WGroupBox*  _memberGroup;
        WCheckBox * _variables;
        WCheckBox * _varStatic;
        WCheckBox * _functions;
        WCheckBox * _virtual;
        WCheckBox * _funcStatic;

        WDefPushButton *    _okButton;
        WPushButton *       _defaultButton;
        WPushButton *       _cancelButton;
};


#endif //__MEMFILT_H__
