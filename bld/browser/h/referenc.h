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


#ifndef __REFERENCE_H__
#define __REFERENCE_H__

#include "wbrdefs.h"
#include <drgetref.h>
#include <wobject.hpp>
#include <string.h>
#include <stdlib.h>

class Module;
class Reference : public WObject
{
public:
                Reference( dr_ref_info * ref, char *name );
    virtual     ~Reference();

    char *      sourceFile() {return _file;}
    char *      name();
    ulong       line() { return _line; }
    uint        column() { return _column; }
    uint        length() { return strlen( _name ); }
    void        editRef();
private:
    char        _file[_MAX_PATH];
    char *      _name;
    ulong       _line;
    uint        _column;
};
#endif // __REFERENCE_H__
