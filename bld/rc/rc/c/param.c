/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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

#include <errno.h>
#include "wio.h"
#ifdef __NT__
#include "windows.h"
#endif
#include "global.h"
#include "errprt.h"
#include "idedll.h"
#include "preproc.h"
#include "rcldstr.h"
#include "rcerrors.h"
#include "rcmem.h"
#include "swchar.h"
#include "dbtable.h"
#include "unitable.h"
#include "rccore.h"
#include "pathgrp2.h"
#include "param.h"
#include "banner.h"
#include "usage.h"

#include "clibext.h"


#ifdef DEVBUILD
    #define __location " (" __FILE__ "," __xstr(__LINE__) ")"
    #define DbgNever()  (RcFatalError( ERR_TEXT_FROM_CPP, "should never execute this" __location ))
#else
    #define DbgNever()
#endif

static int          nofilenames = 0;
static const char   *switch_start = "";
static bool         banner_printed = false;

/*
 * extensions for executables supported by OW resource compiler
 * The strings are in the format of the _splitpath function
 */
static const char *ExeExt[] =   {
    "exe",
    "dll",
    "drv",
    "scr",          /* Windows 3.1 screen saver apps */
    NULL
};

static void ConsoleMessage( const char *str, ... )
{
    OutPutInfo          errinfo;
    va_list             args;
    char                *parm;

    va_start( args, str );
    parm = va_arg( args, char * );
    InitOutPutInfo( &errinfo );
    errinfo.severity = SEV_BANNER;
    RcMsgFprintf( &errinfo, str, parm );
    va_end( args );
}

static void PrintBanner( void )
/*****************************/
{
    if( !banner_printed ) {
        if( !CmdLineParms.Quiet ) {
            ConsoleMessage(
                banner1t( "Windows and OS/2 Resource Compiler" ) "\n"
                banner1v( _WRC_VERSION_ ) "\n"
                banner2 "\n"
                banner2a( 1993 ) "\n"
                banner3 "\n"
                banner3a "\n"
            );

        }
        banner_printed = true;
    }
}

static void PrintUsage( void )
/****************************/
{
    int         index;
    char        buf[256];

    PrintBanner();
    if( CmdLineParms.ConsoleTTY
      && !CmdLineParms.Quiet ) {
        ConsoleMessage( "\n" );
    }
    for( index = MSG_USAGE_BASE; index < MSG_USAGE_BASE + MSG_USAGE_COUNT; index++ ) {
        GetRcMsg( index, buf, sizeof( buf ) );
        ConsoleMessage( "%s\n", buf );
    }
}

static void BadCmdLine( int error_code )
/***************************************
 * SIGNAL CMD-LINE ERROR
 */
{
    char        buffer[128];
    size_t      len;

    CmdScanChar();
    while( !CmdScanSwEnd() ) {
        CmdScanChar();
    }
    len = CmdScanAddr() - switch_start;
    if( len > sizeof( buffer ) - 1 )
        len = sizeof( buffer ) - 1;
    strncpy( buffer, switch_start, len );
    buffer[len] = '\0';
    RcError( error_code, buffer );
}

// BAD CHAR DETECTED
void BadCmdLineChar( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}
// BAD ID DETECTED
void BadCmdLineId( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}
// BAD NUMBER DETECTED
void BadCmdLineNumber( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}
// BAD PATH DETECTED
void BadCmdLinePath( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}
// BAD FILE DETECTED
void BadCmdLineFile( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}
// BAD TEXT DETECTED
void BadCmdLineOption( void )
{
    BadCmdLine( ERR_UNKNOWN_MULT_OPTION );
}

static void SetMBRange( unsigned from, unsigned to, char data )
/**************************************************************
 * set the CharSetLen array up to recognize multi-byte character
 * sequences
 */
{
    unsigned    i;

    for( i = from; i <= to; i++ ) {
        CharSetLen[i] = data;
    }
}

static void SetMBChars( const char *bytes )
/*****************************************/
{
    unsigned    i;

    for( i = 0; i < 256; i++ ) {
        CharSetLen[i] = bytes[i];
    }
}

#ifdef __NT__

