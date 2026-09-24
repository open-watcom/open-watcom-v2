/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NE/LE/LX dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stddef.h>

#include "wdglb.h"
#include "wdfunc.h"

/* Check for a very old NE module with shorter header. */
#define IS_OLD_NE( x )      (x.segment_off < offsetof( os2_exe_header, align ))

static  const_string_table os2_exe_msg[] = {
    "2link version                                         = ",
    "2offset of entry table                                = ",
    "2length of entry table                                = ",
    "432-bit chksum                                        = ",
    "2module flags                                         = ",
    "2segment number of auto data segment                  = ",
    "2initial size of local heap added to auto data seg    = ",
    "2initial size of stack to be added to auto data seg   = ",
    "2initial value of IP on entry                         = ",
    "2initial segment number for setting CS on entry       = ",
    "2initial value of SP on entry                         = ",
    "2segment number for setting SS on entry               = ",
    "2number of entries in segment table                   = ",
    "2number of entries in module reference table          = ",
    "2number of bytes in nonresident names table           = ",
    "2offset of beg of segment table (rel NH)              = ",
    "2offset of beg of resource table                      = ",
    "2offset of beg of resident names table                = ",
    "2offset of beg of module reference table              = ",
    "2offset of beg of imported names table                = ",
    "4offset of nonresident names table (rel file)         = ",
    "2number of movable entry points listed in entry table = ",
    NULL
};

static  const_string_table os2_exe_msg_new[] = {
    "2alignment shift count (0 => 9)                       = ",
    "2number of resource segments (OS/2 only)              = ",
    "1target OS (1==OS/2, 2==Windows, 3==DOS4, 4==Win386)  = ",
    "1other flags                                          = ",
    "2offset of gangload area  (Windows only)              = ",
    "2length of gangload area  (Windows only)              = ",
    "2swap area                (Windows only)              = ",
    "2expected Windows version (Windows only)              = ",
    NULL
};

static  const_string_table os2_386_msg[] = {
    "1byte order (0==little endian, 1==big endian)      = ",
    "1word order       \"                \"               = ",
    "4linear EXE format level                           = ",
    "2cpu type                                          = ",
    "2os type (1==OS/2, 2==Windows, 3==DOS4, 4==Win386) = ",
    "4module version                                    = ",
    "4module flags                                      = ",
    "4# module pages                                    = ",
    "4object # for initial EIP                          = ",
    "4initial EIP                                       = ",
    "4object # for initial ESP                          = ",
    "4initial ESP                                       = ",
    "4page size                                         = ",
    "4last page size (LE)/page shift (LX)               = ",
    "4fixup section size                                = ",
    "4fixup section checksum                            = ",
    "4loader section size                               = ",
    "4loader section checksum                           = ",
    "4object table offset                               = ",
    "4# of objects in module                            = ",
    "4object page map offset                            = ",
    "4object iterated data map offset                   = ",
    "4offset of resource table                          = ",
    "4# of resource entries                             = ",
    "4offset of resident name table                     = ",
    "4offset of entry table                             = ",
    "4offset of module directive table                  = ",
    "4# of module directives                            = ",
    "4offset of fixup page table                        = ",
    "4offset of fixup record table                      = ",
    "4offset of import module name table                = ",
    "4# of entries in import module name table          = ",
    "4offset of import procedure name table             = ",
    "4offset of per-page checksum table                 = ",
    "4offset of enumerated data pages                   = ",
    "4# of pre-load pages                               = ",
    "4offset of non-resident names table (rel file)     = ",
    "4size of non-resident names table                  = ",
    "4non-resident names table checksum                 = ",
    "4object # for automatic data object                = ",
    "4offset of the debugging information               = ",
    "4size of the debugging information                 = ",
    "4# of instance pages in the preload section        = ",
    "4# of instance pages in the demand load section    = ",
    "4size of heap (for 16-bit apps)                    = ",
    "4size of stack                                     = ",
    NULL
};

static  const_string_table os2_obj_msg[] = {
    "4virtual memory size             = ",
    "4          relocation base address          = ",
    "4          object flag bits                 = ",
    "4          object page table index          = ",
    "4          # of object page table entries   = ",
    "4          reserved                         = ",
    NULL
};

