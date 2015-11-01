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


#define DIALOG_OPTION 3007

#define CTL_OPT_AUTO 103
#define CTL_OPT_BELL 104
#define CTL_OPT_IMPLICIT 105
#define CTL_OPT_RECURSE 106
#define CTL_OPT_FLIP 107
#define CTL_OPT_CASE 108
#define CTL_OPT_RADIX 109
#define CTL_OPT_DCLICK 110
#define CTL_OPT_BR_ON_WRITE 111
#define CTL_OPT_NOHEX 112
#define CTL_OPT_OK 100
#define CTL_OPT_DEFAULTS 102
#define CTL_OPT_CANCEL 101

#ifndef RC
extern  bool    DlgOptSet( void );
#endif
