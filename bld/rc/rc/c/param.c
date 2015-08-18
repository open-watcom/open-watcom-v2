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
* Description:  WRC command line parameter parsing.
*
****************************************************************************/

#include "wio.h"
#include "global.h"
#include "preproc.h"
#include "errors.h"
#include "rcmem.h"
#include "swchar.h"
#include "dbtable.h"
#ifdef __OSI__
 #include "ostype.h"
#endif
#include "leadbyte.h"
#include "clibext.h"
#include "rccore.h"


/* forward declaration */
static bool scanEnvVar( const char *varname, int *nofilenames );

extern void RcAddCPPArg( char * newarg )
/***************************************/
{
    size_t  numargs;    /* number of args in list at end of this function */
    char    **arg;
    char    **cppargs;

    cppargs = CmdLineParms.CPPArgs;

    if( CmdLineParms.CPPArgs == NULL ) {
        /* 3 is 1 for the command, 1 for newarg, 1 for NULL */
        numargs = 3;
        cppargs = RcMemMalloc( numargs * sizeof( char * ) );
        /* cppargs[0] is reserved for the name of the command so set it */
        /* to the null string for now */
        cppargs[0] = "";
    } else {
        arg = CmdLineParms.CPPArgs;
        while( *arg != NULL ) {
            arg++;
        }
        /* + 2 for the NULL arg and the new arg */
        numargs = ( arg - CmdLineParms.CPPArgs ) + 2;
        cppargs = RcMemRealloc( cppargs, numargs * sizeof( char * ) );
    }

    cppargs[numargs - 2] = newarg;
    cppargs[numargs - 1] = NULL;

    CmdLineParms.CPPArgs = cppargs;
} /* RcAddCPPArg */


/*
 * SetMBRange - set the CharSetLen array up to recognize multi-byte character
 *              sequences
 */
void SetMBRange( unsigned from, unsigned to, char data ) {
/********************************************************/
    unsigned    i;

    for( i = from; i <= to; i++ ) {
        CharSetLen[i] = data;
    }
}

static void SetMBChars( const char *bytes ) {
/*******************************************/
    unsigned    i;

    for( i = 0; i < 256; i++ ) {
        CharSetLen[i] = bytes[i];
    }
}

static bool scanString( char *buf, const char *str, unsigned len )
/*****************************************************************/
{
    bool        have_quote;
    char        c;

    have_quote = false;
    while( isspace( *str ) )
        ++str;
    while( (c = *str++) != '\0' && len > 0 ) {
        if( c == '\"' ) {
            have_quote = !have_quote;
        } else {
            *buf++ = c;
            len--;
        }
    }
    *buf = '\0';
    return( have_quote );
}

static bool ScanMultiOptArg( const char * arg )
/*********************************************/
{
    bool    contok;

    contok = true;

    for( ; *arg != '\0' && contok; arg++ ) {
        switch( tolower( *arg ) ) {
        case 'e':
            CmdLineParms.GlobalMemEMS = true;
            break;
        case 'k':
            CmdLineParms.SegmentSorting = SEG_SORT_NONE;
            break;
        case 'l':
            CmdLineParms.EMSDirect = true;
            break;
        case 'm':
            CmdLineParms.EMSInstance = true;
            break;
#if 0
        case 'n':
            CmdLineParms.NoProtectCC = true;
            break;
#endif
        case 'p':
            CmdLineParms.PrivateDLL = true;
            break;
        case 'q':
            CmdLineParms.Quiet = true;
            break;
        case 'r':
            CmdLineParms.Pass1Only = true;
            break;
        case 't':
            CmdLineParms.ProtModeOnly = true;
            break;
        case '?':
            CmdLineParms.PrintHelp = true;
            contok = false;
            break;
        default:
            RcError( ERR_UNKNOWN_OPTION, *arg );
            contok = false;
            break;
        }
    }
    return( contok );
} /* ScanMultiOptArg */

