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


#define SRC_BUF_SIZE    2048
#define PRODUCTION_BUFFER_SIZE  (1024*4)

#define EOF_CHAR                256
#define CONTINUE_CHAR_STRING    257
#define MACRO_CHAR              258

typedef struct fcb_struct {     /* file control block structure */
        char *  src_name;       /* pointer to file name */
        int     src_fno;        /* file name id */
        FNAMEPTR src_flist;     /* pointer to flist_name struct */
        int     src_line;       /* current line number in source file */
        FILE *  src_fp;         /* pointer to FILE struct */
        struct fcb_struct * prev_file; /* pointer to previous fcb */
        int     prev_currchar;  /* value of CurrChar */
        int     src_cnt;        /* number of bytes left in buffer */
        char __FAR *src_ptr;    /* pointer to next character in buffer */
        int     src_bufsize;    /* size of buffer */
        char __FAR *src_buf;    /* source buffer */
        int     peeking;        /* non-zero => peeking with WCSQLPP */
  #if   _CPU ==  370  // just use for 370
        unsigned  colum;          /* start reading at colum  */
        unsigned  trunc;          /* stop  reading at trunc  */
        unsigned  prevcount;      /* leftovers from prev read */
  #endif
        long      rseekpos;      /* if closed because of too many files reopen */
} FCB;
