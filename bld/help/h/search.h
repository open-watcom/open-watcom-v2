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


#ifndef _SEARCH_H_INCLUDED_
#define _SEARCH_H_INCLUDED_
#include "index.h"
#include "helpio.h"
#include "stdui.h"
#include "uidialog.h"

struct HelpHdl{
    HelpFp      fp;
    HelpHeader  header;
    uint_16     *itemindex;
    char        *def_topic;
    char        *desc_str;
};

typedef struct HelpHdl  *HelpHdl;

typedef struct {
    HelpHdl             hdl;
    unsigned            page;
    unsigned            entry;
    unsigned long       offset;
} HelpSrchInfo;

char *HelpFindNext( HelpSrchInfo *info );
unsigned HelpFindFirst( HelpHdl hdl, char *name, HelpSrchInfo *info );
HelpHdl InitHelpSearch( HelpFp fp );
void FiniHelpSearch( HelpHdl hdl );
char *HelpFindPrev( HelpSrchInfo *info );
unsigned long HelpGetOffset( HelpSrchInfo cursor );
unsigned long HelpFindTopicOffset( HelpHdl hdl, char *topic );
char *HelpGetIndexedTopic( HelpHdl hdl, unsigned index );
char *GetDefTopic( HelpHdl hdl );
char *GetDescrip( HelpHdl hdl );

char *HelpSearch( HelpHdl hdl );
void HelpDialogCallBack( a_dialog *info );
void SearchDlgFini( void );
#endif
