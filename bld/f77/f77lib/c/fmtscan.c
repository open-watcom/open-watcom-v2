/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Format statement scanning routines for compile and run time
*
****************************************************************************/


#include "ftnstd.h"
#include <ctype.h>
#include "format.h"
#include "errcod.h"
#include "fmtdef.h"
#include "fmtdat.h"
#include "fmtscan.h"
#include "rterr.h"
#include "rfmtemit.h"


/* Forward declarations */
static  void    FCode( void );
static  void    FReal( byte format_code );

typedef struct f_procs {
    byte        code;
    void        (*routine)(void);
} f_procs;

static  void    FSkipSpaces( void )
// Skip spaces between format codes.
{
    for(;;) {
        if( Fmt_charptr >= Fmt_end )
            break;
        if( *Fmt_charptr != ' ' )
            break;
        ++Fmt_charptr;
    }
}

static  bool    R_FRecEos( void ) {
// Attempt to recognize the end of a format string.
    FSkipSpaces();
    return( Fmt_charptr >= Fmt_end );
}

static  bool    R_FR_Char( char test_char )
// Formatted I/O character recognition.
// This routine returns true if the specified character is found
// Note that the format character is in lower case when the
// comparison is performed.
{
    if( R_FRecEos() )
        return( false );
    if( tolower( *Fmt_charptr ) != test_char )
        return( false );
    ++Fmt_charptr;
    FSkipSpaces();
    return( true );
}

static  int     R_FConst( void ) {
// Scan a non-negative constant in a format string.
//     - blanks are ignored
//     - returns -1 if no constant was found
    char        PGM *start_char;
    int         result;
    char        cur_char;

    FSkipSpaces();
    result = 0;
    start_char = Fmt_charptr;
    for(;;) {
        if( R_FRecEos() ) break;
        cur_char = *Fmt_charptr;
        if( isdigit( cur_char ) == 0 ) break;
        result *= 10;
        if( result >= 0 ) {
            result += ( cur_char - '0' );
            if( result >= 0 ) {
                ++Fmt_charptr;
                FSkipSpaces();
            } else {
                RTErr( FM_CONST );
            }
        } else {
            RTErr( FM_CONST );
        }
    }
    if( Fmt_charptr == start_char ) {
        result = -1;
    }
    return( result );
}

static  void    GetRepSpec( void ) {
// Get a possible repeat specification.
    bool        minus;

    if( ( *Fmt_charptr != '-' ) && ( *Fmt_charptr != '+' ) ) {
        Fmt_rep_spec = R_FConst();
        if( ( tolower( *Fmt_charptr ) == 'p' ) && ( Fmt_rep_spec == -1 ) ) {
            RTErr( FM_CONST );
        }
    } else {
        minus = *Fmt_charptr == '-';
        ++Fmt_charptr;
        Fmt_rep_spec = R_FConst();
        FSkipSpaces();
        if( tolower( *Fmt_charptr ) == 'p' ) {
            if( Fmt_rep_spec <= 0 ) {
                RTErr( FM_CONST );
            } else if( minus ) {
                Fmt_rep_spec = -Fmt_rep_spec;
            }
        } else {
            // set the repeat specifier so that an error will be detected
            // by a subsequent routine
            Fmt_rep_spec = 0;
        }
    }
}

static  void    R_FSpec( void )
// Process a format specification.
{
    Fmt_delimited = YES_DELIM;
    for(;;) {
        if( R_FRecEos() )
            return;
        if( R_FR_Char( ')' ) )
            break;
        GetRepSpec();
        FCode();
        if( Fmt_paren_level <= 0 ) {
            return;
        }
    }
    --Fmt_paren_level;
    Fmt_delimited = NO_DELIM;
    R_FEmCode( RP_FORMAT );
}

static  int     R_FRPConst( void ) {
// Get a required positive constant in a format string.
    int result;

    result = R_FConst();
    if( result <= 0 ) {
        RTErr( FM_CONST );
    }
    return( result );
}

static  bool    R_FReqChar( char test_string, int err_code )
// Formatted I/O token scanner.
// This routine generates an error condition if the specified character
// is not found.
{
    if( R_FR_Char( test_string ) )
        return( true );
    RTErr( err_code );
    return( false );
}

static  bool    FNoRep( void )
// Make sure that no repeat specification was given.
// Return true if no repeat spec was given.
{
    if( Fmt_rep_spec == -1 )
        return( true );
    RTErr( FM_NO_REP );
    return( false );
}

