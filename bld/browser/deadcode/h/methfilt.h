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


#ifndef methfilt_h
#define methfilt_h

#include <wdialog.hpp>
#include "wbrdefs.h"
#include "wbrwin.h"
#include "filtflag.h"

#define NUM_FILT_FLAGS  10
#define NUM_FILT_BTTNS  7

class WCheckBox;
class WComboBox;

class MethodFilter : public WDialog
{
public:
                MethodFilter( int x, int y, bool hasvirt );
                ~MethodFilter() {}

    virtual void initialize();

    FilterFlags getCurrentFlags() { return _flags; }

// callbacks
    void        makeDefault( WWindow * );
    void        okButton( WWindow* );
    void        cancelButton( WWindow* );
    void        SetAllSelected( WWindow* );

private:
    FilterFlags readCurrentFlags();

    WCheckBox * _filterBttns[ NUM_FILT_FLAGS ];
    WComboBox * _filterBox;
    bool        _hasvirt;
    FilterFlags _flags;
};

#endif // methfilt_h

