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
* Description:  Linker message output.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "linkstd.h"
#include "command.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "mapio.h"
#include "loadfile.h"
#include "demangle.h"
#include "msg.h"

char *MsgStrings[] = {
    #define pick( name, string ) string,
    #include "wlbanner.h"
    #undef pick
};

static  char *          LocFile;
static  char *          LocMem;
static  int             LocRec;
static  MSG_ARG_LIST    MsgArgInfo;
static  char *          CurrSymName;

static unsigned         MakeExeName( char *, unsigned );
static void             IncremIndex( void );
static void             FileOrder( char rc_buff[], int which_file );
static int              UseArgInfo( void );

#define MSG_ARRAY_SIZE ((MSG_MAX_ERR_MSG_NUM / 8) + 1)

unsigned_32     MaxErrors;
bool            BannerPrinted;

byte MsgFlags[ MSG_ARRAY_SIZE ];


void ResetMsg( void )
/**************************/
{
    LocFile = NULL;
    LocMem = NULL;
    LocRec = 0;
    MsgArgInfo.index = -1;
    memset( MsgFlags, 0xFF, MSG_ARRAY_SIZE );
}

unsigned FmtStr( char *buff, unsigned len, char *fmt, ... )
/****************************************************************/
{
    va_list args;

    va_start( args, fmt );
    return( DoFmtStr( buff, len, fmt, &args ) );
}

unsigned DoFmtStr( char *buff, unsigned len, char *src, va_list *args )
/****************************************************************************/
/* quick vsprintf routine                                           */
/* assumptions - format string does not end in '%'                  */
/*             - only use of '%' is as follows                      */
/*                  %s  : string                                    */
/*                  %tn : n character string (%ns)                  */
/*                  %c  : character                                 */
/*                  %x  : 4 digit hex number (%4x)                  */
/*                  %h  : 8 digit hex number (%8x)                  */
/*                  %d  : decimal                                   */
/*                  %l  : long decimal                              */
/*                  %a  : address   ( %x:%x or 32 bit, depends on format) */
/*                  %A  : address   ( %x:%h or 32 bit, depends on format) */
/*                  %S  : symbol name                               */
/*                  %f  : an executable format name                 */
/********************************************************************/
{
    char            ch;
    char            *dest;
    char            *str;
    unsigned_16     num;
    unsigned_32     num2;
    unsigned        size;
    targ_addr *     addr;
    unsigned int    i;
    static char     hexchar[] = "0123456789abcdef";
    int             temp;

    dest = buff;
    for(;;) {
        ch = *src++;
        if( ch == '\0' || len == 1 ) break;
        if( ch != '%' ) {
            *dest++ = ch;
            len--;
        } else {
            ch = *src++;
            switch( ch ) {
            case 'S' :
                if( UseArgInfo() ) {
                    str = MsgArgInfo.arg[MsgArgInfo.index].symb->name;
                    IncremIndex();
                } else {
                    str = va_arg( *args, symbol * )->name;
                }
                if( !(LinkFlags & DONT_UNMANGLE) ) {
                    size = __demangle_l( str, 0, dest, len );
                    if( size > (len-1) ) size = len - 1;
                    CurrSymName = dest;
                } else {
                    size = strlen( str );
                    if( size > len ) size = len;
                    memcpy( dest, str, size );
                    CurrSymName = str;
                }
                len -= size;
                dest += size;
                break;
            case 's' :
                if( UseArgInfo() ) {
                    str = MsgArgInfo.arg[MsgArgInfo.index].string;
                    IncremIndex();
                } else {
                    str = va_arg( *args, char * );
                }
                size = strlen( str );
                if( size > len ) size = len;
                memcpy( dest, str, size );
                len -= size;
                dest += size;
                break;
            case 't' :
                str = va_arg( *args, char * );
                num = *src++ - '0';
                num = num * 10 + *src++ - '0';
                if( num > len ) num = len;
                while( (*str != '\0') && (num > 0) ) {
                    *dest++ = *str++;
                    num--;
                }
                while( num > 0 ) {
                    *dest++ = ' ';
                    num--;
                }
                len -= num;
                break;
            case 'c' :
                *dest++ = va_arg( *args, int );
                len--;
                break;
            case 'x' :
                if( UseArgInfo() ) {
                    num = MsgArgInfo.arg[MsgArgInfo.index].int_16;
                    IncremIndex();
                } else {
                    num = va_arg( *args, unsigned int );
                }
                if( len < 4 ) return( dest - buff );    //NOTE: premature return
                dest += 4;
                len -= 4;
                str = dest;
                for( i = 4; i > 0; i-- ) {
                    *--str = hexchar[num & 0x0f];
                    num >>= 4;
                }
                break;
            case 'h' :
                num2 = va_arg( *args, unsigned_32 );
                if( len < 8) return( dest - buff );     //NOTE: premature return
                dest += 8;
                len -= 8;
                str = dest;
                for( i = 8; i > 0; i-- ) {
                    *--str = hexchar[num2 & 0x0f];
                    num2 >>= 4;
                }
                break;
            case 'd' :
                if( len < 5 ) return( dest - buff );    // NOTE: premature return
                if( UseArgInfo() ) {
                    num = MsgArgInfo.arg[MsgArgInfo.index].int_16;
                    IncremIndex();
                } else {
                    num = va_arg( *args, unsigned int );
                }
                ultoa( num, dest, 10 );
                size = strlen( dest );
                dest += size;
                len -= size;
                break;
            case 'l' :
                if( len < 10 ) return( dest - buff );   //NOTE: premature return
                if( UseArgInfo() ) {
                    num2 = MsgArgInfo.arg[MsgArgInfo.index].int_32;
                    IncremIndex();
                } else {
                    num2 = va_arg( *args, unsigned_32 );
                }
                ultoa( num2, dest, 10 );
                size = strlen( dest );
                dest += size;
                len -= size;
                break;
            case 'a':
            case 'A':
                if( UseArgInfo() ) {
                    addr = MsgArgInfo.arg[MsgArgInfo.index].address;
                    IncremIndex();
                } else {
                    addr = va_arg( *args, targ_addr * );
                }
                temp = MsgArgInfo.index;
                MsgArgInfo.index = -1;
                if( FmtData.type & MK_FLAT ) {
                    size = FmtStr( dest, len, "%h", addr->off );
                } else if( FmtData.type & MK_QNX_FLAT) {
                    size = FmtStr( dest, len, "%h", FindLinearAddr( addr ) );
                } else if( FmtData.type & (MK_ELF|MK_PE) ) {
                    size = FmtStr( dest, len, "%h", FindLinearAddr2( addr ) );
                } else if( FmtData.type & MK_ID_SPLIT ) {
                    if( addr->seg == CODE_SEGMENT ) {
                        size = FmtStr( dest, len, "CODE:%h", addr->off );
                    } else {
                        size = FmtStr( dest, len, "DATA:%h", addr->off );
                    }
                } else if( (FmtData.type & MK_32BIT) || ch == 'A' ) {
                    size = FmtStr( dest, len, "%x:%h", addr->seg, addr->off );
                } else {
                    size = FmtStr( dest, len, "%x:%x", addr->seg, (unsigned short)addr->off );
                }
                dest += size;
                len -= size;
                MsgArgInfo.index = temp;
                break;
            case 'f':
                num = MakeExeName( dest, len );
                dest += num;
                len -= num;
                break;
            }
        }
    }
    MsgArgInfo.index = -1;
    *dest = '\0';
    return( dest - buff );
}

