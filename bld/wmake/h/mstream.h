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
* Description:  mstream.c interfaces
*
****************************************************************************/


#ifndef _MSTREAM_H
#define _MSTREAM_H  1
#include "mtypes.h"

/*
 * This is the type which GetCHR returns - this is the stream's data type.
 */
typedef INT16   STRM_T;


/*
 * special stream 'characters'
 */
#define STRM_MAGIC      (-2)    /* this value is never placed in the stream by
                                 * the stream routines - someone must do an
                                 * UnGetCH( STRM_MAGIC )
                                 */
#define STRM_END        (-1)    /* no more data in waiting */
#define TMP_EOL         (-3)    /* temporary place holder for microsoft eol */
                                /* to see whether or not we should check for */
                                /* bang                                      */
#define TMP_LEX_START   (-4)    /*  temporary place holder for lexParser*/

/*
 * prototypes
 */
extern RET_T    InsFile( const char *namem, BOOLEAN envsearch );
extern void     InsOpenFile( int fh );
extern void     InsString( const char *str, BOOLEAN weFree );
extern void     UnGetCH( STRM_T c );
extern STRM_T   GetCHR( void );
extern void     StreamInit( void );
extern void     StreamFini( void );
extern RET_T    GetFileLine( const char **pname, UINT16 *pline );
extern int      IsStreamEOF( void );

#endif /* !_MSTREAM_H */
