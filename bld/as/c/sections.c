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


#include "as.h"

#define HASH_TABLE_SIZE     53
#define SEC_ATTR_DEFAULT    (OWL_SEC_ATTR_PERM_READ | OWL_SEC_ATTR_PERM_WRITE)

extern owl_file_handle OwlFile;

struct obj_section {
    obj_section_handle  next;
    owl_section_handle  owl_hdl;
    char                *name;
    owl_section_type    type;
    owl_alignment       alignment;
};

static obj_section_handle   sectionHashTable[ HASH_TABLE_SIZE ];

static void sectionFields( obj_section_handle section, char *name, owl_section_type type, owl_alignment align ) {
//***************************************************************************************************************

    if( name ) section->name = AsStrdup( name );
    // Otherwise we keep the old name.

    section->owl_hdl = OWLSectionInit( OwlFile, section->name, type, align );
    section->type = type;
    section->alignment = align;

    SymSetSection( SymLookup( section->name ) );
}

static obj_section_handle sectionCreate( char *name, owl_section_type type, owl_alignment align ) {
//*************************************************************************************************

    obj_section_handle  section;
    obj_section_handle  *bucket;

    section = MemAlloc( sizeof( *section ) );
    sectionFields( section, name, type, align );

    bucket = &sectionHashTable[ AsHashVal( name, HASH_TABLE_SIZE ) ];
    section->next = *bucket;
    *bucket = section;

    return( section );
}

static obj_section_handle doNewSection( char *name, owl_section_type *ptype, owl_alignment align ) {
//**************************************************************************************************

    if( ptype ) {
        return( sectionCreate( name, *ptype, align ) );
    }
    return( sectionCreate( name, SEC_ATTR_DEFAULT, 0 ) );
}

extern owl_section_handle SectionOwlHandle( obj_section_handle hdl ) {
//********************************************************************

    return( hdl->owl_hdl );
}

extern obj_section_handle SectionLookup( char *name ) {
//*****************************************************

    obj_section_handle  section;

    section = sectionHashTable[ AsHashVal( name, HASH_TABLE_SIZE ) ];
    while( section ) {
        if( !strcmp( section->name, name ) ) break;
        section = section->next;
    }
    return( section );
}

extern void SectionInit( void ) {
//*******************************

    // make sure these names are in asdrectv.c (in table asm_directives[])
    #define PICK( a,b,c,d ) SymSetSection( SymLookup( b ) );
    #include "sections.inc"
    #undef PICK
}

extern void SectionFini( void ) {
//*******************************

    obj_section_handle  section, next;
    int                 ctr, n;

    n = sizeof( sectionHashTable ) / sizeof( *sectionHashTable );
    for( ctr = 0; ctr < n; ctr++ ) {
        section = sectionHashTable[ ctr ];
        while( section ) {
            next = section->next;
            MemFree( section->name );
            MemFree( section );
            section = next;
        }
        sectionHashTable[ ctr ] = NULL;
    }
}

extern void SectionSwitch( char *name, owl_section_type *ptype, owl_alignment align ) {
//*************************************************************************************
// Switch to a section with the name, type, and alignment specified.
// If no such section name exists, we create a new one.
// If it does exist, we switch to that section given that the type and
// alignment remains the same. (ptype == NULL if no attr is specified)

    obj_section_handle  section;

    section = SectionLookup( name );
    if( section ) {
        // Then we switch to this section. Check type and alignment first.
        if( ptype ) {
            // type or alignment specified - have to match old ones.
            if( section->type != *ptype || align != section->alignment ) {
                Error( SEC_ATTR_CONFLICT );
            }
        }
    } else {
        section = doNewSection( name, ptype, align );
    }
    CurrentSection = section->owl_hdl;
}

extern void SectionNew( char *name, owl_section_type *ptype, owl_alignment align ) {
//**********************************************************************************
// Create a new section regardless of whether a section by this name already
// exists. The old one is non-accessible from then on.

    obj_section_handle  section;

    section = SectionLookup( name );
    if( section ) {
        OWLSectionFini( section->owl_hdl );
        if( ptype ) {
            sectionFields( section, NULL, *ptype, align );
        } else {
            sectionFields( section, NULL, SEC_ATTR_DEFAULT, 0 );
        }
    } else {
        section = doNewSection( name, ptype, align );
    }
    CurrentSection = section->owl_hdl;
}
