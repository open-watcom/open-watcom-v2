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
* Description:  Entry points for ORL routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "orl.h"
#include "orllevel.h"
#include "orlentry.h"
#include "orlflhnd.h"
#include "pcobj.h"

orl_handle ORLENTRY ORLInit( orl_funcs *funcs )
/*********************************************/
{
    orl_handle                  orl_hnd;

    orl_hnd = (orl_handle)ORL_CLI_ALLOC( funcs, sizeof( ORL_LCL_STRUCT( orl_handle ) ) );
    if( orl_hnd != NULL ) {
        LCL_ORL_HND( orl_hnd )->error = ORL_OKAY;
        LCL_ORL_HND( orl_hnd )->funcs = *funcs;
        LCL_ORL_HND( orl_hnd )->elf_hnd = ElfInit( &LCL_ORL_HND( orl_hnd )->funcs );
        if( LCL_ORL_HND( orl_hnd )->elf_hnd == NULL ) {
            ORL_CLI_FREE( funcs, (void *)orl_hnd );
            return( NULL );
        }
        LCL_ORL_HND( orl_hnd )->coff_hnd = CoffInit( &LCL_ORL_HND( orl_hnd )->funcs );
        if( LCL_ORL_HND( orl_hnd )->coff_hnd == NULL ) {
            ORL_CLI_FREE( funcs, (void *)orl_hnd );
            return( NULL );
        }
        LCL_ORL_HND( orl_hnd )->omf_hnd = OmfInit( &LCL_ORL_HND( orl_hnd )->funcs );
        if( LCL_ORL_HND( orl_hnd )->omf_hnd == NULL ) {
            ORL_CLI_FREE( funcs, (void *)orl_hnd );
            return( NULL );
        }
        LCL_ORL_HND( orl_hnd )->first_file_hnd = NULL;
    }
    return( orl_hnd );
}

orl_return ORLENTRY ORLGetError( orl_handle orl_hnd )
/***************************************************/
{
    return( LCL_ORL_HND( orl_hnd )->error );
}

orl_return ORLENTRY ORLFini( orl_handle orl_hnd )
/***********************************************/
{
    orl_return      return_val;

    if( ( return_val = ElfFini( LCL_ORL_HND( orl_hnd )->elf_hnd ) ) != ORL_OKAY )
        return( return_val );
    if( ( return_val = CoffFini( LCL_ORL_HND( orl_hnd )->coff_hnd ) ) != ORL_OKAY )
        return( return_val );
    if( ( return_val = OmfFini( LCL_ORL_HND( orl_hnd )->omf_hnd ) ) != ORL_OKAY )
        return( return_val );
    while( LCL_ORL_HND( orl_hnd )->first_file_hnd != NULL ) {
        return_val = ORLRemoveFileLinks( LCL_ORL_HND( orl_hnd )->first_file_hnd );
        if( return_val != ORL_OKAY ) {
            return( return_val );
        }
    }
    ORL_FUNCS_FREE( LCL_ORL_HND( orl_hnd ), (void *)orl_hnd );
    return( ORL_OKAY );
}

