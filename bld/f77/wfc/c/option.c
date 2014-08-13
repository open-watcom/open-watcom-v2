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
* Description:  command and source line option processing
*
****************************************************************************/


#include "ftnstd.h"
#include "progsw.h"
#include "errcod.h"
#include "cpopt.h"
#include "optflags.h"
#include "global.h"
#include "intcnv.h"
#include "csetinfo.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "comio.h"
#include "inout.h"
#include "iopath.h"
#include "pathlist.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

extern  char            *SkipBlanks( char * );
extern  void            MacroDEFINE( char *,uint );
extern  void            MacroUNDEFINE( char *,uint );
extern  void            MacroIFDEF( char *,uint );
extern  void            MacroIFNDEF( char *,uint );
extern  void            MacroELIFDEF( char *,uint );
extern  void            MacroELIFNDEF( char *,uint );
extern  void            MacroELSE( void );
extern  void            MacroENDIF( void );
extern  void            ProcPragma( char * );
extern  void            __UseChineseCharSet( void );
extern  void            __UseJapaneseCharSet( void );
extern  void            __UseKoreanCharSet( void );

extern  character_set   CharSetInfo;

extern  uint            DataThreshold;

/* Forward declarations */
static  int     GetDirective( char *buff );
static  void    ScanOpts( char *buff );

// Compiler directives

#define CD_INCLUDE      1
#define CD_EJECT        2
#define CD_PRAGMA       3
#define CD_IFDEF        4
#define CD_IFNDEF       5
#define CD_ENDIF        6
#define CD_ELSE         7
#define CD_ELIFDEF      8
#define CD_ELIFNDEF     9
#define CD_DEFINE       10
#define CD_UNDEFINE     11

static const char __FAR * const __FAR CompDrctvs[] = {
    "INCLUDE",
    "EJECT",
    "PRAGMA",
    "IFDEF",
    "IFNDEF",
    "ENDIF",
    "ELSE",
    "ELSEIFDEF",
    "ELSEIFNDEF",
    "DEFINE",
    "UNDEFINE",
    NULL
};

#define DISK_MASK       (OPT_TYPE|OPT_PRINT)

// Used for matching OZOpts bits
#define _BitsMatched( bits, ptrn )      !((bits & ptrn) ^ ptrn)

static  char    *SkipOpt( char *buff ) {
//======================================

// Skip over an option.

    while( isalnum( *buff ) ) {
        buff++;
    }
    return( buff );
}


static  bool    GetValue( opt_entry *optn, char *ptr, char **val ) {
//==================================================================

// Get pointer to option value.

    *val = SkipBlanks( ptr );
    if( ( **val != '=' ) && ( **val != '#' ) ) {
        Warning( CO_NEED_EQUALS, optn->option );
        return( FALSE );
    } else {
        *val = SkipBlanks( *val + sizeof( char ) );
        return( TRUE);
    }
}


static  void    BitOption( opt_entry *optn, bool negated ) {
//==========================================================

// Process an option that has a bit value.

    ftnoption   opt_bit;

    opt_bit = optn->value;
    if( !negated ) {
        // TYPE turns off PRINT and vice-versa
        if( opt_bit & DISK_MASK ) {
            NewOptions &= ~DISK_MASK;
            NewOptions |= OPT_LIST;
        }
        if( opt_bit & OPT_LIST ) {
            SetLst( TRUE );
        }
        // SAVE turns off AUTOMATIC and vice-versa
        if( opt_bit & ( OPT_SAVE | OPT_AUTOMATIC ) ) {
            NewOptions &= ~( OPT_SAVE | OPT_AUTOMATIC );
        }
        NewOptions |= opt_bit;
    } else if( opt_bit & OPT_NO_NO ) {
        Warning( CO_BAD_NO, optn->option );
    } else {
        NewOptions &= ~opt_bit;
        if( opt_bit & OPT_LIST ) {
            SetLst( FALSE );
        }
    }
}


static  void    XLOption( opt_entry *optn, bool negated ) {
//=========================================================

// Extend source line beyond column 72.

    optn = optn;
    if( negated ) {
        LastColumn = LAST_COL;
    } else {
        LastColumn = SRCLEN;
    }
}


