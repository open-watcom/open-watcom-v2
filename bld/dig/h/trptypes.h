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


#ifndef _TRPTYPES_H_INCLUDED
#define _TRPTYPES_H_INCLUDED

#include "bool.h"
#include "digtypes.h"

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

/*
 * Trap interface function/pointer macros
 */
#define TRAPENTRY_PTR_NAME(x)       ptr##x
#define TRAPENTRY_PTR(x)            (*TRAPENTRY_PTR_NAME(x))
#define TRAPENTRY_PTR_CAST(x)       (TRAPENTRY_FUNC_##x((*)))
#define TRAPENTRY_FUNC(x)           TRAPENTRY_FUNC_##x(x)
#define TRAPENTRY_FUNC_PTR(x)       TRAPENTRY_FUNC_##x(TRAPENTRY_PTR(x))

#include "digpck.h"
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
#include "digunpck.h"

#if defined( __WINDOWS__ )
typedef void __far hook_fn(unsigned, unsigned);
#endif

typedef mx_entry        DIGFAR *mx_entry_p;
typedef in_mx_entry     DIGFAR *in_mx_entry_p;

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

/*
 * Trap OS specific interface functions prototype macros
 */
#if defined( __OS2__ )
  #if defined( _M_I86 )
#define TRAPENTRY_FUNC_TellHandles(x)   void TRAPENTRY x (void __far *,void __far *)
  #else
#define TRAPENTRY_FUNC_TellHandles(x)   void TRAPENTRY x (HAB, HWND)
  #endif
#define TRAPENTRY_FUNC_TellHardMode(x)  char TRAPENTRY x (char)
#elif defined( __NT__ )
#define TRAPENTRY_FUNC_InfoFunction(x)  void TRAPENTRY x (HWND)
#elif defined( __WINDOWS__ )
#define TRAPENTRY_FUNC_InfoFunction(x)  void TRAPENTRY x (HWND)
#define TRAPENTRY_FUNC_GetHwndFunc(x)   HWND TRAPENTRY x (void)
#define TRAPENTRY_FUNC_InputHook(x)     void TRAPENTRY x (hook_fn *)
#define TRAPENTRY_FUNC_HardModeCheck(x) bool TRAPENTRY x (void)
#define TRAPENTRY_FUNC_SetHardMode(x)   void TRAPENTRY x (bool)
#define TRAPENTRY_FUNC_UnLockInput(x)   void TRAPENTRY x (void)
#endif

#endif
