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


#define IS_RELOC_RELATIVE( type )   ( type == OWL_RELOC_BRANCH_REL || type == OWL_RELOC_JUMP_REL )

#ifdef _STANDALONE_

typedef enum {
    #define PICK( a, b, c, d ) AS_SECTION_##a,
    #include "sections.inc"
    #undef PICK
} reserved_section;

extern owl_section_handle       CurrentSection;

extern void ObjSetObjFile( char *obj_name );
extern void ObjSwitchSection( reserved_section );
extern bool ObjInit( char *file_name );
extern bool ObjLabelDefined( sym_handle );
extern void ObjEmitLabel( sym_handle );
extern void ObjEmitNumericLabel( uint_32 );
extern void ObjFlushLabels( void );
extern void ObjEmitData( owl_section_handle section, void *buffer, int size, bool do_align );
extern void ObjNopPad( owl_section_handle section, uint_8 count );
extern void ObjNullPad( owl_section_handle section, uint_8 count );
extern owl_offset ObjAlign( owl_section_handle section, uint_8 alignment );
//extern owl_offset ObjTellOffset( owl_section_handle section );
extern void ObjEmitReloc( owl_section_handle section, void *target, owl_reloc_type type, bool do_align, bool named_sym );
extern void ObjDirectEmitReloc( owl_section_handle section, owl_offset offset, void *target, owl_reloc_type type, bool named_sym );
extern void ObjRelocsFini();
extern void ObjFini();
extern sym_obj_hdl ObjSymbolInit( char *name );
#else
extern void AsiObjInit( void );
extern void AsiObjSetLocation( owl_offset offset );
extern bool AsiObjEmitMetaReloc( owl_reloc_type type, bool align );
extern void AsiObjEmitRelocAddend( owl_reloc_type type, uint_32 addend );
extern bool AsiObjLabelDefined( sym_handle );
extern void AsiObjEmitLabel( sym_handle );
extern void AsiObjEmitNumericLabel( uint_32 );
extern void AsiObjFlushLabels( void );
extern void AsiObjEmitData( void *buffer, int size, bool do_align );
extern void AsiObjDirectEmitData( void *buffer, int size );
extern void AsiObjNopPad( uint_8 count );
extern void AsiObjNullPad( uint_8 count );
extern owl_offset AsiObjAlign( uint_8 alignment );
//extern owl_offset AsiObjTellOffset( void );
extern void AsiObjEmitReloc( void *target, owl_reloc_type type, bool do_align, bool named_sym );
extern void AsiObjDirectEmitReloc( owl_offset offset, void *target, owl_reloc_type type, bool named_sym );
extern void AsiObjRelocsFini();
extern void AsiObjFini();
extern sym_obj_hdl AsiObjSymbolInit( char *name );

#define CurrAlignment                   AsCurrAlignment

#define ObjInit                         AsiObjInit
#define ObjSetLocation                  AsiObjSetLocation
#define ObjEmitMetaReloc                AsiObjEmitMetaReloc
#define ObjEmitRelocAddend              AsiObjEmitRelocAddend
#define ObjLabelDefined                 AsiObjLabelDefined
#define ObjEmitLabel                    AsiObjEmitLabel
#define ObjEmitNumericLabel             AsiObjEmitNumericLabel
#define ObjStackLabel                   AsiObjStackLabel
#define ObjFlushLabels                  AsiObjFlushLabels
#define ObjEmitData                     AsiObjEmitData
#define ObjDirectEmitData               AsiObjDirectEmitData
#define ObjNopPad                       AsiObjNopPad
#define ObjNullPad                      AsiObjNullPad
#define ObjAlign                        AsiObjAlign
//#define ObjTellOffset                 AsiObjTellOffset
#define ObjEmitReloc                    AsiObjEmitReloc
#define ObjDirectEmitReloc              AsiObjDirectEmitReloc
#define ObjRelocsFini                   AsiObjRelocsFini
#define ObjFini                         AsiObjFini
#define ObjSymbolInit                   AsiObjSymbolInit
#endif

extern uint_8                   CurrAlignment;

