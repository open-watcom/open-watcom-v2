/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2006 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Verify that NOMINMAX works correctly with <windows.h>.
*               This is in OWSTL because, if the macros max(a,b) and
*               min(a,b) are defined by <windows.h> when it is included
*               before <algorithm> or <limits> (or <deque> or <vector>,
*               which include <limits>), then compiler errors result
*               because the macros conflict with the template functions
*               (<algorithm>) or static member functions (<limits>) with
*               the same names.
*
* Useage Notes: This file should be used to test these two files:
*                   Windefs.h in h\nt
*                   Win16.h in h\win
*               It has been tested with Windows XP and OS/2 and it works
*               with bt=nt or bt=os2 (respectively), provided the include
*               path includes exactly one of h\nt or h\win (depending on
*               which one is being tested).
*               To verify this test's functionality, simply open the
*               file(s) given above and de-activate (comment out) the
*               NOMINMAX macro test. When the macro test in the file is
*               disabled, this test will report failure; when the macro
*               test is enabled (uncommented), this test will report
*               success. Doing this one file at a time will show that
*               each test is, in fact, testing the correct file.
*
****************************************************************************/

#include <iostream>

#include "sanity.cpp"

#define NOMINMAX

// mmWin16 is used with NO_EXT_KEYS to trap use of -za (strict ANSI
// compliance) on the compiler command line. 
// win16.h, it appears, is not ANSI-compliant.

#if defined( mmWin16 )
#if !defined( NO_EXT_KEYS )
#include <windows.h>
#endif
#else
#include <windows.h>
#endif

bool some_test( )
{
    bool rc = true;

// The macro FAIL is not used because it uses cout. See below.

#if defined( min ) || defined( max )
    rc = false;
#endif

    return( rc );
}

int main( ) {
    int rc = 0;
    int original_count = heap_count( );

// Using error_msg prevents cout from affecting the final heap check.

    char const * error_msg = NULL;

    try {

        if( !some_test( ) ) {

// The output string here depends on which test was done.

#if defined( mmWin16 )
            error_msg = "mmwin16 test failed!\n";
#else
            error_msg = "mmwindef test failed!\n";
#endif
            rc = 1;
        }

// heap_ok uses cout to output the parameter. See below.

        if( !heap_ok( "Possible heap problem!\n" ) ) rc = 1;
    }
    
    catch( ... ) {
        error_msg = "Unexpected exception of unexpected type.\n";
        rc = 1;
    }

// Because cout uses the heap in outputting a string literal, this test
// will fail if heap_ok() reports an error. FAIL is not used in
// some_test() so that my test's failure report will not producing a 
// bogus error report here.

// Since the approach shown here really won't scale well with multiple tests,
// it might be better to restructure main() so that this test is only done
// if all the other tests are passed. This would avoid the bogus report
// caused by using cout, allow FAIL to be used as intended, and still test
// the heap when this test would actually find an error.

// It might be desireable to modify FAIL to report the module as well as
// the line number of the error.

    if( heap_count( ) != original_count ) {
        error_msg = "Possible memory leak!\n";
        rc = 1;
    }

// Emit the error_msg, if there is one. This would entirely disappear if
// the above note is followed and main() is restructured.

    if( error_msg != NULL ) {
        std::cout << error_msg;
    }
    
    return( rc );
}

