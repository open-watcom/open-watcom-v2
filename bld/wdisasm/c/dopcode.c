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
#include "pcode.h"

#undef pcode
#define _DEBUG_
#include "pcode.h"

#undef pcode
#undef _DEBUG_
#define _PARMS_
#include "pcode.h"

int SwitchCount;
char CompareIns;        /* non-zero implies previous opcode was a compare */


uint_16 DoPcode()
/***************/

{
    uint_16        curr_size;
    int            disp;
    unsigned int   len;

    if( SwitchCount != 0 ) {
        switch( Opcode ) {
        case BSwitch:  len = 1; break;
        case ISwitch:  len = 2; break;
        case LSwitch:  len = 4; break;
        }
        if( Pass != 2 ) --SwitchCount;
        curr_size = 0;
    } else if( CompareIns != 0 ) {
        if( Pass != 2 ) CompareIns = 0;
        Opcode = GetDataByte();
        switch( Opcode ) {
        case hop_a:
        case hop_ae:
        case hop_b:
        case hop_be:
        case hop_e:
        case hop_ne:
        case hop_g:
        case hop_ge:
        case hop_l:
        case hop_le:
            disp = GetDataByte();
            FindLabel( InsAddr + 1, GetOffset() + disp, Segment );
            curr_size = 2;
            len = 0;
            break;
        case jmp_a:
        case jmp_ae:
        case jmp_b:
        case jmp_be:
        case jmp_e:
        case jmp_ne:
        case jmp_g:
        case jmp_ge:
        case jmp_l:
        case jmp_le:
            disp = GetDataWord();
            FindLabel( InsAddr + 1, GetOffset() + disp, Segment );
            curr_size = 3;
            len = 0;
            break;
        }
    } else {
        Opcode = GetDataByte();
        switch( Opcode ) {
        case TwoByteOp:
            SecondByte = GetDataByte();
            curr_size = 2;
            len = P2_Parms[SecondByte];
            switch( SecondByte ) {
            case RetStructn:
                GetDataWord();          /* get structure size */
                curr_size += 2;
            case RetFloatn:
            case RetDoublen:
                --len;
                ++curr_size;
                if( (unsigned char) GetDataByte() == 0xCB ) {
                    len = 0;   /* no parms to pop */
                }
                break;
            }
            break;
        case BSwitch:
        case ISwitch:
        case LSwitch:
            len = GetDataByte();
            len += GetDataByte() << 8;
            if( Pass != 2 ) SwitchCount = len;
            len = 0;
            curr_size = 3;
            break;
        case PushNearAddr:
            disp = GetDataWord();
            FindSymbol( InsAddr + 1 );
            curr_size = 3;
            len = 0;
            break;
        case jmp_short:
            disp = GetDataByte();
            FindLabel( InsAddr + 1, GetOffset() + disp, Segment );
            curr_size = 2;
            len = 0;
            break;
        case jmp_long:
            disp = GetDataWord();
            FindLabel( InsAddr + 1, GetOffset() + disp, Segment );
            curr_size = 3;
            len = 0;
            break;
        case CmpInt:
        case CmpInt0:
        case CmpLong:
        case CmpNULL:
        case CmpFloat:
        case CmpDouble:
            if( Pass != 2 )  CompareIns = 1;
            curr_size = 1;
            len = P_Parms[Opcode];
            break;
        case VoidFunc:
        case IntFunc:
        case LongFunc:
        case DoubleFunc:
        default:
            curr_size = 1;
            len = P_Parms[Opcode];
        }
    }
    while( len != 0 ) {
        GetDataByte();
        --len;
        ++curr_size;
    }
    return( curr_size );
}


void FormatPcode( char *buf )
/***************************/

