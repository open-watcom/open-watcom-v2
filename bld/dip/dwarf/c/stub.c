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
        Stub definitions to stop the C library from hauling in stuff
        we don't want.
*/
#if defined(__NT__) || defined(__DOS__) || defined(__OS2__) || defined(__QNX__)

#pragma off(unreferenced);
double _matherr( why, who, arg1, arg2, result )
    int why;
    char *who;
    double *arg1, *arg2, result;
{
    return( result );
}
#if defined(__X86__)
void __set_ERANGE() {};
void __FPE_exception() {};

int __FPE_handler;
#elif defined(__AXP__)
int DllMainCRTStartup()
{
    return( 1 );
}
#endif

#endif

#if defined(__X86__)
int _real87, _8087;
int fltused_;
#elif defined(__AXP__)
int _fltused_;
#endif
