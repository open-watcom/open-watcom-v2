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
    #define EDITAPI     __export __stdcall
#elif defined( __WINDOWS__ )
    typedef char far *editstring;
    #define EDITAPI     __export far pascal
#elif defined( __OS2__ )
    typedef char *editstring;
    #define EDITAPI     __syscall
#else
    #error Unsupported OS
    #define EDITAPI
#endif

typedef enum {
    EDIT_MINIMIZE,
    EDIT_RESTORE,
    EDIT_SHOWNORMAL
} show_method;

// this file should be identical for all app implementations
int EDITAPI EDITConnect( void );
int EDITAPI EDITFile( editstring, editstring );
int EDITAPI EDITLocate( long, int, int );
int EDITAPI EDITLocateError( long, int, int, int, editstring );
int EDITAPI EDITShowWindow( show_method );
int EDITAPI EDITDisconnect( void );
int EDITAPI EDITSaveFiles( void );
int EDITAPI EDITSaveThisFile( const char * );
int EDITAPI EDITQueryThisFile( const char * );

#ifdef __cplusplus
}
#endif