orl_file_format ORLFileIdentify( orl_handle orl_hnd, FILE *fp )
/*************************************************************/
{
    unsigned char       *magic;
    unsigned_16         machine_type;
    unsigned_16         offset;
    unsigned_16         len;
    unsigned char       chksum;

    magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, 4 );
    if( magic == NULL ) {
        return( ORL_UNRECOGNIZED_FORMAT );
    }
    if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, -4, SEEK_CUR ) ) {
        return( ORL_UNRECOGNIZED_FORMAT );
    }
    if( magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F' ) {
        return( ORL_ELF );
    }

    // See if this is the start of an OMF object file
    // the first record must be a THEADR record and we check that it is
    // valid, if it is then we assume that this is an OMF object file.
    if( magic[0] == CMD_THEADR ) {
        len = magic[1] | ( magic[2] << 8 );
        len -= magic[3];
        len -= 2;
        if( len == 0 ) {
            // This looks good so far, we must now check the record
            len = magic[3] + 1;
            if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, 4, SEEK_CUR ) ) {
                return( ORL_UNRECOGNIZED_FORMAT );
            }
            chksum = magic[0] + magic[1] + magic[2] + magic[3];
            magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, len );
            if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, -(long)( 4 + len ), SEEK_CUR ) ) {
                return( ORL_UNRECOGNIZED_FORMAT );
            }
            if( magic != NULL ) {
                // Go on to check record checksum
                while( len ) {
                    chksum += *magic;
                    len--;
                    magic++;
                }
                magic--;
                if( *magic == 0 || chksum == 0 ) {
                    // seems to be a correct OMF record to start the OBJ
                    return( ORL_OMF );
                }
            } else {
                magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, 4 );
                if( magic == NULL ) {
                    return( ORL_UNRECOGNIZED_FORMAT );
                } else if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, -4, SEEK_CUR ) ) {
                    return( ORL_UNRECOGNIZED_FORMAT );
                }
            }
        }
    }

    machine_type = *(unsigned_16 *)magic;
    switch( machine_type ) {
    case COFF_IMAGE_FILE_MACHINE_I860:
    case COFF_IMAGE_FILE_MACHINE_I386A:
    case COFF_IMAGE_FILE_MACHINE_I386:
    case COFF_IMAGE_FILE_MACHINE_R3000:
    case COFF_IMAGE_FILE_MACHINE_R4000:
    case COFF_IMAGE_FILE_MACHINE_ALPHA:
    case COFF_IMAGE_FILE_MACHINE_POWERPC:
    case COFF_IMAGE_FILE_MACHINE_AMD64:
    case COFF_IMAGE_FILE_MACHINE_UNKNOWN:
        return( ORL_COFF );
    }
    // Is it PE?
    if( magic[0] == 'M' && magic[1] == 'Z' ) {
        if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, 0x3c, SEEK_CUR ) ) {
            return( ORL_UNRECOGNIZED_FORMAT );
        }
        magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, 0x4 );
        if( magic == NULL ) {
            return( ORL_UNRECOGNIZED_FORMAT );
        }
        offset = *(unsigned_16 *)magic;
        if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, offset - 0x40, SEEK_CUR ) ) {
            return( ORL_UNRECOGNIZED_FORMAT );
        }
        magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, 4 );
        if( magic == NULL ) {
            return( ORL_UNRECOGNIZED_FORMAT );
        }
        if( magic[0]=='P' && magic[1] == 'E' && magic[2] == '\0' && magic[3] == '\0' ) {
            magic = ORL_FUNCS_READ( LCL_ORL_HND( orl_hnd ), fp, 4 );
            if( magic == NULL ) {
                return( ORL_UNRECOGNIZED_FORMAT );
            }
            if( ORL_FUNCS_SEEK( LCL_ORL_HND( orl_hnd ), fp, -(long)(offset+8), SEEK_CUR ) ) {
                return( ORL_UNRECOGNIZED_FORMAT );
            }
            machine_type = *(unsigned_16 *)magic;
            switch( machine_type ) {
            case COFF_IMAGE_FILE_MACHINE_I860:
            case COFF_IMAGE_FILE_MACHINE_I386:
            case COFF_IMAGE_FILE_MACHINE_R3000:
            case COFF_IMAGE_FILE_MACHINE_R4000:
            case COFF_IMAGE_FILE_MACHINE_ALPHA:
            case COFF_IMAGE_FILE_MACHINE_POWERPC:
            case COFF_IMAGE_FILE_MACHINE_AMD64:
                return( ORL_COFF );
            }
        }
    }
    return( ORL_UNRECOGNIZED_FORMAT );
}

