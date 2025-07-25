/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of sysconf.  The supported parameters can
*               always be expanded.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include "linuxsys.h"


#ifndef PAGE_SIZE
  #if defined( _M_IX86 )
    #define PAGE_SIZE 4096
  #endif
#endif

#ifndef SYS_CLK_TCK
#define SYS_CLK_TCK 100
#endif


static int __sysconf_nprocessors_conf( void )
/********************************************
 * I did not find a better method to find out the number
 * of installed CPUs than reading this value from the kernel
 * by file /sys/devices/system/cpu/present
 */
{
    int ret;
    FILE *fp;
    size_t len;
    char *str;
    char *start;

    fp = fopen( "/sys/devices/system/cpu/present", "rb" );
    if( fp == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }
    fseek( fp, 0, SEEK_END );
    len = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    str = malloc( len + 1 );
    fread( str, 1, len, fp );
    str[len] = '\0';
    fclose( fp );
    /*
     * search maximal number on read line
     * numbers are separated by single non-digit character
     */
    ret = 0;
    start = str;
    while( *start != '\0' ) {
        char *end;
        int val;

        val = strtol( start, &end, 10 );
        if( end == start )
            break;
        if( ret < val )
            ret = val;
        if( *end == '\0' )
            break;
        start = end + 1;
    }
    free( str );
    return( ret + 1 );
}

static int __sysconf_nprocessors( void )
{
    syscall_res res;
    unsigned char mask[128];    /* enough space for 1024 cores */
    int ret;
    int i;
    int used;

    res = sys_call3( SYS_sched_getaffinity, (u_long)0, (u_long)(sizeof(mask)), (u_long)mask );
    if ( __syscall_iserror( res ) ) {
        _RWD_errno = __syscall_errno( res );
        return( -1 );
    }
    used = __syscall_val( int, res );

    ret = 0;
    for( i = 0; i < used ; i++ ) {
        while( mask[i] ) {
            mask[i] &= mask[i] - 1;
            ret++;
        }
    }
    return( ret );
}

static int __sysconf_rlimit_int( int rlimid )
{
    struct rlimit rls;

    if( getrlimit( rlimid, &rls ) == 0 ) {
        return( (int)rls.rlim_cur );
    }
    return( -1 );
}

static int __sysconf_pages( int name )
{
    long quantity;
    struct sysinfo info;

    quantity = -1;
    if( sysinfo( &info ) == 0 ) {
        if( name == _SC_PHYS_PAGES ) {
            quantity = info.totalram;
        } else {
            quantity = info.sharedram + info.bufferram;
        }
    }

#ifdef PAGE_SIZE
    return( (int)( quantity / PAGE_SIZE ) );
#else
    _RWD_errno = EINVAL;
    return( -1 );
#endif
}

static int __sysconf_somaxconn( void )
{
    FILE *fp;
    int ret;
    char line[16];

    ret = 128;
    fp = fopen( "/proc/sys/net/core/somaxconn", "r" );
    if( fp != NULL ) {
        if( fgets( line, sizeof( line ), fp ) != NULL) {
            ret = atoi( line );
        }
        fclose( fp );
    }
    return ret;
}

_WCRTLINK long sysconf( int name )
{
    int ret;

    ret = -1;
    switch( name ) {
    case _SC_ARG_MAX:
        ret = ARG_MAX;
        break;
    case _SC_CHILD_MAX:
        ret = __sysconf_rlimit_int( RLIMIT_NPROC );
        break;
    case _SC_CLK_TCK:
        ret = SYS_CLK_TCK;
        break;
    case _SC_NGROUPS_MAX:
        ret = NGROUPS_MAX;
        break;
    case _SC_OPEN_MAX:
        ret = __sysconf_rlimit_int( RLIMIT_NOFILE );
        break;
    case _SC_JOB_CONTROL:
    case _SC_SAVED_IDS:
    case _SC_REALTIME_SIGNALS:
        /* Linux supports job control, saved ids, realtime...*/
        ret = 1;
        break;
    case _SC_VERSION:
        /* We'll report that we're trying to conform
         * to POSIX.1 Sept. 2008
         */
        ret = 200809;
        break;
    case _SC_STREAM_MAX:
        /* There is no limit on streams */
        ret = -1;
        break;
    case _SC_TZNAME_MAX:
        ret = TZNAME_MAX;
        break;
    case _SC_PAGESIZE:
        /* Also used for _SC_PAGE_SIZE */
#ifdef PAGE_SIZE
        ret = PAGE_SIZE;
#else
        _RWD_errno = EINVAL;
        ret = -1;
#endif
        break;
    case _SC_PHYS_PAGES:
    case _SC_AVPHYS_PAGES:
        ret = __sysconf_pages( name );
        break;
    case _SC_NPROCESSORS_CONF:
        ret = __sysconf_nprocessors_conf();
        break;
    case _SC_NPROCESSORS_ONLN:
        ret = __sysconf_nprocessors();
        break;
    case _SC_SOMAXCONN:
        ret = (long)__sysconf_somaxconn();
        break;
    case _SC_THREAD_ATTR_STACKADDR:
    case _SC_THREAD_ATTR_STACKSIZE:
    case _SC_MONOTONIC_CLOCK:
        ret = (long)1;
        break;
    case _SC_THREAD_STACK_MIN:
        /* We don't actually have one, but we'll
         * assume one page
         */
        ret = (long)PAGE_SIZE;
        break;
    case _SC_THREAD_PRIORITY_SCHEDULING:
    case _SC_THREAD_PROCESS_SHARED:
        ret = (long)0;
        break;
    default:
        _RWD_errno = EINVAL;
        break;
    }

    return( ret );
}