static void SetNativeLeadBytes( void )
/************************************/
{
    CPINFO      info;
    unsigned    i;

    GetCPInfo( CP_ACP, &info );
    for( i = 0; info.LeadByte[i] != 0 && info.LeadByte[i + 1] != 0; i += 2 ) {
        SetMBRange( info.LeadByte[i], info.LeadByte[i + 1], 1 );
    }
}

static size_t NativeDBStringToUnicode( size_t len, const char *str, char *buf )
/*****************************************************************************/
{
    size_t      ret;
    size_t      outlen;

    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len * 2;
        }
        ret = (unsigned)MultiByteToWideChar( CP_ACP, 0, str, (int)len, (LPWSTR)buf, (int)outlen );
    } else {
        ret = 0;
    }
    return( ret * 2 );
}

#endif

static void LoadCodePageFile( const char *cpfile )
{
    RcStatus            ret;
    char                path[_MAX_PATH];

    ret = LoadCharTable( cpfile, path );
    switch( ret ) {
    case RS_FILE_NOT_FOUND:
        RcFatalError( ERR_CANT_FIND_CHAR_FILE, cpfile );
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
    case RS_OK:
        SetMBChars( GetLeadBytes() );
        ConvToUnicode = DBStringToUnicode;
        break;
    }
}

static void initMBCodePage( void )
/********************************/
{
    /*
     * Lead-byte and trail-byte ranges for code pages used in Far East
     * editions of Windows 95.
     *
     *                 Character           Code    Lead-Byte   Trail-Byte
     * Language        Set Name            Page    Ranges      Ranges
     *
     * Chinese
     * (Simplified)    GB 2312-80          CP 936  0xA1-0xFE   0xA1-0xFE
     *
     * Chinese
     * (Traditional)   Big-5               CP 950  0x81-0xFE   0x40-0x7E
     *                                                         0xA1-0xFE
     *
     * Japanese        Shift-JIS (Japan
     *                 Industry Standard)  CP 932  0x81-0x9F   0x40-0xFC
     *                                             0xE0-0xFC   (except 0x7F)
     *
     * Korean
     * (Wansung)       KS C-5601-1987      CP 949  0x81-0xFE   0x41-0x5A
     *                                                         0x61-0x7A
     *                                                         0x81-0xFE
     *
     * Korean
     * (Johab)         KS C-5601-1992      CP 1361 0x84-0xD3   0x41-0x7E
     *                                             0xD8        0x81-0xFE
     *                                             0xD9-0xDE   (Government
     *                                             0xE0-0xF9   standard:
     *                                                         0x31-0x7E
     *                                                         0x41-0xFE)
     */
    switch( CmdLineParms.MBCharSupport ) {
    case DB_TRADITIONAL_CHINESE:
        SetMBRange( 0x81, 0xfe, 1 );
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 )
            LoadCodePageFile( "950.uni" );
        break;
    case DB_WANSUNG_KOREAN:
        SetMBRange( 0x81, 0xfe, 1 );
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 )
            LoadCodePageFile( "949.uni" );
        break;
    case DB_SIMPLIFIED_CHINESE:
        SetMBRange( 0xA1, 0xfe, 1 );
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 )
            LoadCodePageFile( "936.uni" );
        break;
    case DB_KANJI:
        SetMBRange( 0x81, 0x9f, 1 );
        SetMBRange( 0xe0, 0xfc, 1 );
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 )
            LoadCodePageFile( "kanji.uni" );
        break;
    case MB_UTF8:
    case MB_UTF8_KANJI:
        SetMBRange( 0xc0, 0xdf, 1 );
        SetMBRange( 0xe0, 0xef, 2 );
        SetMBRange( 0xf0, 0xf7, 3 );
        SetMBRange( 0xf8, 0xfb, 4 );
        SetMBRange( 0xfc, 0xfd, 5 );
        SetUTF8toUnicode();
        if( CmdLineParms.MBCharSupport == MB_UTF8_KANJI ) {
            SetUTF8toCP932();
        } else {
            SetUTF8toUTF8();
        }
        break;
    case MB_NONE:
        if( CmdLineParms.CodePageFile != NULL ) {
            LoadCodePageFile( CmdLineParms.CodePageFile );
#ifdef __NT__
        } else {
            SetNativeLeadBytes();
            ConvToUnicode = NativeDBStringToUnicode;
#endif
        }
        break;
    default:
        break;
    }
}

