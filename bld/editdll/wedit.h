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
* Description:  Editor control DLL interface.
*
****************************************************************************/


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined( __NT__ )
    typedef char *editstring;
#elif defined( __WINDOWS__ )
    typedef char __far *editstring;
#elif defined( __OS2__ )
    typedef char *editstring;
#else
    #error Unsupported OS
#endif

#if defined( WEDITDLL )
    #define EDITDLLENTRY    __declspec(dllexport)
#else
    #define EDITDLLENTRY
#endif

#if defined( __OS2__ )
    #define EDITAPI     APIENTRY
#elif defined( __NT__ ) || defined( __WINDOWS__ )
    #define EDITAPI     WINAPI
#else
    #define EDITAPI
#endif

typedef enum {
    EDIT_MINIMIZE,
    EDIT_RESTORE,
    EDIT_SHOWNORMAL
} show_method;

// this file should be identical for all app implementations
EDITDLLENTRY extern int EDITAPI EDITConnect( void );
EDITDLLENTRY extern int EDITAPI EDITFile( editstring, editstring );
EDITDLLENTRY extern int EDITAPI EDITLocate( long, int, int );
EDITDLLENTRY extern int EDITAPI EDITLocateError( long, int, int, int, editstring );
EDITDLLENTRY extern int EDITAPI EDITShowWindow( int );
EDITDLLENTRY extern int EDITAPI EDITDisconnect( void );
EDITDLLENTRY extern int EDITAPI EDITSaveFiles( void );
EDITDLLENTRY extern int EDITAPI EDITSaveThisFile( const char * );
EDITDLLENTRY extern int EDITAPI EDITQueryThisFile( const char * );

#ifdef __cplusplus
}
#endif
