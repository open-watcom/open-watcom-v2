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


// listing file control constants

#define LF_PAGE_BOUNDARY        6       //  "6 lines less than LinesPerPage"
                                        //  lines are printed to the listing
                                        //  file
#define LF_NEW_LINE     0x01    //  BIT FLAGS - put out new line
#define LF_PAGE_FLAG    0x02    //            - put out form feed
#define LF_SKIP_FLAG    0x04    //            - double space this line
#define LF_QUIET        0x08    //            - c$nolist has been seen
#define LF_STMT_LISTED  0x10    //            - the statment was listed
#define LF_OFF  ~( LF_NEW_LINE | LF_PAGE_FLAG | LF_SKIP_FLAG )

#define LIST_BUFF_SIZE  132     // listing file buffer size
#define ERR_BUFF_SIZE   256     // error file buffer size
#define TERM_BUFF_SIZE  80      // terminal file buffer size

#define LFERR_BUFF_SIZE \
    ( (ERR_BUFF_SIZE > LIST_BUFF_SIZE) ? ERR_BUFF_SIZE : LIST_BUFF_SIZE )
