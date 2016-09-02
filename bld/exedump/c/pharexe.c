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
* Description:  PharLap executable dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#include "wdglb.h"
#include "wdfunc.h"


static  const_string_table phar_exe_msg[] = {
    "2file size mod 512                         = ",
    "2file size in blocks                       = ",
    "2number of relocation items                = ",
    "2size of header in paragraphs              = ",
    "2minimum data in 4k pages                  = ",
    "2maximum data in 4k pages                  = ",
    "4initial ESP                               = ",
    "2checksum                                  = ",
    "4initial EIP                               = ",
    "2offset of first relocation item           = ",
    "2overlay number                            = ",
    "2always one                                = ",
    NULL
};

static  const_string_table phar_ext_msg[] = {
    "2file format level (1==flat, 2==multi-seg) = ",
    "2header size in bytes                      = ",
    "4file size in bytes                        = ",
    "216-bit checksum                           = ",
    "4offset of run-time parameter block        = ",
    "4size of run-time parameter block          = ",
    "4offset of relocation table                = ",
    "4size of relocation table                  = ",
    "4offset of segment information table       = ",
    "4size of segment information table         = ",
    "2size of a seg info table entry            = ",
    "4offset of program load image              = ",
    "4size of program load image                = ",
    "4offset of symbol table                    = ",
    "4size of symbol table                      = ",
    "4offset of program GDT in load image       = ",
    "4size of program GDT in load image         = ",
    "4offset of program LDT in load image       = ",
    "4size of program LDT in load image         = ",
    "4offset of program IDT in load image       = ",
    "4size of program IDT in load image         = ",
    "4offset of program TSS in load image       = ",
    "4size of program TSS in load image         = ",
    "4minmum number of extra bytes to allocate  = ",
    "4maxmum number of extra bytes to allocate  = ",
    "4base offset of program image              = ",
    "4initial ESP                               = ",
    "2initial SS                                = ",
    "4initial EIP                               = ",
    "2initial CS                                = ",
    "2initial LDT                               = ",
    "2initial TSS                               = ",
    "2load image flags                          = ",
    "4load image memory requirement             = ",
    NULL
};

static  const_string_table rtp_blk_msg[] = {
    "2-MINREAL   switch value                   = ",
    "2-MAXREAL   switch value                   = ",
    "2-MINIBUF   switch value                   = ",
    "2-MAXIBUF   switch value                   = ",
    "2-NISTACK   switch value                   = ",
    "2-ISTKSIZE  switch value                   = ",
    "4-REALBREAK switch value                   = ",
    "2-CALLBUFS  switch value                   = ",
    "2386 DOS extender flags                    = ",
    "2unpriviledged flag                        = ",
    NULL
};

static  const_string_table tss_msg[] = {
    "2        LINK TO PREV TSS    = ",
    "2        zero                = ",
    "4        ESP0                = ",
    "2        SS0                 = ",
    "2        zero                = ",
    "4        ESP1                = ",
    "2        SS1                 = ",
    "2        zero                = ",
    "4        ESP2                = ",
    "2        SS2                 = ",
    "2        zero                = ",
    "4        CR3                 = ",
    "4        EIP                 = ",
    "4        EFLAGS              = ",
    "4        EAX                 = ",
    "4        ECX                 = ",
    "4        EDX                 = ",
    "4        EBX                 = ",
    "4        ESP                 = ",
    "4        EBP                 = ",
    "4        ESI                 = ",
    "4        EDI                 = ",
    "2        ES                  = ",
    "2        zero                = ",
    "2        CS                  = ",
    "2        zero                = ",
    "2        SS                  = ",
    "2        zero                = ",
    "2        DS                  = ",
    "2        zero                = ",
    "2        FS                  = ",
    "2        zero                = ",
    "2        GS                  = ",
    "2        zero                = ",
    "2        LDT                 = ",
    "2        zero                = ",
    "2        debug trap bit      = ",
    "2        I/O map base        = ",
    NULL
};

typedef struct {
    unsigned_16 seg_sel;
    unsigned_16 flags;
    unsigned_32 base_off;
    unsigned_32 min_extra;
} seg_info_rec;

/*
 * Dump the rex relocation table.
 */
