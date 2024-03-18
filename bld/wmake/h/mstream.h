/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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

/*
 * This is the type which GetCHR returns - this is the stream's data type.
 *
 * NOTE:
 *  the code in the mglob.c and mtypes.h depend on STRM_T definition
 *
 *  if you do some change in STRM_T then do
 *   - update definition of macro STRM_PLACEHOLDERS in mtypes.h
 *   - update code in mglob.c
 */
typedef enum {
    /*
     * temporary place holder for lexParser
     */
    STRM_TMP_LEX_START = -4,
    /*
     * temporary place holder for microsoft EOL to see whether or not
     * we should check for bang
     */
    STRM_TMP_EOL       = -3,
    /*
     * this value is never placed in the stream by the stream routines
     * someone must do an UnGetCHR( STRM_MAGIC )
     */
    STRM_MAGIC         = -2,
    /*
     * no more data in waiting
     */
    STRM_END           = -1,

    /*
     * all base and extended ASCII characters
     */
    STRM_ASCII_MIN     = 0x00,
    STRM_ASCII_MAX     = 0xFF,

} STRM_T;

/*
 * prototypes
 */
extern bool     InsFile( const char *namem, bool envsearch );
extern void     InsOpenFile( FILE *fp );
extern void     InsString( const char *str, bool weFree );
extern void     UnGetCHR( STRM_T s );
extern STRM_T   GetCHR( void );
extern void     StreamInit( void );
extern void     StreamFini( void );
extern bool     GetFileLine( const char **pname, UINT16 *pline );
extern bool     IsStreamEOF( void );

#endif /* !_MSTREAM_H */
