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


typedef bool (*DRWLKMODF)( dr_handle, void *, dr_search_context * );
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

extern dr_language      DRGetLanguageAT( dr_handle );
extern char             *DRGetProducer( dr_handle );
extern dr_model         DRGetMemModelAT( dr_handle );
extern dr_language      DRGetLanguage( void );
extern char             *DRGetName( dr_handle );
extern unsigned         DRGetNameBuff( dr_handle entry, char *buff, unsigned length );
extern unsigned         DRGetScopedNameBuff( dr_handle entry, char *buff, unsigned max );
extern unsigned         DRGetCompDirBuff( dr_handle entry, char *buff, unsigned length );
extern long             DRGetColumn( dr_handle );
extern long             DRGetLine( dr_handle );
extern char             *DRGetFileName( dr_handle );
extern void             DRGetFileNameList( DRFNAMECB, void * );
extern char             *DRIndexFileName( dr_handle mod, dr_fileidx fileidx );
extern dr_access        DRGetAccess( dr_handle );
extern bool             DRIsMemberStatic( dr_handle );
extern bool             DRIsFunc( dr_handle );
extern bool             DRIsParm( dr_handle entry );
extern bool             DRIsStatic( dr_handle );
extern bool             DRIsArtificial( dr_handle );
extern bool             DRIsSymDefined( dr_handle );
extern dr_virtuality    DRGetVirtuality( dr_handle );
extern unsigned         DRGetByteSize( dr_handle );
extern bool             DRGetLowPc( dr_handle, uint_32 * );
extern bool             DRGetHighPc( dr_handle, uint_32 * );
extern dr_handle        DRGetContaining( dr_handle );
extern dr_handle        DRDebugPCHDef( dr_handle );
extern bool             DRWalkModFunc( dr_handle, bool, DRWLKMODF, void * );
extern bool             DRWalkBlock( dr_handle, dr_srch, DRWLKBLK, void * );
extern dr_handle        DRWalkParent( dr_search_context * );
extern bool             DRStartScopeAT( dr_handle, uint_32 * );
extern unsigned         DRGetAddrSize( dr_handle );
extern bool             DRWalkScope( dr_handle, DRWLKBLK, void * );
extern dr_tag_type      DRGetTagType( dr_handle );
extern bool             DRWalkModTypes( dr_handle, DRWLKMODF, void * );
