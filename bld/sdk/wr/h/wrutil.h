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


WRDLLENTRY extern char *   WRAPI WRConvertStringFrom( char *str, char *from, char *to );
WRDLLENTRY extern char *   WRAPI WRConvertFrom( char *str, char from, char to );
WRDLLENTRY extern char *   WRAPI WRConvertTo( char *str, char to, char from );
WRDLLENTRY extern char *   WRAPI WRConvertStringTo( char *str, char *to, char *from );

WRDLLENTRY extern void     WRAPI WRMassageFilter( char *filter );

WRDLLENTRY extern int      WRAPI WRmbcs2unicode( char *src, char **dest, int *len );
WRDLLENTRY extern int      WRAPI WRmbcs2unicodeBuf( char *src, char *dest, int len );
WRDLLENTRY extern int      WRAPI WRunicode2mbcs( char *src, char **dest, int *len );
WRDLLENTRY extern int      WRAPI WRunicode2mbcsBuf( char *src, char *dest, int len );

WRDLLENTRY extern int      WRAPI WRStrlen( char *str, int is32Bit );
WRDLLENTRY extern int      WRAPI WRStrlen32( char *str );

WRDLLENTRY extern char *   WRAPI WRWResIDNameToStr( WResIDName *name );

WRDLLENTRY extern int      WRAPI WRFindFnOffset( char *name );