static  char    *SkipToken( char *buff ) {
//========================================

    for(;;) {
        if( *buff == NULLCHAR ) break;
        if( *buff == ' ' ) break;
        if( *buff == '\t' ) break;
        buff++;
    }
    return( buff );
}


static  void    DefOption( opt_entry *optn, char *ptr ) {
//=======================================================

// Define a macro.

    optn = optn;
    MacroDEFINE( ptr, SkipToken( ptr ) - ptr );
}


static  void    PathOption( opt_entry *optn, char *ptr )
//============================================================
// Process "INCPATH=" option.
{
    char        *p;
    char        *old_list;
    int         old_len;
    int         len;

    optn = optn;
    len = strlen( ptr );
    // skip quotes
    if( ptr[0] == '"' && ptr[len - 1] == '"' ) {
        len -= 2;
        ++ptr;
        ptr[len] = NULLCHAR;
    }
    if( len == 0 )
        return;
    if( IncludePath == NULL ) {
        p = IncludePath = FMemAlloc( len + 1 );
    } else {
        old_list = IncludePath;
        old_len = strlen( old_list );
        IncludePath = FMemAlloc( old_len + 1 + len + 1 );
        memcpy( IncludePath, old_list, len );
        FMemFree( old_list );
        p = IncludePath + old_len;
    }
    while( *ptr != NULLCHAR ) {
        if( p != IncludePath )
            *p++ = PATH_LIST_SEP;
        ptr = GetPathElement( ptr, NULL, &p );
    }
    *p = NULLCHAR;
}

void    FIncludePathInit( void )
//==============================
{
    char    *env;
    size_t  len;
    char    *p;

    FIncludePath = NULL;
    env = getenv( "FINCLUDE" );
    if( env != NULL && *env != '\0' ) {
        len = strlen( env );
        p = FIncludePath = FMemAlloc( len + 1 );
        while( *env != NULLCHAR ) {
            if( p != FIncludePath )
                *p++ = PATH_LIST_SEP;
            env = GetPathElement( env, NULL, &p );
        }
        *p = NULLCHAR;
    }
}

void    FIncludePathFini( void )
//==============================
{
    if( FIncludePath != NULL ) {
        FMemFree( FIncludePath );
    }
}

void    FiniProcCmd( void ) {
//===========================

    if( IncludePath != NULL ) {
        FMemFree( IncludePath );
    }
}


static  unsigned_32     OptV( opt_entry *optn, char *ptr ) {
//==========================================================

// Process an option that requires a value.

    unsigned_32 number;

    if( !isdigit( *ptr ) ) {
        Warning( CO_WANT_NUMBER, optn->option );
    }
    number = 0;
    for(;;) {
        number = 10 * number + ( *ptr - '0' );
        ptr++;
        if( isdigit( *ptr ) == 0 ) break;
    }
    return( number );
}



#define _Excl( excl_bits )  if( opt_bit & ( excl_bits ) ) {       \
                                CGOpts &= ~( excl_bits );         \
                            }


static  void    CGOption( opt_entry *optn, bool negated ) {
//=========================================================

// Process a code generator option.

    unsigned_32 opt_bit;

    opt_bit = optn->value;
    if( !negated ) {
#if _CPU == 8086
        _Excl( CGOPT_M_MEDIUM | CGOPT_M_LARGE | CGOPT_M_HUGE );
#elif _CPU == 386
        _Excl( CGOPT_M_FLAT | CGOPT_M_SMALL | CGOPT_M_COMPACT |
               CGOPT_M_MEDIUM | CGOPT_M_LARGE );
        _Excl( CGOPT_BD | CGOPT_BM );
        _Excl( CGOPT_STACK_CHK | CGOPT_STACK_GROW );
#elif _CPU == _AXP || _CPU == _PPC
        _Excl( CGOPT_BD | CGOPT_BM );
#else
  #error Unknown platform
#endif
        _Excl( CGOPT_DB_LINE | CGOPT_DB_LOCALS );
        _Excl( CGOPT_DI_CV | CGOPT_DI_DWARF | CGOPT_DI_WATCOM );
        CGOpts |= opt_bit;
    } else if( !(opt_bit & CGOPT_NO_NO) ) {
        CGOpts &= ~opt_bit;
    } else {
        Warning( CO_BAD_NO, optn->option );
    }
}