static char *MakeFileName( const char *infilename, const char *ext )
/******************************************************************/
{
    pgroup2     pg;
    size_t      len;
    char        *out;

    len = strlen( infilename ) + 1;
    if( ext == NULL ) {
        out = RcMemAlloc( len );
        strcpy( out, infilename );
    } else {
        _splitpath2( infilename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        out = RcMemAlloc( len + ( 1 + strlen( ext ) - strlen( pg.ext ) ) );
        _makepath( out, pg.drive, pg.dir, pg.fname, ext );
    }
    return( out );
} /* MakeFileName */

static void CheckExtension( char **filename, const char *defext )
/***************************************************************/
{
    pgroup2     pg;
    size_t      len;

    _splitpath2( *filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' ) {
        len = strlen( *filename ) + ( 1 + strlen( defext ) ) + 1;
        RcMemFree( *filename );
        *filename = RcMemAlloc( len );
        _makepath( *filename, pg.drive, pg.dir, pg.fname, defext );
    }
} /* CheckExtension */

static void CheckPass2Only( void )
/********************************/
{
    pgroup2     pg;
    char        **check_ext;

    _splitpath2( CmdLineParms.InFileName, pg.buffer, NULL, NULL, NULL, &pg.ext );
    if( pg.ext[0] == '.'
      && stricmp( pg.ext + 1, "res" ) == 0 ) {
        CmdLineParms.Pass2Only = true;
    } else {
        /*
         * if the extension is in the ExeExt list then we want pass2 only
         * and there is no resource file to merge
         */
        for( check_ext = (char **)ExeExt; *check_ext != NULL; check_ext++ ) {
            if( pg.ext[0] == '.'
              && stricmp( pg.ext + 1, *check_ext ) == 0 ) {
                CmdLineParms.Pass2Only = true;
                CmdLineParms.NoResFile = true;
            }
        }
    }
} /* CheckPass2Only */


static int SetParms( void )
/*************************/
{
    const char  *defext;

    if( nofilenames == 0 ) {
        RcError( ERR_FILENAME_NEEDED );
        return( 1 );
    }
    if( nofilenames > 2 ) {
        RcError( ERR_TOO_MANY_FILENAMES );
    }

    CheckExtension( &CmdLineParms.InFileName, "rc" );
    CheckPass2Only();
    /*
     * was an EXE file name given
     */
    if( CmdLineParms.InExeFileName == NULL ) {
        if( CmdLineParms.NoResFile ) {
            CmdLineParms.InExeFileName = MakeFileName( CmdLineParms.InFileName, NULL );
        } else {
            CmdLineParms.InExeFileName = MakeFileName( CmdLineParms.InFileName, "exe" );
        }
    } else {
        CheckExtension( &CmdLineParms.InExeFileName, "exe" );
    }
    /*
     * was an output RES file name given
     */
    if( CmdLineParms.PreprocessOnly ) {
        defext = "lst";
    } else {
        defext = "res";
    }
    if( CmdLineParms.OutResFileName == NULL ) {
        CmdLineParms.OutResFileName = MakeFileName( CmdLineParms.InFileName, defext );
    } else {
        CheckExtension( &CmdLineParms.OutResFileName, defext );
    }
    /*
     * was an output EXE file name given
     */
    if( CmdLineParms.OutExeFileName == NULL ) {
        CmdLineParms.OutExeFileName = MakeFileName( CmdLineParms.InExeFileName, NULL );
    } else {
        CheckExtension( &CmdLineParms.OutExeFileName, "exe" );
    }
    /*
     * check for the existance of the input files
     */
    if( !( CmdLineParms.Pass2Only
      && CmdLineParms.NoResFile ) ) {
        if( access( CmdLineParms.InFileName, F_OK ) != 0 ) {
            RcError( ERR_CANT_FIND_FILE, CmdLineParms.InFileName );
            return( 1 );
        }
    }
    if( !CmdLineParms.Pass1Only
      && !CmdLineParms.PreprocessOnly ) {
        if( access( CmdLineParms.InExeFileName, F_OK ) != 0 ) {
            RcError( ERR_CANT_FIND_FILE, CmdLineParms.InExeFileName );
            return( 1 );
        }
    }

    if( CmdLineParms.GenAutoDep
      && CmdLineParms.MSResFormat ) {
        RcError( ERR_OPT_NOT_VALID_TOGETHER, "-ad", "-zm" );
        return( 1 );
    }
    if( CmdLineParms.PreprocessOnly
      && CmdLineParms.NoPreprocess ) {
        RcError( ERR_OPT_NOT_VALID_TOGETHER, "-o", "-zn" );
        return( 1 );
    }
    return( 0 );

} /* SetParms */

void ScanParamInit( void )
/************************/
{
    memset( &CmdLineParms, 0, sizeof( RCParams ) );

    CmdLineParms.IgnoreCWD = IgnoreCWD;
    CmdLineParms.IgnoreINCLUDE = IgnoreINCLUDE;

} /* ScanParamInit */

void ScanParamFini( void )
/************************/
{
    ExtraRes            *tmpres;
#ifndef NO_REPLACE
    FRStrings           *strings;
#endif

    FreeCvtTable();
    FreeCharTable();

    while( (tmpres = CmdLineParms.ExtraResFiles) != NULL ) {
        CmdLineParms.ExtraResFiles = CmdLineParms.ExtraResFiles->next;
        RcMemFree( tmpres );
    }
    RcMemFree( CmdLineParms.InFileName );
    CmdLineParms.InFileName = NULL;
    RcMemFree( CmdLineParms.InExeFileName );
    CmdLineParms.InExeFileName = NULL;
    RcMemFree( CmdLineParms.OutResFileName );
    CmdLineParms.OutResFileName = NULL;
    RcMemFree( CmdLineParms.OutExeFileName );
    CmdLineParms.OutExeFileName = NULL;
    if( CmdLineParms.CodePageFile != NULL ) {
        RcMemFree( CmdLineParms.CodePageFile );
        CmdLineParms.CodePageFile = NULL;
    }
#ifndef NO_REPLACE
    while( (strings = CmdLineParms.FindReplaceStrings) != NULL ) {
        CmdLineParms.FindReplaceStrings = CmdLineParms.FindReplaceStrings->next;
        RcMemFree( strings );
    }
    if( CmdLineParms.PrependString != NULL ) {
        RcMemFree( CmdLineParms.PrependString );
        CmdLineParms.PrependString = NULL;
    }
#endif

} /* ScanParamFini */

#ifndef NO_REPLACE
char *FindAndReplace( char *stringFromFile, FRStrings *frStrings )
/****************************************************************/
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

    for( ; frStrings != NULL; frStrings = frStrings->next ) {
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
            /*
             * checking if a replacement is to be done, then allocating memory
             */
            replacedString = RcMemAlloc( lenOfStringFromFile+1 );
            for( k = 0; k < lenOfStringFromFile; k++ ) {
                replacedString[k] = '\0';
            }
            for( ; i <= lenOfStringFromFile; ++i ) {
                foundString = strstr( stringFromFile+i, frStrings->findString );
                if( foundString == NULL ) {
                    strcpy( &replacedString[j], &stringFromFile[i] );
                    break;
                }
                while( foundString != &stringFromFile[i] ) {
                    /*
                     * while the ptr is not where the replacment string is, copy.
                     */
                    replacedString[j] = stringFromFile[i];
                    i++;
                    j++;
                }
                if( diffInLen > 0 ) {
                    /*
                     * allocating more memory if the string to replace is
                     * bigger than the string to find
                     */
                    newMemSize = lenOfStringFromFile + 1 + diffInLen * ( noOfInstances + 1 );
                    replacedString = RcMemRealloc( replacedString, newMemSize );
                }
                strcpy( &replacedString[j], frStrings->replaceString );
                j = j + lenOfReplaceString;
                i = i + lenOfFindString-1;
                noOfInstances++;
            }
        }
        if( replacedString != NULL
          && frStrings->next != NULL ) {
            stringFromFile = RcMemRealloc( stringFromFile, strlen( replacedString ) + 1 );
            strcpy( stringFromFile, replacedString );
            RcMemFree( replacedString );
            replacedString = NULL;
        }
    }

    if( replacedString != NULL ) {
        RcMemFree( stringFromFile );
        return( replacedString );
    } else {
        RcMemFree( replacedString );
        return( stringFromFile );
    }
}

