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
* Description:  QNX and Linux specific trap implementation stuff
*
****************************************************************************/


typedef struct {
    trap_version        (TRAPENTRY *init_func)( char *, char *, bool);
    unsigned            (TRAPENTRY *req_func)( unsigned, mx_entry *, unsigned, mx_entry * );
    void                (TRAPENTRY *fini_func)( void );
} trap_requests;

typedef struct {
    unsigned long       len;
    char                ***environ;
    void                **_slib_func;
    void                *(*malloc)( unsigned );
    void                *(*realloc)( void *, unsigned );
    void                (*free)( void * );
    char                *(*getenv)( const char * );
    void                (*(*signal)( int __sig, void (*__func)(int) ))(int);
} trap_callbacks;
