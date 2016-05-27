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
* Description:  DWARF informational functions prototypes.
*
****************************************************************************/


typedef bool (*DRWLKMODF)( drmem_hdl, void *, dr_search_context * );
typedef bool (*DRFNAMECB)( char *, void * );

typedef enum {
    DR_SRCH_func_var,
    DR_SRCH_func,
    DR_SRCH_class,
    DR_SRCH_enum,
    DR_SRCH_typedef,
    DR_SRCH_ctypes,
    DR_SRCH_cpptypes,
    DR_SRCH_var,
    DR_SRCH_parm,
    DR_SRCH_label,
    DR_SRCH_LAST,
}dr_srch;

typedef enum {
    DR_TAG_FUNCTION,
    DR_TAG_CLASS,
    DR_TAG_ENUM,
    DR_TAG_TYPEDEF,
    DR_TAG_VARIABLE,
    DR_TAG_LABEL,
    DR_TAG_NAMESPACE,
    DR_TAG_NONE         /* not any of the above */
} dr_tag_type;

extern dr_language      DRGetLanguageAT( drmem_hdl );
extern char             *DRGetProducer( drmem_hdl );
extern dr_model         DRGetMemModelAT( drmem_hdl );
extern dr_language      DRGetLanguage( void );
extern char             *DRGetName( drmem_hdl );
extern size_t           DRGetNameBuff( drmem_hdl entry, char *buff, size_t length );
extern size_t           DRGetScopedNameBuff( drmem_hdl entry, char *buff, size_t max );
extern size_t           DRGetCompDirBuff( drmem_hdl entry, char *buff, size_t length );
extern long             DRGetColumn( drmem_hdl );
extern long             DRGetLine( drmem_hdl );
extern char             *DRGetFileName( drmem_hdl );
extern void             DRGetFileNameList( DRFNAMECB, void * );
extern char             *DRIndexFileName( drmem_hdl mod, dr_fileidx fileidx );
extern dr_access        DRGetAccess( drmem_hdl );
extern bool             DRIsMemberStatic( drmem_hdl );
extern bool             DRIsFunc( drmem_hdl );
extern bool             DRIsParm( drmem_hdl entry );
extern bool             DRIsStatic( drmem_hdl );
extern bool             DRIsArtificial( drmem_hdl );
extern bool             DRIsSymDefined( drmem_hdl );
extern dr_virtuality    DRGetVirtuality( drmem_hdl );
extern unsigned         DRGetByteSize( drmem_hdl );
extern bool             DRGetLowPc( drmem_hdl, uint_32 * );
extern bool             DRGetHighPc( drmem_hdl, uint_32 * );
extern drmem_hdl        DRGetContaining( drmem_hdl );
extern drmem_hdl        DRDebugPCHDef( drmem_hdl );
extern bool             DRWalkModFunc( drmem_hdl, bool, DRWLKMODF, void * );
extern bool             DRWalkBlock( drmem_hdl, dr_srch, DRWLKBLK, void * );
extern drmem_hdl        DRWalkParent( dr_search_context * );
extern bool             DRStartScopeAT( drmem_hdl, uint_32 * );
extern unsigned         DRGetAddrSize( drmem_hdl );
extern bool             DRWalkScope( drmem_hdl, DRWLKBLK, void * );
extern dr_tag_type      DRGetTagType( drmem_hdl );
extern bool             DRWalkModTypes( drmem_hdl, DRWLKMODF, void * );