orl_file_handle ORLENTRY ORLFileInit( orl_handle orl_hnd, FILE *fp, orl_file_format type )
{
    orl_file_handle     orl_file_hnd;

    switch( type ) {
    case( ORL_ELF ):
    case( ORL_COFF ):
    case( ORL_OMF ):
        orl_file_hnd = (orl_file_handle)ORL_FUNCS_ALLOC( LCL_ORL_HND( orl_hnd ), sizeof( ORL_LCL_STRUCT( orl_file_handle ) ) );
        if( orl_file_hnd == NULL ) {
            LCL_ORL_HND( orl_hnd )->error = ORL_OUT_OF_MEMORY;
            break;
        }
        LCL_FIL_HND( orl_file_hnd )->type = type;
        switch( type ) {
        case ORL_ELF:
            LCL_ORL_HND( orl_hnd )->error = ElfFileInit( LCL_ORL_HND( orl_hnd )->elf_hnd, fp, &LCL_FIL_HND( orl_file_hnd )->file_hnd.elf );
            break;
        case ORL_COFF:
            LCL_ORL_HND( orl_hnd )->error = CoffFileInit( LCL_ORL_HND( orl_hnd )->coff_hnd, fp, &LCL_FIL_HND( orl_file_hnd )->file_hnd.coff );
            break;
        case ORL_OMF:
            LCL_ORL_HND( orl_hnd )->error = OmfFileInit( LCL_ORL_HND( orl_hnd )->omf_hnd, fp, &LCL_FIL_HND( orl_file_hnd )->file_hnd.omf );
            break;
        default:
            break;
        }
        if( LCL_ORL_HND( orl_hnd )->error != ORL_OKAY ) {
            ORL_FUNCS_FREE( LCL_ORL_HND( orl_hnd ), (void *)orl_file_hnd );
            break;
        }
        ORLAddFileLinks( LCL_ORL_HND( orl_hnd ), LCL_FIL_HND( orl_file_hnd ) );
        return( orl_file_hnd );
    default:    //ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( NULL );
}

orl_return ORLENTRY ORLFileFini( orl_file_handle orl_file_hnd )
{
    orl_return          return_val;

    /* jump table replace: */
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return_val = ElfFileFini( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf );
        break;
    case( ORL_COFF ):
        return_val = CoffFileFini( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff );
        break;
    case( ORL_OMF ):
        return_val = OmfFileFini( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf );
        break;
    default:    // ORL_UNRECOGNIZED_FORMAT
        return( ORL_ERROR );
    }

    if( return_val != ORL_OKAY )
        return( return_val );
    return( ORLRemoveFileLinks( LCL_FIL_HND( orl_file_hnd ) ) );
}


orl_rva ORLENTRY ORLExportTableRVA( orl_file_handle orl_file_hnd )
{
    orl_rva     rva = 0;

    if( LCL_FIL_HND( orl_file_hnd )->type == ORL_COFF ) {
        rva = CoffExportTableRVA( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff );
    }

    return( rva );
}

