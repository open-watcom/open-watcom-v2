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
* Description:  Disassembler test program.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dis.h"

unsigned DisCliGetAlign( void *d, unsigned off, unsigned align )
{
    unsigned mod;

    mod = off % align;
    return( off + ((align - mod) % align) );
}

dis_return DisCliGetData( void *d, unsigned off, unsigned size, void *data )
{
    memcpy( data, (char *)d + off, size );
    return( DR_OK );
}

unsigned DisCliValueString( void *d, dis_dec_ins *ins, unsigned op, char *buff )
{
    char        *p = buff;

    p[0] = '\0';
    switch( ins->op[op].type & DO_MASK ) {
    case DO_RELATIVE:
    case DO_MEMORY_REL:
        *p++ = '.';
        if( ins->op[op].value < 0 ) {
            *p++ = '-';
            ins->op[op].value = -ins->op[op].value;
        } else {
            *p++ = '+';
        }
        /* fall through */
    case DO_IMMED:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        sprintf( p, "0x%8.8lx", ins->op[op].value );
        break;
    }
    return( strlen( buff ) );
}

static void print_cpu_types( void )
{
    printf( "supported CPU types:\n" );
#if DISCPU & DISCPU_axp
    printf( "    %2x   - Alpha AXP\n", DISCPU_axp );
#endif
#if DISCPU & DISCPU_ppc
    printf( "    %2x   - PowerPC\n", DISCPU_ppc );
#endif
#if DISCPU & DISCPU_x86
    printf( "    %2x   - x86\n", DISCPU_x86 );
#endif
#if DISCPU & DISCPU_jvm
    printf( "    %2x   - Java bytecode\n", DISCPU_jvm );
#endif
#if DISCPU & DISCPU_sparc
    printf( "    %2x   - Sun SPARC\n", DISCPU_sparc );
#endif
#if DISCPU & DISCPU_mips
    printf( "    %2x   - MIPS\n", DISCPU_mips );
#endif
#if DISCPU & DISCPU_x64
    printf( "    %2x   - AMD64\n", DISCPU_x64 );
#endif
}

int main( void )
{
    char                op_buff[80];
    char                name_buff[80];
    char                data[64];
    char                *out;
    char                *charp;
    char                *datap;
    unsigned long       hex;
    dis_dec_ins         ins;
    dis_handle          handle;
    dis_cpu             cpu;
    dis_format_flags    format;
    dis_inst_flags      flag;
    unsigned            radix;

    radix = 16;
    cpu = DISCPU_none;
    format = DFF_NONE;
    flag.u.all = DIF_NONE;
    for( ;; ) {
        printf("Enter a byte sequence or '?' for help:\n");
        gets( op_buff );
        if( strcmp( op_buff, "?" ) == 0 ) {
            printf( "end          - terminate program\n" );
            printf( "cpu <num>    - set the active CPU\n" );
            printf( "flag <num>   - set the instruction flags\n" );
            printf( "format <num> - set the formatting flags\n" );
            printf( "radix <num>  - set the input radix for byte sequences\n" );
        } else if( strcmp( op_buff, "end" ) == 0 ) {
            break;
        } else if( memcmp( op_buff, "cpu", 3 ) == 0 ) {
            if( cpu != DISCPU_none ) DisFini( &handle );
            cpu = strtoul( &op_buff[4], NULL, 16 );
            if( DisInit( cpu, &handle, 0 ) != DR_OK ) {
                printf("Init Failed\n");
                print_cpu_types();
                cpu = DISCPU_none;
            }
        } else if( memcmp( op_buff, "flag", 4 ) == 0 ) {
            flag.u.all = strtoul( &op_buff[5], NULL, 16 );
        } else if( memcmp( op_buff, "format", 6 ) == 0 ) {
            format = strtoul( &op_buff[7], NULL, 16 );
        } else if( memcmp( op_buff, "radix", 5 ) == 0 ) {
            radix = strtoul( &op_buff[6], NULL, 10 );
        } else if( cpu == DISCPU_none ) {
            printf( "CPU has not been established\n" );
        } else {
            charp = op_buff;
            datap = data;
            for( ;; ) {
                hex = strtoul( charp, &out, radix );
                if( charp == out ) break;
                charp = out;
                switch( DisInsSizeInc( &handle ) ) {
                case 1:
                    if( hex <= 0xff ) {
                        *(unsigned_8 *)datap = hex;
                        datap += sizeof( unsigned_8 );
                        break;
                    }
                    /* fall through */
                case 2:
                    if( hex <= 0xffff ) {
                        *(unsigned_16 *)datap = hex;
                        datap += sizeof( unsigned_16 );
                        break;
                    }
                    /* fall through */
                case 4:
                    *(unsigned_32 *)datap = hex;
                    datap += sizeof( unsigned_32 );
                    break;
                }
            }
            printf("\n");
            DisDecodeInit( &handle, &ins );
            ins.flags = flag;
            if( DisDecode( &handle, &data, &ins ) == DR_OK ) {
                DisFormat( &handle, NULL, &ins, format, name_buff, op_buff );
                printf( "size:%d <%s> <%s>\n", ins.size, name_buff, op_buff );
            } else {
                printf( "decode failed\n" );
            }
        }
        printf("\n");
    }
    if( cpu != DISCPU_none ) {
        DisFini( &handle );
    }
    return( 0 );
}