static  const_string_table map_flgs[] = {
    "Valid",
    "Iterated",
    "Invalid",
    "Zeroed",
    "Range",
    "Compressed",
    "Unknown",
    "Unknown",
    "M3-packed"
};


/*
 * dump the NE module flags word
 */
static void dmp_mod_flags_ne( unsigned_16 flags, unsigned_8 target )
/******************************************************************/
{
    char            buffer[512];
    char            *p;

    p = buffer;
    if( flags & OS2_IS_DLL ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "LIBRARY" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PROGRAM" );
    }
    if( target != TARGET_OS2
      && (flags & OS2_IS_DLL) ) {
        if( flags & WIN_PRIVATE_DLL ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "PRIVATEDLL" );
        }
    }
    if( flags & OS2_LINK_ERROR ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "LINKERRORSDETECTED" );
    }
    switch( flags & OS2_COMPATIBILITY_MASK ) {
    case OS2_NOT_PM_COMPATIBLE:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NOTWINDOWCOMPAT" );
        break;
    case OS2_PM_COMPATIBLE:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "WINDOWCOMPAT" );
        break;
    case OS2_PM_APP:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "WINDOWAPI" );
        break;
    }
    if( flags & OS2_NEEDS_MATH_CO ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NEEDFPU" );
    }
    if( target == TARGET_OS2
      && flags & OS2_NEEDS_80386 ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NEED386" );
    }
    if( target != TARGET_OS2
      && flags & WIN_EMS_GLOBAL_MEM ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "EMSGLOBAL" );
    }
    if( target == TARGET_OS2
      && flags & OS2_NEEDS_80286 ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NEED286" );
    }
    if( target != TARGET_OS2
      && flags & WIN_EMS_BANK_INSTANCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "EMSBANK" );
    }
    if( target == TARGET_OS2
      && flags & WIN_USES_EMS_DIRECT ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NONCONFORMING" );
    }
    if( target != TARGET_OS2
      && flags & WIN_USES_EMS_DIRECT ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "EMSDIRECT" );
    }
    if( flags & OS2_PROT_MODE_ONLY ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PROTMODEONLY" );
    }
    if( target == TARGET_OS2
      && flags & OS2_INIT_INSTANCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "INITINSTANCE" );
    }
    if( target != TARGET_OS2
      && flags & OS2_INIT_INSTANCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "REALMODE" );
    }
    if( target == TARGET_OS2
      && flags & OS2_IS_DLL ) {
        if( flags & WIN_PRIVATE_DLL ) {
            p += sprintf( p, GET_OR_FMT( p == buffer ), "PRIVATEDLL" );
        }
    }
    if( flags & OS2_MULT_AUTO ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "MULTIPLEDATA" );
    }
    if( flags & OS2_SINGLE_AUTO ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "SINGLEDATA" );
    }
    *p = '\0';
    Wdputs( buffer );
}


#define OSF_MODTYPE_MASK    0x38000UL
/*
 * dump the LE/LX module flags word
 */
static void dmp_mod_flags_lelx( unsigned_32 flags, unsigned_16 ostype )
/*********************************************************************/
{
    char            buffer[512];
    char            *p;

    /* unused parameters */ (void)ostype;

    p = buffer;
    if( (flags & OSF_MODTYPE_MASK) == OSF_VIRT_DEVICE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "VIRTDEVICE" );
    } else if( (flags & OSF_MODTYPE_MASK) == OSF_PHYS_DEVICE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PHYSDEVICE" );
    } else if( (flags & OSF_MODTYPE_MASK) == OSF_IS_DLL ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "LIBRARY" );
    } else if( Options_dmp & ZERO_BITS ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PROGRAM" );
    }
    if( flags & OSF_SINGLE_DATA ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "SINGLEDATA" );
    }
    if( flags & OSF_INIT_INSTANCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "INITINSTANCE" );
    }
    if( flags & OSF_TERM_INSTANCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "TERMINSTANCE" );
    }
    if( flags & OSF_IS_PROT_DLL ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PROTDLL" );
    }
    if( flags & OSF_INTERNAL_FIXUPS_DONE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NO_INT_FIXUPS" );
    }
    if( flags & OSF_EXTERNAL_FIXUPS_DONE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NO_EXT_FIXUPS" );
    }
    if( flags & OSF_LINK_ERROR ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "LINKERRORSDETECTED" );
    }
    switch( flags & OS2_COMPATIBILITY_MASK ) {
    case OSF_NOT_PM_COMPATIBLE:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "NOTWINDOWCOMPAT" );
        break;
    case OSF_PM_COMPATIBLE:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "WINDOWCOMPAT" );
        break;
    case OSF_PM_APP:
        p += sprintf( p, GET_OR_FMT( p == buffer ), "WINDOWAPI" );
        break;
    }
    if( flags & 0x80000 ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "MPUNSAFE" );
    }
    *p = '\0';
    Wdputs( buffer );
}


