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


#include "cvars.h"


extern  void    CMemFree();
extern  void    CClose();
extern  void    FreeFNames(void);
extern  void    FreeRDir( void );
extern  void    PragmaFini();
extern  void    SymsPurge();
extern  void    QuadFini();
extern  void    FreeDataQuads();


void InitPurge()
/**************/
{
}


static void Purge( void **ptr )
{
    if( *ptr != NULL ) {
        CMemFree( *ptr );
        *ptr = NULL;
    }
}


void SwitchPurge()
/****************/
{
    SWITCHPTR                   sw;
    CASEPTR                     c_entry, c_tmp;

    while( sw = SwitchStack ) {
        SwitchStack = sw->prev_switch;
        c_entry = sw->case_list;
        while( c_entry != NULL ) {
            c_tmp = c_entry->next_case;
            CMemFree( c_entry );
            c_entry = c_tmp;
        }
        CMemFree( sw );
    }
}


void LitsPurge()
/**************/
{
#if 0
    STRING_LITERAL *lit;

    lit = CStringList;
    while( lit != NULL ) {
        CStringList = lit->next_string;
        CMemFree( lit );
        lit = CStringList;
    }
#endif
}


void SrcPurge()
/*************/
{
    FCB *src_file;

    src_file = SrcFile;
    while( src_file != NULL ) {
        SrcFile = src_file->prev_file;
        CClose( src_file->src_fp );
        FEfree( src_file->src_buf );
        CMemFree( src_file );
        src_file = SrcFile;
    }
}


void PurgeMemory()
/****************/
{
//  MacroPurge();
    SymsPurge();        /* calls TypesPurge */
    PragmaFini();
    FreeDataQuads();
    LitsPurge();
    FreeFNames();
    FreeRDir();
    SrcPurge();
    SwitchPurge();
//  PurgeBlockStack();
    Purge( &HFileList );
//  Purge( &ErrSym );           /* ErrSym is in CPermArea */
    Purge( &SavedId );
    FreePreCompiledHeader();
}