orl_return ORLENTRY ORLFileScan( orl_file_handle orl_file_hnd, const char *desired, orl_sec_return_func return_func )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfFileScan( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf, desired, return_func ) );
    case( ORL_COFF ):
        return( CoffFileScan( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff, desired, return_func ) );
    case( ORL_OMF ):
        return( OmfFileScan( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf, desired, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

orl_machine_type ORLENTRY ORLFileGetMachineType( orl_file_handle orl_file_hnd )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfFileGetMachineType( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf ) );
    case( ORL_COFF ):
        return( CoffFileGetMachineType( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff ) );
    case( ORL_OMF ):
        return( OmfFileGetMachineType( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_file_flags ORLENTRY ORLFileGetFlags( orl_file_handle orl_file_hnd )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfFileGetFlags( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf ) );
    case( ORL_COFF ):
        return( CoffFileGetFlags( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff ) );
    case( ORL_OMF ):
        return( OmfFileGetFlags( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_file_size ORLENTRY ORLFileGetSize( orl_file_handle orl_file_hnd )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfFileGetSize( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf ) );
    case( ORL_COFF ):
        return( CoffFileGetSize( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff ) );
    case( ORL_OMF ):
        return( OmfFileGetSize( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_file_type ORLENTRY ORLFileGetType( orl_file_handle orl_file_hnd )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfFileGetType( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf ) );
    case( ORL_COFF ):
        return( CoffFileGetType( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff ) );
    case( ORL_OMF ):
        return( OmfFileGetType( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_file_format ORLENTRY ORLFileGetFormat( orl_file_handle orl_file_hnd )
{
    return( LCL_FIL_HND( orl_file_hnd )->type );
}

orl_sec_handle ORLENTRY ORLFileGetSymbolTable( orl_file_handle orl_file_hnd )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfFileGetSymbolTable( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffFileGetSymbolTable( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfFileGetSymbolTable( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

const char * ORLENTRY ORLSecGetName( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetName( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffSecGetName( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetName( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( NULL );
}

orl_return ORLENTRY ORLSecGetBase( orl_sec_handle orl_sec_hnd, orl_sec_base *sec_base )
{
    if( sec_base != NULL ) {
        switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
        case( ORL_ELF ):
            return( ElfSecGetBase( (elf_sec_handle)orl_sec_hnd, sec_base ) );
        case( ORL_COFF ):
            sec_base->u._32[I64HI32] = 0;
            sec_base->u._32[I64LO32] = CoffSecGetBase( (coff_sec_handle)orl_sec_hnd );
            return( ORL_OKAY );
        case( ORL_OMF ):
            sec_base->u._32[I64HI32] = 0;
            sec_base->u._32[I64LO32] = OmfSecGetBase( (omf_sec_handle)orl_sec_hnd );
            return( ORL_OKAY );
        default:    // ORL_UNRECOGNIZED_FORMAT
            break;
        }
        sec_base->u._32[I64HI32] = 0;
        sec_base->u._32[I64LO32] = 0;
    }
    return( ORL_ERROR );
}

orl_sec_size ORLENTRY ORLSecGetSize( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetSize( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffSecGetSize( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetSize( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_type ORLENTRY ORLSecGetType( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetType( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffSecGetType( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetType( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_alignment ORLENTRY ORLSecGetAlignment( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetAlignment( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffSecGetAlignment( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetAlignment( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_flags ORLENTRY ORLSecGetFlags( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetFlags( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffSecGetFlags( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetFlags( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_handle ORLENTRY ORLSecGetStringTable( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfSecGetStringTable( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffSecGetStringTable( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfSecGetStringTable( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

orl_sec_handle ORLENTRY ORLSecGetSymbolTable( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfSecGetSymbolTable( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffSecGetSymbolTable( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfSecGetSymbolTable( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

orl_sec_handle ORLENTRY ORLSecGetRelocTable( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfSecGetRelocTable( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffSecGetRelocTable( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfSecGetRelocTable( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

orl_linnum ORLENTRY ORLSecGetLines( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( NULL );
    case( ORL_COFF ):
        return( CoffSecGetLines( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetLines( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( NULL );
}

orl_table_index ORLENTRY ORLSecGetNumLines( orl_sec_handle orl_sec_hnd )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( 0 );
    case( ORL_COFF ):
        return( CoffSecGetNumLines( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfSecGetNumLines( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_offset ORLENTRY ORLSecGetOffset( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_COFF ) {
        return( CoffSecGetOffset( (coff_sec_handle)orl_sec_hnd ) );
    }
    return( 0 );
}

const char * ORLENTRY ORLSecGetClassName( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_OMF ) {
        return( OmfSecGetClassName( (omf_sec_handle)orl_sec_hnd ) );
    }
    return( 0 );
}

orl_sec_combine ORLENTRY ORLSecGetCombine( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_OMF ) {
        return( OmfSecGetCombine( (omf_sec_handle)orl_sec_hnd ) );
    }
    return( ORL_SEC_COMBINE_NONE );
}

orl_sec_frame ORLENTRY ORLSecGetAbsFrame( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_OMF ) {
        return( OmfSecGetAbsFrame( (omf_sec_handle)orl_sec_hnd ) );
    }
    return( ORL_SEC_NO_ABS_FRAME );
}

orl_sec_handle ORLENTRY ORLSecGetAssociated( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_OMF ) {
        return( OmfSecGetAssociated( (omf_sec_handle)orl_sec_hnd ) );
    }
    return( ORL_NULL_HANDLE );
}

orl_group_handle ORLENTRY ORLSecGetGroup( orl_sec_handle orl_sec_hnd )
{
    if( LCL_SEC_HND( orl_sec_hnd )->type == ORL_OMF ) {
        return( OmfSecGetGroup( (omf_sec_handle)orl_sec_hnd ) );
    }
    return( ORL_NULL_HANDLE );
}

orl_return ORLENTRY ORLSecGetContents( orl_sec_handle orl_sec_hnd, unsigned char **buffer )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecGetContents( (elf_sec_handle)orl_sec_hnd, buffer ) );
    case( ORL_COFF ):
        return( CoffSecGetContents( (coff_sec_handle)orl_sec_hnd, buffer ) );
    case( ORL_OMF ):
        return( OmfSecGetContents( (omf_sec_handle)orl_sec_hnd, buffer ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

orl_return ORLENTRY ORLSecQueryReloc( orl_sec_handle orl_sec_hnd, orl_sec_offset sec_offset, orl_reloc_return_func return_func )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecQueryReloc( (elf_sec_handle)orl_sec_hnd, sec_offset, return_func ) );
    case( ORL_COFF ):
        return( CoffSecQueryReloc( (coff_sec_handle)orl_sec_hnd, sec_offset, return_func ) );
    case( ORL_OMF ):
        return( OmfSecQueryReloc( (omf_sec_handle)orl_sec_hnd, sec_offset, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

orl_return ORLENTRY ORLSecScanReloc( orl_sec_handle orl_sec_hnd, orl_reloc_return_func return_func )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSecScanReloc( (elf_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_COFF ):
        return( CoffSecScanReloc( (coff_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_OMF ):
        return( OmfSecScanReloc( (omf_sec_handle)orl_sec_hnd, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}


orl_table_index ORLENTRY ORLCvtSecHdlToIdx( orl_sec_handle orl_sec_hnd )
/**********************************************************************/
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfCvtSecHdlToIdx( (elf_sec_handle)orl_sec_hnd ) );
    case( ORL_COFF ):
        return( CoffCvtSecHdlToIdx( (coff_sec_handle)orl_sec_hnd ) );
    case( ORL_OMF ):
        return( OmfCvtSecHdlToIdx( (omf_sec_handle)orl_sec_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_handle ORLENTRY ORLCvtIdxToSecHdl( orl_file_handle orl_file_hnd, orl_table_index idx )
/********************************************************************************************/
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfCvtIdxToSecHdl( LCL_FIL_HND( orl_file_hnd )->file_hnd.elf, idx ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffCvtIdxToSecHdl( LCL_FIL_HND( orl_file_hnd )->file_hnd.coff, idx ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfCvtIdxToSecHdl( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf, idx ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

orl_return ORLENTRY ORLRelocSecScan( orl_sec_handle orl_sec_hnd, orl_reloc_return_func return_func )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfRelocSecScan( (elf_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_COFF ):
        return( CoffRelocSecScan( (coff_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_OMF ):
        return( OmfRelocSecScan( (omf_sec_handle)orl_sec_hnd, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

orl_return ORLENTRY ORLSymbolSecScan( orl_sec_handle orl_sec_hnd, orl_symbol_return_func return_func )
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSymbolSecScan( (elf_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_COFF ):
        return( CoffSymbolSecScan( (coff_sec_handle)orl_sec_hnd, return_func ) );
    case( ORL_OMF ):
        return( OmfSymbolSecScan( (omf_sec_handle)orl_sec_hnd, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

orl_return ORLENTRY ORLNoteSecScan( orl_sec_handle orl_sec_hnd, orl_note_callbacks *cbs, void *cookie )
/*****************************************************************************************************/
{
    switch( LCL_SEC_HND( orl_sec_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfNoteSecScan( (elf_sec_handle)orl_sec_hnd, cbs, cookie ) );
    case( ORL_COFF ):
        return( CoffNoteSecScan( (coff_sec_handle)orl_sec_hnd, cbs, cookie ) );
    case( ORL_OMF ):
        return( OmfNoteSecScan( (omf_sec_handle)orl_sec_hnd, cbs, cookie ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}


const char * ORLENTRY ORLSymbolGetName( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSymbolGetName( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( CoffSymbolGetName( (coff_symbol_handle)orl_symbol_hnd ) );
    case( ORL_OMF ):
        return( OmfSymbolGetName( (omf_symbol_handle)orl_symbol_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( NULL );
}

orl_return ORLENTRY ORLSymbolGetValue( orl_symbol_handle orl_symbol_hnd, orl_symbol_value *sym_value )
{
    if( sym_value != NULL ) {
        switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
        case( ORL_ELF ):
            return( ElfSymbolGetValue( (elf_symbol_handle)orl_symbol_hnd, sym_value ) );
        case( ORL_COFF ):
            sym_value->u._32[I64HI32] = 0;
            sym_value->u._32[I64LO32] = CoffSymbolGetValue( (coff_symbol_handle)orl_symbol_hnd );
            return( ORL_OKAY );
        case( ORL_OMF ):
            sym_value->u._32[I64HI32] = 0;
            sym_value->u._32[I64LO32] = OmfSymbolGetValue( (omf_symbol_handle)orl_symbol_hnd );
            return( ORL_OKAY );
        default:     //ORL_UNRECOGNIZED_FORMAT
            break;
        }
        sym_value->u._32[I64HI32] = 0;
        sym_value->u._32[I64LO32] = 0;
    }
    return( ORL_ERROR );
}

orl_symbol_binding ORLENTRY ORLSymbolGetBinding( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSymbolGetBinding( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( CoffSymbolGetBinding( (coff_symbol_handle)orl_symbol_hnd ) );
    case( ORL_OMF ):
        return( OmfSymbolGetBinding( (omf_symbol_handle)orl_symbol_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_symbol_type ORLENTRY ORLSymbolGetType( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSymbolGetType( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( CoffSymbolGetType( (coff_symbol_handle)orl_symbol_hnd ) );
    case( ORL_OMF ):
        return( OmfSymbolGetType( (omf_symbol_handle)orl_symbol_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

unsigned char ORLENTRY ORLSymbolGetRawInfo( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( ElfSymbolGetRawInfo( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( 0 ); // no COFF equivilent
    case( ORL_OMF ):
        return( OmfSymbolGetRawInfo( (omf_symbol_handle)orl_symbol_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( 0 );
}

orl_sec_handle ORLENTRY ORLSymbolGetSecHandle( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_sec_handle)ElfSymbolGetSecHandle( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( (orl_sec_handle)CoffSymbolGetSecHandle( (coff_symbol_handle)orl_symbol_hnd ) );
    case( ORL_OMF ):
        return( (orl_sec_handle)OmfSymbolGetSecHandle( (omf_symbol_handle)orl_symbol_hnd ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_NULL_HANDLE );
}

orl_symbol_handle ORLENTRY ORLSymbolGetAssociated( orl_symbol_handle orl_symbol_hnd )
{
    switch( LCL_SYM_HND( orl_symbol_hnd )->type ) {
    case( ORL_ELF ):
        return( (orl_symbol_handle)ElfSymbolGetAssociated( (elf_symbol_handle)orl_symbol_hnd ) );
    case( ORL_COFF ):
        return( (orl_symbol_handle)CoffSymbolGetAssociated( (coff_symbol_handle)orl_symbol_hnd ) );
    case( ORL_OMF ):
        // NYI: call to an OMF func. here.
        return( NULL );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( NULL );
}

orl_return ORLENTRY ORLGroupsScan( orl_file_handle orl_file_hnd, orl_group_return_func return_func )
{
    switch( LCL_FIL_HND( orl_file_hnd )->type ) {
    case( ORL_ELF ):
    case( ORL_COFF ):
        return( ORL_OKAY );
    case( ORL_OMF ):
        return( OmfGroupsScan( LCL_FIL_HND( orl_file_hnd )->file_hnd.omf, return_func ) );
    default:    // ORL_UNRECOGNIZED_FORMAT
        break;
    }
    return( ORL_ERROR );
}

const char * ORLENTRY ORLGroupName( orl_group_handle orl_group_hnd )
{
    if( LCL_GRP_HND( orl_group_hnd )->type == ORL_OMF ) {
        return( OmfGroupName( (omf_grp_handle)orl_group_hnd ) );
    }
    return( NULL );
}

orl_table_index ORLENTRY ORLGroupSize( orl_group_handle orl_group_hnd )
{
    if( LCL_GRP_HND( orl_group_hnd )->type == ORL_OMF ) {
        return( OmfGroupSize( (omf_grp_handle)orl_group_hnd ) );
    }
    return( 0 );
}

const char * ORLENTRY ORLGroupMember( orl_group_handle orl_group_hnd, orl_table_index idx )
{
    if( LCL_GRP_HND( orl_group_hnd )->type == ORL_OMF ) {
        return( OmfGroupMember( (omf_grp_handle)orl_group_hnd, idx ) );
    }
    return( NULL );
}
