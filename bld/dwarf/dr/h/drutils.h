/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF reader utility functions prototypes.
*
****************************************************************************/


#define OP2HDL(x)   ((drmem_hdl)(pointer_uint)(x))
#define HDL2OP(x)   ((stack_op)(pointer_uint)(x))

#define DR_ContextOP(x,y)       HDL2OP(DR_Context(x,y))
#define DR_ContextPushOP(x,y)   DR_ContextPush(x,OP2HDL(y))
#define DR_ContextPopOP(x)      HDL2OP(DR_ContextPop(x))

/* data types */

typedef enum {
    DO_NOTHING,
    SET_CLASS,
    SET_FUNCTION
} stack_op;

typedef enum {
    DR_FORMCL_address,
    DR_FORMCL_block,
    DR_FORMCL_data,
    DR_FORMCL_flag,
    DR_FORMCL_ref,
    DR_FORMCL_ref_addr,
    DR_FORMCL_string,
    DR_FORMCL_indirect,
} dr_formcl;

typedef struct {
    drmem_hdl               handle;     // original handle
    dr_search_context       *context;    // context to resume search
    unsigned                tag;        // the actual tag.
} mod_scan_info;

typedef bool (*DR_CUWLK)( drmem_hdl, drmem_hdl, mod_scan_info *, void * );
typedef bool (*DR_CHILDCB)( drmem_hdl, drmem_hdl, void * );

/* function prototypes */
extern long         DR_InfoLength( drmem_hdl  );
extern bool         DR_ScanCompileUnit( dr_search_context *, DR_CUWLK, const dw_tagnum *, dr_depth, void * );
extern void         DR_AllChildren( drmem_hdl, DR_CHILDCB, void * );
extern void         DR_SkipForm( drmem_hdl *, dw_formnum );
extern unsigned_32  DR_ReadConstant( drmem_hdl, drmem_hdl );
extern unsigned_32  ReadConst( dw_formnum form, drmem_hdl  );
extern drmem_hdl    DR_ReadReference( drmem_hdl, drmem_hdl );
extern unsigned_32  DR_ReadAddr( drmem_hdl, drmem_hdl );
extern char         *DR_ReadString( drmem_hdl, drmem_hdl );
extern unsigned_32  DR_ReadInt( drmem_hdl where, unsigned size );
extern int          DR_ReadFlag( drmem_hdl, drmem_hdl );
extern char         *DR_CopyDbgSecString( drmem_hdl *, unsigned_32 offset );
extern bool         DR_ScanForAttrib( drmem_hdl *, drmem_hdl *, dw_atnum );
extern bool         DR_SearchArray( const dw_tagnum *, dw_tagnum );
extern unsigned     DR_GetAddrSize( drmem_hdl );
extern drmem_hdl    DR_SkipTag( drmem_hdl * );
extern dw_tagnum    DR_GetTag( drmem_hdl entry );
extern dw_tagnum    DR_ReadTag( drmem_hdl *entry, drmem_hdl *abbrev );
extern bool         DR_ReadTagEnd( drmem_hdl *entry, drmem_hdl *abbrev, dw_tagnum * );
extern void         DR_GetCompileUnitHdr( drmem_hdl, DR_CUWLK, void * );
extern char         *DR_GetName( drmem_hdl, drmem_hdl );
extern void         DR_SkipChildren( drmem_hdl *, drmem_hdl * );
extern void         DR_SkipAttribs( drmem_hdl, drmem_hdl * );
extern void         DR_SkipRest( drmem_hdl, drmem_hdl * );
extern drmem_hdl    DR_FindCompileUnit( drmem_hdl );
extern dr_cu_handle DR_FindCompileInfo( drmem_hdl );
extern bool         DR_ScanAllCompileUnits( dr_search_context *, DR_CUWLK, const dw_tagnum *, dr_depth, void * );
extern bool         DR_WalkCompileUnit( drmem_hdl, DR_CUWLK, const dw_tagnum *, dr_depth , void * );

extern bool         DR_WalkChildren( drmem_hdl mod, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DR_WalkContaining( drmem_hdl mod, drmem_hdl target, DRWLKBLK wlk, void *d );
extern bool         DR_WalkSiblings( drmem_hdl curr, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DR_WalkScope( drmem_hdl mod, const dw_tagnum *tags, DRWLKBLK wlk, void *d );
extern void         DR_ContextPush( dr_context_stack *, drmem_hdl );
extern drmem_hdl    DR_ContextPop( dr_context_stack * );
extern drmem_hdl    DR_Context( dr_context_stack *stack, int up );
extern void         DR_FreeContextStack( dr_context_stack * );

extern dr_formcl    DR_FormClass( dw_formnum form );

/* data declarations */
extern const dw_tagnum * const SearchTypes[];
extern const dw_tagnum * const SearchTags[];
extern const dw_tagnum         ScanKidsTags[];
extern const dw_tagnum         DeclarationTags[];
extern const dw_tagnum         FunctionTags[];
extern const dw_tagnum         VariableTags[];
extern const dw_tagnum         ClassTags[];
extern const dw_tagnum         TypedefTags[];
extern const dw_tagnum         EnumTags[];
extern const dw_tagnum         LabelTags[];