/*
 * Dump the New Executable Header, if any.
 */
bool Dmp_os2_head( void )
/***********************/
{
    Wlseek( New_exe_off );
    Wread( &Os2_head, sizeof( os2_exe_header ) );
    if( Os2_head.signature != EXESIGN_NE ) {
        return( false );
    }
    if( IS_OLD_NE( Os2_head )
      || Os2_head.align == 0 ) {
        Os2_head.align = 9;
    }
    Banner( "New EXE Header (OS/2 or Windows)" );
    Wdputs( "file offset = " );
    Puthex( New_exe_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    Dump_header( (char *)&Os2_head.version, os2_exe_msg, 4 );
    if( !IS_OLD_NE( Os2_head ) ) {
        Dump_header( (char *)&Os2_head.align, os2_exe_msg_new, 4 );
    }
    Wdputslc( "\n" );
    Wdputs( "Module flags = " );
    Puthex( Os2_head.info, 4 );
    Wdputs( ": " );
    dmp_mod_flags_ne( Os2_head.info, Os2_head.target );
    Wdputslc( "\n" );
    Wdputslc( "\n" );
    Dmp_seg_tab();
    Dmp_resrc_tab_ne();
    Dmp_tables_ne();
    if( Options_dmp & OS2_SEG_DMP ) {
        Dmp_segments();
    }
    if( Int_seg_tab != NULL ) {
        free( Int_seg_tab );
    }
    return( true );
}

/*
 * dump the object page table
 */
static void dmp_obj_page( object_record obj )
/*******************************************/
{
    unsigned_32     j;
    unsigned_32     offset;
    unsigned_32     file_ofs;
    map_entry       map;

    if( Form == FORM_LX ) {
        offset = ( obj.mapidx - 1 ) * sizeof( lx_map_entry );
    } else {
        offset = ( obj.mapidx - 1 ) * sizeof( le_map_entry );
    }
    offset += New_exe_off + Os2_386_head.objmap_off;
    for( j = 0; j < obj.mapsize; ++j ) {
        Data_count++;
        Wdputs( "    page # " );
        Putdecbz( Data_count, 3 );
        Wdputs( "  map page = " );
        switch( Form ) {
        case FORM_LE:
            Wlseek( offset );
            Wread( &map, sizeof( le_map_entry ) );
            offset += sizeof( le_map_entry );
            Puthex( map.le.page_num[0], 2 );
            Puthex( map.le.page_num[1], 2 );
            Puthex( map.le.page_num[2], 2 );
            Wdputs( "H file ofs = " );
            file_ofs = map.le.page_num[2] + (map.le.page_num[1] << 8) + ((unsigned_32)map.le.page_num[0] << 16);
            if( file_ofs )
                file_ofs = ((file_ofs - 1) * Os2_386_head.page_size) + Os2_386_head.page_off;
            Puthex( file_ofs, 8 );
            Wdputs( "H flgs = " );
            Puthex( map.le.flags, 2 );
            Wdputs( "H " );
            Wdputs( map_flgs[ map.le.flags ] );
            if( Options_dmp & OS2_SEG_DMP ) {
                Dmp_page_seg_le();
            }
            break;
        case FORM_LX:
            Wlseek( offset );
            Wread( &map, sizeof( lx_map_entry ) );
            offset += sizeof( lx_map_entry );
            Puthex( map.lx.page_offset, 8 );
            Wdputs( "H size = " );
            Puthex( map.lx.data_size, 4 );
            Wdputs( "H flgs = " );
            Puthex( map.lx.flags, 2 );
            Wdputs( "H " );
            Wdputs( map_flgs[ map.lx.flags ] );
            if( map.lx.flags < ARRAY_SIZE( map_flgs ) ) {
                Wdputs( map_flgs[map.lx.flags] );
            } else {
                Wdputs( "Unknown" );
            }
            if( Options_dmp & OS2_SEG_DMP ) {
                Dmp_page_seg_lx( map );
            }
            break;
        }
        Wdputslc( "\n" );
    }
}

/*
 * dump the object flags word
 */
static void dmp_obj_flags( unsigned_32 flags )
/********************************************/
{
    char            buffer[256];
    char            *p;

    p = buffer;
    if( flags & OBJ_READABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "READABLE" );
    }
    if( flags & OBJ_WRITEABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "WRITABLE" );
    }
    if( flags & OBJ_EXECUTABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "EXECUTABLE" );
    }
    if( flags & OBJ_RESOURCE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "RESOURCE" );
    }
    if( flags & OBJ_DISCARDABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "DISCARDABLE" );
    }
    if( flags & OBJ_SHARABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "SHARED" );
    }
    if( flags & OBJ_HAS_PRELOAD ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PRELOAD" );
    }
    if( flags & OBJ_HAS_INVALID ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "INVALID" );
    }
    if( flags & OBJ_PERM_SWAPPABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "SWAPPABLE" );
    }
    if( flags & OBJ_PERM_RESIDENT ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "RESIDENT" );
    }
    if( flags & OBJ_PERM_LOCKABLE ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "PERM_LOCKABLE" );
    }
    if( flags & OBJ_ALIAS_REQUIRED ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "16:16_ALIAS" );
    }
    if( flags & OBJ_BIG ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "BIG" );
    }
    if( flags & OBJ_CONFORMING ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "CONFORMING" );
    }
    if( flags & OBJ_IOPL ) {
        p += sprintf( p, GET_OR_FMT( p == buffer ), "IOPL" );
    }
    *p = '\0';
    Wdputs( buffer );
}

