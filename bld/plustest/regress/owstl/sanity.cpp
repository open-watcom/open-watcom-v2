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
* Description: This file contains helper function/macros to facilitate
*              sanity checks in the other OWSTL test programs. The
*              facilities defined here are facilities of interest to
*              all programs in the test suite.
*
*              This file is intended to be #included into the other test
*              files (it contains macros).
****************************************************************************/

#define FAIL \
    { std::cout << "FAIL! Line: " << __LINE__ << "\n"; return( false ); }


#if defined(__WATCOMC__)

  #include <malloc.h>

  #define INSANE(x) ( !( x )._Sane( ) )

  bool heap_ok( const char *msg )
  {
    bool rc = true;
    int heap_status = _heapchk( );
    if( heap_status != _HEAPOK && heap_status != _HEAPEMPTY ) {
      std::cout << "!!! HEAP CONSISTENCY FAILURE: " << msg << "\n";
      rc = false;
    }
    return( rc );
  }

  int heap_count( )
  {
    int used_size = 0;
    struct _heapinfo info;

    info._pentry = NULL;
    if( _heapwalk( &info ) != _HEAPOK )
      return( 0 );
    info._pentry = NULL;
    while( _heapwalk( &info ) != _HEAPEND ) {
      if( info._useflag == _USEDENTRY ) {
        used_size += info._size;
      }
    }
    return( used_size );
  }

  void heap_dump()
  {
    struct _heapinfo h_info;
    int heap_status;

    h_info._pentry = NULL;
    for(;;) {
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK ) break;
        std::cout << (h_info._useflag == _USEDENTRY ? "USED" : "FREE")<<
            " block at " << reinterpret_cast<int>(h_info._pentry) <<
            " of size " << h_info._size << "\n" ;
    }

    switch( heap_status ) {
    case _HEAPEND:
      std::cout << "OK - end of heap\n" ;
      break;
    case _HEAPEMPTY:
      std::cout << "OK - heap is empty\n" ;
      break;
    case _HEAPBADBEGIN:
      std::cout << "ERROR - heap is damaged\n" ;
      break;
    case _HEAPBADPTR:
      std::cout << "ERROR - bad pointer to heap\n" ;
      break;
    case _HEAPBADNODE:
      std::cout << "ERROR - bad node in heap\n" ;
      break;
    default:
      std::cout << "unexpected!\n";
    }
  }

  
  
  
#else

  #define INSANE(x) false

  bool heap_ok( const char * )
  {
    return( true );
  }

  int heap_count( )
  {
    return( 0 );
  }
  
  heap_dump( )
  {
  }

#endif
