



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */








#ifndef _treewalk_cxx_ /* Tue Mar 15 12:53:06 1994 */
#define _treewalk_cxx_


#define _no_cl_treewalk_typedefs_
#include "base/treewalk.h"
#include "base/binding.h"




// --------------------- CL_PostOrderWalker -----------------------------




template <class ItemType>
class StackEntry: public CL_Object {

public:
    CL_TreeNode<ItemType>* node;
    long                   index;

    StackEntry (CL_TreeNode<ItemType>* n) : node (n), index (0) {};
};


// The stack defines the path to the node that will be returned on the
// next call to Next.


#ifdef __GNUC__
#pragma implementation
#endif


template <class ItemType>
CL_PostOrderWalker<ItemType>::CL_PostOrderWalker
    (CL_TreeNode<ItemType>*  node)
: _subtreeRoot (node)
{
}

template <class ItemType>
CL_PostOrderWalker<ItemType>::~CL_PostOrderWalker ()
{
    _stack.DestroyContents();
}


template <class ItemType>
void CL_PostOrderWalker<ItemType>::Reset ()
{
    _stack.DestroyContents ();
    CL_TreeNode<ItemType>* node = _subtreeRoot;
    while (node) {
        StackEntry<ItemType>* p = new StackEntry<ItemType> (node);
        if (!p)
            return; // No memory
        _stack.Add (p);
        node = node->Child(0);
    }
}




template <class ItemType>
bool CL_PostOrderWalker<ItemType>::More ()
{
    return (_stack.Size() > 0);
}



template <class ItemType>
CL_TreeNode<ItemType>* CL_PostOrderWalker<ItemType>::Next ()
{
    long n = _stack.Size();
    if (n <= 0)
        return NULL;
    StackEntry<ItemType>* current = (StackEntry<ItemType>*) _stack[n-1];
    if (n == 1) {
        CL_TreeNode<ItemType>* retVal = current->node;
        _stack.DestroyContents ();
        return retVal;
    }

    // So the stack has at least two entries:
    // Does current node have a right sibling?
    StackEntry<ItemType>* parent = (StackEntry<ItemType>*) _stack[n-2];
    CL_TreeNode<ItemType>* ret_val = current->node;
    if (parent->node->ChildCount() > parent->index + 1) {
        // Yes, there's a right sibling; move to it
        parent->index++;
        current->node = parent->node->Child (parent->index);
        // Now move left and down
        CL_TreeNode<ItemType>* node = current->node;
        do {
            node = node->Child(0);
            if (!node) break;
            StackEntry<ItemType>* p = new StackEntry<ItemType> (node);
            if (!p)
                return NULL; // No memory
            _stack.Add (p);
        } while (1);
    }
    else {
        // No right sibling; just move to the parent
        StackEntry<ItemType>* p = (StackEntry<ItemType>*)
            _stack.ExtractRightmost();
        delete p;
    }
    return ret_val;
}



// --------------------- CL_PreOrderWalker -----------------------------





template <class ItemType>
CL_PreOrderWalker<ItemType>::CL_PreOrderWalker
  (CL_TreeNode<ItemType>*  node)
: _subtreeRoot (node)
{
}


template <class ItemType>
CL_PreOrderWalker<ItemType>::~CL_PreOrderWalker ()
{
    _stack.DestroyContents();
}



template <class ItemType>
void CL_PreOrderWalker<ItemType>::Reset ()
{
    _stack.DestroyContents ();
    CL_TreeNode<ItemType>* node = _subtreeRoot;
    if (!node)
        return;
    StackEntry<ItemType>* p = new StackEntry<ItemType> (node);
    if (!p)
        return; // No memory
    _stack.Add (p);
}




template <class ItemType>
bool CL_PreOrderWalker<ItemType>::More ()
{
    return (_stack.Size() > 0);
}



template <class ItemType>
CL_TreeNode<ItemType>* CL_PreOrderWalker<ItemType>::Next ()
{
    long n = _stack.Size();
    if (n <= 0)
        return NULL;
    StackEntry<ItemType>* current = (StackEntry<ItemType>*) _stack[n-1];
    CL_TreeNode<ItemType>* ret_val = current->node;

    if (!current->node->IsLeaf()) {
        // It's an internal node; move to its leftmost child
        CL_TreeNode<ItemType>* node = current->node->Child(0);
        StackEntry<ItemType>* p = new StackEntry<ItemType> (node);
        if (!p)
            return NULL; // No memory
        _stack.Add (p);
    }
    else {
        // It's a leaf; move up the tree until we find an ancestor that has
        // a right sibling (the ancestor might be the current node itself)
        StackEntry<ItemType>* p = (StackEntry<ItemType>*)
            _stack.ExtractRightmost ();
        delete p;
        if (_stack.Size() <= 0) // We just finished
            return NULL;
        do {
            long n = _stack.Size();
        if (n <= 0) break;
            p = (StackEntry<ItemType>*) _stack[n-1];
            if (p->index < p->node->ChildCount() - 1)
                break;
            delete (StackEntry<ItemType>*) _stack.ExtractRightmost();
        } while (1);
        if (_stack.Size() > 0) {
            // We still have a few more nodes
            p->index++;
            StackEntry<ItemType>* q = new StackEntry<ItemType>
                (p->node->Child (p->index));
            if (!q) // No memory
                return NULL;
            _stack.Add (q);
        }
    }
    return ret_val;
}





#include "base/treewdef.h"

template class  CL_PostOrderWalker<CL_ObjectPtr>;
template class  CL_PreOrderWalker<CL_ObjectPtr> ;
template class  StackEntry<CL_ObjectPtr>;

template class  CL_PostOrderWalker<CL_VoidPtr>  ;
template class  CL_PreOrderWalker<CL_VoidPtr>   ;
template class  StackEntry<CL_VoidPtr>;

template class  CL_PreOrderWalker<long> ;
template class  CL_PostOrderWalker<long>;
template class  StackEntry<long>;


#endif /* _treewalk_cxx_ */

