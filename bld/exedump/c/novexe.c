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
* Description:  NLM dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>

#include "wdglb.h"
#include "wdfunc.h"


static  char    *nlm_exe_msg[] = {
    "4code image offset                         = ",
    "4code image size                           = ",
    "4data image offset                         = ",
    "4data image size                           = ",
    "4uninitialized data size                   = ",
    "4custom data offset                        = ",
    "4custom data size                          = ",
    "4module dependency offset                  = ",
    "4number of module dependencies             = ",
    "4relocation fixup offset                   = ",
    "4number of relocation fixups               = ",
    "4external references offset                = ",
    "4number of external references             = ",
    "4publics offset                            = ",
    "4number of publics                         = ",
    "4debug info offset                         = ",
    "4number of debug records                   = ",
    "4code start offset                         = ",
    "4exit procedure offset                     = ",
    "4check unload procedure offset             = ",
    "4module type                               = ",
    "4flags                                     = ",
    "1description length                        = ",
    NULL
};

static  char    *nlm_date_msg[] = {
    "4major version number                      = ",
    "4minor version number                      = ",
    "4revision                                  = ",
    "4year                                      = ",
    "4month (January == 1)                      = ",
    "4day                                       = ",
    NULL
};

static  char    *nlm_ext_msg[] = {
    "4language id                               = ",
    "4message file offset                       = ",
    "4message file length                       = ",
    "4message count                             = ",
    "4help file offset                          = ",
    "4help file length                          = ",
    "4RPC data offset                           = ",
    "4RPC data length                           = ",
    "4shared code offset                        = ",
    "4shared code length                        = ",
    "4shared data offset                        = ",
    "4shared data length                        = ",
    "4shared relocation fixup offset            = ",
    "4shared relocation fixup count             = ",
    "4shared external reference offset          = ",
    "4shared external reference count           = ",
    "4shared publics offset                     = ",
    "4shared publics count                      = ",
    "4shared debug record offset                = ",
    "4shared debug record count                 = ",
    "4shared initialization offset              = ",
    "4shared exit procedure offset              = ",
    "4product id                                = ",
    "4reserved                                  = ",
    "4reserved                                  = ",
    "4reserved                                  = ",
    "4reserved                                  = ",
    "4reserved                                  = ",
    "4reserved                                  = ",
    NULL
};


/*
 * Dump module dependency table.
 */
