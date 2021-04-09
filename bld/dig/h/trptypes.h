/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _TRPTYPES_H_INCLUDED
#define _TRPTYPES_H_INCLUDED

#include "digtypes.h"
#if defined( __WINDOWS__ )
    #include "setevent.h"
#endif

#if defined( __WINDOWS__ )
    #define     TRAPENTRY   __far __pascal
#elif defined( _M_I86 )
    #define     TRAPENTRY   __far __saveregs
#elif defined( __WATCOMC__ ) && ( defined( __DOS__ ) || defined( __DSX__ ) )
    #define     TRAPENTRY   __saveregs
#else
    #define     TRAPENTRY
#endif

#define TRAP_MAJOR_VERSION      18
#define TRAP_MINOR_VERSION      0
#define OLD_TRAP_MINOR_VERSION  0

#define REQUEST_FAILED ((trap_retval)-1)

#if OLD_TRAP_MINOR_VERSION > 0
    #define TrapVersionOK( ver )  (((ver).major == TRAP_MAJOR_VERSION) && \
                                   ((ver).minor >= OLD_TRAP_MINOR_VERSION))
#else
    #define TrapVersionOK( ver )  ((ver).major == TRAP_MAJOR_VERSION)
#endif

#define TRP_REQUEST(x)      *((access_req *)(x)[0].ptr)

#define TRAP_SYM1(a,b,c)    a ## b ## _ ## c
#define TRAP_SYM(a,b)       TRAP_SYM1( Req, a, b )

#include "digpck.h"
typedef struct {
    unsigned_8      major;
    unsigned_8      minor;
    unsigned_8      remote;
} trap_version;

typedef unsigned_8  access_req;
typedef dig_elen    trap_elen;
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
    char            line_eol[2];
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
#include "digunpck.h"

#if defined( _M_I86 )
typedef mx_entry        __far *mx_entry_p;
typedef in_mx_entry     __far *in_mx_entry_p;
#else
typedef mx_entry        *mx_entry_p;
typedef in_mx_entry     *in_mx_entry_p;
#endif

typedef const void      *in_data_p;
typedef void            *out_data_p;

typedef trap_version    TRAPENTRY trap_init_func( const char *, char *, bool );
typedef trap_retval     TRAPENTRY trap_req_func( trap_elen, in_mx_entry_p, trap_elen, mx_entry_p );
typedef void            TRAPENTRY trap_fini_func( void );

/*
 * UNIX specific trap implementation stuff
 */
typedef struct {
    trap_init_func      *init_func;
    trap_req_func       *req_func;
    trap_fini_func      *fini_func;
} trap_requests;

typedef struct {
    unsigned long       len;
    char                ***environp;
    void                **_slib_func;
    void                *(*malloc)( size_t );
    void                *(*realloc)( void *, size_t );
    void                (*free)( void * );
    char                *(*getenv)( const char * );
    void                (*(*signal)( int __sig, void (*__func)(int) ))(int);
} trap_callbacks;

typedef const trap_requests *trap_load_func( const trap_callbacks *client );

#endif
