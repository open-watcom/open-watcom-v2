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



#include <string.h>
#include <watcom.h>
#include "dpmi.h"
#include "dsxutil.h"
#ifdef __OSI__
  #include <stdlib.h>
#else
  extern int                    _psp;
#endif


#define PSP_ENV_VARS_OFF        0x2c



extern char             *StrCopy( char *, char * );


char *DOSEnvFind( char *src )
{
#ifdef __OSI__
    return( getenv( src ) );
#else
    char        *env;
    char        *p;

    env = (char *)DPMIGetSegmentBaseAddress( *(addr_seg *)
        ((unsigned_8 *)DPMIGetSegmentBaseAddress( _psp )+PSP_ENV_VARS_OFF));
    do {
        p = src;
        do {
            if( *p == '\0' && *env == '=' ) return( env + 1 );
        } while( *env++ == *p++ );
        while( *env++ != '\0' )
            ;
    } while( *env != '\0' );
    return( NULL );
#endif
}

int EnvLkup( char *src, char *dst, int max_len )
{
    char        *env;

    max_len = max_len; // nyi obey
    env = DOSEnvFind( src );
    if( env == NULL ) env = "";
    return( StrCopy( env, dst ) - dst );
}
