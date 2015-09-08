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
* Description:  Executable dumper internal interface.
*
****************************************************************************/


/*
 * os2exe.c
 */
extern bool Dmp_os2_head( void );
extern bool Dmp_386_head( void );

/*
 * peexe.c
 */
extern bool Dmp_pe_head( void );
extern void dmp_objects( unsigned );
extern void DumpCoffHdrFlags( unsigned_16 );
extern bool Dmp_pe_tab( void );

/*
 * novexe.c
 */
extern bool Dmp_nlm_head( void );

/*
 * pharexe.c
 */
extern bool Dmp_phar_head( void );

/*
 * qnxexe.c
 */
extern bool Dmp_qnx_head( void );

/*
 * elfexe.c
 */
extern bool Dmp_elf_head( void );
extern bool Dmp_elf_header( unsigned long );
extern bool Dmp_lib_head( void );

/*
 * machoexe.c
 */
extern bool Dmp_macho_header( unsigned long );
extern bool Dmp_macho_head( void );

/*
 * wdwarf.c
 */
extern bool Dmp_dwarf( void );
extern bool Dmp_mdbg_head( void );

/*
 * wsect.c
 */
extern const char *Getname( uint_32, readable_name *, size_t );
extern uint_8 *DecodeULEB128( const uint_8 *, uint_32 * );
extern uint_8 *DecodeSLEB128( const uint_8 *, int_32 * );
uint_8 *find_abbrev( uint_32 start, uint_32 code );
extern void dump_abbrevs( const uint_8 *input, uint length );
extern void Dump_specific_section( uint, const uint_8 *, uint );
extern void Dump_all_sections( void );
extern void Dump_lines( const uint_8 *, uint );
extern uint Lookup_section_name( const char * );

/*
 * dumpwv.c
 */
extern void Dump_section( void );
extern void Get_local_name( char *, unsigned_8 *, unsigned_8 * );
extern unsigned_8 *Get_type_index( unsigned_8 *, unsigned_16 * );
extern unsigned_8 *Dump_location_expression( unsigned_8 *, char * );

/*
 * typewv.c
 */
extern void Dmp_type( int, unsigned_32 * );

/*
 * wpetbls.c
 */
extern void Dmp_exports( void );

extern void Dmp_imports( void );
extern void Dmp_exp_tab( void );

/*
 * wperes.c
 */
extern void Dmp_fixups( void );
extern void Dmp_resources( void );

/*
 * wdseg.c
 */
extern void Dmp_seg_tab( void );

/*
 * wdtab.c
 */
extern void Dmp_ne_tbls( void );
extern void Dmp_le_lx_tbls( void );
extern void Dmp_ordinal( unsigned_16 );
extern bool Dmp_os2_exports( void );

/*
 * wdfix.c
 */
extern void Dmp_fixrec_tab( unsigned_32 );
extern void Dmp_fixpage_tab( unsigned_32, unsigned_32 );

/*
 * wdata.c
 */
extern void Dmp_relocs( void );
extern void Dmp_segments( void );
extern void Dmp_le_page_seg( void );
extern void Dmp_lx_page_seg( map_entry );
extern void Dmp_seg_data( unsigned long, unsigned long );
extern void dmp_mult_data_line( char *, unsigned_16, unsigned_16 );
extern void dmp_data_line( char *, unsigned_16, unsigned_16 );

/*
 * wdprs.c
 */
extern void Puthex( unsigned_32, unsigned_16 );
extern void Putdec( unsigned_32 );
extern void Putdecs( signed_32 );
extern void Putdecl( unsigned_32, unsigned_16 );
extern void Putdecsl( signed_32, unsigned_16 );
extern void Putdecbz( unsigned_32, unsigned_16);
extern void Putdecsbz( signed_32, unsigned_16);
extern void Parse_option( void );
extern uint_32 get_u32( uint_32 *src );
extern int_32  get_s32( int_32 *src );
extern uint_16 get_u16( uint_16 *src );
extern int_16  get_s16( int_16 *src );

/*
 * dosexe.c
 */
extern bool Dmp_dos_head( void );

/*
 * d16mexe.c
 */
extern bool Dmp_d16m_head( void );

/*
 * wdio.c
 */
extern bool             Weof( void );
extern void             Wread( void *, unsigned_32 );
extern void             Wlseek( unsigned long );
extern unsigned long    WFileSize( void );
extern void             Wdputc( char );
extern void             Wdputs( const char * );
extern void             Wdputslc( const char * );
extern void             Dump_header( void *, const char * const * );
extern void             Dump_asciiz( unsigned long offset );
extern void             DumpFlags( unsigned_32, unsigned_32, const char * const *, const char * );
extern void             *Wmalloc( size_t );
extern void             Banner( const char * );

/*
 * wdres.c
 */
extern void Dmp_resrc_tab( void );
extern void Dmp_resrc2_tab( void );

/*
 * coff.c
 */
extern bool Dmp_coff_head( void );
extern bool Dmp_ar_head( void );
extern const char *Coff_obj_name( const char * );

/*
 * dumpcv.c
 */
extern bool Dmp_cv_head( void );

/*
 * dumphll.c
 */
extern bool Dmp_hll_head( void );
