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

static HMODULE          mod_hdl = 0;
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

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    TRAP_EXTFUNC_PTR( TellHandles ) = NULL;
    TRAP_EXTFUNC_PTR( TellHardMode ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( mod_hdl != 0 ) {
        DosFreeModule( mod_hdl );
        mod_hdl = 0;
    }
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    const char          *base_name;
    size_t              len;
    trap_init_func      *init_func;
    char                filename[CCHMAXPATH];
    digld_error         err;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    base_name = parms;
    len = 0;
    for( ; *parms != '\0'; parms++ ) {
        if( *parms == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        len++;
    }
    /*
     * To prevent conflicts with the 16-bit DIP DLLs, the 32-bit versions have the "D32"
     * extension. We will search for them along the PATH (not in LIBPATH);
     */
#ifdef _M_I86
    if( len == 3
      && LOW( base_name[0] ) == 's' && LOW( base_name[1] ) == 't' && LOW( base_name[2] ) == 'd' ) {
        unsigned        version;
        char            os2ver;

        DosGetVersion( (PUSHORT)&version );
        os2ver = version >> 8;
        if( os2ver >= 20 ) {
            base_name = "std32";
        } else {
            base_name = "std16";
        }
        len = 5;
    }
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, ".DLL", filename, sizeof( filename ) ) == 0 ) {
#else
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, ".D32", filename, sizeof( filename ) ) == 0 ) {
#endif
        return( DIGS_ERR_CANT_FIND_MODULE );
    }
    if( LOAD_MODULE( filename, mod_hdl ) ) {
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    err = DIGS_ERR_BAD_MODULE_FILE;
    if( GET_PROC_ADDRESS( mod_hdl, 1, init_func )
      && GET_PROC_ADDRESS( mod_hdl, 2, FiniFunc )
      && GET_PROC_ADDRESS( mod_hdl, 3, ReqFunc ) ) {
        if( !GET_PROC_ADDRESS( mod_hdl, 4, TRAP_EXTFUNC_PTR( TellHandles ) ) ) {
            TRAP_EXTFUNC_PTR( TellHandles ) = NULL;
        }
        if( !GET_PROC_ADDRESS( mod_hdl, 5, TRAP_EXTFUNC_PTR( TellHardMode ) ) ) {
            TRAP_EXTFUNC_PTR( TellHardMode ) = NULL;
        }
        *trap_ver = init_func( parms, buff, trap_ver->remote );
        err = DIGS_ERR_BUF;
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( DIGS_OK );
            }
            err = DIGS_ERR_WRONG_MODULE_VERSION;
        }
    }
    UnLoadTrap();
    return( err );
}
