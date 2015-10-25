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


#define CONT_LINES      31      // maximum # of continuation lines
#define SRCLEN          132     // maximum length of SrcBuff - by default it's
                                // 80; if "xline" option used, it's 132

#define TOKLEN          ((SRCLEN-CONT_COL)*(CONT_LINES+1))

#define STMT_START      0
#define STMT_CONT       1
#define STMT_COMMENT    2

#define FIRST_COL       1       //  first column of FORTRAN source line
#define CONT_COL        6       //  continuation column
#define STAT_COL        7       //  start of statement area
#define LAST_COL        72      //  end of statement area

// INCLUDE option flags:

#define INC_LIB_MEMBER  0x01    //  including from a library member
#define INC_DATA_OPTION 0x02    //  C$DATA option specified
                                //  (this is only used for main file)
#define INC_PENDING     0x04    //  c$include pending
#define CONC_PENDING    0x08    //  conclude pending

typedef struct source_t {
    struct source_t     *link;
    file_handle         fileptr;
    int                 rec;
    ftnoption           options;
    char                *name;
    unsigned_8          flags;
} source_t;
