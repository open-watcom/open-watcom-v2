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
* Description:  Constants for callback filter dialog.
*
****************************************************************************/


/*
 * When future messages are to be added their constants should be the
 * next avalable integer and CFILTER_LAST_MSG must be updated.  The fact
 * that the constants are consecutive is used in accessing the callback
 * filter data structure.
 */

#define CFILTER_FIRST_MSG       CFILTER_START
#define CFILTER_START           101
#define CFILTER_ADVREQ          102
#define CFILTER_ADVDATA         103
#define CFILTER_CONNECT         104
#define CFILTER_ERROR           105
#define CFILTER_EXEC            106
#define CFILTER_POKE            107
#define CFILTER_REQ             108
#define CFILTER_ADVSTOP         109
#define CFILTER_UNREG           110
#define CFILTER_CONFIRM         111
#define CFILTER_WILD            112
#define CFILTER_DISCON          113
#define CFILTER_XACT            114
#define CFILTER_REG             115
#define CFILTER_LAST_MSG        CFILTER_REG
#define CFILTER_ALL             301
#define CFILTER_NONE            302