#define _OZExcl( excl_bits )  if( opt_bit & ( excl_bits ) ) {       \
                                   OZOpts &= ~( excl_bits );        \
                              }

static  void    OZOption( opt_entry *optn, bool negated ) {
//=========================================================

// Process a optimization option.

    unsigned_32 opt_bit;

    opt_bit = optn->value;
    if( !negated ) {
        _OZExcl( OZOPT_O_SPACE | OZOPT_O_TIME );
        OZOpts |= opt_bit;
    } else if( !(opt_bit & OZOPT_NO_NO) ) {
        OZOpts &= ~opt_bit;
    } else {
        Warning( CO_BAD_NO, optn->option );
    }
}


#define _CPUExcl( excl_bits )  if( opt_bit & ( excl_bits ) ) {       \
                                   CPUOpts &= ~( excl_bits );        \
                               }

#if _CPU == 8086 || _CPU == 386
static  void    CPUOption( opt_entry *optn, bool negated ) {
//==========================================================

// Process a code generator option.

    unsigned_32 opt_bit;

    opt_bit = optn->value;
    if( !negated ) {
#if _CPU == 8086
        _CPUExcl( CPUOPT_8086 | CPUOPT_80186 | CPUOPT_80286 |
                  CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586 );
#else
        _CPUExcl( CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586 );
#endif
        _CPUExcl( CPUOPT_FPC | CPUOPT_FPI | CPUOPT_FPI87 )
        _CPUExcl( CPUOPT_FPC | CPUOPT_FP287 | CPUOPT_FP387 | CPUOPT_FP5 );
        CPUOpts |= opt_bit;
    } else if( !(opt_bit & CPUOPT_NO_NO) ) {
        CPUOpts &= ~opt_bit;
    } else {
        Warning( CO_BAD_NO, optn->option );
    }
}
#endif


static  void    DTOption( opt_entry *optn, char *ptr ) {
//======================================================

// Process "DT=" option.

    DataThreshold = OptV( optn, ptr );
}


static  void    FOOption( opt_entry *optn, char *ptr ) {
//======================================================

// Process "FO=" option.

    optn = optn;
    ObjName = ptr;
}


static  void    NegOption( opt_entry *optn, bool negated ) {
//==========================================================

// Turn off bit option.

    optn = optn;
    negated = negated;
    NewOptions &= ~optn->value;
    if( optn->value == DISK_MASK ) {
        NewOptions |= OPT_LIST;
    }
}


static  void    ChiOption( opt_entry *optn, bool negated ) {
//==========================================================

    optn = optn; negated = negated;
    __UseChineseCharSet();
    NewOptions &= ~( OPT_CHINESE | OPT_JAPANESE | OPT_KOREAN );
    NewOptions |= OPT_CHINESE;
}


static  void    JapOption( opt_entry *optn, bool negated ) {
//==========================================================

    optn = optn; negated = negated;
    __UseJapaneseCharSet();
    NewOptions &= ~( OPT_CHINESE | OPT_JAPANESE | OPT_KOREAN );
    NewOptions |= OPT_JAPANESE;
}


static  void    KorOption( opt_entry *optn, bool negated ) {
//==========================================================

    optn = optn; negated = negated;
    __UseKoreanCharSet();
    NewOptions &= ~( OPT_CHINESE | OPT_JAPANESE | OPT_KOREAN );
    NewOptions |= OPT_KOREAN;
}


#define opt( name, bit, flags, actionstr, actionneg, desc ) name, desc, flags, bit, actionstr, actionneg

#include "optinfo.h"


