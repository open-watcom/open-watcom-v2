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


// simple fixed size stack with no error checking
#include <iostream.h>

template <class T,int size>
    struct Fixed_Size_Stack {
        Fixed_Size_Stack() : index(0)
        {
        }
        void push( T x )
        {
            storage[index] = x;
            ++index;
        }
        T pop( void )
        {
            --index;
            return storage[index];
        }
    private:
        T storage[size];
        unsigned index;
    };

const int TEST_SIZE = 10;

void main()
{
    Fixed_Size_Stack<int,TEST_SIZE> reversing;

    cout << "pushing: ";
    for( int i = 0; i < TEST_SIZE; ++i ) {
        cout << i << " ";
        reversing.push( i );
    }
    cout << endl;
    cout << "popping: ";
    for( int j = 0; j < TEST_SIZE; ++j ) {
        cout << reversing.pop() << " ";
    }
    cout << endl;
}
