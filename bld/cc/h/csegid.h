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


/* segment identifiers */
enum  segments {
        SEG_CODE = 1,           /* code segment */
        SEG_CONST,              /* literal strings */
        SEG_CONST2,             /* other static initialized data */
        SEG_DATA,
        SEG_YIB,                /* finializer segment begin */
        SEG_YI,                 /* finializer segment data */
        SEG_YIE,                /* finializer segment end */
        SEG_THREAD_BEG,         /* beg thread */
        SEG_THREAD,             /* __declspec(thread) data segment */
        SEG_THREAD_END,         /* end thread */
        SEG_BSS,                /* uninitialized data */
        SEG_STACK,              /* used only by front end */
        SEG_END                 /* end */
};
#define FIRST_PRIVATE_SEGMENT  (SEG_END)