static  bool    Match( char *buff, const char __FAR *list, bool value ) {
//=======================================================================

// Determine if option matches.

    for(;;) {
        if( *buff == NULLCHAR ) break;
        if( *buff == ' ' ) break;
        if( *buff == '\t' ) break;
        if( value ) {
            if( ( *buff == '=' ) || ( *buff == '#' ) ) break;
        }
        if( isalpha( *buff ) ) {
            if( toupper( *buff ) != toupper( *list ) ) return( FALSE );
        } else {
            if( *buff != *list ) return( FALSE );
        }
        ++list;
        ++buff;
    }
    if( isalpha( *list ) ) {
        if( isupper( *list ) ) return( FALSE );
    } else {
        if( *list != NULLCHAR ) return( FALSE );
    }
    return( TRUE );
}


static  opt_entry       *GetOptn( char *buff, bool *negated ) {
//=============================================================

// Parse an option.

    opt_entry   *optn;

    optn = CompOptns;
    *negated = FALSE;
    if( ( toupper( buff[ 0 ] ) == 'N' ) && ( toupper( buff[ 1 ] ) == 'O' ) ) {
        *negated = TRUE;
        buff += 2 * sizeof( char );
    }
    for( optn = CompOptns; optn->option != NULL; ++optn ) {
        if( optn->flags & CTG ) continue;
        if( Match( buff, optn->option, optn->flags & VAL ) ) {
            return( optn );
        }
    }
    return( NULL );
}


void    CmdOption( char *buff ) {
//===============================

// Process an option that can appear on the command line.

    opt_entry   *optn;
    bool        negated;
    char        *value;

    optn = GetOptn( buff, &negated );
    if( optn == NULL ) {
        Warning( CO_NOT_RECOG, buff );
    } else {
        if( optn->flags & VAL ) {
            if( negated ) {
                Warning( CO_BAD_NO, optn->option );
            }
            if( GetValue( optn, SkipOpt( buff ), &value ) ) {
                optn->proc_rtnstr( optn, value );
            }
        } else {
            optn->proc_rtnbool( optn, negated );
        }
    }
}