static  void    FChkDelimiter( void )
// Make sure that an element has been delimited.
{
    if( Fmt_delimited != YES_DELIM ) {
        RTExtension( FM_ASSUME_COMMA );
    }
    Fmt_delimited = NO_DELIM;
}

static  void    R_FLiteral( void )
// Process a literal format code.
{
    int         lit_length;
    char        PGM *cur_char_ptr;

    FChkDelimiter();
    if( FNoRep() ) {
        lit_length = 0;
        cur_char_ptr = ++Fmt_charptr;
        for(;;) {
            if( *Fmt_charptr == '\'' ) {
                ++Fmt_charptr;
                if( *Fmt_charptr != '\'' ) {
                    break;
                }
            }
            ++lit_length;
            ++Fmt_charptr;
            if( Fmt_charptr >= Fmt_end ) {
                break;
            }
        }
        if( Fmt_charptr >= Fmt_end ) {
            RTErr( FM_QUOTE );
        }
        if( Fmt_charptr < Fmt_end ) {
            R_FEmCode( H_FORMAT );
            R_FEmNum( lit_length );
            for(;;) {
                if( *cur_char_ptr == '\'' ) {
                    ++cur_char_ptr;
                    if( *cur_char_ptr != '\'' ) {
                        break;
                    }
                }
                R_FEmChar( cur_char_ptr );
                ++cur_char_ptr;
            }
        }
    }
}

static  void    R_FH( void )
// Process an H format code.
// Note that Fmt_rep_spec represents the desired length of
// the H format code.
{
    FChkDelimiter();
    if( ( Fmt_rep_spec <= 0 ) || ( Fmt_charptr + Fmt_rep_spec >= Fmt_end ) ) {
        RTErr( FM_WIDTH );
    } else {
        R_FEmCode( H_FORMAT );
        R_FEmNum( Fmt_rep_spec );
        for(;;) {
            R_FEmChar( Fmt_charptr );
            ++Fmt_charptr;
            if( --Fmt_rep_spec == 0 ) {
                break;
            }
        }
    }
}

static  void    R_FComma( void )
// Process a comma format delimiter/code.
{
    if( FNoRep() ) {
        if( !R_FRecEos() && ( *Fmt_charptr != ',' ) &&
            ( *Fmt_charptr != ')' ) ) {
            Fmt_delimited = YES_DELIM;
        } else {
            RTErr( FM_DELIM );
        }
    }
}

static  bool    FRep( void )
// Validate and emit the repeat specification.
// Return true if the repeat spec was valid.
{
    if( Fmt_rep_spec == 0 ) {
        RTErr( FM_INV_REP );
        return( false );
    } else if( Fmt_rep_spec > 1 ) {
        R_FEmCode( REP_FORMAT );
        R_FEmNum( Fmt_rep_spec );
    }
    return( true );
}

static  void    R_FSlash( void )
// Process a slash format delimeter/code.
{
    if( FNoRep() ) {
        Fmt_delimited = YES_DELIM;
        Fmt_rep_spec = 0;
        for(;;) {
            ++Fmt_rep_spec;
            if( !R_FR_Char( '/' ) ) {
                break;
            }
        }
        FRep();
        R_FEmCode( SL_FORMAT );
    }
}

static  void    R_FX( void )
// Process an X format code.
// Note that Fmt_rep_spec represents the number of spaces
// that are desired to skipped.
{
    FChkDelimiter();
    if( Fmt_rep_spec == -1 ) {
        Fmt_rep_spec = 1;
        RTExtension( FM_ASSUME_CONST );
    } else if( Fmt_rep_spec <= 0 ) {
        RTErr( FM_CONST );
    }
    R_FEmCode( X_FORMAT );
    R_FEmNum( Fmt_rep_spec );
}

static  void    R_FI( void )
// Process an I format code.
{
    int         fmt_width;
    int         fmt_min;

    FChkDelimiter();
    if( FRep() ) {
        fmt_width = R_FRPConst();
        if( fmt_width > 0 ) {
            if( R_FR_Char( '.' ) ) {
                fmt_min = R_FConst();
                if( ( fmt_width < fmt_min ) || ( fmt_min < 0 ) ) {
                    RTErr( FM_MODIFIER );
                }
            } else {
                fmt_min = 1;
            }
            R_FEmCode( I_FORMAT );
            R_FEmByte( fmt_width );
            R_FEmByte( fmt_min );
        }
    }
}