#define IS_VOWEL(c) (((c)=='a')||((c)=='e')||((c)=='i')||((c)=='o')||((c)=='u'))

static unsigned MakeExeName( char * buff, unsigned max )
/******************************************************/
/* make up the "an OS/2 executable" string. */
{
    char        rc_buff[RESOURCE_MAX_SIZE];
    exe_format  format;
    unsigned    len;
    char *      str;
    unsigned    num;

    if( max <= 3 ) return( 0 );
    len = 1;
    *buff++ = 'a';
    if( FmtData.osname != NULL ) {
        str = FmtData.osname;
    } else {
        format = FmtData.type;
        for( ;; ) {
            num = blog_32( format );
            format &= ~(1 << num);
            if( format == 0 )
                break;
        }
        Msg_Get( MSG_FILE_TYPES_0 + num, rc_buff );
        str = rc_buff;
    }
    if( IS_VOWEL( tolower(*str) ) ) {
        *buff++ = 'n';
        len++;
    }
    *buff++ = ' ';
    num = strlen(str);
    len += num + 2;
    if( len > max ) return( len - (num + 2) );
    memcpy( buff, str, num );
    buff += num;
    *buff++ = ' ';
    if( FmtData.dll ) {
        Msg_Get( MSG_CREATE_TYPE_DLL, rc_buff );
        str = rc_buff;
    } else {
        Msg_Get( MSG_CREATE_TYPE_EXE, rc_buff );
        str = rc_buff;
    }
    num = strlen( str );
    len += num;
    if( len > max ) return( len - num );
    memcpy( buff, str, num + 1 );       /* +1 for the nullchar */
    return( len );
}

