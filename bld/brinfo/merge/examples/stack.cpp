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


#include "stack.hpp"

static const int        StackSize=10;

Stack::Stack()
{
    _size = StackSize;
    _top = -1;
    _stack = new int[StackSize];
}

Stack::~Stack()
{
    delete[] _stack;
}

bool Stack::IsEmpty() const
{
    return _top<0;
}

bool Stack::IsFull() const
{
    return _top == StackSize-1;
}

int Stack::Size() const
{
    return _size;
}

int Stack::Top() const
{
    if( !IsEmpty() ){
        return _stack[_top];
    } else {
        return 0;
    }
}

void Stack::Push(int i)
{
    if( !IsFull() ){
        _stack[++_top] = i;
    }
}

void Stack::Pop()
{
    if( !IsEmpty() ){
        _top--;
    }
}
