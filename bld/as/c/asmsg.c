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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifdef _STANDALONE_
#include <process.h>
#include <unistd.h>
#include <stdlib.h>
#include "wressetr.h"
#include "wreslang.h"
#endif
#include "as.h"

#ifndef _STANDALONE_
// No res file to use. Just compile in the messages...
#undef PICK
// Use English message unless compiled with "-dJAPANESE_MSG"
#if !defined( JAPANESE_MSG )
#define PICK( id, e_msg, j_msg )    e_msg,
#else
#define PICK( id, e_msg, j_msg )    j_msg,
#endif
static char *asMessages[] = {
    "IMPOSSIBLE",
    #include "as.msg"
};
#endif

#ifdef _STANDALONE_
#define NIL_HANDLE      ((int)-1)

#define NO_RES_MESSAGE "Error: could not open message resource file\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)

static HANDLE_INFO      hInstance = {0};
static unsigned         msgShift;
extern long             FileShift;

static long resSeek( int handle, long position, int where ) {
