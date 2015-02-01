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
* Description:  Declares the public interface to the output buffer (and so to
*               the output file or device).
*                   ob_flush()
*                   ob_insert_block()
*                   ob_insert_byte()
*                   ob_insert_ps_text_start()
*                   ob_setup()
*                   ob_teardown()
*
****************************************************************************/

#ifndef OUTBUFF_H_INCLUDED
#define OUTBUFF_H_INCLUDED

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern void ob_flush( void );
extern void ob_insert_block( const char *in_block, size_t count, bool out_trans, bool out_text, font_number font );
extern void ob_insert_byte( unsigned char in_char );
extern void ob_insert_ps_text_end( bool htab_done, font_number font );
extern void ob_insert_ps_text_start( void );
extern void ob_setup( void );
extern void ob_teardown( void );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* OUTBUFF_H_INCLUDED */
