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


extern int      __sigfpe_handler(int);
extern void     _ClearFPE(void);

/*
 * MICROSOFT, as usual, refuses to document stuff that other compiler
 * vendors need, so this had to be reverse engineered
 */
#define UNWINDING       0x6


#define APP_ERR "Application Error: "

#if defined(__PPC__)
    #define FMT_STRING wsprintf
#elif defined(__AXP__)
    #define FMT_STRING wsprintf

    extern unsigned long _GetFPCR(void);
    extern void          _SetFPCR(unsigned long);
#else
    unsigned int sw;
    #define FMT_STRING wsprintf

    extern DWORD GetFromFS(DWORD off);
    extern void  PutToFS(DWORD value, DWORD off);

    #pragma aux (__outside_CLIB) sig_func;

    #pragma aux GetFromFS = \
            "mov        eax,fs:[eax]" \
            parm[eax] value[eax];

    #pragma aux PutToFS = \
            "mov        fs:[edx], eax" \
            parm[eax] [edx];
#endif
