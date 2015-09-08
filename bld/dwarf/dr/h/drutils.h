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

#define OP2HDL(x)   ((dr_handle)(pointer_int)(x))
#define HDL2OP(x)   ((stack_op)(pointer_int)(x))

#define DWRContextOP(x,y)       HDL2OP(DWRContext(x,y))
#define DWRContextPushOP(x,y)   DWRContextPush(x,OP2HDL(y))
#define DWRContextPopOP(x)      HDL2OP(DWRContextPop(x))

typedef struct {
    dr_handle           handle;         // original handle
    dr_search_context   *context;       // context to resume search
    unsigned            tag;            // the actual tag.
} mod_scan_info;

typedef bool (*DWRCUWLK)( dr_handle, dr_handle, mod_scan_info *, void * );
typedef bool (*DWRCHILDCB)( dr_handle, dr_handle, void * );

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
extern long         DWRInfoLength( dr_handle  );
extern bool         DWRScanCompileUnit( dr_search_context *, DWRCUWLK, const dw_tagnum *, dr_depth, void * );
extern void         DWRAllChildren( dr_handle, DWRCHILDCB, void * );
extern void         DWRSkipForm( dr_handle *, dw_formnum );
extern unsigned_32  DWRReadConstant( dr_handle, dr_handle );
extern unsigned_32  ReadConst( dw_formnum form, dr_handle  );
extern dr_handle    DWRReadReference( dr_handle, dr_handle );
extern unsigned_32  DWRReadAddr( dr_handle, dr_handle );
extern char         *DWRReadString( dr_handle, dr_handle );
extern unsigned_32  DWRReadInt( dr_handle where, unsigned size );
extern int          DWRReadFlag( dr_handle, dr_handle );
extern char         *DWRCopyDbgSecString( dr_handle *, unsigned_32 offset );
extern bool         DWRScanForAttrib( dr_handle *, dr_handle *, dw_atnum );
extern bool         DWRSearchArray( const dw_tagnum *, dw_tagnum );
extern unsigned     DWRGetAddrSize( dr_handle );
extern dr_handle    DWRSkipTag( dr_handle * );
extern dw_tagnum    DWRGetTag( dr_handle entry );
extern dw_tagnum    DWRReadTag( dr_handle *entry, dr_handle *abbrev );
extern bool         DWRReadTagEnd( dr_handle *entry, dr_handle *abbrev, dw_tagnum * );
extern void         DWRGetCompileUnitHdr( dr_handle, DWRCUWLK, void * );
extern char         *DWRGetName( dr_handle, dr_handle );
extern void         DWRSkipChildren( dr_handle *, dr_handle * );
extern void         DWRSkipAttribs( dr_handle, dr_handle * );
extern void         DWRSkipRest( dr_handle, dr_handle * );
extern dr_handle    DWRFindCompileUnit( dr_handle );
extern compunit_info *DWRFindCompileInfo( dr_handle );
extern bool         DWRScanAllCompileUnits( dr_search_context *, DWRCUWLK, const dw_tagnum *, dr_depth, void * );
extern bool         DWRWalkCompileUnit( dr_handle, DWRCUWLK, const dw_tagnum *, dr_depth , void * );

extern bool         DWRWalkChildren( dr_handle mod, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DWRWalkContaining( dr_handle mod, dr_handle target, DRWLKBLK wlk, void *d );
extern bool         DWRWalkSiblings( dr_handle curr, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d );
extern bool         DWRWalkScope( dr_handle mod, const dw_tagnum *tags, DRWLKBLK wlk, void *d );
extern void         DWRContextPush( dr_context_stack *, dr_handle );
extern dr_handle    DWRContextPop( dr_context_stack * );
extern dr_handle    DWRContext( dr_context_stack *stack, int up );
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
