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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "vi.h"
#include "rxsupp.h"


/*
 * IsMagicCharRegular - check if character is magic and regular (no-magic meaning)
 */
bool IsMagicCharRegular( char ch )
{
    if( strchr( EditVars.Majick.str, ch ) != NULL ) {
        return( true );
    }
    return( false );

} /* IsMagicCharRegular */

/*
 * CurrentRegComp - compile current regular expression
 */
vi_rc CurrentRegComp( const char *str )
{
    if( CurrentRegularExpression != NULL ) {
        _MemFreeArray( CurrentRegularExpression );
    }
    CurrentRegularExpression = RegComp( str );
    return( RegExpError );

} /* CurrentRegComp */

/*
 * GetCurrRegExpColumn
 */
int GetCurrRegExpColumn( const char *data )
{
    int cl;

    cl = (int)( CurrentRegularExpression->startp[0] - data );
    return( cl );

} /* GetCurrRegExpColumn */

/*
 * GetCurrRegExpLength
 */
int GetCurrRegExpLength( void )
{
    int len;

    len = (int)( CurrentRegularExpression->endp[0] - CurrentRegularExpression->startp[0] );
    return( len );

} /* GetCurrRegExpLength */

/*
 * MakeExpressionNonRegular - escape out all magical chars
 */
void MakeExpressionNonRegular( char *str )
{
    int         i, j = 0, k;
    char        *foo;

    k = strlen( str );
    foo = StaticAlloc();
    for( i = 0; i < k; i++ ) {
        if( str[i] == '/' ) {
            foo[j++] = '\\';
        } else if( strchr( META, str[i] ) != NULL ) {
            foo[j++] = '\\';
            if( !EditFlags.Magic ) {
                if( IsMagicCharRegular( str[i] ) ) {
                    j--;
                }
            }
        }
        foo[j++] = str[i];
    }
    foo[j] = '\0';
    strcpy( str, foo );
    StaticFree( foo );

} /* MakeExpressionNonRegular */


static bool old_CaseIgnore = false;
static bool old_Magic1     = true;
static magic_type old_Majick = { "" };

void RegExpAttrSave( int caseignore, const char *majick )
{
    old_CaseIgnore = EditFlags.CaseIgnore;
    old_Magic1     = EditFlags.Magic;
    strcpy( old_Majick.str, EditVars.Majick.str );

    if( caseignore != -1 ) {
        EditFlags.CaseIgnore = ( caseignore ) ? true : false;
    }
    if( majick == NULL ) {
        EditFlags.Magic = true;
    } else {
        EditFlags.Magic = false;
        strcpy( EditVars.Majick.str, majick );
    }
}

void RegExpAttrRestore( void )
{
    EditFlags.CaseIgnore = old_CaseIgnore;
    EditFlags.Magic      = old_Magic1;
    strcpy( EditVars.Majick.str, old_Majick.str );
    *old_Majick.str = '\0';
}

static bool old_Magic2 = true;

void RegExpMagicSave( void )
{
    old_Magic2 = EditFlags.Magic;
    EditFlags.Magic = true;
}

void RegExpMagicRestore( void )
{
    EditFlags.Magic = old_Magic2;
}
