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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include "drwatcom.h"
#include "sdkasm.h"

#define MAX_BUFF        256
#define UNREADABLE      0

extern DWORD            InsAddr;

#ifdef __NT__
static HANDLE           processHandle;
static ModuleNode       *curModule;
static int              MinAddrSpaces = 20;
#endif
static ADDRESS          currentAddr;
static char             disasmBuf[MAX_BUFF];
static char             hexTable[] = "0123456789ABCDEF";
static DisAsmRtns       disasmInfo;


/*
 * LongToHex - convert integer to a hex string
 */
static int LongToHex( char *str, DWORD value, int len )
{
   int i;

    for( i=len-1; i>=0; i-- ) {
        str[ i ] = hexTable[ value & 0xf ];
        value >>= 4;
    }
    if( !isdigit( *str ) ) {
        memmove( str + 1, str, ++len );
        *str = '0';
    }
    str[ len ] = 0;
    return( len );

} /* LongToHex */


/*
 * ConvertAddress - convert a address into a string
 */
static char * ConvertAddress( ADDRESS *addr, char *buff, int blen, int neartest )
{
    int         len;
    char        off[20];
    syminfo     si;
    int         i;

    blen = blen;

    if( FindSymbol( addr, &si ) ) {
        if( si.symoff == 0L ) {
            sprintf( buff, "%s ", si.name );
        } else {
            sprintf( buff, "%s+%lx ", si.name, si.symoff );
        }
    } else {
        if( IsSeg32( addr->seg ) ) {
            sprintf( off, "%08lx", addr->offset );
        } else {
            sprintf( off, "%04x ", (WORD) addr->offset );
        }
        if( neartest && currentAddr.seg == addr->seg ) {
            strcpy( buff, off );
        } else {
            if( si.segnum != -1 && si.segnum != 0 ) {
                sprintf( buff,"%s:%d:%s", si.name, si.segnum, off );
            } else {
                sprintf( buff, "%04x:%s",addr->seg, off );
            }
        }
    }
    len = strlen( buff );
    if( len < MinAddrSpaces ) {
        for( i=0; i<MinAddrSpaces-len; i++ ) {
            buff[len+i] = ' ';
        }
        buff[len+i] = 0;
        len += i;
    }
    return( buff+len );

} /* ConvertAddress */

/*
 * DrWatJmpLabel -- process a label
 */
static char *DrWatJmpLabel( DWORD offset, DWORD off )
{
    ADDRESS     addr;

    off = off;
    addr.seg = currentAddr.seg;
    addr.offset = offset;
    ConvertAddress( &addr, disasmBuf, MAX_BUFF, TRUE );
    return( disasmBuf );

} /* DrWatJmpLabel */


/*
 * DrWatToStr -- convert integer to hex string
 */
static char *DrWatToStr( DWORD value, WORD length, DWORD off )
{

    off = off;
    length = length;

    LongToHex( disasmBuf, value, 4 );
    return( disasmBuf );

} /* DrWatToStr */


/*
 * DrWatToIndex -- convert to index
 */
static char *DrWatToIndex( unsigned long value, DWORD off )
{

    off = off;
    if( (long)value < 0 ) {
        LongToHex( disasmBuf + 1, -(long)value, 4 );
        disasmBuf[0] = '-';
    } else {
        LongToHex( disasmBuf, value, 4 );
    }
    return( disasmBuf );

} /* DrWatToIndex */

/*
 * DrWatToBrStr -- convert to branch string
 */
static char *DrWatToBrStr( DWORD value, DWORD  off )
{
    int  len;

    off = off;

    disasmBuf[ 0 ] = '[';
    len = LongToHex( &disasmBuf[ 1 ], value, 4 );
    disasmBuf[ len + 1 ] = ']';
    disasmBuf[ len + 2 ] = 0;
    return( disasmBuf );

} /* DrWatToBrString */


/*
 * DrWatToSegStr -- convert to segment string
 */
static char *DrWatToSegStr( DWORD value, WORD seg, DWORD off )
{
    ADDRESS     addr;
    off = off;

    addr.seg = seg;
    addr.offset  = value;
    ConvertAddress( &addr, disasmBuf, MAX_BUFF, FALSE );
    return( disasmBuf );

} /* DrWatToSegStr */


/*
 *    Memory access routines used by disassembler
 *    ===========================================
 *
 *    DrWatGetDataByte   ; get byte and advance ptr
 *    DrWatGetNextByte   ; get byte
 *    DrWatGetDataWord   ; get word and advance ptr
 *    DrWatGetNextWord   ; get word
 *    DrWatGetOffset     ; get current offset in segment
 *    DrWatEndOfSegment  ; at end of segment ?
 */
#ifdef __NT__
static short DrWatGetNextByte( void )
{
    char        byte;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &byte,
                        sizeof( char ), NULL ) ) return( byte );
    return( UNREADABLE );
} /* DrWatGetNextByte */

