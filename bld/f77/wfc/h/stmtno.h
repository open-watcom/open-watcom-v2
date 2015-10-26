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


typedef struct stnumbers {
    label_id            branches;       //  label on branch check routine
    label_id            formats;        //  label on format check routine
    bool                wild_goto;      //  true if assigned goto without list
    bool                var_format;     //  true if variable used for format
    bool                in_remote;      //  true if currently in REMOTE BLOCK
    unsigned_16         blk_before;     //  CSHead->block before statement
} stnumbers;

#define SN_INIT         0x0000
#define SN_DEFINED      0x0001
#define SN_FORMAT       0x0002
#define SN_BAD_BRANCH   0x0004
#define SN_BRANCHED_TO  0x0008
#define SN_AFTR_BRANCH  0x0010
#define SN_ASSIGNED     0x0020
#define SN_ONLY_DO_TERM 0x0040
#define SN_IN_REMOTE    0x0080
#define SN_EXECUTABLE   0x0100
#define SN_IN_GOTO_LIST 0x0200  // appeared in assigned GOTO list
#define SN_ADD_65535    0x0400  // add 65535 to statement number

#define SN_INIT_MASK    ~SN_ADD_65535
