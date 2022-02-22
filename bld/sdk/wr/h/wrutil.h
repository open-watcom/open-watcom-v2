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


#define WRLEN_AUTO  ((size_t)-1)

WRDLLENTRY extern char      *WRAPI WRConvertStringFrom( const char *str, const char *from, const char *to );
WRDLLENTRY extern char      *WRAPI WRConvertFrom( const char *str, char from, char to );
WRDLLENTRY extern char      *WRAPI WRConvertTo( const char *str, char to, char from );
WRDLLENTRY extern char      *WRAPI WRConvertStringTo( const char *str, const char *to, const char *from );

WRDLLENTRY extern void      WRAPI WRMassageFilter( char *filter );

WRDLLENTRY extern size_t    WRAPI WRStrlen( const char *str, bool is32Bit );
WRDLLENTRY extern size_t    WRAPI WRStrlen32( const char *str );

WRDLLENTRY extern char      *WRAPI WRStringFromWResIDName( WResIDName *name );

WRDLLENTRY extern size_t    WRAPI WRFindFnOffset( const char *name );

WRDLLENTRY extern size_t    WRAPI WRCalcStrlen( const char *str, bool is32bit );
WRDLLENTRY extern size_t    WRAPI WRDataFromString( const char *str, bool is32bit, char *data );
WRDLLENTRY extern char      *WRAPI WRStringFromData( const char **pdata, bool is32bit );
