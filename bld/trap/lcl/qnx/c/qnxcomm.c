/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <unistd.h>
#include <process.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/debug.h>
#include <sys/stat.h>
#include <sys/proc_msg.h>
#include <sys/osinfo.h>
#include <sys/psinfo.h>
#include <sys/seginfo.h>
#include <sys/sched.h>
#include <sys/vc.h>
#include <sys/magic.h>
#include <sys/wait.h>
#include <sys/dumper.h>
#include <sys/console.h>
#include <sys/dev.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "qnxcomm.h"


const char *CollectNid( const char *name, size_t len, nid_t *nidp )
{
    const char  *ptr;
    nid_t       nid;
    char        ch;

    nid = 0;
    ptr = name;
    if( ptr[0] == '/' && ptr[1] == '/' ) {
        len -= 2;
        ptr += 2;
        //NYI: will need beefing up when NID's can be symbolic
        while( len > 0 ) {
            ch = *ptr;
            if( ch < '0' || ch > '9' )
                break;
            nid = ( nid * 10 ) + ( ch - '0' );
            ++ptr;
            --len;
        }
        //NYI: how do I check to see if NID is valid?
        if( len > 0 ) {
            switch( ptr[0] ) {
            CASE_SEPS
                break;
            default:
                nid = 0;
                ptr = name;
                break;
            }
        }
    }
    *nidp = nid;
    return( ptr );
}


trap_retval TRAP_CORE( Read_user_keyboard )( void )
{
    struct _console_ctrl    *con;
    unsigned                con_num;
    int                     con_hdl;
    int                     con_mode;
    char                    chr;
    //NYI: what about QNX windows?
    static char             con_name[] = "/dev/conXX";
    unsigned                timeout;
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

#   define FIRST_DIGIT (sizeof( con_name ) - 3)

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    timeout = acc->wait * 10;
    if( timeout == 0 )
        timeout = -1;
    ret->key = '\0';
    con = console_open( 2, O_WRONLY );
    if( con == NULL ) {
        return( sizeof( *ret ) );
    }
    con_num = console_active( con, -1 );
    console_close( con );
    con_name[FIRST_DIGIT + 0] = (con_num / 10) + '0';
    con_name[FIRST_DIGIT + 1] = (con_num % 10) + '0';

    con_hdl = open( con_name, O_RDONLY );
    if( con_hdl < 0 ) {
        if( timeout == -1 )
            timeout = 50;
        sleep( timeout / 10 );
        return( sizeof( *ret ) );
    }
    con_mode = dev_mode( con_hdl, 0, _DEV_MODES );
    if( dev_read( con_hdl, &chr, 1, 1, 0, timeout, 0, 0 ) == 1 ) {
        if( chr == '\xff' ) {
            read( con_hdl, &chr, 1 );
            chr = '\0';
        }
        ret->key = chr;
    }
    dev_mode( con_hdl, con_mode, _DEV_MODES );
    close( con_hdl );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Get_err_text )( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    strcpy( err_txt, strerror( acc->err ) );
    return( strlen( err_txt ) + 1 );
}


trap_retval TRAP_CORE( Split_cmd )( void )
{
    const char          *cmd;
    const char          *start;
    split_cmd_ret       *ret;
    size_t              len;
    nid_t               nid;

    start = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    cmd = CollectNid( start, len, &nid );
    len -= cmd - start;
    while( len > 0 ) {
        switch( *cmd ) {
        CASE_SEPS
            break;
        default:
            while( len > 0 ) {
                switch( *cmd ) {
                CASE_SEPS
                    ret->parm_start = 1;
                    len = 0;
                    continue;
                }
                ++cmd;
            }
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
{
    return( 0 );
}
