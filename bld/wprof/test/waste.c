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
* Description:  Simple CPU cycle wasting program, useful for testing
*               the profiler.
*
****************************************************************************/


#ifndef ITER
    #define ITER 5000
#endif

unsigned long count;

void f10()
{
    int i;
    int j;
    for( i = 0; i < 10; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
}
void f20()
{
    int i;
    int j;
    for( i = 0; i < 20; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
}
void f30()
{
    int i;
    int j;
    for( i = 0; i < 30; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
}
void f40()
{
    int i;
    int j;
    for( i = 0; i < 40; ++i ) {
        for( j = 0; j < ITER; ++j ) {
            ++count;
        }
    }
}

int main()
{
    int i;
    for( i = 0; i < 100; ++i ) {
        f10();
        f20();
        f30();
        f40();
    }
    return 0;
}
