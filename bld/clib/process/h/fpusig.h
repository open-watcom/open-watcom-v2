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


/* define status word bits */
#if defined(_M_IX86)
    #define     SW_BUSY         0x8000
    #define     SW_IREQ         0x0080
    #define     SW_XCPT_FLAGS   0x007f
    #define     SW_C1           0x0200

#pragma pack(__push,1);
    typedef union {
        struct status_word {
            unsigned short      ie : 1;
            unsigned short      de : 1;
            unsigned short      ze : 1;
            unsigned short      oe : 1;
            unsigned short      ue : 1;
            unsigned short      pe : 1;
            unsigned short      sf : 1;
            unsigned short      es : 1;
            unsigned short      c0 : 1;
            unsigned short      c1 : 1;
            unsigned short      c2 : 1;
            unsigned short      st : 3;
            unsigned short      c3 : 1;
            unsigned short      b  : 1;
        }                       b;
        unsigned long   sw;
    } status_word;
#pragma pack(__pop);
#elif defined(__AXP__)
    #define FPCR_XCPT_FLAGS     0x03f00000     // Assumes high word of FPCR
    #define FPCR_SUMMARY_BIT    0x80000000
#elif defined(__PPC__)
    // Undefined
#else
    #error *** Processor Type Not Supported ***
#endif
