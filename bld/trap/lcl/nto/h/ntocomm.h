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
* Description:  Internal header file for QNX Neutrino trap file.
*
****************************************************************************/


#include <sys/stat.h>
#include <sys/elf.h>
#include <sys/link.h>
#include <sys/debug.h>
#include <sys/procfs.h>


#ifdef DEBUG_OUT

extern void Out( const char *str );
extern void OutNum( unsigned long i );
#define dbg_print( x )  printf x 

#else

#define Out( x )
#define OutNum( x )
#define dbg_print( x )

#endif

/* Type indentifying the debuggee process. For Neutrino we use
 * the procfs file descriptor associated with debuggee.
 */
typedef int     pid_handle;

/* Internal trap file data structures */

typedef struct {
    unsigned    at_end          : 1;
    unsigned    loaded_proc     : 1;
    unsigned    fork            : 1;
    unsigned    have_rdebug     : 1;
    procfs_run  run;
    int         procfd;
    pid_t       pid;
    pthread_t   tid;
    char        procfs_path[PATH_MAX];
    unsigned    node;
    pid_t       proc;
    pid_t       son;
    int         sig;
    unsigned    version;
    long        cpu;
    long        fpu;
    int         save_in;
    int         save_out;
    addr_off    rdebug_va;
    addr_off    ld_bp_va;
    addr_off    dynsec_va;
}  process_info;

extern process_info ProcInfo;
extern char         **dbg_environ;

/* Internal trap file helper functions */
extern char         *StrCopy( const char *, char * );
extern unsigned     TryOnePath( const char *, struct stat *, const char *, char * );
extern unsigned     FindFilePath( bool, const char *, char * );
//extern char         *CollectNid( char *name, unsigned len, nid_t *nidp );

extern unsigned     ReadMem( pid_handle procfs_fd, void *ptr, addr_off offv, unsigned size );
extern unsigned     WriteMem( pid_handle procfs_fd, void *ptr, addr_off offv, unsigned size );
extern int          GetLinkMap( pid_handle pid, addr_off dbg_lmap_off, struct link_map *local_lmap );
extern int          GetLdInfo( pid_handle pid, addr_off dynsec_off, addr_off *rdebug_off, addr_off *ld_bp_off );
extern addr_off     GetDynSection( const char *exe_name );
extern int          MapAddrToLinkVA( pid_handle pid, addr_off addr, addr_off *map_addr );
extern char         *dbg_strcpy( pid_handle pid, char *, const char * );

extern void         AddProcess( const char *exe_name, addr_off dynsection );
extern int          AddLibs( pid_handle pid, addr_off first_lmap );
extern void         ProcessLdBreakpoint( pid_handle pid, addr_off rdebug_va );