static void dmp_rex_reloc( void )
/*******************************/
{
    unsigned_32     offset;
    unsigned_16     i;

    if( Options_dmp & FIX_DMP ) {
        Wdputslc( "\n" );
        Wlseek( Phar_head.reloc_offset );
        Banner( "Offset Relocation Table" );
        Wdputslc( "80000000H = 32-bits wide quantity to be fixed up\n" );
        Wdputslc( "     else   16-bits wide quantity to be fixed up\n" );
        for( i = 0; i < Phar_head.num_relocs; i++ ) {
            Wread( &offset, sizeof( unsigned_32 ) );
            if( i != 0 ) {
                if( (i) % 6 == 0 ) {
                    Wdputslc( "\n" );
                } else {
                    Wdputs( "      " );
                }
            }
            Puthex( offset, 8 );
        }
        Wdputslc( "\n" );
    }
}

/*
 * Dump the run-time parameter block.
 */
static void dmp_rtp_tbl( void )
/*****************************/
{
    rtpblock        rtp_blk;

    Wdputslc( "\n" );
    Wlseek( Phar_ext_head.rtp_offset );
    Wread( &rtp_blk, sizeof( rtpblock ) );
    if( rtp_blk.signature == RTP_SIGNATURE ) {
        Banner( "Run-time Parameter Block" );
        Dump_header( (char *)&rtp_blk.minreal, rtp_blk_msg );
        Wdputslc( "\n" );
    }
}

/*
 * Dump the run-time parameter block.
 */
static void dmp_seg_info_tbl( void )
/**********************************/
{
    seg_info_rec    seg_info;
    unsigned        i;

    if( Phar_ext_head.sit_size ) {
        Wlseek( Phar_ext_head.sit_offset );
        Banner( "Segment Information Table" );
        Wdputslc( "segment selector    flags       base offset     min extra bytes\n" );
        Wdputslc( "        =====       =====       =========       =========\n" );
        for( i = 0; i < Phar_ext_head.sit_size; i += sizeof( seg_info_rec ) ) {
            Wread( &seg_info, sizeof( seg_info_rec ) );
            Wdputs( "        " );
            Puthex( seg_info.seg_sel, 4 );
            Wdputs( "H       " );
            Puthex( seg_info.flags, 4 );
            Wdputs( "H       " );
            Puthex( seg_info.base_off, 8 );
            Wdputs( "H       " );
            Puthex( seg_info.min_extra, 8 );
            Wdputslc( "H\n" );
        }
    }
}

/*
 * Dump the run-time parameter block.
 */
static void dmp_reloc_tbl( void )
/*******************************/
{
    unsigned_32     offset;
    unsigned_16     segment;
    unsigned_32     i;

    if( (Options_dmp & FIX_DMP) && Phar_ext_head.reloc_size ) {
        Wlseek( Phar_ext_head.reloc_offset );
        Banner( "Program Relocation Table" );
        i = 0;
        while( i < Phar_ext_head.reloc_size ) {
            Wread( &offset, sizeof( unsigned_32 ) );
            Wread( &segment, sizeof( unsigned_16 ) );
            if( i != 0 ) {
                if( i % 30 == 0 ) {
                    Wdputslc( "\n" );
                } else {
                    Wdputs( "  " );
                }
            }
            Puthex( segment, 4 );
            Wdputc( ':' );
            Puthex( offset, 8 );
            i += sizeof( unsigned_32 ) + sizeof(unsigned_16);
        }
    }
}

/*
 * Dump the tss.
 */
static void dmp_tss( void )
/*************************/
{
    TSS             task_ss;
    unsigned_32     image;


    if( Phar_ext_head.tss_size ) {
        Wdputslc( "\n" );
        Wlseek( Phar_ext_head.tss_offset + Phar_ext_head.load_offset );
        Wread( &task_ss, sizeof( TSS ) );
        Banner( "Task State Segment" );
        Dump_header( (char *)&task_ss.backlink, tss_msg );
    }
    if( Options_dmp & (DOS_SEG_DMP | OS2_SEG_DMP) ) {
        image = Phar_ext_head.gdt_size + Phar_ext_head.ldt_size +
                                            Phar_ext_head.tss_size;
        Wdputslc( "\n" );
        Banner( "Segments" );
        Dmp_seg_data( Phar_ext_head.load_offset + image,
                                    Phar_ext_head.load_size - image );
    }
}