void PrependToString( ScanValue *value, char *stringFromFile )
/************************************************************/
{

    int     lenOfPrependString = 0;
    int     lenOfStringFromFile;

    lenOfStringFromFile = value->string.length;
    if( CmdLineParms.Prepend ) {
        if( strcmp( stringFromFile, "" ) != 0 ) {
            lenOfPrependString =  strlen( CmdLineParms.PrependString );
            value->string.string = RcMemAlloc( lenOfStringFromFile
                                   + lenOfPrependString + 1);
            strcpy( value->string.string, CmdLineParms.PrependString );
        } else {
            // in this case the lenOfPrependString is zero, so the
            // strcpy will not fail.
            value->string.string = RcMemAlloc( lenOfStringFromFile + 1 );
        }
        strcpy( &value->string.string[ lenOfPrependString ], stringFromFile );
        value->string.length = lenOfStringFromFile + lenOfPrependString;
    } else {
        value->string.string = RcMemAlloc( ( lenOfStringFromFile+1 ) );
        strcpy( value->string.string, stringFromFile );
        value->string.length = lenOfStringFromFile;
    }
    RcMemFree( stringFromFile );
}
#endif

static char *ReadIndirectFile( char *name )
/*****************************************/
{
    char        *env;
    char        *str;
    FILE        *fp;
    size_t      len;
    char        ch;

    env = NULL;
    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        fseek( fp, 0, SEEK_END );
        len = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        env = RcMemAlloc( len + 1 );
        len = fread( env, 1, len, fp );
        env[len] = '\0';
        fclose( fp );
        // zip through characters changing \r, \n etc into ' '
        for( str = env; *str != '\0'; ++str ) {
            ch = *str;
            if( ch == '\r' || ch == '\n' ) {
                *str = ' ';
            }
#if !defined(__UNIX__)
            if( ch == 0x1A ) {      // if end of file
                *str = '\0';        // - mark end of str
                break;
            }
#endif
        }
    }
    return( env );
}

