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


#ifndef __FOREST_INTERFACE_H
#define __FOREST_INTERFACE_H

#include <wobject.hpp>

/*
 * ForestListInterface provides a wrapper for accessing a forest of
 * trees by a "nested" list box.
 *
 * To use : derive from this class an appropriate implementation for
 *          your data structure; instantiate the nested list box with
 *          a pointer to an object of the derived class
 */
class ForestListInterface : public WObject {

public :
/*-------------------------------------------------------------------------
  The constructor should load up any data and construct the forest
  if necessary.
  -------------------------------------------------------------------------*/
    ForestListInterface() {}
    ~ForestListInterface() {}

    void destroySelf() { delete this; }

/*-------------------------------------------------------------------------
  This is called when the nested list box requests information about a
  child of a node.  Given a pointer to the parent node and the index
  of the child that information is requested about, the method should
  return :
        NULL if the index (or node pointer) is invalid

    otherwise

        The name of the child.
        A pointer to the the data structure associated with the node.

  Note : If passed NULL as pParent, it is requesting a root node

  -------------------------------------------------------------------------*/
    virtual     void    getChildData( void * pParent,
                                      int index,
                                      void * & pChild,
                                      char * & pChildName,
                                      int & numChildren ) = 0;

/*-------------------------------------------------------------------------
  This is called by the nested list box to find out how many children
  a node has.

  If passed a NULL pointer, then it is requesting the number of roots in
  the forest.
  -------------------------------------------------------------------------*/
    virtual     int     getNumChildren( void * pParent ) = 0;
};

#endif

