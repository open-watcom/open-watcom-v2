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


/*
        Client overridable aspects of the TIX support.

        ui_tix_path     is the last chance directory for finding TIX files
                        (the trailing slash is required).

        ui_tix_missing  is called when UI can not find the appropriate TIX
                        file. The parm is the name of the TIX file being
                        looked for (minus the trailing ".tix"). If the
                        function returns zero, the UI library initialization
                        will fail (uiinit will return FALSE). Returning a
                        non-zero value will allow UI library initialization
                        to succeed (assuming nothing else goes wrong).
*/

#include <stdio.h>
#include "tixparse.h"

char ui_tix_path[] = "/usr/watcom/tix/";

int ui_tix_missing( const char *name )
{
    name = name;
    return( 1 );
}
