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


/* data types */

typedef enum {
    DO_NOTHING,
    SET_CLASS,
    SET_FUNCTION
} stack_op;

typedef struct {
    dr_handle           handle;         // original handle
    dr_search_context * context;        // context to resume search
    unsigned            tag;            // the actual tag.
} mod_scan_info;

/* function prototypes */
typedef bool (*DWRCUWLK) (dr_handle, dr_handle,mod_scan_info *, void  *);
extern long DWRInfoLength( dr_handle  );
extern bool DWRScanCompileUnit( dr_search_context *,
                           DWRCUWLK,
                           unsigned_16 const *, dr_depth, void * );
extern void       DWRAllChildren( dr_handle,
                                   bool (*)(dr_handle, dr_handle, void *),
                                   void * );
extern void        DWRSkipForm( dr_handle *, unsigned_16 );
extern unsigned_32 DWRReadConstant( dr_handle, dr_handle );
extern unsigned_32 ReadConst( unsigned form, dr_handle  );
extern dr_handle   DWRReadReference( dr_handle, dr_handle );
extern dr_handle   DWRReadAddr( dr_handle, dr_handle );
extern char *      DWRReadString( dr_handle, dr_handle );
extern int         DWRReadFlag( dr_handle, dr_handle );
extern char *      DWRCopyString( dr_handle * );
extern dw_atnum    DWRScanForAttrib( dr_handle *, dr_handle *, dw_atnum );
extern bool        DWRSearchArray( unsigned_16 const *, unsigned_16 );
extern unsigned    DWRGetAddrSize( dr_handle );
extern dr_handle   DWRGetAbbrev( dr_handle * );
extern void        DWRGetCompileUnitHdr( dr_handle,
                           DWRCUWLK,
                           void * );
extern char *      DWRGetName( dr_handle, dr_handle );
extern void        DWRSkipChildren( dr_handle *, dr_handle * );
extern void        DWRSkipAttribs( dr_handle, dr_handle * );
extern void        DWRSkipRest( dr_handle, dr_handle * );
extern dr_handle   DWRFindCompileUnit( dr_handle );
extern compunit_info * DWRFindCompileInfo( dr_handle );
extern bool DWRScanAllCompileUnits( dr_search_context *,
                           DWRCUWLK,
                           unsigned_16 const *, dr_depth, void * );
extern bool DWRWalkCompileUnit( dr_handle,
                        DWRCUWLK,
                        unsigned_16 const *, dr_depth , void * );

extern int DWRWalkChildren( dr_handle mod,
                           unsigned_16 const *tags,
                           DRWLKBLK *wlks, void *d );
extern int DWRWalkContaining( dr_handle mod,
                             dr_handle target,
                             DRWLKBLK wlk, void *d );
extern bool DWRWalkSiblings( dr_handle          curr,
                             unsigned_16  const *tags,
                             DRWLKBLK *wlks,  void *d );
extern int DWRWalkScope( dr_handle mod,
                         unsigned_16 const *tags,
                         DRWLKBLK wlks,   void *d );
extern void     DWRContextPush( dr_context_stack *, uint_32 );
extern uint_32  DWRContextPop( dr_context_stack * );
extern dr_handle DWRContext( dr_context_stack * stack, int up );
extern void     DWRFreeContextStack( dr_context_stack * );

typedef enum {
    DWR_FORMCL_address,
    DWR_FORMCL_block,
    DWR_FORMCL_data,
    DWR_FORMCL_flag,
    DWR_FORMCL_ref,
    DWR_FORMCL_ref_addr,
    DWR_FORMCL_string,
    DWR_FORMCL_indirect,
}dwr_formcl;

extern dwr_formcl DWRFormClass( unsigned_16 form );
/* data declarations */

extern unsigned_16 const * const SearchTypes[];
extern unsigned_16 const * const SearchTags[];
extern unsigned_16 const         ScanKidsTags[];
extern unsigned_16 const         DeclarationTags[];
extern unsigned_16 const         FunctionTags[];
extern unsigned_16 const         VariableTags[];
extern unsigned_16 const         ClassTags[];
extern unsigned_16 const         TypedefTags[];
extern unsigned_16 const         EnumTags[];
extern unsigned_16 const         LabelTags[];