{
    unsigned int   len;
    unsigned int   i;
    short int      value;
    long  int      long_val;
    char          *p;
    uint_32        addr;

    memset( buf, ' ', 20 );
    if( SwitchCount != 0 ) {
        strcpy( buf, DBstring() );
        p = buf + strlen(buf);
        switch( Opcode ) {
        case BSwitch:
            itoa( DataString[0], p, 10 );
            break;
        case ISwitch:
            buf[1] = 'W';
            itoa( (DataString[1] << 8) + DataString[0], p, 10 );
            break;
        case LSwitch:
            buf[1] = 'D';
            value = (DataString[1] << 8) + DataString[0];
            long_val = (DataString[3] << 8) + DataString[2];
            long_val = (long_val << 16) + (unsigned short int)value;
            ltoa( long_val, p, 10 );
            break;
        }
        --SwitchCount;
        len = 0;
    } else if( CompareIns != 0 ) {
        CompareIns = 0;
        strcpy( buf, PJmp_Codes[Opcode] );
        len = PJmp_Parms[Opcode];
        i = 1;
        p = buf + strlen(buf);
        *p = ' ';
        switch( Opcode ) {
        case hop_a:
        case hop_ae:
        case hop_b:
        case hop_be:
        case hop_e:
        case hop_ne:
        case hop_g:
        case hop_ge:
        case hop_l:
        case hop_le:
            value = (signed char)DataString[1];
            break;
        case jmp_a:
        case jmp_ae:
        case jmp_b:
        case jmp_be:
        case jmp_e:
        case jmp_ne:
        case jmp_g:
        case jmp_ge:
        case jmp_l:
        case jmp_le:
            value = (DataString[2] << 8) + DataString[1];
            break;
        }
        strcpy( &buf[16], JmpLabel( GetOffset() + value, 1 ) );
        len = 0;
    } else {
        if( Opcode == TwoByteOp ) {
            strcpy( buf, P2_Codes[SecondByte] );
            len = P2_Parms[SecondByte];
            i = 2;
            if( SecondByte == RetStructn ) {            /* 03-may-91 */
                p = buf + strlen(buf);
                *p++ = ',';
                DoIToHS( p, DataString[i], 3 );
                p += 3;
                *p++ = ',';
                ++i;
                DoIToHS( p, DataString[i], 3 );
                p += 3;
                *p = '\0';
                ++i;
            }
            if( SecondByte == RetStructn ||
                SecondByte == RetFloatn  ||
                SecondByte == RetDoublen ) {
                strcat( buf, ",  retf" );
                len = 2;
                if( DataString[i] == 0xCB )  len = 0;
                ++i;
            }
        } else {
            strcpy( buf, P_Codes[Opcode] );
            len = P_Parms[Opcode];
            i = 1;
        }
        p = buf + strlen(buf);
        switch( Opcode ) {
        case VoidFunc:
        case IntFunc:
        case LongFunc:
        case DoubleFunc:
        case PushGblAddr:
            addr = 0;
            for( i = 1; i <= 4; i++ ) {
                addr = (addr << 8) + DataString[i];
            }
            *p = ' ';
            strcpy( &buf[16], ToStr( addr, 8, 1 ) );
            len = 0;
            break;
        case TwoByteOp:
            if( SecondByte == StructFunc  ||  SecondByte == FloatFunc ) {
                addr = 0;
                for( i = 2; i <= 5; i++ ) {
                    addr = (addr << 8) + DataString[i];
                }
                *p = ' ';
                strcpy( &buf[16], ToStr( addr, 8, 2 ) );
                len = 0;
            } else {
                if( len != 0 )  *p++ = ',';
            }
            break;
        case PushNearAddr:
            addr = 0;
            for( i = 1; i <= 2; i++ ) {
                addr = (addr << 8) + DataString[i];
            }
            *p = ' ';
            strcpy( &buf[16], ToStr( addr, 4, 1 ) );
            len = 0;
            break;
        case PushBConst:
            *p = ' ';
            itoa( (signed char)DataString[1], &buf[16], 10 );
            len = 0;
            break;
        case VoidINFunc:                                /* 17-may-91 */
        case IntINFunc:
        case LongINFunc:
        case FloatINFunc:
        case DoubleINFunc:
        case StructINFunc:
            strcat( buf, ",  retf" );
            len = 0;
            break;
        case BSwitch:
        case ISwitch:
        case LSwitch:
            *p = ' ';
            value = (DataString[2] << 8) + DataString[1];
            SwitchCount = value;
            itoa( value, &buf[16], 10 );
            len = 0;
            break;
        case jmp_short:
            *p = ' ';
            value = (signed char)DataString[1];
            strcpy( &buf[16], JmpLabel( GetOffset() + value, 1 ) );
            len = 0;
            break;
        case jmp_long:
            *p = ' ';
            value = (DataString[2] << 8) + DataString[1];
            strcpy( &buf[16], JmpLabel( GetOffset() + value, 1 ) );
            len = 0;
            break;
        case PushWConst:
            *p = ' ';
            value = (DataString[2] << 8) + DataString[1];
            itoa( value, &buf[16], 10 );
            len = 0;
            break;
        case PushLConst:
            *p = ' ';
            value = (DataString[2] << 8) + DataString[1];
            long_val = (DataString[4] << 8) + DataString[3];
            long_val = (long_val << 16) + (unsigned short int)value;
            ltoa( long_val, &buf[16], 10 );
            len = 0;
            break;
        case CmpInt:
        case CmpInt0:
        case CmpLong:
        case CmpNULL:
        case CmpFloat:
        case CmpDouble:
            CompareIns = 1;
        default:
            if( len != 0 )  *p++ = ',';
        }
    }
    while( len != 0 ) {
        DoIToHS( p, DataString[i], 3 );
        p += 3;
        *p = '\0';
        ++i;
        --len;
        if( len != 0 )  *p++ = ',';
    }
}
