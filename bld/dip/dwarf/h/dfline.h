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




extern void  InitCueList( cue_list *ctl );
extern  seg_cue *InitSegCue( cue_list *ctl, addr_seg seg, dword offset );
extern  void AddCue( seg_cue *ctl, dr_line_data *new );
extern  int FindCueOffset( cue_list *list, addr_ptr *mach, cue_item *ret );
typedef enum{
    LOOK_LOW,         // look for next lower than cue
    LOOK_HIGH,        // look for next higher than cue
    LOOK_CLOSEST,     // look for lower or equal to cue
    LOOK_FILE,        // look for any cue with file
}dfline_search;
typedef enum{
    LINE_NOT,
    LINE_FOUND,
    LINE_CLOSEST,
    LINE_WRAPPED,
}dfline_find;
extern   dfline_find FindCue( cue_list    *list,
                              cue_item    *item,
                              dfline_search what );

extern void  FiniCueInfo( cue_list *list );