static int UseArgInfo( void )
/***************************/
{
    return( MsgArgInfo.index >= 0 );
}

static void IncremIndex( void )
/*****************************/
{
    MsgArgInfo.index++;
}

void Locator( char *filename, char *mem, unsigned rec )
/************************************************************/
{
    LocFile = filename;
    LocMem = mem;
    LocRec = rec;
}

static void LocateFile( unsigned num )
/************************************/
{
    unsigned    rec;

    if( num & LOC ) {
        if( num & (LOC_REC & ~LOC) ) {
            rec = RecNum;
        } else {
            rec = 0;
        }
        if( CurrMod == NULL ) {
            if( CmdFile == NULL ) {
                Locator( NULL, NULL, 0 );
            } else {
                Locator( CmdFile->name, NULL, 0 );
            }
         } else {
            Locator( CurrMod->f.source->file->name, CurrMod->name, rec );
        }
    }
}

unsigned CalcMsgNum( unsigned num )
/*********************************/
// map the internal enum onto the value that the user sees.
{
    unsigned    class;

    class = (num & CLASS_MSK) >> NUM_SHIFT;
    class = (class + 1) / 2;
    return class * 1000 + (num & NUM_MSK);
}

unsigned GetMsgPrefix( char *buff, unsigned max_len, unsigned num )
/*****************************************************************/
{
    unsigned    prefixlen;
    unsigned    class;
    char        rc_buff[RESOURCE_MAX_SIZE];

    prefixlen = 0;
    *buff = '\0';
    class = num & CLASS_MSK;
    if( class >= (WRN & CLASS_MSK) ) {
        if( class == (WRN & CLASS_MSK) ) {
            Msg_Get( MSG_WARNING, rc_buff );
        } else {
            Msg_Get( MSG_ERROR, rc_buff );
        }
        prefixlen = FmtStr( buff, max_len, rc_buff, CalcMsgNum( num ) );
    }
    return( prefixlen );
}

static void MessageFini( unsigned num, char *buff, unsigned len )
/***************************************************************/
{
    unsigned    prefixlen;
    unsigned    class;
    char        prefix[ MAX_MSG_SIZE ];

    prefixlen = 0;
    class = num & CLASS_MSK;
    if( class >= (ERR & CLASS_MSK) ) {
        LinkState |= LINK_ERROR;
    }
    if( num & OUT_TERM ) {
        if( !(LinkFlags & QUIET_FLAG) ) {
            WLPrtBanner();
            WriteStdOutInfo( buff, num, CurrSymName );
        } else if( class != (INF & CLASS_MSK)) {
            WriteStdOutInfo( buff, num, CurrSymName );
        }
    }
    if( (num & OUT_MAP) && (MapFile != NIL_FHANDLE) ) {
        prefixlen = GetMsgPrefix( prefix, MAX_MSG_SIZE, num );
        BufWrite( prefix, prefixlen );
        BufWrite( buff, len );
        WriteMapNL( 1 );
    }
    if( class == (FTL & CLASS_MSK) )
        Suicide();
    /* yells are counted as errors for limits */
    if(( class == (YELL & CLASS_MSK) ) || ( class >= (MILD_ERR & CLASS_MSK) )) {
        if( LinkFlags & MAX_ERRORS_FLAG ) {
            MaxErrors--;
            if( MaxErrors == 0 ) {
                LnkMsg( FTL+MSG_TOO_MANY_ERRORS, NULL );
            }
        }
    }
}

void LnkMsg(
    unsigned    num,    // A message number + control flags
    char        *types, // Conversion qualifiers
    ... )               // Arguments to interpolate into message
/**************************************************
 * report a linker message
 *
 * num   selects a message containing substitutions; both printf and %digit
 * types is either NULL or the order of interpolated arguments.
 */
{
    va_list     args;
    int         which_file = 0;
    unsigned    len;
    char        rc_buff[ RESOURCE_MAX_SIZE ];
    char        buff[ MAX_MSG_SIZE ];

    if( !TestBit( MsgFlags, num & NUM_MSK ) )
        return;
    CurrSymName = NULL;
    LocateFile( num );
    len = 0;
    if( LocFile != NULL ) {
        which_file += 1;
    }
    if( LocMem != NULL ) {
        which_file += 2;
    }
    if( LocRec != 0 ) {
        which_file += 4;
    }
    if( which_file != 0 ) {
        if( Token.how == SYSTEM ) {
            Msg_Get( MSG_SYS_BLK, rc_buff );
            which_file = 1;
        } else if( Token.how == ENVIRONMENT ) {
            Msg_Get( MSG_ENVIRON, rc_buff );
            which_file = 1;
        } else {
            Msg_Get( MSG_FILE_REC_NAME_0 + which_file - 1, rc_buff );
        }
        FileOrder( rc_buff, which_file );
        len += FmtStr( &buff[len], MAX_MSG_SIZE - len, rc_buff );
        if( num & LINE ) {
            if( Token.how != SYSTEM && Token.how != ENVIRONMENT ) {
                Msg_Get( MSG_LINE, rc_buff );
                Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "d", Token.line );
                len += FmtStr( &buff[len], MAX_MSG_SIZE - len, rc_buff );
            }
        }
        LocFile = NULL;
        LocMem = NULL;
        LocRec = 0;
    }

    va_start( args, types );
    Msg_Get( num & NUM_MSK, rc_buff );
    Msg_Put_Args( rc_buff, &MsgArgInfo, types, &args );
    va_end( args );
    len += FmtStr( &buff[len], MAX_MSG_SIZE - len, rc_buff );
    MessageFini( num, buff, len );
}

