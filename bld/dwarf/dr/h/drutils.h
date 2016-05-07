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
* Description:  DWARF reader utility functions prototypes.
*
****************************************************************************/


/* data types */

typedef enum {
    DO_NOTHING,
    SET_CLASS,
    SET_FUNCTION
} stack_op;

#define OP2HDL(x)   ((drmem_hdl)(pointer_int)(x))
#define HDL2OP(x)   ((stack_op)(pointer_int)(x))

#define DWRContextOP(x,y)       HDL2OP(DWRContext(x,y))
#define DWRContextPushOP(x,y)   DWRContextPush(x,OP2HDL(y))
#define DWRContextPopOP(x)      HDL2OP(DWRContextPop(x))

typedef struct {
    drmem_hdl           handle;         // original handle
    dr_search_context   *context;       // context to resume search
    unsigned            tag;            // the actual tag.
} mod_scan_info;

typedef bool (*DWRCUWLK)( drmem_hdl, drmem_hdl, mod_scan_info *, void * );
typedef bool (*DWRCHILDCB)( drmem_hdl, drmem_hdl, void * );

typedef enum {
    DWR_FORMCL_address,
    DWR_FORMCL_block,
    DWR_FORMCL_data,
    DWR_FORMCL_flag,
    DWR_FORMCL_ref,
    DWR_FORMCL_ref_addr,
    DWR_FORMCL_string,
    DWR_FORMCL_indirect,
} dwr_formcl;

/* function prototypes */
extern long         DWRInfoLength( drmem_hdl  );
extern bool         DWRScanCompileUnit( dr_search_context *, DWRCUWLK, const dw_tagnum *, dr_depth, void * );
extern void         DWRAllChildren( drmem_hdl, DWRCHILDCB, void * );
extern void         DWRSkipForm( drmem_hdl *, dw_formnum );
extern unsigned_32  DWRReadConstant( drmem_hdl, drmem_hdl );
extern unsigned_32  ReadConst( dw_formnum form, drmem_hdl  );
extern drmem_hdl    DWRReadReference( drmem_hdl, drmem_hdl );
extern unsigned_32  DWRReadAddr( drmem_hdl, drmem_hdl );
extern char         *DWRReadString( drmem_hdl, drmem_hdl );
extern unsigned_32  DWRReadInt( drmem_hdl where, unsigned size );
extern int          DWRReadFlag( drmem_hdl, drmem_hdl );
extern char         *DWRCopyDbgSecString( drmem_hdl *, unsigned_32 offset );
extern bool         DWRScanForAttrib( drmem_hdl *, drmem_hdl *, dw_atnum );
extern bool         DWRSearchArray( const dw_tagnum *, dw_tagnum );
extern unsigned     DWRGetAddrSize( drmem_hdl );
extern drmem_hdl    DWRSkipTag( drmem_hdl * );
extern dw_tagnum    DWRGetTag( drmem_hdl entry );
extern dw_tagnum    DWRReadTag( drmem_hdl *entry, drmem_hdl *abbrev );
extern bool         DWRReadTagEnd( drmem_hdl *entry, drmem_hdl *abbrev, dw_tagnum * );
extern void         DWRGetCompileUnitHdr( drmem_hdl, DWRCUWLK, void * );
extern char         *DWRGetName( drmem_hdl, drmem_hdl );
extern void         DWRSkipChildren( drmem_hdl *, drmem_hdl * );
extern void         DWRSkipAttribs( drmem_hdl, drmem_hdl * );
extern void         DWRSkipRest( drmem_hdl, drmem_hdl * );
extern drmem_hdl    DWRFindCompileUnit( drmem_hdl );
extern compunit_info *DWRFindCompileInfo( drmem_hdl );
extern bool         DWRScanAllCompileUnits( dr_search_context *, DWRCUWLK, const dw_tagnum *, dr_depth, void * );
extern bool         DWRWalkCompileUnit( drmem_hdl, DWRCUWLK, const dw_tagnum *, dr_depth , void * );

extern bool         DWRWalkChildren( drmem_hdl mod, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DWRWalkContaining( drmem_hdl mod, drmem_hdl target, DRWLKBLK wlk, void *d );
extern bool         DWRWalkSiblings( drmem_hdl curr, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DWRWalkScope( drmem_hdl mod, const dw_tagnum *tags, DRWLKBLK wlk, void *d );
extern void         DWRContextPush( dr_context_stack *, drmem_hdl );
extern drmem_hdl    DWRContextPop( dr_context_stack * );
extern drmem_hdl    DWRContext( dr_context_stack *stack, int up );
extern void         DWRFreeContextStack( dr_context_stack * );

extern dwr_formcl   DWRFormClass( dw_formnum form );

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
