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


/* [main.cpp|001]
 *  main.cpp (console applications)
*/

#include "wpch.hpp"


/*  Redefine the C++ "new" and "delete" operators to go through
    the component library's memory checker.
*/
#if defined( _DEBUG ) && defined( new )
#   undef new
#   undef delete
#   define _REDEFINE_WNEW
#endif
W_REDEFINENEW
W_REDEFINEDELETE
#ifdef _REDEFINE_WNEW
#   include "wnew.hpp"
#   undef _REDEFINE_WNEW
#endif


WModuleHandle _ApplicationModule = NULLHMODULE;
static WApplication _Application;
WApplication *Application = &_Application;

extern void Test( void );

int main( int argc, const char *argv[] )
/***************************************
*/
{
    int ret = -1;

    if( Application->Initialize( NULL, 0, NULL, 0 ) ) {

        //  Add your code here
        Test();

        Application->Uninitialize();
    }
    return ret;
}
