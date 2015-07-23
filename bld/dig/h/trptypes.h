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
* Description:  Basic trap file types and interface functions.
*
****************************************************************************/


#ifndef TRPTYPES_H

#include "bool.h"
#include "digtypes.h"
#include "digpck.h"


#if defined( __WATCOMC__ ) && defined( __WINDOWS__ )
    #define     TRAPENTRY   DIGFAR __pascal
#elif defined( __WATCOMC__ ) && ( defined( _M_I86 ) || defined( __DOS__ ) || defined( __DSX__ ) )
    #define     TRAPENTRY   DIGFAR __saveregs
#else
    #define     TRAPENTRY   DIGFAR
#endif

#define TRAP_MAJOR_VERSION      17
#define TRAP_MINOR_VERSION      1
#define OLD_TRAP_MINOR_VERSION  0

#define REQUEST_FAILED ((trap_retval)-1)

#if OLD_TRAP_MINOR_VERSION > 0
    #define TrapVersionOK( ver )  (((ver).major == TRAP_MAJOR_VERSION) && \
                                   ((ver).minor >= OLD_TRAP_MINOR_VERSION))
#else
    #define TrapVersionOK( ver )  ((ver).major == TRAP_MAJOR_VERSION)
#endif

#define TRP_REQUEST(x)  *((access_req *)(x)[0].ptr)

typedef struct {
    unsigned_8      major;
    unsigned_8      minor;
    unsigned_8      remote;
} trap_version;

typedef unsigned_8  access_req;
typedef unsigned_16 trap_elen;
typedef unsigned_32 trap_error;
typedef unsigned_32 trap_mhandle;   /* module handle */
typedef unsigned_32 trap_phandle;   /* process handle */
typedef unsigned_32 trap_shandle;   /* supplementary service handle */

typedef trap_elen   trap_retval;

#include "pushpck1.h"

typedef struct {
    access_req      core_req;
    trap_shandle    id;
} _WCUNALIGNED supp_prefix;

typedef struct {
    char            ext_separator;
    char            drv_separator;
    char            path_separator[2];
    char            newline[2];
} file_components;

#include "poppck.h"

typedef struct {
    void        *ptr;
    trap_elen   len;
} mx_entry;

typedef struct {
    const void  *ptr;
    trap_elen   len;
} in_mx_entry;

typedef mx_entry        DIGFAR *mx_entry_p;
typedef in_mx_entry     DIGFAR *in_mx_entry_p;

typedef const void      *in_data_p;
typedef void            *out_data_p;

typedef trap_version    TRAPENTRY trap_init_func( const char *, char *, bool );
typedef trap_retval     TRAPENTRY trap_req_func( trap_elen, in_mx_entry_p, trap_elen, mx_entry_p );
typedef void            TRAPENTRY trap_fini_func( void );

/* Client interface routines */
extern char             *LoadDumbTrap( trap_version * );
extern char             *LoadTrap( const char *, char *, trap_version * );
extern void             TrapSetFailCallBack( void (*func)(void) );
extern unsigned         TrapAccess( unsigned, in_mx_entry_p, unsigned, mx_entry_p );
extern unsigned         TrapSimpAccess( unsigned, in_data_p, unsigned, out_data_p );
extern void             KillTrap(void);

#include "digunpck.h"

#define TRPTYPES_H

#endif