static  void    R_FColon( void )
// Process a colon.
{
    FNoRep();
    R_FEmCode( C_FORMAT );
    Fmt_delimited = YES_DELIM;
}

static  void    R_FA( void )
// Process an A format code.
{
    int         fmt_width;

    FChkDelimiter();
    if( FRep() ) {
        fmt_width = R_FConst();
        if( fmt_width == 0 ) {
            RTErr( FM_WIDTH );
        } else if( fmt_width < 0 ) {
            // width of 0 is a flag to indicate that the width corresponds
            // to the size of the variable being processed
            fmt_width = 0;
        }
        R_FEmCode( A_FORMAT );
        R_FEmNum( fmt_width );
    }
}

static  void    R_FT( void )
// Process a T, TL or TR format specifier.
{
    int         width;

    FChkDelimiter();
    if( FNoRep() ) {
        if( R_FR_Char( 'l' ) ) {
            R_FEmCode( TL_FORMAT );
        } else if( R_FR_Char( 'r' ) ) {
            R_FEmCode( TR_FORMAT );
        } else {
            R_FEmCode( T_FORMAT );
        }
        width = R_FRPConst();
        if( width > 0 ) {
            R_FEmNum( width );
        }
    }
}

static  void    R_FS( void )
// Process an S, SP or SS format specifier.
{
    FChkDelimiter();
    if( FNoRep() ) {
        if( R_FR_Char( 'p' ) ) {
            R_FEmCode( SP_FORMAT );
        } else if( R_FR_Char( 's' ) ) {
            R_FEmCode( SS_FORMAT );
        } else {
            R_FEmCode( S_FORMAT );
        }
    }
}

static  void    R_FB( void )
// Process a BN or BZ format specifier.
{
    FChkDelimiter();
    if( FNoRep() ) {
        if( R_FR_Char( 'n' ) ) {
            R_FEmCode( BN_FORMAT );
        } else {
            R_FReqChar( 'z', FM_FMTCHAR );
            R_FEmCode( BZ_FORMAT );
        }
    }
}

static  void    R_FL( void )
// Process an L format code.
{
    int         width;

    FChkDelimiter();
    if( FRep() ) {
        R_FEmCode( L_FORMAT );
        width = R_FRPConst();
        if( width > 0 ) {
            R_FEmByte( width );
        }
    }
}

static  void    R_FD( void )
// Process a D format code.
{
    FReal( D_FORMAT );
}

static  void    R_FQ( void )
// Process a Q format code.
{
    RTExtension( FM_Q_FORMAT );
    FReal( Q_FORMAT );
}

static  void    R_FF( void )
// Process an F format code.
{
    FReal( F_FORMAT );
}

static  void    R_FE( void )
// Process an E format code.
{
    FReal( E_FORMAT );
}

static  void    R_FG( void )
// Process a G format code.
{
    FReal( G_FORMAT );
}

static  void    FReal( byte format_code )
// Process an F, D, Q, E or G format code.
{
    int         fmt_width;
    int         fmt_modifier;
    int         fmt_exp;

    if( Fmt_delimited == NO_DELIM ) {
        RTExtension( FM_ASSUME_COMMA );
    }
    Fmt_delimited = NO_DELIM;
    if( !FRep() )
        return;
    fmt_width = R_FRPConst();
    if( fmt_width <= 0 )
        return;
    if( !R_FReqChar( '.', FM_DECIMAL ) )
        return;
    fmt_modifier = R_FConst();
    if( ( fmt_modifier < 0 ) || ( fmt_width < fmt_modifier ) ) {
        RTErr( FM_MODIFIER );
        return;
    }
    fmt_exp = 0;
    if( ( format_code == E_FORMAT ) || ( format_code == G_FORMAT ) ) {
        if( isalpha( *Fmt_charptr ) ) {
            if( ( format_code == E_FORMAT ) || R_FR_Char( 'e' ) ) {
                if( format_code == E_FORMAT ) {
                    if( !R_FR_Char( 'e' ) ) {
                        if( !R_FR_Char( 'd' ) ) {
                            if( !R_FReqChar( 'q', FM_FMTCHAR ) )
                                return;
                            format_code = EQ_FORMAT;
                            RTExtension( FM_Q_EXT );
                        } else {
                            format_code = ED_FORMAT;
                            RTExtension( FM_D_EXT );
                        }
                    }
                }
                fmt_exp = R_FRPConst();
                if( fmt_exp <= 0 )
                    return;
                if( fmt_width < fmt_modifier ) {
                    RTErr( FM_MODIFIER );
                }
            }
        }
    }
    R_FEmCode( format_code );
    R_FEmByte( fmt_width );
    R_FEmByte( fmt_modifier );
    if( ( format_code == E_FORMAT )  || ( format_code == ED_FORMAT ) ||
        ( format_code == EQ_FORMAT ) || ( format_code == G_FORMAT ) ) {
        R_FEmByte( fmt_exp );
    }
}

