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


#ifndef __EDMODULE_H__
#define __EDMODULE_H__

#include "edmodlst.h"

class Module;
class WVList;

class EdModule : public EditModuleList
{
public:
                        EdModule( WWindow * prt, const char * name,
                                  WVList & en, WVList & dis );
                        ~EdModule();

    virtual void        initialize();

protected:
    virtual void        okButton( WWindow * );
    virtual void        cancelButton( WWindow* );
    virtual void        addButton( WWindow* );
    virtual void        helpButton( WWindow* );
            bool        contextHelp( bool );

    virtual void        loadBox();


private:
    const   char *      _name;
            WVList &    _enabled;
            WVList &    _disabled;
};

#endif