static bool scanDefine( OPT_STRING **h )
{
    char        *m;
    char        *p;

    if( OPT_GET_FILE( h ) ) {
        m = (*h)->data;
        p = strchr( m, '=' );
        if( p != NULL ) {
            *p = ' ';
        } else {
            *h = RcMemRealloc( *h, sizeof( **h ) + strlen( m ) + 2 );
            strcat( (*h)->data, " 1" );
        }
        return( true );
    }
    return( false );
}

static bool scanTarget( unsigned *p )
{
    const char  *str;
    size_t      len;
    char        buff[16];

    CmdRecogEquals();
    len = CmdScanId( &str );
    if( len > sizeof( buff ) - 1 ) {
        len = sizeof( buff ) - 1;
    }
    strncpy( buff, str, len );
    buff[len] = '\0';
    strupr( buff );
    if( strcmp( buff, "WINDOWS" ) == 0 || strcmp( buff, "WIN" ) == 0 ) {
        *p = RC_TARGET_OS_WIN16;
    } else if( strcmp( buff, "NT" ) == 0 ) {
        *p = RC_TARGET_OS_WIN32;
    } else if( strcmp( buff, "OS2" ) == 0 ) {
        *p = RC_TARGET_OS_OS2;
    } else {
        BadCmdLineId();
        return( false );
    }
    return( true );
}

#ifndef NO_REPLACE
static bool scanSearchReplace( OPT_STRING **h )
{
    return( OPT_GET_OPTION( h ) );
}
#endif

static void reverseList( OPT_STRING **h )
{
    OPT_STRING *s;
    OPT_STRING *p;
    OPT_STRING *n;

    s = *h;
    *h = NULL;
    for( p = s; p != NULL; p = n ) {
        n = p->next;
        p->next = *h;
        *h = p;
    }
}

