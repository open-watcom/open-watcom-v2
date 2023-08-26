/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Trap module loader for OS/2.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#include <wos2.h>
#include "trpld.h"
#include "trpsys.h"
#include "tcerr.h"


#ifdef _M_I86
#define GET_PROC_ADDRESS(m,s,f)   (DosGetProcAddr( m, "#" #s, (PFN FAR *)&f ) == 0)
#define LOAD_MODULE(n,m)          (DosLoadModule( NULL, 0, (char *)n, &m ) != 0 )
#else
#define GET_PROC_ADDRESS(m,s,f)   (DosQueryProcAddr( m, s, NULL, (PFN FAR *)&f ) == 0)
#define LOAD_MODULE(n,m)          (DosLoadModule( NULL, 0, n, &m ) != 0 )
#endif

#define LOW(c)      ((c) | 0x20)

#define pick(n,r,p,ar,ap)   typedef r TRAPENTRY (*TRAP_EXTFUNC_TYPE(n)) ## p;
#include "_trpextf.h"
#undef pick

#define pick(n,r,p,ar,ap)   static TRAP_EXTFUNC_TYPE(n) TRAP_EXTFUNC_PTR(n);
#include "_trpextf.h"
#undef pick

static HMODULE          TrapFile = 0;
static trap_fini_func   *FiniFunc = NULL;

bool IsTrapFilePumpingMessageQueue( void )
{
    return( TRAP_EXTFUNC_PTR( TellHandles ) != NULL );
}

bool TRAP_EXTFUNC( TellHandles )( HAB hab, HWND hwnd )
{
    if( TRAP_EXTFUNC_PTR( TellHandles ) != NULL ) {
        TRAP_EXTFUNC_PTR( TellHandles )( hab, hwnd );
        return( true );
    }
    return( false );
}


char TRAP_EXTFUNC( TellHardMode )( char hard )
{
    if( TRAP_EXTFUNC_PTR( TellHardMode ) != NULL ) {
        return( TRAP_EXTFUNC_PTR( TellHardMode )( hard ) );
    }
    return( 0 );
}

void KillTrap( void )
{
    ReqFunc = NULL;
    TRAP_EXTFUNC_PTR( TellHandles ) = NULL;
    TRAP_EXTFUNC_PTR( TellHardMode ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapFile != 0 ) {
        DosFreeModule( TrapFile );
        TrapFile = 0;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                *p;
    char                chr;
    trap_init_func      *init_func;
    char                filename[CCHMAXPATH];
    char                trpname[CCHMAXPATH];

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    p = trpname;
    for( ; (chr = *parms) != '\0'; parms++ ) {
        if( chr == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        *p++ = chr;
    }
    *p = '\0';
#ifdef _M_I86
    if( LOW( trpname[0] ) == 's' && LOW( trpname[1] ) == 't'
      && LOW( trpname[2] ) == 'd' && trpname[3] == '\0' ) {
        unsigned        version;
        char            os2ver;

        DosGetVersion( (PUSHORT)&version );
        os2ver = version >> 8;
        if( os2ver >= 20 ) {
            strcpy( trpname, "std32" );
        } else {
            strcpy( trpname, "std16" );
        }
    }
#endif
    /* To prevent conflicts with the 16-bit DIP DLLs, the 32-bit versions have the "D32"
     * extension. We will search for them along the PATH (not in LIBPATH);
     */
#ifdef _M_I86
    strcat( trpname, ".DLL" );
#else
    strcat( trpname, ".D32" );
#endif
    _searchenv( trpname, "PATH", filename );
    if( *filename == '\0' ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, trpname );
        return( buff );
    }
    if( LOAD_MODULE( filename, TrapFile ) ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, filename );
        return( buff );
    }
    buff[0] = '\0';
    if( GET_PROC_ADDRESS( TrapFile, 1, init_func )
      && GET_PROC_ADDRESS( TrapFile, 2, FiniFunc )
      && GET_PROC_ADDRESS( TrapFile, 3, ReqFunc ) ) {
        if( !GET_PROC_ADDRESS( TrapFile, 4, TRAP_EXTFUNC_PTR( TellHandles ) ) ) {
            TRAP_EXTFUNC_PTR( TellHandles ) = NULL;
        }
        if( !GET_PROC_ADDRESS( TrapFile, 5, TRAP_EXTFUNC_PTR( TellHardMode ) ) ) {
            TRAP_EXTFUNC_PTR( TellHardMode ) = NULL;
        }
        *trap_ver = init_func( parms, buff, trap_ver->remote );
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( NULL );
            }
        }
    }
    if( buff[0] == '\0' )
        strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    KillTrap();
    return( buff );
}