/*
 * Dump the gdt, ldt, idt.
 */
static void dmp_descriptor( unsigned_32 size )
/********************************************/
{
    descriptor      desc;
    unsigned_32     value;
    unsigned_32     limit;
    unsigned_32     base;
    unsigned        i;

    Wdputslc( "      seg base       seg limit      seg flg1     seg flg2\n" );
    Wdputslc( "      ========       =========      ========     ========\n" );
    for( i = 0; i < size; i += sizeof( descriptor ) ) {
        Wread( &desc, sizeof( descriptor ) );
        value = desc.bits2 & DESC_LIMIT_HIGH_MASK;
        limit = desc.limitlow | value << 16;
        value = desc.basemid;
        base = desc.baselow | value << 16;
        value = desc.basehigh;
        base = base | value << 24;
        Wdputs( "      " );
        Puthex( base, 8 );
        Wdputs( "H      " );
        Puthex( limit, 8 );
        Wdputs( "H        " );
        Puthex( desc.bits1, 2 );
        Wdputs( "H           " );
        Puthex( desc.bits2 >> 4, 1 );
        Wdputslc( "H\n" );
    }
}

/*
 * Dump the gdt, ldt, idt.
 */
static void dmp_dts( void )
/*************************/
{
    unsigned_32     size;

    size = Phar_ext_head.gdt_size;
    if( size ) {
        Wdputslc( "\n" );
        Banner( "GDT table" );
        Wlseek( Phar_ext_head.gdt_offset + Phar_ext_head.load_offset );
        dmp_descriptor( size );
    }
    size = Phar_ext_head.ldt_size;
    if( size ) {
        Wdputslc( "\n" );
        Banner( "LDT table" );
        Wlseek( Phar_ext_head.ldt_offset + Phar_ext_head.load_offset );
        dmp_descriptor( size );
    }
    size = Phar_ext_head.idt_size;
    if( size ) {
        Wdputslc( "\n" );
        Banner( "IDT table" );
        Wlseek( Phar_ext_head.idt_offset + Phar_ext_head.load_offset );
        dmp_descriptor( size );
    }
}

/*
 * Dump the Pharlap Executable Header, if any.
 */
bool Dmp_phar_head( void )
/************************/
{
    unsigned_32     offset;

    Wlseek( New_exe_off );
    Wread( &Phar_head, sizeof( simple_header ) );
    if( Phar_head.signature == SIMPLE_SIGNATURE ) {
        Banner( "Pharlap EXE Header" );
        Dump_header( (char *)&Phar_head.mod_size, phar_exe_msg );
    }
    if( Phar_head.signature == REX_SIGNATURE ) {
        Banner( "Pharlap REX Header" );
        Dump_header( (char *)&Phar_head.mod_size, phar_exe_msg );
        dmp_rex_reloc();
    }
    if( Phar_head.signature == SIMPLE_SIGNATURE ||
                            Phar_head.signature == REX_SIGNATURE ) {
        if( Options_dmp & (DOS_SEG_DMP | OS2_SEG_DMP) ) {
            offset = Phar_head.reloc_offset + Phar_head.num_relocs *
                                            sizeof( unsigned_32 );
            Wdputslc( "\n" );
            Banner( "Segments" );
            Dmp_seg_data( offset, Phar_head.mod_size + 512 *
                                    (Phar_head.file_size-1) - offset );
        }
        return( true );
    }
    Wlseek( New_exe_off );
    Wread( &Phar_ext_head, sizeof( Phar_ext_head.signature ) );
    if( Phar_ext_head.signature == EXTENDED_SIGNATURE ) {
        Wread( (char *)&Phar_ext_head + sizeof( Phar_ext_head.signature ),
            sizeof( extended_header ) - sizeof( Phar_ext_head.signature ) );
        Banner( "Pharlap EXE Extended Header" );
        Dump_header( (char *)&Phar_ext_head.format_level, phar_ext_msg );
        dmp_rtp_tbl();
        dmp_seg_info_tbl();
        dmp_reloc_tbl();
        dmp_dts();
        dmp_tss();
        return( true );
    }
    return( false );
}