static bool ScanOptionsArg( const char * arg )
/********************************************/
{
    bool        contok;
    ExtraRes    *resfile;
    FRStrings   *frStrings;
    char        *temp=NULL;
    char        *p;
    char        *delims = ",";
    size_t      findlen = 0;
//    size_t      replen = 0;

    contok = true;

    switch( tolower( *arg ) ) {
    case '\0':
        RcError( ERR_NO_OPT_SPECIFIED );
        contok = false;
        break;
    case 'a':
        arg++;
        if( tolower( *arg ) == 'd' ) {
            CmdLineParms.GenAutoDep = true;
        } else if( tolower( *arg ) == 'p' ) {
            arg++;
            if( *arg == '=' )
                arg++;
            if( scanString( CmdLineParms.PrependString, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            CmdLineParms.Prepend = true;
            break;
        } else {
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = false;
        }
        break;
    case '3':
        arg++;
        switch( tolower( *arg ) ) {
        case '0':
            CmdLineParms.VersionStamp30 = true;
            break;
        case '1':
            CmdLineParms.VersionStamp30 = false;
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = false;
            break;
        }
        break;
    case 'b':
        arg++;
        if( tolower( *arg ) == 't' ) {
            arg++;
            if( *arg == '=' )
                arg++;
            if( stricmp( arg, "windows" ) == 0 || stricmp( arg, "win" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
            } else if( stricmp( arg, "nt" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
            } else if( stricmp( arg, "os2" ) == 0 ) {
                CmdLineParms.TargetOS = RC_TARGET_OS_OS2;
            } else {
                RcError( ERR_UNKNOWN_TARGET_OS, arg );
                contok = false;
            }
        } else {
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = false;
        }
        break;
    case 'c':
        arg++;
        if( *arg == '=' )
            arg++;
        if( scanString( CmdLineParms.CodePageFile, arg, _MAX_PATH  ) ) {
            RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
        }
        break;
    case 'd':
        /* temporary until preprocessing done inline */
        /* -1 to get the '-' or '/' as well */
        /* the cast is so the argument won't be const */
        RcAddCPPArg( (char *)arg - 1 );
        break;
    case 'f':
        arg++;
        switch( tolower( *arg ) ) {
        case 'o':
            arg++;
            if( *arg == '=' )
                arg++;
            if( scanString( CmdLineParms.OutResFileName, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            break;
        case 'r':
            arg++;
            if( *arg == '=' )
                arg++;
            resfile = RcMemMalloc( sizeof( ExtraRes ) + _MAX_PATH );
            if( scanString( resfile->name, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
                RcMemFree( resfile );
            } else {
                resfile->next = CmdLineParms.ExtraResFiles;
                CmdLineParms.ExtraResFiles = resfile;
            }
            break;
        case 'e':
            arg++;
            if( *arg == '=' )
                arg++;
            if( scanString( CmdLineParms.OutExeFileName, arg, _MAX_PATH  ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = false;
            break;
        }
        break;
    case 'g':
        arg++;
        if( *arg == '=' )
            arg++;
        temp = RcMemMalloc( strlen( arg ) + 1 );
        if( scanString( temp, arg, _MAX_PATH  ) ) {
            RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
        }
        frStrings = RcMemMalloc( sizeof( FRStrings ) + strlen( arg ) + 2 );
        p = strtok( temp, delims );
        if( p != NULL ) {
            findlen = strlen( p );
            strcpy( frStrings->buf, p );
            frStrings->findString = frStrings->buf;
        } else {
            RcError( ERR_SYNTAX_STR, "/g=" );
            contok = false;
        }
        p = strtok( NULL, delims );
        if( p != NULL ) {
//            replen = strlen( p );
            strcpy( &frStrings->buf[findlen+1], p );
            frStrings->replaceString = &frStrings->buf[findlen+1];
        } else {
            RcError( ERR_SYNTAX_STR, frStrings->findString  );
            contok = false;
        }
        frStrings->next = CmdLineParms.FindReplaceStrings;
        CmdLineParms.FindReplaceStrings = frStrings;
        CmdLineParms.FindAndReplace = true;
        RcMemFree( temp );
        break;
    case 'i':
        arg++;
        if( *arg == '=' )
            arg++;
        temp = RcMemMalloc( _MAX_PATH );
        if( scanString( temp, arg, _MAX_PATH  ) ) {
            RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
        }
        PP_AddIncludePath( temp );
        RcMemFree( temp );
        break;
    case 'o':
        CmdLineParms.PreprocessOnly = true;
        break;
    case 's':
        arg++;
        switch( tolower( *arg ) ) {
        case '0':
            CmdLineParms.SegmentSorting = SEG_SORT_NONE;
            break;
        case '1':
            CmdLineParms.SegmentSorting = SEG_SORT_PRELOAD_ONLY;
            break;
        case '2':
            CmdLineParms.SegmentSorting = SEG_SORT_MANY;
            break;
        }
        break;
    case 'w':
        arg++;
        if( *arg == 'r' ) {
//          CmdLineParms.WritableRes = true;
        }
        break;
#if defined( YYDEBUG ) || defined( SCANDEBUG )
    case 'v':
        arg++;
        switch( tolower( *arg ) ) {
    #if defined( YYDEBUG )
        case '1':
            CmdLineParms.DebugParser = 1;
            break;
    #endif
    #if defined( YYDEBUG ) && defined( SCANDEBUG )
        case '2':
            CmdLineParms.DebugParser = 1;
            CmdLineParms.DebugScanner = 1;
            break;
        case '3':
            CmdLineParms.DebugScanner = 1;
            break;
    #endif
    #if defined( SCANDEBUG )
        default:
            CmdLineParms.DebugScanner = 1;
            break;
    #endif
        }
        break;
#endif
    case 'x':
        arg++;
        if( tolower( *arg ) == 'b' ) {
            CmdLineParms.NoTargetDefine = true;
        } else if( tolower( *arg ) == 'c' ) {
            CmdLineParms.IgnoreCWD = true;
        } else {
            CmdLineParms.IgnoreINCLUDE = true;
        }
        break;
    case 'z':
        arg++;
        switch( tolower( *arg ) ) {
        case 'm':
            CmdLineParms.MSResFormat = true;
            break;
        case 'n':
            CmdLineParms.NoPreprocess = true;
            break;
        /*
            Lead-byte and trail-byte ranges for code pages used in Far East
            editions of Windows 95.

                        Character           Code    Lead-Byte   Trail-Byte
        Language        Set Name            Page    Ranges      Ranges

        Chinese
        (Simplified)    GB 2312-80          CP 936  0xA1-0xFE   0xA1-0xFE

        Chinese
        (Traditional)   Big-5               CP 950  0x81-0xFE   0x40-0x7E
                                                                0xA1-0xFE

        Japanese        Shift-JIS (Japan
                        Industry Standard)  CP 932  0x81-0x9F   0x40-0xFC
                                                    0xE0-0xFC   (except 0x7F)

        Korean
        (Wansung)       KS C-5601-1987      CP 949  0x81-0xFE   0x41-0x5A
                                                                0x61-0x7A
                                                                0x81-0xFE

        Korean
        (Johab)         KS C-5601-1992      CP 1361 0x84-0xD3   0x41-0x7E
                                                    0xD8        0x81-0xFE
                                                    0xD9-0xDE   (Government
                                                    0xE0-0xF9   standard:
                                                                0x31-0x7E
                                                                0x41-0xFE)
        */
        case 'k':
            arg++;
            switch( tolower( *arg ) ) {
            case '1':
                SetMBRange( 0x81, 0xfe, 1 );
                CmdLineParms.MBCharSupport = DB_TRADITIONAL_CHINESE;
                break;
            case '2':
                SetMBRange( 0x81, 0xfe, 1 );
                CmdLineParms.MBCharSupport = DB_WANSUNG_KOREAN;
                break;
            case '3':
                SetMBRange( 0xA1, 0xfe, 1 );
                CmdLineParms.MBCharSupport = DB_SIMPLIFIED_CHINESE;
                break;
            case '0':
            case ' ':
            case '\0':
                SetMBRange( 0x81, 0x9f, 1 );
                SetMBRange( 0xe0, 0xfc, 1 );
                CmdLineParms.MBCharSupport = DB_KANJI;
                break;
            case 'u':
                if( arg[1] == '8' ) {
                    arg++;
                    SetMBRange( 0xc0, 0xdf, 1 );
                    SetMBRange( 0xe0, 0xef, 2 );
                    SetMBRange( 0xf0, 0xf7, 3 );
                    SetMBRange( 0xf8, 0xfb, 4 );
                    SetMBRange( 0xfc, 0xfd, 5 );
                    CmdLineParms.MBCharSupport = MB_UTF8;
                    break;
                }
                // fall down
            default:
                RcError( ERR_UNKNOWN_MULT_OPTION, arg - 2 );
                contok = false;
                break;
            }
            break;
        default:
            RcError( ERR_UNKNOWN_MULT_OPTION, arg - 1 );
            contok = false;
            break;
        }
        break;
    default:            /* option that could have others with it */
        contok = ScanMultiOptArg( arg ) && contok;
        break;
    }
    return( contok );
} /* ScanOptionsArg */

static void MakeFileName( char * infilename, char * outfilename, char * ext )
/***************************************************************************/
{
    char    name[_MAX_FNAME];
    char    dir[_MAX_DIR];
    char    drive[_MAX_DRIVE];

    _splitpath( infilename, drive, dir, name, NULL );
    _makepath( outfilename, drive, dir, name, ext );
} /* MakeFileName */

static void CheckExtension( char * filename, char * defext )
/**********************************************************/
{
    char    name[_MAX_FNAME];
    char    drive[_MAX_DRIVE];
    char    dir[_MAX_DIR];
    char    ext[_MAX_EXT];

    _splitpath( filename, drive, dir, name, ext );
    if( *ext == '\0' ) {
        _makepath( filename, drive, dir, name, defext );
    }
} /* CheckExtension */

/* extensions for Windows executables */
/* The strings are in the format of the _splitpath function */
static const char *ExeExt[] =   {
    ".EXE",
    ".DLL",
    ".DRV",
    ".SCR",                     /* Windows 3.1 screen saver apps */
    NULL
};

static void CheckPass2Only( void )
/********************************/
{
    char    ext[_MAX_EXT];
    char    **check_ext;

    _splitpath( CmdLineParms.InFileName, NULL, NULL, NULL, ext );
    if( stricmp( ext, ".RES" ) == 0 ) {
        CmdLineParms.Pass2Only = true;
    } else {
        /* if the extension is in the ExeExt list then we want pass2 only */
        /* and there is no resource file to merge */
        for( check_ext = (char **)ExeExt; *check_ext != NULL; check_ext++ ) {
            if( stricmp( ext, *check_ext ) == 0 ) {
                CmdLineParms.Pass2Only = true;
                CmdLineParms.NoResFile = true;
            }
        }
    }
} /* CheckPass2Only */

static void CheckParms( void )
/****************************/
{
    char        *defext;

    CheckExtension( CmdLineParms.InFileName, "rc" );
    CheckPass2Only();

    /* was an EXE file name given */
    if( CmdLineParms.InExeFileName[0] == '\0' ) {
        if( CmdLineParms.NoResFile ) {
            strncpy( CmdLineParms.InExeFileName, CmdLineParms.InFileName, _MAX_PATH );
        } else {
            MakeFileName( CmdLineParms.InFileName,
                             CmdLineParms.InExeFileName, "exe" );
        }
    } else {
        CheckExtension( CmdLineParms.InExeFileName, "exe" );
    }

    /* was an output RES file name given */
    if( CmdLineParms.PreprocessOnly ) {
        defext = "lst";
    } else {
        defext = "res";
    }
    if( CmdLineParms.OutResFileName[0] == '\0' ) {
        MakeFileName( CmdLineParms.InFileName,
                      CmdLineParms.OutResFileName, defext );
    } else {
        CheckExtension( CmdLineParms.OutResFileName, defext );
    }

    /* was an output EXE file name given */
    if( CmdLineParms.OutExeFileName[0] == '\0' ) {
        strncpy( CmdLineParms.OutExeFileName, CmdLineParms.InExeFileName, _MAX_PATH );
    } else {
        CheckExtension( CmdLineParms.OutExeFileName, "exe" );
    }

    /* check for the existance of the input files */
    if( !( CmdLineParms.Pass2Only && CmdLineParms.NoResFile ) ) {
        if( access( CmdLineParms.InFileName, F_OK ) != 0 ) {
            RcFatalError( ERR_CANT_FIND_FILE, CmdLineParms.InFileName );
        }
    }
    if( !CmdLineParms.Pass1Only && !CmdLineParms.PreprocessOnly ) {
        if( access( CmdLineParms.InExeFileName, F_OK ) != 0 ) {
            RcFatalError( ERR_CANT_FIND_FILE, CmdLineParms.InExeFileName );
        }
    }

    if( CmdLineParms.GenAutoDep && CmdLineParms.MSResFormat ) {
        RcFatalError( ERR_OPT_NOT_VALID_TOGETHER, "-ad", "-zm" );
    }
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        switch( CmdLineParms.MBCharSupport ) {
        case DB_SIMPLIFIED_CHINESE:
            strcpy( CmdLineParms.CodePageFile, "936.uni" );
            break;
        case DB_TRADITIONAL_CHINESE:
            strcpy( CmdLineParms.CodePageFile, "950.uni" );
            break;
        case DB_KANJI:
            strcpy( CmdLineParms.CodePageFile, "kanji.uni" );
            break;
        case DB_WANSUNG_KOREAN:
            strcpy( CmdLineParms.CodePageFile, "949.uni" );
            break;
        }
    }
    if( CmdLineParms.PreprocessOnly && CmdLineParms.NoPreprocess ) {
        RcFatalError( ERR_OPT_NOT_VALID_TOGETHER, "-o", "-zn" );
    }

} /* CheckParms */


static void defaultParms( void )
/******************************/
{
#ifdef SCANDEBUG
    CmdLineParms.DebugScanner = false;
#endif
#ifdef YYDEBUG
    CmdLineParms.DebugParser = false;
#endif
    CmdLineParms.MBCharSupport = MB_NONE;
    CmdLineParms.PrintHelp = false;
    CmdLineParms.Quiet = false;
    CmdLineParms.Pass1Only = false;
    CmdLineParms.Pass2Only = false;
    CmdLineParms.NoResFile = false;
    CmdLineParms.IgnoreCWD = IgnoreCWD;
    CmdLineParms.IgnoreINCLUDE = IgnoreINCLUDE;
    CmdLineParms.NoTargetDefine = false;
    CmdLineParms.PrivateDLL = false;
    CmdLineParms.GlobalMemEMS = false;
    CmdLineParms.EMSInstance = false;
    CmdLineParms.EMSDirect = false;
    CmdLineParms.ProtModeOnly = false;
    CmdLineParms.SegmentSorting = SEG_SORT_MANY;
    CmdLineParms.FindAndReplace = false;
    CmdLineParms.Prepend = false;
//    CmdLineParms.WritableRes = false;
    CmdLineParms.InFileName[0] = '\0';
    CmdLineParms.InExeFileName[0] = '\0';
    CmdLineParms.OutResFileName[0] = '\0';
    CmdLineParms.OutExeFileName[0] = '\0';
    CmdLineParms.CodePageFile[0] = '\0';
    CmdLineParms.PrependString[0] = '\0';
    CmdLineParms.CPPArgs = NULL;
    CmdLineParms.VersionStamp30 = false;
    CmdLineParms.NoProtectCC = false;
    CmdLineParms.NoPreprocess = false;
    CmdLineParms.GenAutoDep = false;
    CmdLineParms.PreprocessOnly = false;
    CmdLineParms.ExtraResFiles = NULL;
    CmdLineParms.FindReplaceStrings = NULL;
#ifdef __OSI__
    if( __OS == OS_NT ) {
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
    } else {
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
    }
#elif defined( __NT__ )
    CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
#elif defined( __OS2__ )
    CmdLineParms.TargetOS = RC_TARGET_OS_OS2;
#else
    CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
#endif
} /* defaultParms */


static int getcharUTF8( const char **p, uint_32 *c )
{
    int     len;
    int     i;
    uint_32 value;

    value = *c;
    len = CharSetLen[value];
    if( len == 1 ) {
        value &= 0x1F;
    } else if( len == 2 ) {
        value &= 0x0F;
    } else if( len == 3 ) {
        value &= 0x07;
    } else if( len == 4 ) {
        value &= 0x03;
    } else if( len == 5 ) {
        value &= 0x01;
    } else {
        return( 0 );
    }
    for( i = 0; i < len; ++i ) {
        value = ( value << 6 ) + ( **p & 0x3F );
        (*p)++;
    }
    *c = value;
    return( len );
}


static int UTF8StringToUnicode( int len, const char *str, char *buf )
/*******************************************************************/
{
    int             ret;
    int             outlen;
    uint_32         unicode;
    int             i;

    ret = 0;
    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len;
        }
        for( i = 0; i < len; i++ ) {
            unicode = (unsigned char)*str++;
            i += getcharUTF8( &str, &unicode );
            if( ret < outlen ) {
                *buf++ = unicode;
                *buf++ = unicode >> 8;
                ret++;
            }
        }
    }
    return( ret * 2 );
}

static void getCodePage( void ) {
/********************************/

    RcStatus            ret;
    char                path[_MAX_PATH];

    if( CmdLineParms.MBCharSupport == MB_UTF8 ) {
        ConvToUnicode = UTF8StringToUnicode;
    } else if( CmdLineParms.CodePageFile[0] != '\0' ) {
        ret = OpenTable( CmdLineParms.CodePageFile, path );
        switch( ret ) {
        case RS_FILE_NOT_FOUND:
            RcFatalError( ERR_CANT_FIND_CHAR_FILE, CmdLineParms.CodePageFile );
            break;
        case RS_READ_ERROR:
            RcFatalError( ERR_READING_CHAR_FILE, path, strerror( errno ) );
            break;
        case RS_READ_INCMPLT:
        case RS_BAD_FILE_FMT:
            RcFatalError( ERR_BAD_CHAR_FILE, path );
            break;
        case RS_WRONG_VER:
            RcFatalError( ERR_WRONG_CHAR_FILE_VER, path );
            break;
        case RS_OPEN_ERROR:
            RcFatalError( ERR_CANT_OPEN_CHAR_FILE, path, strerror( errno ) );
            break;
        default:
            SetMBChars( GetLeadBytes() );
            break;
        }
#ifdef __NT__
    } else {
        if( MB_NONE == CmdLineParms.MBCharSupport ) {
            SetNativeLeadBytes();
            ConvToUnicode = NativeDBStringToUnicode;
        }
#endif
    }
}

static bool doScanParams( int argc, char *argv[], int *nofilenames )
/*******************************************************************/
{
    const char *arg;
    int     switchchar;
    bool    contok;         /* continue with main execution */
    int     currarg;

    contok = true;
    switchchar = _dos_switch_char();
    for( currarg = 1; currarg < argc && contok; currarg++ ) {
        arg = argv[currarg];
        if( *arg == switchchar || *arg == '-' ) {
            contok = ScanOptionsArg( arg + 1 ) && contok;
        } else if( *arg == '@' ) {
            contok = scanEnvVar( arg + 1, nofilenames ) && contok;
        } else if( *arg == '?' ) {
            CmdLineParms.PrintHelp = true;
            contok = false;
        } else if( *nofilenames == 0 ) {
            if( scanString( CmdLineParms.InFileName, arg, _MAX_PATH ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            (*nofilenames)++;
        } else if( *nofilenames == 1 ) {
            if( scanString( CmdLineParms.InExeFileName, arg, _MAX_PATH ) ) {
                RcError( ERR_UNMATCHED_QUOTE_ON_CMD_LINE );
            }
            (*nofilenames)++;
        } else {
            RcError( ERR_TOO_MANY_ARGS, arg );
            contok = false;
        }
    }
    return( contok );
}

extern int ParseEnvVar( const char *env, char **argv, char *buf )
/***************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *env.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         switchchar;
    int         argc;
    char        *bufend;
    bool        got_quote;

    switchchar = _dos_switch_char();
    bufend = buf;
    argc = 1;
    if( argv != NULL )
        argv[0] = ""; //fill in the program name
    for( ;; ) {
        got_quote = false;
        while( isspace( *env ) && *env != '\0' ) env++;
        start = env;
        if( buf != NULL ) {
            argv[argc] = bufend;
        }
        if( *env == switchchar || *env == '-' ) {
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env ++;
        }
        while( ( got_quote || !isspace( *env ) ) && *env != '\0' ) {
            if( *env == '\"' ) {
                got_quote = !got_quote;
            }
            if( buf != NULL ) {
                *bufend = *env;
                bufend++;
            }
            env++;
        }
        if( start != env ) {
            argc++;
            if( buf != NULL ) {
                *bufend = '\0';
                bufend++;
            }
        }
        if( *env == '\0' ) {
            break;
        }
    }
    return( argc );
}

static bool scanEnvVar( const char *varname, int *nofilenames )
/*************************************************************/
{
    /*
     * Pass nofilenames and analysis of getenv(varname) into argc and argv
     * to doScanParams. Return view on usability of data. (true is usable.)
     *
     * Recursion is supported but circularity is rejected.
     *
     * The analysis is fairly similar to that done in clib(initargv@_getargv).
     * It is possible to use that function but it is not generally exported and
     * ParseEnvVar() above is called from other places.
     */
    typedef struct EnvVarInfo {
        struct EnvVarInfo       *next;
        char                    *varname;
        char                    **argv; /* points into buf */
        char                    buf[1]; /* dynamic array */
    } EnvVarInfo;

    int                 argc;
    EnvVarInfo          *info;
    static EnvVarInfo   *stack = 0; // Needed to detect recursion.
    size_t              argvsize;
    size_t              argbufsize;
    char                *env;
    size_t              varlen;     // size to hold varname copy.
    bool                result;     // doScanParams Result.

    env = RcGetEnv( varname );
    if( env == NULL ) {
        RcWarning( ERR_ENV_VAR_NOT_FOUND, varname );
        return( true );
    }
    // This used to cause stack overflow: set foo=@foo && wrc @foo.
    for( info = stack; info != NULL; info = info->next ) {
#if !defined( __UNIX__ )
        if( stricmp( varname, info->varname ) == 0 ) // Case-insensitive
#else
        if( strcmp( varname, info->varname ) == 0 )  // Case-sensitive
#endif
            RcFatalError( ERR_RCVARIABLE_RECURSIVE, varname );
    }
    argc = ParseEnvVar( env, NULL, NULL );  // count parameters.
    argbufsize = strlen( env ) + 1 + argc;  // inter-parameter spaces map to 0
    argvsize = ( argc + 1 ) * sizeof( char * ); // sizeof argv[argc+1]
    varlen = strlen( varname ) + 1;         // Copy taken to detect recursion.
    info = RcMemMalloc( sizeof( *info ) + argbufsize + argvsize + varlen );
    info->next = stack;
    stack = info;                           // push info on stack
    info->argv = (char **)info->buf;
    ParseEnvVar( env, info->argv, info->buf + argvsize );
    info->varname = info->buf + argvsize + argbufsize;
    strcpy( info->varname, varname );
    info->argv[argc] = NULL;    //there must be a NULL element on the end
                                // of the list
    result = doScanParams( argc, info->argv, nofilenames );
    stack = info->next;                     // pop stack
    RcMemFree( info );
    return( result );
}

bool ScanParams( int argc, char * argv[] )
/*****************************************/
{
    int     nofilenames;    /* number of filename parms read so far */
    bool    contok;         /* continue with main execution */

    nofilenames = 0;
    defaultParms();
    contok = doScanParams( argc, argv, &nofilenames );
    if( argc < 2 ) {                                    /* 26-mar-94 */
        CmdLineParms.PrintHelp = true;
        contok = false;
    }
    if( contok ) {
        if( nofilenames == 0 ) {
            RcError( ERR_FILENAME_NEEDED );
            contok = false;
        } else {
            CheckParms();
            getCodePage();
        }
    }
    return( contok );
} /* ScanParams */

extern void ScanParamShutdown( void )
/***********************************/
{
    ExtraRes            *tmpres;
    FRStrings           *strings;

    if( CmdLineParms.CPPArgs != NULL ) {
        RcMemFree( CmdLineParms.CPPArgs );
    }
    PP_IncludePathFini();
    while( CmdLineParms.ExtraResFiles != NULL ) {
        tmpres = CmdLineParms.ExtraResFiles;
        CmdLineParms.ExtraResFiles = CmdLineParms.ExtraResFiles->next;
        RcMemFree( tmpres );
    }
    while( CmdLineParms.FindReplaceStrings != NULL ) {
        strings = CmdLineParms.FindReplaceStrings;
        CmdLineParms.FindReplaceStrings = CmdLineParms.FindReplaceStrings->next;
        RcMemFree( strings );
    }
} /* ScanParamShutdown */

extern char *FindAndReplace( char *stringFromFile, FRStrings *frStrings )
/***********************************************************************/
{
    char                *replacedString = NULL;
    char                *foundString;
    size_t              lenOfStringFromFile;
    size_t              lenOfFindString;
    size_t              lenOfReplaceString;
    size_t              diffInLen;
    size_t              newMemSize;
    size_t              i, j, k;
    int                 noOfInstances; //this is the number of instances
                                       //of the find string in the string
                                       //from the file

    while( frStrings != NULL ) {
        i = 0;
        j = 0;
        k = 0;
        noOfInstances = 0;
        newMemSize = 0;
        foundString = NULL;
        replacedString =  NULL;
        lenOfFindString = strlen( frStrings->findString );
        lenOfReplaceString = strlen( frStrings->replaceString );
        lenOfStringFromFile = strlen( stringFromFile );
        diffInLen = lenOfReplaceString - lenOfFindString; //used for reallocation
        if( strstr( stringFromFile, frStrings->findString ) != NULL ) {
            //checking if a replacement is to be done, then allocating memory
            replacedString = RcMemMalloc( lenOfStringFromFile+1 );
            for( k=0; k < lenOfStringFromFile; k++ ) {
                replacedString[k] = '\0';
            }
            while( i <= lenOfStringFromFile ) {
                foundString = strstr( stringFromFile+i, frStrings->findString );
                if( foundString != NULL ) {
                    while( foundString != &stringFromFile[i] ) {
                    //while the ptr is not where the replacment string is, copy.
                        replacedString[j] = stringFromFile[i];
                        i++;
                        j++;
                    }//end of while
                    if( diffInLen > 0 ) {
                        //allocating more memory if the string to replace is
                        //bigger than the string to find
                        newMemSize = lenOfStringFromFile + 1 + diffInLen * ( noOfInstances + 1 );
                        replacedString = RcMemRealloc( replacedString, newMemSize );
                    }
                    strcpy( &replacedString[j], frStrings->replaceString );
                    j = j + lenOfReplaceString;
                    i = i + lenOfFindString-1;
                    noOfInstances++;
                } else {
                    strcpy( &replacedString[j], &stringFromFile[i] );
                    break;
                }//end of if-else
                i++;
            }//end of while
        }
        if( replacedString != NULL && frStrings->next != NULL ) {
            stringFromFile = RcMemRealloc( stringFromFile, strlen( replacedString ) + 1 );
            strcpy( stringFromFile, replacedString );
            RcMemFree( replacedString );
            replacedString = NULL;
        }
        frStrings =  frStrings->next;
    }

    if( replacedString != NULL ) {
        RcMemFree( stringFromFile );
        return( replacedString );
    } else {
        RcMemFree( replacedString );
        return( stringFromFile );
    }
}
