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
* Description:  Declararions for linker message output machinery.
*
****************************************************************************/


#define MSG_BASE                0

#include "wlmsgs.h"

#define RESOURCE_MAX_SIZE       128

typedef union msg_arg {
    symbol          *symb;
    char            *string;
    char            c;
    unsigned_16     int_16;
    unsigned_32     int_32;
    targ_addr       *address;
} MSG_ARG;

typedef struct msg_arg_list {
    int             index;
    MSG_ARG         arg[5];
} MSG_ARG_LIST;

#include <stdarg.h>

extern bool     InitMsg( void );
extern void     ResetMsg( void );
extern bool     Msg_Get( int resourceid, char *buffer );
extern void     Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *, const char *, ... );
extern void     Msg_Put_Args( char rc_buff[], MSG_ARG_LIST *, const char *, va_list *);
extern void     Msg_Write_Map( int resourceid, ... );
extern bool     FiniMsg( void );

extern int      WLinkItself;
