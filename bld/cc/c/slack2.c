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


/*  This file contains slack data to act as a buffer between the
 *  end of the front end data and the start of the code generator data.
 *  The amount of data
 *  in this file should be adjusted so that the start of the real data
 *  begins on the same address from one patch to the next. Without this
 *  slack data, the addresses of all the variables could change because
 *  the code grew or shrank causing the data to start on a different
 *  address.
 */
char SlackData2[128];   // slack data for the _BSS segment

/* The following acts as a buffer between statically initialized
 * data in the front-end and statically initialized data in the
 * code generator.
 */
char SlackData2_const[128] =
        "WATCOM-WATCOM-WATCOM-WATCOM-WATCOM-WATCOM-WATCOM"
        "WATCOM-WATCOM-WATCOM-WATCOM-WATCOM-WATCOM-WATCOM";