/*
 * dump the LE/LX object table
 */
static void dmp_obj_table( void )
/*******************************/
{
    unsigned_16     i;
    object_record   os_obj;

    Banner( "Object Table" );
    for( i = 0; i < Os2_386_head.num_objects; i++ ) {
        Wlseek( New_exe_off + Os2_386_head.objtab_off
                            + i * sizeof( object_record ) );
        Wread( &os_obj, sizeof( object_record ) );
        Wdputs( "object " );
        Putdecbz( i + 1, 2 );
        Wdputs( ": " );
        Dump_header( &os_obj.size, os2_obj_msg, 4 );
        Wdputs( "          flags = " );
        Puthex( os_obj.flags, 8 );
        Wdputs( ": " );
        dmp_obj_flags( os_obj.flags );
        Wdputslc( "\n" );
        if( Options_dmp & PAGE_DMP ) {
            dmp_obj_page( os_obj );
        }
        Wdputslc( "\n" );
    }
}

/*
 * Dump the 386 Executable Header, if any.
 */
bool Dmp_386_head( void )
/***********************/
{
    unsigned_16             signature;

    Wlseek( New_exe_off );
    Wread( &signature, sizeof( signature ) );
    if( signature == EXESIGN_LE ) {
        Form = FORM_LE;
        Banner( "Linear EXE Header (OS/2 V2.x) - LE" );
    } else if( signature == EXESIGN_LX ) {
        Form = FORM_LX;
        Banner( "Linear EXE Header (OS/2 V2.x) - LX" );
    } else {
        return( false );
    }
    Wlseek( New_exe_off );
    Wread( &Os2_386_head, sizeof( Os2_386_head ) );
    Wdputs( "file offset = " );
    Puthex( New_exe_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    Dump_header( (char *)&Os2_386_head.byte_order, os2_386_msg, 4 );
    Wdputslc( "\n" );
    Wdputs( "Module flags = " );
    Puthex( Os2_386_head.flags, 8 );
    Wdputs( ": " );
    dmp_mod_flags_lelx( Os2_386_head.flags, Os2_386_head.os_type );
    Wdputslc( "\n" );
    Wdputslc( "\n" );
    dmp_obj_table();
    Dmp_resrc_tab_lelx();
    Dmp_tables_lelx();
    return( true );
}