static void AddInpFileName( const char *infile )
{
    if( infile != NULL && *infile != '\0' ) {
        switch( nofilenames ) {
        case 0:
            CmdLineParms.InFileName = RcMemStrDup( infile );
            break;
        case 1:
            CmdLineParms.InExeFileName = RcMemStrDup( infile );
            break;
        case 2:
            /*
             * set nofilenames to overflow
             */
            break;
        default:
            return;
        }
        nofilenames++;
    }
}

#include "cmdlnprs.gc"

int ProcOptions( OPT_STORAGE *data, const char *str )
/***************************************************/
{
#define MAX_NESTING 32
    const char  *save[MAX_NESTING];
    char        *buffers[MAX_NESTING];
    int         level;
    int         ch;
    OPT_STRING  *fname;
    const char  *penv;
    char        *ptr;

    if( str != NULL ) {
        level = -1;
        CmdScanLineInit( str );
        for( ;; ) {
            CmdScanSkipWhiteSpace();
            ch = CmdScanChar();
            if( ch == '@' ) {
                switch_start = CmdScanAddr() - 1;
                CmdScanSkipWhiteSpace();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    penv = NULL;
                    level++;
                    if( level < MAX_NESTING ) {
                        ptr = NULL;
                        penv = getenv( fname->data );
                        if( penv == NULL ) {
                            ptr = ReadIndirectFile( fname->data );
                            penv = ptr;
                        }
                        if( penv != NULL ) {
                            save[level] = CmdScanLineInit( penv );
                            buffers[level] = ptr;
                        }
                    }
                    if( penv == NULL ) {
                        level--;
                    }
                    OPT_CLEAN_STRING( &fname );
                }
                continue;
            }
            if( ch == '\0' ) {
                if( level < 0 )
                    break;
                RcMemFree( buffers[level] );
                CmdScanLineInit( save[level] );
                level--;
                continue;
            }
            if( CmdScanSwitchChar( ch ) ) {
                switch_start = CmdScanAddr() - 1;
                OPT_PROCESS( data );
            } else {  /* collect file name */
                CmdScanUngetChar();
                switch_start = CmdScanAddr();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    AddInpFileName( fname->data );
                    OPT_CLEAN_STRING( &fname );
                }
            }
        }
    }
    return( 0 );
#undef MAX_NESTING
}

#if 0
static void OptAddString( OPT_STRING **h, char const *s )
{
    OPT_STRING *value;

    value = RcMemAlloc( sizeof( *value ) + strlen( s ) );
    strcpy( value->data, s );
    value->next = *h;
    *h = value;
}
#endif

static char *SetStringOption( char **o, OPT_STRING **h )
/******************************************************/
{
    OPT_STRING *s;
    char *p;

    s = *h;
    p = NULL;
    if( s != NULL ) {
        if( s->data[0] != '\0' ) {
            p = RcMemStrDup( s->data );
        }
        OPT_CLEAN_STRING( h );
    }
    if( o != NULL ) {
        RcMemFree( *o );
        *o = p;
    }
    return( p );
}

