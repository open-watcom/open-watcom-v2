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
* Description:  overlay routines used by both overlaying schemes.
*
****************************************************************************/


#include <dos.h>
#include <stddef.h>
#include <string.h>
#include "ovlstd.h"

extern void __near __OvlMsg__( unsigned msg )
//=========================================
// Write message.
{
    char __far  *ptr;

    ptr = __OVLMSGS__[ msg ];
    TinyFarWrite( TIO_STDERR_FILENO, ptr + sizeof( char ), *ptr );
}

extern void __near __OvlExit__( unsigned msg )
//==========================================
// Terminate execution.
{
    __OvlMsg__( msg );
    TinyTerminateProcess( -1 );
}

void __near __OvlCodeLoad__( ovltab_entry_ptr ovl, tiny_handle_t fp )
//=================================================================
// Load code portion of section.
{
    unsigned        block_size; // number of paragraphs
    unsigned        code_size;  // number of paragraphs
    tiny_ret_t      status;
    unsigned        load_seg;

    load_seg = ovl->code_handle;
    for( code_size = ovl->num_paras; code_size > 0; code_size -= block_size ) {
        block_size = 0x0fff;
        if( code_size < 0x0fff ) {
            block_size = code_size;
        }
        status = __OvlRead__( fp, MK_FP( load_seg, 0 ), block_size << 4 );
        if( TINY_ERROR( status ) )
            __OvlExit__( OVL_IO_ERR );
        load_seg += block_size;
    }
}

#define RELOC_CAPACITY 32

int __near __OvlRelocLoad__( ovltab_entry_ptr ovl, tiny_handle_t fp )
//=================================================================
// Relocate section code; returns 1 if section is self-referential.
{
    unsigned        buffered_relocs;
    unsigned        num_relocs;
    int             self_ref = 0;
    dos_addr __far  *relocs;
    dos_addr        reloc_buffer[ RELOC_CAPACITY ];
    unsigned __far  *fixup;
    tiny_ret_t      status;

    for( num_relocs = ovl->relocs; num_relocs > 0; ) {
        buffered_relocs = RELOC_CAPACITY;
        if( num_relocs < RELOC_CAPACITY ) {
            buffered_relocs = num_relocs;
        }
        relocs = reloc_buffer;
        status = __OvlRead__( fp, relocs, buffered_relocs * sizeof(dos_addr) );
        if( TINY_ERROR( status ) )
            __OvlExit__( OVL_IO_ERR );
        num_relocs -= buffered_relocs;
        while( buffered_relocs != 0 ) {
            fixup = MK_FP( ovl->code_handle + relocs->seg, relocs->off );
            if( (unsigned)(*fixup - ovl->start_para) < ovl->num_paras ) {
                self_ref = 1;
                *fixup += (ovl->code_handle - ovl->start_para);
            } else {
                *fixup += __OVLTAB__.prolog.delta;
            }
            relocs++;
            buffered_relocs--;
        }
    }
    return( self_ref );
}

// Maximum size of a filename allowed here.
#define FNMAX 80

#ifdef OVL_DEBUG
extern void __near __OvlNum__( unsigned ovl_num )
//=============================================
// Write overlay number.
{
    char    buff[ 4 ];  // assume overlay number < 9999
    int     len;
    int     power;

    len = 0;
    for( power = 1000; power > 0; power /= 10 ) {
        buff[ len ] = '0' + ovl_num / power;
        len++;
        ovl_num = ovl_num - ( ovl_num / power ) * power;
    }
    TinyWrite( TIO_STDERR_FILENO, &buff, len );
}
#endif

static char __far *getpathenv( void )
/*********************************/
/* get the PATH environment variable */
{
    char __far  *ptr;

    for( ptr = MK_FP( *(unsigned __far *)MK_FP( __OVLPSP__, 0x2c ), 0 ); *ptr; ++ptr ) {
        if(    ptr[0] == 'P'
            && ptr[1] == 'A'
            && ptr[2] == 'T'
            && ptr[3] == 'H'
            && ptr[4] == '=' )
            return( ptr + 5 );
        for( ; *ptr != '\0'; ) {
            ++ptr;
        }
    }
    return( __OVLNULLSTR__ );
}

extern void __far __CloseOvl__( void )
//==================================
{
    if( __OVLFILEPREV__ != 0xFFFF ) {
        __OvlClose__( TINY_INFO( __OVLHDLPREV__ ) );
        __OVLFILEPREV__ = 0xFFFF;
    }
}


extern tiny_ret_t __near __OpenOvl__( unsigned offset )
//===================================================
// Open section file.
{
    char __far  *fname;
    char __far  *cmd;
    char        chr;
    char __far  *lastslash;
    char __far  *fnstart;
    char __far  *path;
    char        buf[ FNMAX ];
    tiny_ret_t  status;
    unsigned    isexe;

    if( offset == __OVLFILEPREV__ ) {
        status = __OVLHDLPREV__;
    } else {
        __CloseOvl__();
        __OVLFILEPREV__ = offset;
        isexe = offset & OVE_EXE_FILENAME;
        offset &= ~OVE_EXE_FILENAME;
        fname = ((char __far *)&__OVLTAB__) + offset;
        // strip drive/directory from fname
        for( fnstart = fname; *fnstart != '\0'; ++fnstart ) {
            switch( *fnstart ) {
            case '\\':
            case '/':
            case ':':
                fname = fnstart + 1;
                break;
            }
        }
        if( __OVLFLAGS__ & OVL_DOS3 ) {
            // go through environment to find path of .EXE file.
            for( cmd = MK_FP( *(unsigned __far *)MK_FP( __OVLPSP__, 0x2c ), 0 ); cmd[ 0 ] | cmd[ 1 ]; )
                ++cmd;
            cmd += 4;
            // now replace executable name with fname.
            fnstart = buf;
            lastslash = NULL;
            while( *cmd != '\0' ) {
                chr = *fnstart++ = *cmd++;
                if( chr == '\\' || chr == '/' || chr == ':' ) {
                    lastslash = fnstart;  // NOTE: this points to the char after.
                }
            }
            *fnstart = '\0';
            if( isexe ) {
                fnstart = buf;      // use the specified filename
            } else if( lastslash == NULL ) {  //no path
                fnstart = fname;
            } else {
                cmd = fname;
                while( *cmd != '\0' ) {
                    *lastslash++ = *cmd++;
                }
                *lastslash = '\0';
                fnstart = buf;
            }
        } else {
            fnstart = fname;
        }
        path = getpathenv();
        for( ;; ) {
            status = __OvlOpen__( fnstart );
            if( TINY_OK( status ) )
                break;
            if( *path == '\0' )
                break;
            cmd = buf;
            for( ; *path != ';' && *path != '\0'; ) {
                *cmd++ = *path++;
            }
            if( *path == ';' )
                ++path;
            if( cmd > buf && cmd[ -1 ] != ':' && cmd[ -1 ] != '\\' && cmd[ -1 ] != '/' )
                *cmd++ = '\\';
            fnstart = fname;
            for( ; *fnstart != '\0'; ) {
                *cmd++ = *fnstart++;
            }
            *cmd = '\0';
            fnstart = buf;
        }
        __OVLHDLPREV__ = status;
    }
    return( status );
}
