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


#if defined(__WINDOWS__)

#include <windows.h>
#include <stdlib.h>     // for __win_xxx_flags

HINSTANCE hInstDLL;             // Saved module handle.

int FAR PASCAL LibMain( HANDLE inst, WORD dataseg,
                        WORD heapsize, LPSTR cmdline  )
/*****************************************************/

  {
    hInstDLL = inst;
    dataseg = dataseg;              /* ref'd to avoid warnings */
    heapsize = heapsize;
    cmdline = cmdline;
    // Fix for malloc() & realloc() memory to belong to DLL
    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    return( inst );
  }


int far PASCAL WEP( int parm )
/*************************************/

/* terminate the DLL */

  {
    parm = parm;
    return( 1 );
  }

#elif defined(__OS2__)

int main()
{
    return( 1 );
}
#elif defined(__WINDOWS_NT__) || defined( __NT__ )

#include <windows.h>

HANDLE hInstDLL;                // Saved module handle.

int __stdcall LibMain( HANDLE inst, ULONG reasoncalled, LPVOID reserved )
/********************************************************************/
{
    reserved = reserved;

    switch( reasoncalled ) {
        case DLL_PROCESS_ATTACH:
            GetTopWindow( NULL );       // drag in nt.lib
            hInstDLL = inst;
            break;
        default:
            break;
    }

    return( 1 );
}

#endif