static void HandleRcMsg( unsigned num, va_list *args )
/****************************************************/
/* getting an error message from resource compiler code */
{
    unsigned    len;
    char        rc_buff[RESOURCE_MAX_SIZE];
    char        buff[ MAX_MSG_SIZE ];

    num |= ERR;
    len = 0;
    CurrSymName = NULL;
    Msg_Get( num & NUM_MSK, rc_buff );
    len += DoFmtStr( &buff[len], MAX_MSG_SIZE - len, rc_buff, args );
    MessageFini( num, buff, len );
}

void RcWarning( unsigned num, ... )
/****************************************/
{
    va_list args;

    va_start( args, num );
    HandleRcMsg( num, &args );
}

void RcError( unsigned num, ... )
/**************************************/
{
    va_list args;

    va_start( args, num );
    HandleRcMsg( num, &args );
}

static void FileOrder( char rc_buff[], int which_file )
/*****************************************************/
{
    switch( which_file ) {
        case 1:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "s", LocFile );
            break;
        case 2:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "s", LocMem );
            break;
        case 3:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "12", LocFile, LocMem );
            break;
        case 4:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "d", LocRec );
            break;
        case 5:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "sd", LocFile, LocRec );
            break;
        case 6:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "sd", LocMem, LocRec );
            break;
        case 7:
            Msg_Do_Put_Args( rc_buff, &MsgArgInfo, "12d", LocFile, LocMem,
                        LocRec );
            break;
    }
}

void WLPrtBanner( void )
/*****************************/
// print the banner, if it hasn't already been printed.
{
    char *  msg;

    if( !BannerPrinted ) {
        msg = MsgStrings[ PRODUCT ];
        WriteStdOutInfo( msg, BANNER, NULL );
        msg = MsgStrings[ COPYRIGHT ];
        WriteStdOutInfo( msg, BANNER, NULL );
        msg = MsgStrings[ TRADEMARK ];
        WriteStdOutInfo( msg, BANNER, NULL );
        msg = MsgStrings[ TRADEMARK2 ];
        WriteStdOutInfo( msg, BANNER, NULL );
        BannerPrinted = TRUE;
    }
}

bool SkipSymbol( symbol * sym )
/************************************/
{
    if( sym->info & SYM_STATIC && !(MapFlags & MAP_STATICS) ) return TRUE;
#if defined(__WATCOMC__)
    { int art;

    art = __is_mangled_internal( sym->name, 0 ); // KLUDGE: it doesn't need len
    return !(MapFlags & MAP_ARTIFICIAL) && art == __MANGLED_INTERNAL;
    }
#else
    return FALSE;
#endif
}

int SymAlphaCompare( const void *a, const void *b )
/********************************************************/
{
    symbol *    left;
    symbol *    right;
    const char *leftname;
    const char *rightname;
    size_t      leftsize;
    size_t      rightsize;
    int         result;

    left = *((symbol **) a);
    right = *((symbol **) b);
    if( !(LinkFlags & DONT_UNMANGLE) ) {
        __unmangled_name( left->name, 0, &leftname, &leftsize );
        __unmangled_name( right->name, 0, &rightname, &rightsize );
    } else {
        leftname = left->name;
        rightname = right->name;
        leftsize = strlen( leftname );
        rightsize = strlen( rightname );
    }
    if( leftsize < rightsize ) {
        result = memicmp( leftname, rightname, leftsize );
        if( result == 0 ) result = -1;  // since leftsize < rightsize;
    } else {
        result = memicmp( leftname, rightname, rightsize );
        if( result == 0 ) {
            if( leftsize > rightsize ) {
                result = 1;
            }
        }
    }
    return result;
}