void    SrcOption( void ) {
//===================

// Process an option that can appear only in the source input stream.

    int         directive;
    char        *buff;

    buff = &SrcBuff[ 2 ];
    directive = GetDirective( buff );
    if( directive == CD_INCLUDE ) {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        CurrFile->flags |= INC_PENDING;
    } else if( directive == CD_EJECT ) {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        LFNewPage();
    } else if( directive == CD_PRAGMA ) {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        ComPrint();
        ProcPragma( SkipOpt( buff ) );
    } else if( directive == CD_DEFINE ) {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        ComPrint();
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroDEFINE( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_UNDEFINE ) {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        ComPrint();
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroUNDEFINE( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_IFDEF ) {
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroIFDEF( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_ELIFDEF ) {
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroELIFDEF( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_IFNDEF ) {
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroIFNDEF( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_ELIFNDEF ) {
        buff = SkipBlanks( SkipOpt( buff ) );
        MacroELIFNDEF( buff, SkipToken( buff ) - buff );
    } else if( directive == CD_ELSE ) {
        MacroELSE();
    } else if( directive == CD_ENDIF ) {
        MacroENDIF();
    } else {
        if( ProgSw & PS_SKIP_SOURCE ) return;
        ComPrint();
        ScanOpts( buff );
        // consider:
        //      c$warn
        //      c$notime=5
        // CO-04 will not be issued unless /warn or c$warn is done.  But
        // in the above case isn't updated unless we do this.
        Options = NewOptions;
    }
}


static  void    OptWarning( int warn, char *opt ) {
//=================================================

// Issue an option warning message.

    char        *ptr;

    ptr = SkipToken( opt );
    *ptr = NULLCHAR;
    Warning( warn, opt );
}


static  void    ScanOpts( char *buff ) {
//======================================

    opt_entry   *optn;
    bool        negated;
    char        *value;
    bool        first_opt;

    if( strlen( SrcBuff ) > LastColumn ) {
        SrcBuff[ LastColumn ] = NULLCHAR;
    }
    first_opt = TRUE;
    for(;;) {
        buff = SkipBlanks( buff );
        if( *buff == NULLCHAR ) break;
        optn = GetOptn( buff, &negated );
        if( optn == NULL ) {
            if( !first_opt ) {
                OptWarning( CO_NOT_RECOG, buff );
            }
            break;
        }
        first_opt = FALSE;
        if( ( optn->flags & SRC ) == 0 ) {
            Warning( CO_NOT_IN_SOURCE, optn->option );
            buff = SkipOpt( buff );
        } else {
            buff = SkipOpt( buff );
            if( optn->flags & VAL ) {
                if( negated ) {
                    Warning( CO_BAD_NO, optn->option );
                }
                if( !GetValue( optn, buff, &value ) ) continue;
                buff = SkipToken( value );
                optn->proc_rtnstr( optn, value );
            } else {
                optn->proc_rtnbool( optn, negated );
            }
        }
    }
}


static  int     GetDirective( char *buff ) {
//==========================================

// Parse a compiler directive.

    int         offset;
    const char  __FAR * const __FAR *drctv;

    drctv = CompDrctvs;
    offset = 0;
    for(;;) {
        if( drctv[ offset ] == NULL ) return( 0 );
        if( Match( buff, drctv[ offset ], FALSE ) ) return( offset + 1 );
        offset++;
    }
}


static  char    *GetOptName( char *buffer, char *opt_name ) {
//===========================================================

// Print option name.

    char        *buff;

    buff = buffer + sizeof( char );
    while( *opt_name != NULLCHAR ) {
        *buff = tolower( *opt_name );
        opt_name++;
        buff++;
    }
    *buff = NULLCHAR;
    return( buff );
}


void    PrtOptions( void ) {
//====================

// Display current options in listing file.

    opt_entry   *optn;
    char        buffer[30];
    char        *buff;
    unsigned_32 number;

    LFSkip();
    PrtLst( "Options:" );
    buffer[ 0 ] = ' ';
    number = 0;
    optn = CompOptns;
    for( optn = CompOptns; optn->option != NULL; optn++ ) {
        if( optn->flags & VAL ) {
            // the following check will only work if
            // OPT_DEFINE is a "VAL" option
            if( optn->value == OPT_DEFINE ) continue;
            if( optn->value == CGOPT_DATA_THRESH ) {
                number = DataThreshold;
            }
            buff = GetOptName( buffer, optn->option );
            *buff = '=';
            ++buff;
            if( optn->value == OPT_INCPATH ) {
                if( IncludePath == NULL ) continue;
                *buff = NULLCHAR;
                PrtLst( buffer );
                PrtLst( IncludePath );
                buffer[ 0 ] = ',';
                continue;
            }
            if( optn->value == CGOPT_OBJ_NAME ) {
                if( ObjName == NULL ) continue;
                *buff = NULLCHAR;
                PrtLst( buffer );
                PrtLst( ObjName );
                buffer[ 0 ] = ',';
                continue;
            }
            buff += sprintf( buff, "%lu", (unsigned long)number );
            PrtLst( buffer );
            buffer[ 0 ] = ',';
        } else {
            if( optn->flags & CG ) {
                if( optn->flags & NEG ) {
                    if( CGOpts & optn->value ) continue;
                } else {
                    if( ( CGOpts & optn->value ) == 0 ) continue;
                }
            } else if( optn->flags & CPU ) {
                if( optn->flags & NEG ) {
                    if( CPUOpts & optn->value ) continue;
                } else {
                    if( ( CPUOpts & optn->value ) == 0 ) continue;
                }
            } else if( optn->flags & OZ ) {
                if( optn->flags & NEG ) {
                    if( _BitsMatched( OZOpts, optn->value ) ) continue;
                } else {
                    if( !_BitsMatched( OZOpts, optn->value ) ) continue;
                }
            } else
            if( optn->value == OPT_XLINE ) {
                if( LastColumn == LAST_COL ) continue;
            } else {
                if( optn->flags & NEG ) {
                    if( Options & optn->value ) continue;
                } else {
                    if( ( Options & optn->value ) == 0 ) continue;
                }
            }
            GetOptName( buffer, optn->option );
            PrtLst( buffer );
            buffer[ 0 ] = ',';
        }
    }
    PrtLstNL( "" );
    LFSkip();
}
