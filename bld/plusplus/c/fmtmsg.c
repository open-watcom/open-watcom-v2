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


#include <stdarg.h>
#include "plusplus.h"
#include "srcfile.h"
#include "vbuf.h"
#include "dbg.h"
#include "fmttype.h"
#include "fmtsym.h"
#include "fmtmsg.h"
#include "errdefns.h"

static void leading( VBUF *pbuf, char lead, int len )
/***************************************************/
{
    while( len-- > 0 ) {
        VStrConcChr( pbuf, lead );
    }
}

static boolean formatClassForSym( SYMBOL sym, VBUF *buf )
/*******************************************************/
{
    CLASSINFO *info;            // - class information for symbol
    boolean retn;               // - return: TRUE ==> is class member
    char *name;                 // - class name

    info = SymClassInfo( sym );
    if( info == NULL ) {
        retn = FALSE;
    } else {
        name = info->name;
        if( name == NULL ) {
            retn = FALSE;
        } else {
            VStrConcStr( buf, info->name );
            retn = TRUE;
        }
    }
    return retn;
}

SYMBOL FormatMsg( VBUF *pbuf, char *fmt, va_list arg )
/****************************************************/
// this function assumes that pbuf is initialized
// all information is concatenated to the end of pbuf
{
    VBUF    prefix, suffix;
    char    cfmt;
    char    local_buf[ 1 + sizeof( long ) * 2 + 1 ];
    unsigned len;
    SYMBOL  retn_symbol;

    retn_symbol = NULL;
    cfmt = *fmt;
    while( cfmt ) {
        if( cfmt == '%' ) {
            fmt++;
            cfmt = *fmt;
            switch( cfmt ) {
            case '1':   /* %01d */
            case '2':   /* %02d */
            case '3':   /* %03d */
            case '4':   /* %04d */
            case '5':   /* %05d */
            case '6':   /* %06d */
            case '7':   /* %07d */
            case '8':   /* %08d */
            case '9':   /* %09d */
                len = sticpy( local_buf, va_arg( arg, int ) ) - local_buf;
                leading( pbuf, '0', ( cfmt - '0' ) - len );
                VStrConcStr( pbuf, local_buf );
                break;
            case 'c':   /* %c */
                VStrConcChr( pbuf, va_arg( arg, char ) );
                break;
            case 's':   /* %s */
                VStrConcStr( pbuf, va_arg( arg, char * ) );
                break;
            case 'u':   /* %u */
                VStrConcDecimal( pbuf, va_arg( arg, unsigned int ) );
                break;
            case 'd':   /* %d */
                VStrConcInteger( pbuf, va_arg( arg, int ) );
                break;
            case 'L':   /* token location */
            {   TOKEN_LOCN *locn;
                locn = va_arg( arg, TOKEN_LOCN * );
                if( locn == NULL ) {
                    VStrConcStr( pbuf, "by compiler" );
                } else {
                    char *src_file = SrcFileName( locn->src_file );
                    if( src_file == NULL ) {
                        VStrConcStr( pbuf, "on the command line" );
                    } else {
                        if( ( CompFlags.ew_switch_used )
                          &&( locn->src_file == SrcFileTraceBackFile() ) ) {
                            VStrConcStr( pbuf, "at: " );
                        } else {
                            VStrConcStr( pbuf, "in: " );
                            VStrConcStr( pbuf, SrcFileName( locn->src_file ) );
                        }
                        VStrConcChr( pbuf, '(' );
                        VStrConcInteger( pbuf, locn->line );
                        if( locn->column ) {
                            if( CompFlags.ew_switch_used ) {
                                VStrConcChr( pbuf, ',' );
                                VStrConcInteger( pbuf, locn->column );
                            } else {
                                VStrConcStr( pbuf, ") (col " );
                                VStrConcInteger( pbuf, locn->column );
                            }
                        }
                        VStrConcChr( pbuf, ')' );
                    }
                }
            }   break;
            case 'N':   /* name */
                FormatName( va_arg( arg, char * ), &prefix );
                VStrConcStr( pbuf, prefix.buf );
                VbufFree( &prefix );
                break;
            case 'F':   /* symbol name (decorated) */
            {   SYMBOL      sym;
                sym = va_arg( arg, SYMBOL );
                FormatSym( sym, &prefix );
                VStrConcStr( pbuf, prefix.buf );
                VbufFree( &prefix );
            }   break;
            case 'S':   /* symbol name (abbreviated) */
            {   SYMBOL      sym;
                SYMBOL_NAME sn;
                char *name;
                sym = va_arg( arg, SYMBOL );
                if( sym == NULL ) {
                    VStrConcStr( pbuf, "module data" );
                } else {
                    if( formatClassForSym( sym, pbuf ) ) {
                        VStrConcStr( pbuf, "::" );
                    }
                    if( SymIsCtor( sym ) ) {
                        formatClassForSym( sym, pbuf );
                    } else if( SymIsDtor( sym ) ) {
                        VStrConcChr( pbuf, '~' );
                        formatClassForSym( sym, pbuf );
                    } else {
                        sn = sym->name;
#ifndef NDEBUG
                        if( sn == NULL ) {
                            CFatal( "FormatMsg -- %S symbol has NULL SYMBOL_NAME" );
                        }
#endif
                        name = sn->name;
#ifndef NDEBUG
                        if( name == NULL ) {
                            CFatal( "FormatMsg -- %S SYMBOL_NAME has NULL name" );
                        }
#endif
                        if( name == CppConversionName() ) {
                            VStrConcStr( pbuf, "operator " );
                            FormatType( SymFuncReturnType( sym )
                                      , &prefix
                                      , &suffix );
                            VbufFree( &suffix );
                        } else {
                            FormatName( name, &prefix );
                        }
                        VStrConcStr( pbuf, prefix.buf );
                        VbufFree( &prefix );
                    }
                    if( sym->flag2 & SF2_TOKEN_LOCN ) {
                        DbgVerify( retn_symbol == NULL, "too many symbols" );
                        retn_symbol = sym;
                    }
                }
            }   break;
            case 'T':   /* type name */
            {   TYPE type = va_arg( arg, TYPE );
                TYPE refed = TypeReference( type );
                if( NULL != refed ) {
                    type = refed;
                }
                FormatType( type, &prefix, &suffix );
                VStrConcStr( pbuf, prefix.buf );
                VStrConcStr( pbuf, suffix.buf );
                VbufFree( &prefix );
                VbufFree( &suffix );
                VStrTruncWhite( pbuf );
                if( NULL != refed ) {
                    VStrConcStr( pbuf, " (lvalue)" );
                }
            }   break;
            default:
                VStrConcChr( pbuf, cfmt );
            }
        } else {
            VStrConcChr( pbuf, cfmt );
        }
        fmt++;
        cfmt = *fmt;
    }
    return retn_symbol;
}
