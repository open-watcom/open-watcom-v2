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
* Description:  Terminal keyboard definitions.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include "uidef.h"
#include "uishift.h"

#include "uivirt.h"
#include "qnxuiext.h"
#include "qdebug.h"

#include "trie.h"
#include "tixparse.h"

extern unsigned short   ct_shift_state;

extern char     *GetTermType( void );
extern void     clear_shift( void );
extern int      nextc( int n );
extern void     nextc_unget( char *, int );
extern EVENT    ck_keyboardevent( void );
extern EVENT    tk_keyboardevent( void );
extern EVENT    td_event( void );
extern void     tm_saveevent( void );

extern int      ck_init( void );
extern int      ck_fini( void );
extern void     ck_arm( void );
extern int      ck_save( void );
extern int      ck_restore( void );
extern int      ck_flush( void );
extern int      ck_stop( void );
extern int      ck_shift_state( void );
extern int      ck_unevent( EVENT ev );
extern bool     init_interminfo( void );
extern int      init_trie( void );
extern int      kb_wait( int secs, int usecs );
