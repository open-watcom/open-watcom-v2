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


//
//  wclcom.h    Definitions for some common list classes used by
//              the WATCOM Container List Classes
//
//  Copyright by WATCOM International Corp. 1988-1993.  All rights reserved.
//

#ifndef _WCLCOM_H_INCLUDED

#ifndef __cplusplus
#error wclcom.h is for use with C++
#endif



//
//  The techniques for specifying the list classes are from
//  'The C++ Programming Language', 2nd Edition, Bjarne Stroustrup.
//




//
//  The WCSLink class is used as a basis for manipulating a number of
//  different containers.  In an intrusive container, this class provides
//  the base for the user defined class of single linked lists.
//
//  This class is intended to be a base class.  Objects of this type should
//  not be created.
//
//  constructor: set the 'next' link field to 0 (will be reset when inserted
//               into the list).
//  destructor: nothing needs to be explicitly destroyed in WCSLink.
//              It is not virtual, to save the size cost of storing a virtual
//              table pointer in every node.
//

class WCSLink {
public:
    WCSLink *       link;

    inline WCSLink() : link( 0 ) {};
    inline ~WCSLink() {};
};




//
//  The WCDLink class is used as a basis for manipulating a number of
//  different containers.  In an intrusive container, this class provides
//  the base for the user defined class of double linked lists.  The
//  WCSLink class is used as a base for this class.
//
//  This class is intended to be a base class.  Objects of this type should
//  not be created.
//
//  constructor: the 'prev' link field is set up by the WCSLink constructor.
//  destructor: nothing needs to be explicitly destroyed in WCDLink.
//              It is not virtual, to save the size cost of storing a virtual
//              table pointer in every node.
//

class WCDLink : public WCSLink {
public:
    WCSLink         prev;


    inline WCDLink() {};
    inline ~WCDLink() {};
};




#define _WCLCOM_H_INCLUDED
#endif