int SetOptions( OPT_STORAGE *data, const char *infile, const char *outfile )
/**************************************************************************/
{
    const char  *p;

    if( data->_question ) {
        CmdLineParms.PrintHelp = true;
        PrintUsage();
        return( 1 );
    }
    if( data->q ) {
        CmdLineParms.Quiet = true;
    }

    PrintBanner();

    AddInpFileName( infile );

    if( data->bt ) {
        CmdLineParms.TargetOS = data->bt_value;
    } else {
#if defined( __NT__ )
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN32;
#elif defined( __OS2__ )
        CmdLineParms.TargetOS = RC_TARGET_OS_OS2;
#else
        CmdLineParms.TargetOS = RC_TARGET_OS_WIN16;
#endif
    }

    if( data->ad ) {
        CmdLineParms.GenAutoDep = true;
    }
#ifndef NO_REPLACE
    if( data->ap ) {
        CmdLineParms.Prepend = true;
        SetStringOption( &CmdLineParms.PrependString, &(data->ap_value) );
    }
#endif
    switch( data->win16_ver ) {
    case OPT_ENUM_win16_ver__10:
        CmdLineParms.Win16VerStamp = VERSION_10_STAMP;
        break;
    case OPT_ENUM_win16_ver__20:
        CmdLineParms.Win16VerStamp = VERSION_20_STAMP;
        break;
    case OPT_ENUM_win16_ver__30:
        CmdLineParms.Win16VerStamp = VERSION_30_STAMP;
        break;
    case OPT_ENUM_win16_ver__31:
    case OPT_ENUM_win16_ver_default:
        CmdLineParms.Win16VerStamp = VERSION_31_STAMP;
        break;
    default:
        DbgNever();
        break;
    }
    if( data->c ) {
        SetStringOption( &CmdLineParms.CodePageFile, &(data->c_value) );
    }
    if( data->d ) {
        OPT_STRING *s;

        reverseList( &(data->d_value) );
        for( s = data->d_value; s != NULL; s = s->next ) {
            PP_Define( s->data );
        }
        OPT_CLEAN_STRING( &(data->d_value) );
    }
    if( data->e ) {
        CmdLineParms.GlobalMemEMS = true;
    }
    if( *outfile != '\0'
      && data->r ) {
        CmdLineParms.OutResFileName = RcMemStrDup( outfile );
        if( data->fo ) {
            OPT_CLEAN_STRING( &(data->fo_value) );
        }
    } else if( data->fo ) {
        SetStringOption( &CmdLineParms.OutResFileName, &(data->fo_value) );
    }
    if( data->fr ) {
        OPT_STRING *s;

        reverseList( &(data->fr_value) );
        for( s = data->fr_value; s != NULL; s = s->next ) {
            if( s->data[0] != '\0' ) {
                ExtraRes *resfile;
                size_t len;

                len = strlen( s->data );
                resfile = RcMemAlloc( sizeof( ExtraRes ) + len + 1 );
                strcpy( resfile->name, s->data );
                resfile->next = CmdLineParms.ExtraResFiles;
                CmdLineParms.ExtraResFiles = resfile;
            }
        }
        OPT_CLEAN_STRING( &(data->fr_value) );
    }
    if( *outfile != '\0'
      && !data->r ) {
        CmdLineParms.OutExeFileName = RcMemStrDup( outfile );
        if( data->fe ) {
            OPT_CLEAN_STRING( &(data->fe_value) );
        }
    } else if( data->fe ) {
        SetStringOption( &CmdLineParms.OutExeFileName, &(data->fe_value) );
    }
#ifndef NO_REPLACE
    if( data->g ) {
        OPT_STRING *s;

        reverseList( &(data->g_value) );
        for( s = data->g_value; s != NULL; s = s->next ) {
            frStrings = RcMemAlloc( sizeof( FRStrings ) + strlen( s->data ) + 1 );
            strcpy( frStrings->buf, s->data );
            frStrings->findString = strtok( frStrings->buf, "," );
            if( frStrings->findString == NULL ) {
                PrintBanner();
                RcError( ERR_SYNTAX_STR, "-g=" );
                RcMemFree( frStrings );
                continue;
            }
            frStrings->replaceString = strtok( NULL, "," );
            if( frStrings->replaceString == NULL ) {
                PrintBanner();
                RcError( ERR_SYNTAX_STR, "-g=" );
                RcMemFree( frStrings );
                continue;
            }
            frStrings->next = CmdLineParms.FindReplaceStrings;
            CmdLineParms.FindReplaceStrings = frStrings;
        }
        CmdLineParms.FindAndReplace = true;
        OPT_CLEAN_STRING( &(data->g_value) );
    }
#endif
    if( data->i ) {
        OPT_STRING *s;

        reverseList( &(data->i_value) );
        for( s = data->i_value; s != NULL; s = s->next ) {
            PP_IncludePathAdd( PPINCLUDE_USR, s->data );
        }
        OPT_CLEAN_STRING( &(data->i_value) );
    }
    if( data->l ) {
        CmdLineParms.EMSDirect = true;
    }
    if( data->m ) {
        CmdLineParms.EMSInstance = true;
    }
#if 0
    if( data->n ) {
        CmdLineParms.NoProtectCC = true;
    }
#endif
    if( data->o ) {
        CmdLineParms.PreprocessOnly = true;
    }
    if( data->p ) {
        CmdLineParms.PrivateDLL = true;
    }
    if( data->r ) {
        CmdLineParms.Pass1Only = true;
    }
    switch( data->segm_sort ) {
    case OPT_ENUM_segm_sort_k:
    case OPT_ENUM_segm_sort_s0:
        CmdLineParms.SegmentSorting = SEG_SORT_NONE;
        break;
    case OPT_ENUM_segm_sort_s1:
        CmdLineParms.SegmentSorting = SEG_SORT_PRELOAD_ONLY;
        break;
    case OPT_ENUM_segm_sort_s2:
    case OPT_ENUM_segm_sort_default:
        CmdLineParms.SegmentSorting = SEG_SORT_MANY;
        break;
    default:
        DbgNever();
        break;
    }
    if( data->t ) {
        CmdLineParms.ProtModeOnly = true;
    }
#if defined( YYDEBUG ) || defined( SCANDEBUG )
    if( data->v ) {
  #if defined( SCANDEBUG )
        CmdLineParms.DebugScanner = 1;
  #endif
  #if defined( YYDEBUG )
        CmdLineParms.DebugParser = 1;
  #endif
    }
  #if defined( SCANDEBUG )
    if( data->v1 ) {
        CmdLineParms.DebugScanner = 1;
    }
  #endif
  #if defined( YYDEBUG )
    if( data->v2 ) {
        CmdLineParms.DebugParser = 1;
    }
  #endif
    if( data->v3 ) {
        CmdLineParms.DebugParser = 1;
        CmdLineParms.DebugScanner = 1;
    }
    if( data->v4 ) {
        CmdLineParms.DebugScanner = 1;
    }
#endif
#if 0
    if( data->wr ) {
        CmdLineParms.WritableRes = true;
    }
#endif
    if( data->x ) {
        CmdLineParms.IgnoreINCLUDE = true;
    }
    if( data->xb ) {
        CmdLineParms.NoTargetDefine = true;
    }
    if( data->xc ) {
        CmdLineParms.IgnoreCWD = true;
    }
    if( data->zm ) {
        CmdLineParms.MSResFormat = true;
    }
    if( data->zn ) {
        CmdLineParms.NoPreprocess = true;
    }
    switch( data->charset ) {
    case OPT_ENUM_charset_zk0:
        CmdLineParms.MBCharSupport = DB_KANJI;
        break;
    case OPT_ENUM_charset_zk1:
        CmdLineParms.MBCharSupport = DB_TRADITIONAL_CHINESE;
        break;
    case OPT_ENUM_charset_zk2:
        CmdLineParms.MBCharSupport = DB_WANSUNG_KOREAN;
        break;
    case OPT_ENUM_charset_zk3:
        CmdLineParms.MBCharSupport = DB_SIMPLIFIED_CHINESE;
        break;
    case OPT_ENUM_charset_zku0:
        CmdLineParms.MBCharSupport = MB_UTF8_KANJI;
        break;
    case OPT_ENUM_charset_zku8:
        CmdLineParms.MBCharSupport = MB_UTF8;
        break;
    case OPT_ENUM_charset_default:
        CmdLineParms.MBCharSupport = MB_NONE;
        break;
    default:
        DbgNever();
        break;
    }

    /*
     * define target macros
     */
    PP_Define_1( "RC_INVOKED" );
    if( !CmdLineParms.NoTargetDefine ) {
        p = NULL;
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            p = "__WINDOWS__";
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            p = "__NT__";
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            p = "__OS2__";
        }
        if( p != NULL ) {
            PP_Define_1( p );
        }
    }
    /*
     * define target include files path
     */
    if( !CmdLineParms.IgnoreINCLUDE ) {
        p = NULL;
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            p = "WINDOWS_INCLUDE";
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            p = "NT_INCLUDE";
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            p = "OS2_INCLUDE";
        }
        if( p != NULL ) {
            PP_IncludePathAdd( PPINCLUDE_SYS, RcGetEnv( p ) );
        }
        PP_IncludePathAdd( PPINCLUDE_SYS, RcGetEnv( "INCLUDE" ) );
    }
    /*
     * initialize character input/output encoding
     */
    initMBCodePage();

    return( SetParms() );

}