static void dmp_module_dep( void )
/********************************/
{
    unsigned_32     i;
    unsigned_8      len;
    char            *name;

    if( Nlm_head.numberOfModuleDependencies == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wlseek( Nlm_head.moduleDependencyOffset );
    Banner( "Module Dependency Table" );
    for( i = 0; i < Nlm_head.numberOfModuleDependencies; i++ ) {
        Wread( &len, sizeof( unsigned_8 ) );
        name = Wmalloc( len );
        Wread( name, len );
        name[len] = '\0';
        Wdputs( "      " );
        Wdputs( name );
        Wdputslc( "\n" );
    }
}

/*
 * Dump relocation fixup table.
 */
static void dmp_reloc_fixup( void )
/*********************************/
{
    unsigned_32     i;
    unsigned_32     reloc;

    if( Nlm_head.numberOfRelocationFixups == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wlseek( Nlm_head.relocationFixupOffset );
    Banner( "Relocation Fixup Table" );
    Wdputslc( "80000000H = target is in code segment\n" );
    Wdputslc( "40000000H = source of fixup is in code segment\n" );
    for( i = 0; i < Nlm_head.numberOfRelocationFixups; i++ ) {
        Wread( &reloc, sizeof( unsigned_32 ) );
        if( i != 0 ) {
            if( (i) % 6 == 0 ) {
                Wdputslc( "\n" );
            } else {
                Wdputs( "      " );
            }
        }
        Puthex( reloc, 8 );
    }
    Wdputslc( "\n" );
}

/*
 * Dump external reference (import) table.
 */
static void dmp_external_ref( void )
/**********************************/
{
    unsigned_32     i,j;
    unsigned_8      len;
    char            *name;
    unsigned_32     num;
    unsigned_32     reloc;

    if( Nlm_head.numberOfExternalReferences  == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wlseek( Nlm_head.externalReferencesOffset );
    Banner( "External Reference (Import) Table" );
    Wdputslc( "80000000H = relocation not relative to current position\n" );
    Wdputslc( "40000000H = relocation to code segment\n" );
    for( i = 0; i < Nlm_head.numberOfExternalReferences; i++ ) {
        Wread( &len, sizeof( unsigned_8 ) );
        name = Wmalloc( len );
        Wread( name, len );
        name[len] = '\0';
        Wdputs( name );
        Wdputs( ",  relocations:" );
        Wdputslc( "\n" );
        Wread( &num, sizeof( unsigned_32 ) );
        Wdputs( "        " );
        for( j = 0; j < num; j++ ) {
            Wread( &reloc, sizeof( unsigned_32 ) );
            if( j != 0 ) {
                if( (j) % 4 == 0 ) {
                    Wdputslc( "\n        " );
                } else {
                    Wdputs( "      " );
                }
            }
            Putdecl( j, 2 );
            Wdputc( ':' );
            Puthex( reloc, 8 );
        }
        Wdputslc( "\n" );
    }
}

/*
 * Dump public entry (export) table.
 */
static void dmp_public_entry( void )
/**********************************/
{
    unsigned_32     i;
    unsigned_8      len;
    char            *name;
    unsigned_32     addr;

    if( Nlm_head.numberOfPublics == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wlseek( Nlm_head.publicsOffset );
    Banner( "Public Entry (Export) Table" );
    Wdputslc( "80000000H = symbol is in code\n" );
    Wdputslc( "      Address         Name\n" );
    Wdputslc( "      =======         ====\n" );
    for( i = 0; i < Nlm_head.numberOfPublics; i++ ) {
        Wread( &len, sizeof( unsigned_8 ) );
        name = Wmalloc( len );
        Wread( name, len );
        name[len] = '\0';
        Wread( &addr, sizeof( unsigned_32 ) );
        Wdputs( "      " );
        Puthex( addr, 8 );
        Wdputs( "        " );
        Wdputs( name );
        Wdputslc( "\n" );
    }
}

/*
 * Dump info from extended header.
 */
static void dmp_extended( void )
/******************************/
{
    if( Nlm_ext_head.messageFileLength != 0 ) {
        Banner( "Message File" );
        Dmp_seg_data( Nlm_ext_head.messageFileOffset, Nlm_ext_head.messageFileLength );
    }
    if( Nlm_ext_head.helpFileLength != 0 ) {
        Banner( "Help File" );
        Dmp_seg_data( Nlm_ext_head.helpFileOffset, Nlm_ext_head.helpFileLength );
    }
    if( Nlm_ext_head.RPCDataLength != 0 ) {
        Banner( "RPC Data" );
        Dmp_seg_data( Nlm_ext_head.RPCDataOffset, Nlm_ext_head.RPCDataLength );
    }
}

/*
 * Dump variable size part of Nlm Executable Header.
 */
static unsigned_32 dmp_nlm_head2( void )
/**************************************/
{
    unsigned_32     offset;
    nlm_header_2    nlm_head2;

    Wread( nlm_head2.descriptionText, Nlm_head.descriptionLength + 1 );
    Wdputs( "description text                          = " );
    Wdputs( nlm_head2.descriptionText );
    offset = Nlm_head.descriptionLength + 1;
    Wread( &nlm_head2.stackSize, sizeof( unsigned_32 ) );
    Wread( &nlm_head2.reserved, sizeof( unsigned_32 ) );
    Wdputslc( "\nstack size                                = " );
    Puthex( nlm_head2.stackSize, 8 );
    Wdputslc( "H\nreserved                                  = " );
    Puthex( nlm_head2.reserved, 8 );
    offset += 2 * sizeof( unsigned_32 );
    Wread( nlm_head2.oldThreadName, OLD_THREAD_NAME_LENGTH );
    nlm_head2.oldThreadName[OLD_THREAD_NAME_LENGTH] = '\0';
    Wdputslc( "H\nold thread name                           = " );
    Wdputs( nlm_head2.oldThreadName );
    offset += OLD_THREAD_NAME_LENGTH;
    Wread( &nlm_head2.screenNameLength, sizeof( unsigned_8 ) );
    Wread( nlm_head2.screenName, nlm_head2.screenNameLength + 1 );
    Wdputslc( "\nscreen name                               = " );
    Wdputs( nlm_head2.screenName );
    offset += nlm_head2.screenNameLength + 1;
    Wread( &nlm_head2.threadNameLength, sizeof( unsigned_8 ) );
    offset += 2 * sizeof( unsigned_8 );
    Wread( nlm_head2.threadName, nlm_head2.threadNameLength + 1 );
    Wdputslc( "\nthread name                               = " );
    Wdputs( nlm_head2.threadName );
    offset += nlm_head2.threadNameLength + 1;
    Wdputslc( "\n" );
    return( offset );
}

/*
 * Dump the Nlm Executable Header, if any.
 */
bool Dmp_nlm_head( void )
/***********************/
{
    unsigned_32     offset;
    nlm_header_3    nlm_head3;
    nlm_header_4    nlm_head4;
    bool            extend;
    char            nlm_name[256];

    Wlseek( 0 );
    Wread( &Nlm_head, sizeof( Nlm_head.signature ) );
    if( memcmp( Nlm_head.signature, NLM_SIGNATURE, sizeof( NLM_SIGNATURE ) - 1 ) ) {
        return( 0 );
    }
    Wread( (char *)&Nlm_head + sizeof( Nlm_head.signature ),
        sizeof( nlm_header ) - sizeof( Nlm_head.signature ) );
    Banner( "Novell EXE Header" );
    Wdputs( "version number                            = " );
    Puthex( Nlm_head.version, 8 );
    Wdputslc( "H\n" );
    Wdputs( "module name                               = " );
    memcpy( nlm_name, &Nlm_head.moduleName[1], Nlm_head.moduleName[0] );
    nlm_name[ (int)Nlm_head.moduleName[0] ] = '\0';
    Wdputs( nlm_name );
    Wdputslc( "\n" );
    Dump_header( (char *)&Nlm_head.codeImageOffset, nlm_exe_msg );
    offset = dmp_nlm_head2();
    offset += sizeof( nlm_header );
    Wlseek( offset );
    Wread( &nlm_head3, sizeof( nlm_header_3 ) );
    if( !memcmp( nlm_head3.versionSignature, VERSION_SIGNATURE, VERSION_SIGNATURE_LENGTH ) ) {
        Dump_header( (char *)&nlm_head3.majorVersion, nlm_date_msg );
        offset += sizeof( nlm_header_3 );
    }
    Wlseek( offset );
    Wread( &nlm_head4, sizeof( nlm_header_4 ) );
    if( !memcmp( nlm_head4.copyrightSignature, COPYRIGHT_SIGNATURE, COPYRIGHT_SIGNATURE_LENGTH ) ) {
        Wdputs( "copy right string                    = " );
        Wdputs( nlm_head4.copyrightString );
        Wdputslc( "\n" );
        offset += sizeof( unsigned_8 ) + COPYRIGHT_SIGNATURE_LENGTH;
        offset += nlm_head4.copyrightLength;
    }
    Wlseek( offset );
    extend = FALSE;
    Wread( &Nlm_ext_head, sizeof( extended_nlm_header ) );
    if( !memcmp( Nlm_ext_head.stamp, EXTENDED_NLM_SIGNATURE, EXTENDED_NLM_SIGNATURE_LENGTH ) ) {
        Dump_header( (char *)&Nlm_ext_head.languageID, nlm_ext_msg );
        extend = TRUE;
    }
    if( Nlm_head.customDataSize != 0 ) {
        Wdputslc( "\n" );
        Banner( "Custom Data" );
        Dmp_seg_data( Nlm_head.customDataOffset, Nlm_head.customDataSize );
    }
    dmp_module_dep();
    if( Options_dmp & FIX_DMP ) {
        dmp_reloc_fixup();
    }
    dmp_external_ref();
    dmp_public_entry();
    Wdputslc( "\n" );
    if( extend ) {
        dmp_extended();
    }
    return( 1 );
}
