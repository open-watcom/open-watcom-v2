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


/*
   Don't touch this file.  Last time we changed it, there were 3 of us,
   and we still messed it up.  If you don't believe me, go talk to Jack,
   John or Geno.
*/
#if defined(__AXP__)
// do nothing
#elif defined(__PPC__)
// do nothing
#elif defined(__386__)
    #pragma aux if_rtn parm [EAX EBX ECX EDX 8087];
    #if defined(__SW_3S)
        #if __WATCOMC__ < 900
            #pragma aux (if_rtn) if_rtn modify [ FS GS ];
        #else
            #ifdef __FLAT__
                #pragma aux (if_rtn) if_rtn modify [ GS ];
            #else
                #pragma aux (if_rtn) if_rtn modify [ GS FS ES ];
            #endif
        #endif
        #if defined(__FPI__)
            #pragma aux (if_rtn) if_rtn value [8087];
        #endif
    #endif
#else
    #pragma aux if_rtn parm [AX BX CX DX 8087];
#endif
