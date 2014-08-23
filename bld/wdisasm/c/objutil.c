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


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "disasm.h"



char  *ToStr( uint_32 value, uint_16 length, uint_32 ins_offset )
/***************************************************************/
{
    char                *name;
    fixup               *fix;
    char                *grp_name;
    int                 len;
    char                sym_name[MAX_NAME_LEN + 1];
    fixup_class         class;

    name = FindSymbol( InsAddr + ins_offset );
    if( name == NULL ) {
        DoIToHS( NameBuff, value, length );
    } else {
        strcpy( sym_name, name );
        NameBuff[ 0 ] = '\0';
        if( !DO_UNIX ) {
            fix = FindFixup( InsAddr + ins_offset, Segment );
            /* CL_LONG_POINTER ???? */
            class = fix->class & 7;
            if( class == CL_OFFSET || class == CL_LONG ) {
                if( Options & FORM_NAME_UPPER ) {
                    strcpy( NameBuff, "OFFSET " );
                } else {
                    strcpy( NameBuff, "offset " );
                }
                if( Options & FORM_ASSEMBLER ) {    /* need group name */
                    grp_name = GetGroupName( fix );
                    if( grp_name != NULL ) {
                        strcat( NameBuff, grp_name );
                        len = strlen( NameBuff );
                        NameBuff[ len ] = ':';
                        NameBuff[ len + 1 ] = '\0';
                    }
                }
            } else if( class == CL_BASE ) {
                if( _Class( fix->target ) != TYPE_GROUP ) {
                    if( Options & FORM_NAME_UPPER ) {
                        strcpy( NameBuff, "SEG " );
                    } else {
                        strcpy( NameBuff, "seg " );
                    }
                }
            }
        }
        strcat( NameBuff, sym_name );
    }
    return( NameBuff );
}


char  *ToIndex( uint_32 value, uint_32 ins_offset )
/*************************************************/
{
    char                *sym_name;

    sym_name = FindSymbol( InsAddr + ins_offset );
    if( sym_name == NULL ) {
        MyIToHS( NameBuff, value );
        return( NameBuff );
    } else {
        return( sym_name );
    }
}


char  *ToBrStr( uint_32 value, uint_32 ins_offset )
/*************************************************/
{
    char                *sym_name;
    int                 len;

    sym_name = FindSymbol( InsAddr + ins_offset );
    if( sym_name == NULL ) {
        len = 0;
        NameBuff[ len ] = '[';
        DoIToHS( &NameBuff[ len + 1 ], value, WORD_SIZE );
        len = strlen( NameBuff );
        NameBuff[ len ] = ']';
        NameBuff[ len + 1 ] = '\0';
        return( NameBuff );
    } else {
        return( sym_name );
    }
}


char  *ToSegStr( uint_32 off, uint_16 seg, uint_32 ins_offset )
/*************************************************************/
{
    char                *sym_name;
    int                 len;

    sym_name = FindSymbol( InsAddr + ins_offset );
    if( sym_name == NULL ) {
        DoIToHS( NameBuff, seg, 4 );
        len = strlen( NameBuff );
        NameBuff[ len ] = ':';
        DoIToHS( &NameBuff[ len + 1 ], off, WORD_SIZE );
        return( NameBuff );
    } else {
        return( sym_name );
    }
}


char  *JmpLabel( uint_32 addr, uint_32 ins_offset )
/*************************************************/
{
    return( FindLabel( InsAddr + ins_offset, addr, Segment ) );
}


void  IToHS( char *ptr, uint_32 value, int len )
/**********************************************/
{
    static const char hex[] = "0123456789abcdef";

#ifdef _M_I86
    /*
     * Because 32-bit shifts are expensive on 16-bit chips, here's a fast way
     * of doing hex conversion.  The compiler thinks better in uint_16's; so
     * we'll cater to it's taste.  Notice that this is VERY INTEL SPECIFIC -
     * especially the shift at the end of the loop.  (ie: it depends on
     * whether the machine is big or little endian.)  This takes half the time
     * of the more general version later.
     */
    uint_16 low;

    if( DO_UNIX ) {
        *ptr++ = '0';
        *ptr++ = 'x';
    }
    ptr += len;         /* advance pointer past end of final number */
    for(;;) {
        low = (uint_16)value;                   /* get low order word */

        *(--ptr) = hex[ low & 0x0f ];           /* do low nibble of word */
        if( --len == 0 ) break;                 /* check if done */

        *(--ptr) = hex[ ( low >> 4 ) & 0x0f ];  /* do next nibble of word */
        if( --len == 0 ) break;                 /* check if done */

            /* treat value as an array, and force compiler to do register
             * moves instead of an expensive shift */
        ((uint_8 *)&value)[0] = ((uint_8 *)&value)[1];
        ((uint_8 *)&value)[1] = ((uint_8 *)&value)[2];
        ((uint_8 *)&value)[2] = ((uint_8 *)&value)[3];
        ((uint_8 *)&value)[3] = 0;
    }

#else
    /* this is the typical routine, with the inner loop unrolled once */
    if( DO_UNIX ) {
        *ptr++ = '0';
        *ptr++ = 'x';
    }
    ptr += len;
    for(;;) {
        *(--ptr) = hex[ value & 0x0f ];
        if( --len == 0 ) break;
        value >>= 4;

        *(--ptr) = hex[ value & 0x0f ];
        if( --len == 0 ) break;
        value >>= 4;
    }
#endif
}


void  DoIToHS( char *ptr, int_32 value, int len )
/***********************************************/
{
    IToHS( ptr, value, len );
    if( DO_UNIX ) {
        ptr += 2;
    } else {
        if( !isdigit( *ptr ) ) {
            memmove( ptr + 1, ptr, ++len );
            *ptr = '0';
        }
        ptr[ len++ ] = 'H';
    }
    ptr[ len ] = '\0';
}


void  MyIToHS( char *ptr, int_32 value )
/**************************************/
{
    char                *str;
    char                numbuff[ 14 ];

    if( value < 0 ) {
        DoIToHS( numbuff, -value, WORD_SIZE );
    } else {
        DoIToHS( numbuff, value, WORD_SIZE );
    }
    str = numbuff;
    if( DO_UNIX ) {
        str += 2;
    }
    while( *str == '0' ) {
        ++str;
    }
    if( DO_UNIX ) {
        if( *str == '\0' ) {
            --str;
        }
        memmove( numbuff + 2, str, strlen( str )  + 1 );
        str = numbuff;
    } else {
        if( !isdigit( *str ) ) {
            --str;
        }
    }
    strcpy( ptr + 1, str );
    if( value < 0 ) {
        ptr[ 0 ] = '-';
    } else {
        ptr[ 0 ] = '+';
    }
}


char *GetGroupName( fixup *fix )
/******************************/

{
    segment             *targ;
    group               *grp;

    if( DO_UNIX ) {
        return( NULL );
    }
    targ = fix->target;
    if( targ->class == TYPE_IMPORT && ((import_sym *)targ)->exported ) {
        targ = ((import_sym *)targ)->u.also_exp->segment;
    }
    if( targ->class == TYPE_SEGMENT ) {
        grp = targ->grouped;
    } else if( targ->class == TYPE_GROUP ) {
        grp = (group *)targ;
    } else {
        grp = NULL;
    }
    if( grp != NULL && grp->name != NULL ) {
        return( grp->name );
    } else {
        return( NULL );
    }
}
