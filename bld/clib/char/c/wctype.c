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


#include "variety.h"
#include <string.h>
#include <wchar.h>
#include "wctype.h"



/****
***** Return a type ID usable by iswctype on success, or zero on error.
****/

_WCRTLINK wctype_t wctype( const char *property )
{
    if( !strcmp( property, "alnum" ) )   return( WCTYPE_ALNUM );
    if( !strcmp( property, "alpha" ) )   return( WCTYPE_ALPHA );
    if( !strcmp( property, "cntrl" ) )   return( WCTYPE_CNTRL );
    if( !strcmp( property, "digit" ) )   return( WCTYPE_DIGIT );
    if( !strcmp( property, "graph" ) )   return( WCTYPE_GRAPH );
    if( !strcmp( property, "lower" ) )   return( WCTYPE_LOWER );
    if( !strcmp( property, "print" ) )   return( WCTYPE_PRINT );
    if( !strcmp( property, "punct" ) )   return( WCTYPE_PUNCT );
    if( !strcmp( property, "space" ) )   return( WCTYPE_SPACE );
    if( !strcmp( property, "upper" ) )   return( WCTYPE_UPPER );
    if( !strcmp( property, "xdigit" ) )  return( WCTYPE_XDIGIT );
    return( 0 );
}
