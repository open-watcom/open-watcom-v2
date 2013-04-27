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
* Description:  Identify ORL section type.
*               
****************************************************************************/


#include "global.h"
#include "identsec.h"
#include "hashtabl.h"
#include "init.h"

extern hash_table NameRecognitionTable;

section_type IdentifySec( orl_sec_handle shnd )
// function to identify a section
{
    orl_sec_type        type;
    orl_sec_flags       flags;
    char *              name;
    hash_data *         data_ptr;

    type = ORLSecGetType( shnd );
    switch( type ) {
        case ORL_SEC_TYPE_SYM_TABLE:
            return( SECTION_TYPE_SYM_TABLE );
        case ORL_SEC_TYPE_DYN_SYM_TABLE:
            return( SECTION_TYPE_DYN_SYM_TABLE );
        case ORL_SEC_TYPE_NO_BITS:
            return( SECTION_TYPE_BSS );
        case ORL_SEC_TYPE_PROG_BITS:
        case ORL_SEC_TYPE_EXPORT:
        case ORL_SEC_TYPE_IMPORT:
            flags = ORLSecGetFlags( shnd );
            if( flags & ORL_SEC_FLAG_EXEC ) {
                return( SECTION_TYPE_TEXT );
            }
            name = ORLSecGetName( shnd );
            data_ptr = HashTableQuery( NameRecognitionTable, (hash_value) name );
            if( data_ptr ) {
                return( (section_type) *data_ptr );
            }
            return( SECTION_TYPE_DATA );
        case ORL_SEC_TYPE_RELOCS:
            // Under OMF the reloc section is virtual and does not contain
            // what would normally be considered data
            if( GetFormat() == ORL_OMF ) {
                return( SECTION_TYPE_RELOCS );
            }
            return( SECTION_TYPE_UNKNOWN );
        case ORL_SEC_TYPE_NOTE:
            if( GetFormat() == ORL_OMF ) {
                return( SECTION_TYPE_DRECTVE );
            }
            name = ORLSecGetName( shnd );
            data_ptr = HashTableQuery( NameRecognitionTable, (hash_value) name );
            if( data_ptr ) {
                return( (section_type) *data_ptr );
            }
            // fall through - unknown*/
        default:
            return( SECTION_TYPE_UNKNOWN );
    }
}
