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


#include <iterator.h>
#include "treeview.h"

#pragma warning 549 9; // sizeof warning
defineAbstractClass(TreeView,Pinup);
#pragma warning 549 4;

TreeView::TreeView()
{
}

TreeView::TreeView(int x, int y, int w, int h, Window& prnt
        , bool showPin)
    :Pinup(x,y,w,h,prnt, showPin)
{
#if 0
    genData();
#endif
}

TreeView::~TreeView()
{
    hide();
}

void TreeView::cleanDisplay()
{
    _REVERSEDO(children,Window,child)
        child->free();
    ENDREV(Window,child)
}


#if 0
// just to generate some dummy model data
class Node : public Object
{
    DeclareClass(Node);
  public:
                Node(char* name, Node* parent);
                Node(){}
    virtual     ~Node(){}
    OrdCollect& parents(){return _parents;}
    OrdCollect& children(){return _children;}
    String&     name(String& s){s=_name;return s;}
  private:
    OrdCollect  _parents;
    OrdCollect  _children;
    String      _name;
};
extern Class* NodeCls;

#include <str.h>
#include <ordcllct.h>
#include "treenode.h"

#pragma warning 549 9; // sizeof warning
defineClass(Node,Object);
#pragma warning 549 4;

Node::Node(char* name, Node* parent)
    :_name(name)
{
    if(parent){
        _parents.add(parent);
        parent->_children.add(this);
    }
}

void TreeView::genData()
{
    // lots of temporory stuff to create dummy data:
    Node* s0=new Node("s0",NIL);

    Node* s0_0=new Node("s0_0",s0);
        Node* s0_0_0=new Node("s0_0_0",s0_0);
            Node* s0_0_0_0=new Node("s0_0_0_0",s0_0_0);
            Node* s0_0_0_1=new Node("s0_0_0_1",s0_0_0);
            Node* s0_0_0_2=new Node("s0_0_0_2",s0_0_0);
            Node* s0_0_0_3=new Node("s0_0_0_3",s0_0_0);
            Node* s0_0_0_4=new Node("s0_0_0_4",s0_0_0);
        Node* s0_0_1=new Node("s0_0_1",s0_0);
            Node* s0_0_1_0=new Node("s0_0_1_0",s0_0_1);
            Node* s0_0_1_1=new Node("s0_0_1_1",s0_0_1);
        Node* s0_0_2=new Node("s0_0_2",s0_0);
            Node* s0_0_2_0=new Node("s0_0_2_0",s0_0_2);
            Node* s0_0_2_1=new Node("s0_0_2_1",s0_0_2);
            Node* s0_0_2_2=new Node("s0_0_2_2",s0_0_2);

    Node* s0_1=new Node("s0_1",s0);
        Node* s0_1_0=new Node("s0_1_0",s0_1);
            Node* s0_1_0_0=new Node("s0_1_0_0",s0_1_0);
            Node* s0_1_0_1=new Node("s0_1_0_1",s0_1_0);
                Node* s0_1_0_1_0=new Node("s0_1_0_1_0",s0_1_0_1);
                Node* s0_1_0_1_0_0=new Node("s0_1_0_1_0_0",s0_1_0_1_0);
                Node* s0_1_0_1_0_1=new Node("s0_1_0_1_0_1",s0_1_0_1_0);
                Node* s0_1_0_1_1=new Node("s0_1_0_1_1",s0_1_0_1);
            Node* s0_1_0_2=new Node("s0_1_0_2",s0_1_0);
        Node* s0_1_1=new Node("s0_1_1",s0_1);
            Node* s0_1_1_0=new Node("s0_1_1_0",s0_1_1);
            Node* s0_1_1_1=new Node("s0_1_1_1",s0_1_1);
            Node* s0_1_1_2=new Node("s0_1_1_2",s0_1_1);
        Node* s0_1_2=new Node("s0_1_2",s0_1);
            Node* s0_1_2_0=new Node("s0_1_2_0",s0_1_2);
        Node* s0_1_3=new Node("s0_1_3",s0_1);
            Node* s0_1_3_0=new Node("s0_1_3_0",s0_1_3);
        Node* s0_1_4=new Node("s0_1_4",s0_1);
            Node* s0_1_4_0=new Node("s0_1_4_0",s0_1_4);


    TreeNode* middle = new TreeNode(50,50,*this,*s0_1
                                , LoadOrdCllctMthd(&Node::parents)
                                , LoadOrdCllctMthd(&Node::children)
                                , GetNameMthd(&Node::name));
    middle->show();
//    middle->showChildren();
}
#endif