static  void    R_FP( void )
// Process a P format delimiter/code.
// Note that Fmt_rep_spec represents the scale factor.
{
    FChkDelimiter();
    Fmt_delimited = P_DELIM;
    R_FEmCode( P_FORMAT );
    R_FEmNum( Fmt_rep_spec );
}

static  void    R_FLParen( void )
// Process a left parenthesis.
{
    FChkDelimiter();
    if( Fmt_paren_level < 2 ) {
        if( Fmt_rep_spec == 0 ) {
            RTErr( FM_INV_REP );
        } else if( Fmt_rep_spec > 1 ) {
            R_FEmCode( REP_FORMAT | REV_CODE );
            R_FEmNum( Fmt_rep_spec );
            R_FEmCode( LP_FORMAT );
        } else {
            R_FEmCode( LP_FORMAT | REV_CODE );
        }
    } else {
        FRep();
        R_FEmCode( LP_FORMAT );
    }
    ++Fmt_paren_level;
    ++Fmt_charptr;
    if( R_FR_Char( ',' ) ) {
        RTErr( FM_DELIM );
    }
    R_FSpec();
}

static  void    R_FZ( void )
// Process a z format code (extension).
{
    int         width;

    FChkDelimiter();
    if( FRep() ) {
        width = R_FConst();
        if( width == 0 ) {
            RTErr( FM_WIDTH );
        } else if( width < 0 ) {
            // width of 0 is a flag to indicate that the width corresponds
            // to the size of the variable being processed
            width = 0;
        }
        R_FEmCode( Z_FORMAT );
        R_FEmByte( width );
        RTExtension( FM_Z_EXT );
    }
}

static  void    R_FM( void )
// Process a $ format code (extension).
{
    FChkDelimiter();
    FNoRep();
    R_FEmCode( M_FORMAT );
    RTExtension( FM_M_EXT );
}

static  const f_procs __FAR FP_Cod[] = {
        '\'',   &R_FLiteral,
        'f',    &R_FF,
        'e',    &R_FE,
        'd',    &R_FD,
        'g',    &R_FG,
        'q',    &R_FQ,
        'h',    &R_FH,
        'l',    &R_FL,
        ',',    &R_FComma,
        '/',    &R_FSlash,
        'x',    &R_FX,
        'i',    &R_FI,
        '(',    &R_FLParen,
        'a',    &R_FA,
        't',    &R_FT,
        's',    &R_FS,
        'b',    &R_FB,
        'p',    &R_FP,
        ':',    &R_FColon,
        '$',    &R_FM,
        '\\',   &R_FM,
        'z',    &R_FZ,
        NULLCHAR, NULL
};

static  void    FCode( void )
// Process a format code.
{
    char                current;
    const f_procs __FAR *f_rtn;

    if( R_FRecEos() ) {
        RTErr( FM_FMTCHAR );
        return;
    }
    current = tolower( *Fmt_charptr );
    f_rtn = FP_Cod;
    for(;;) {
        if( f_rtn->code == NULLCHAR ) {
            RTErr( FM_FMTCHAR );
            return;
        }
        if( f_rtn->code == current )
            break;
        f_rtn++;
    }
    if( ( current != '\'' ) && ( current != '(' ) ) {
        ++Fmt_charptr;
        if( current != 'h' ) {
            FSkipSpaces();
        }
    }
    f_rtn->routine();
}

void    R_FDoSpec( void )
// Process a complete format specification.
{
    FSkipSpaces();
    if( *Fmt_charptr != '(' ) {
        RTErr( PC_NO_OPENPAREN );
    } else {
        R_FSpec();
    }
    if( Fmt_paren_level != 0 ) {
        RTErr( PC_UNMATCH_PAREN );
    }
}