static short DrWatGetDataByte( void )
{
    char        byte;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &byte,
                        sizeof( char ), NULL ) ) {
        currentAddr.offset++;
        return( byte );
    }
    return( UNREADABLE );
} /* DrWatGetDataByte */
#if 0
static short DrWatGetNextWord( void )
{
    WORD        word;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &word,
                        sizeof( WORD ), NULL ) ) {
        return( word );
    }
    return( UNREADABLE );

} /* DrWatGetNextWord */
#endif
static short DrWatGetDataWord( void )
{
    WORD        word;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &word,
                        sizeof( WORD ), NULL ) ) {
        currentAddr.offset += 2;
        return( word );
    }
    return( UNREADABLE );

} /* DrWatGetDataWord */
#if 0
static long DrWatGetNextLong( void )
{
    DWORD       dword;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &dword,
                        sizeof( DWORD ), NULL ) ) {
        return( dword );
    }
    return( UNREADABLE );
} /* DrWatGetNextLong */
#endif
static long DrWatGetDataLong( void )
{
    DWORD       dword;

    if( ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset, &dword,
                        sizeof( DWORD ), NULL ) ) {
        currentAddr.offset += 4;
        return( dword );
    }
    return( UNREADABLE );
} /* DrWatGetDataLong */

static char DrWatEndOfSegment( void )
{
    char        byte;

    return( !ReadProcessMemory( processHandle, (LPVOID)currentAddr.offset,
                                &byte, sizeof( char ), NULL ) );
} /* DrWatEndOfSegment */

void SetDisasmInfo( HANDLE prochdl, ModuleNode *mod ) {
    processHandle = prochdl;
    curModule = mod;
}

static bool IsSeg32( WORD seg ) {
    seg = seg;
    return( true );
}

static BOOL FindSymbol( ADDRESS *addr, syminfo *si ) {

    DWORD       symoff;

    si->segnum = -1;
    si->name[0] = 0;
    if( !StatShowSymbols || curModule == NULL ) {
        return( FALSE );
    }
    if( !GetSymbolName( curModule, addr->offset, si->name, &symoff ) ) {
        return( FALSE );
    }
    si->symoff = symoff;
    return( TRUE );
}

RVALUE FindWatSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo )
{
    DWORD       symoff;
    DWORD       line;
    BOOL        ret;
    if( !GetSymbolName( curModule, addr->offset, si->name, &symoff ) ) {
        return( NOT_FOUND );
    }
    si->symoff = symoff;
    if( getsrcinfo ) {
        ret = GetLineNum( curModule, addr->offset, si->filename,
                          MAX_FILE_NAME, &line );
        if( !ret ) return( NOT_FOUND );
        si->linenum = line;
    }
    return( FOUND );
}
#else

static short DrWatGetNextByte( void )
{
    char        byte;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &byte, 1 );
    return( byte );

} /* DrWatGetNextByte */

static short DrWatGetDataByte( void )
{
    char        byte;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &byte, 1 );
    currentAddr.offset++;
    return( byte );

} /* DrWatGetDataByte */

static short DrWatGetNextWord( void )
{
    WORD        word;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &word, 2 );
    return( word );

} /* DrWatGetNextWord */

static short DrWatGetDataWord( void )
{
    WORD        word;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &word, 2 );
    currentAddr.offset += 2;
    return( word );

} /* DrWatGetDataWord */

static long DrWatGetNextLong( void )
{
    DWORD       dword;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &dword, 4 );
    return( dword );
} /* DrWatGetNextLong */

static long DrWatGetDataLong( void )
{
    DWORD       dword;

    ReadMem( currentAddr.seg, currentAddr.offset, (LPSTR) &dword, 4 );
    currentAddr.offset += 4;
    return( dword );

} /* DrWatGetDataLong */

static char DrWatEndOfSegment( void )
{

    if( !IsValidSelector( currentAddr.seg ) ) {
        return( TRUE );
    }
    if( currentAddr.offset >= GetASelectorLimit( currentAddr.seg ) ) {
        return( TRUE );
    }
    return( FALSE );

} /* DrWatEndOfSegment */
#endif

static DWORD DrWatGetOffset( void )
{
    return( currentAddr.offset );

} /* DrWatGetOffset */

static char *DrWatGetWtkInsName( unsigned ins ) {
    ins = ins; return( "" );
}

static void DrWatDoWtk( void ) {
}

static int DrWatIsWtk() {
    return( 0 );
}

/*
 * RegDrWatcomDisasmRtns - register us to use the interface to the
 *                      disasembler
 */
void RegDrWatcomDisasmRtns( void )
{
    disasmInfo.GetDataByte = DrWatGetDataByte;
    disasmInfo.GetDataWord = DrWatGetDataWord;
    disasmInfo.GetNextByte = DrWatGetNextByte;
    disasmInfo.GetDataLong = DrWatGetDataLong;
    disasmInfo.EndOfSegment = DrWatEndOfSegment;
    disasmInfo.GetOffset = DrWatGetOffset;
    disasmInfo.DoWtk = DrWatDoWtk;
    disasmInfo.IsWtk = DrWatIsWtk;
    disasmInfo.ToStr = DrWatToStr;
    disasmInfo.JmpLabel = DrWatJmpLabel;
    disasmInfo.ToBrStr = DrWatToBrStr;
    disasmInfo.ToIndex = DrWatToIndex;
    disasmInfo.ToSegStr = DrWatToSegStr;
    disasmInfo.GetWtkInsName = DrWatGetWtkInsName;
    RegisterRtns( &disasmInfo );

} /* RegDrWatcomDisasmRtns */

/*
 * AddressString - given an address, make it a string
 */
static char *AddressString( ADDRESS *addr, char *buff )
{

    return( ConvertAddress( addr, buff, MAX_BUFF, FALSE ) );

} /* AddressString */


static void DisAsm( instruction *ins )
{
    if( DrWatEndOfSegment() ) {
        ins->opcode = I_INVALID;
        ins->num_oper = 0;
        ins->ins_size = 1;
        ins->mem_ref_op = NULL_OP;
    } else {
        InsAddr = currentAddr.offset;
        MiscDoCode( ins, IsSeg32( currentAddr.seg ), &disasmInfo );
        if( ins->pref & PREF_FWAIT ) {
            currentAddr.offset -= ins->ins_size - 1;
            ins->opcode = I_WAIT;
            ins->num_oper = 0;
            ins->ins_size = 1;
            ins->mem_ref_op = NULL_OP;
        }
    }
} /* DisAsm */


/*
 * GetInsSize -- get the size of an instruction
 */
static unsigned GetInsSize( ADDRESS *addr )
{
    instruction ins;

    currentAddr = *addr;
    DisAsm( &ins );
    return( ins.ins_size );
}


/*
 * InstructionBackup - go back cnt instructions
 */
void InstructionBackup( int cnt, ADDRESS *addr )
{
    ADDRESS     taddr;
    int         i;

    for( i=0;i<cnt;i++ ) {
        taddr = *addr;
        PreviousInstruction( addr );
        if( addr->seg == NULL ) {
            *addr = taddr;
            return;
        }
    }

} /* InstructionBackup */


/*
 * PreviousInstruction - find the instruction before the one sent in
 */
void PreviousInstruction( ADDRESS *addr )
{
    DWORD       start;
    DWORD       next_off;
    DWORD       curr_off;
    unsigned    backup;

    backup = 0x20;
    if( IsSeg32( addr->seg ) ) {
        backup = 0x40;
    }
    curr_off = addr->offset;
    addr->offset = curr_off-backup;
    if( curr_off <= backup ) {
        addr->offset = 0;
    }

    for( start=addr->offset; start<curr_off; start++ ) {
        addr->offset = start;
        while( 1 ) {
            next_off = addr->offset + GetInsSize( addr );
            if( next_off == curr_off ) {
                return;
            }
            if( (next_off > curr_off) || (next_off < start) ) {
                break;
            }
            addr->offset = next_off;
        }
    }
    addr->seg = NULL;
    addr->offset = NULL;

} /* PreviousInstruction */


static void doFormatIns( char *buff, instruction *ins )
{
    unsigned    format;

    buff[0] = 0;
    format = 0;
    format |= FORM_REG_UPPER | FORM_NAME_UPPER;
    format |= FORM_INDEX_IN;
    MiscFormatIns( buff, ins, format, &disasmInfo );

} /* doFormatIns */


/*
 * Disassemble - disassemble an instruction
 */
unsigned Disassemble( ADDRESS *addr, char *buff, int addbytes )
{
    char        *p;
    instruction ins;
    ADDRESS     tmpaddr;

    if( addbytes) {
        tmpaddr = *addr;
    }
    currentAddr = *addr;
    p = AddressString( addr, buff );
    *p++ = ' ';
    DisAsm( &ins );
    if( addbytes ) {
        char    bytebuff[30];
        char    tmp[5];
        int     i,j;

#ifdef __NT__
        ReadProcessMemory( processHandle, (LPVOID)tmpaddr.offset, bytebuff,
                            ins.ins_size, NULL );
#else
        ReadMem( tmpaddr.seg, tmpaddr.offset, bytebuff, ins.ins_size );
#endif
        if( IsSeg32( tmpaddr.seg ) ) {
            j = 8;
        } else {
            j = 6;
        }
        if( ins.ins_size > j ) {
            j = ins.ins_size;
        }
        for( i=0;i<j; i++ ) {
            if( i < ins.ins_size ) {
                sprintf( tmp,"%02x", (WORD) bytebuff[i] );
                *p++ = tmp[0];
                *p++ = tmp[1];
            } else {
                *p++ = ' ';
                *p++ = ' ';
            }
            *p++ = ' ';
        }
    }
    doFormatIns( p, &ins );
    return( ins.ins_size );

} /* UnAsmLine */
