



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






#ifndef _treeimp_cxx_
#define _treeimp_cxx_


#define _no_cl_tree_typedefs_
#include "base/tree.h"
#include "base/binding.h"
#include "base/basicops.h"

#ifdef __GNUC__
#pragma implementation
#endif


//
//---------------------------Node--------------------
//



template <class ItemType>
CL_TreeNode<ItemType>::CL_TreeNode (CL_TreeNodeLabel l)
: _label (l), _parent (NULL)
{
    _indexInParent = 0;
    _content = CL_Basics<ItemType>::MakeCopy
        (CL_Basics<ItemType>::NullValue());
}


template <class ItemType>
CL_TreeNode<ItemType>* CL_TreeNode<ItemType>::AddChild
    (CL_TreeNodeLabel l, long leftSiblingIndex)
{
    CL_TreeNode<ItemType>* node = new CL_TreeNode<ItemType> (l);
    return (node && AddChild (node, leftSiblingIndex)) ? node : NULL;
}




template <class ItemType>
CL_TreeNode<ItemType>* CL_TreeNode<ItemType>::Child (long index) const
{
    if (index < 0 || index >= _childPtrs.Size())
        return NULL;
    return (CL_TreeNode<ItemType>*) _childPtrs[index];
}
    

template <class ItemType>
void CL_TreeNode<ItemType>::_Destructor ()
{
    CL_Basics<ItemType>::Destroy (_content);
}
    
template <class ItemType>
ItemType& CL_TreeNode<ItemType>::Content ()
{
    return CL_Basics<ItemType>::Deref (_content);
}
    



template <class ItemType>
CL_TreeNode<ItemType>* CL_TreeNode<ItemType>::AddChild
    (CL_TreeNode<ItemType>* node, long leftSiblingIndex)
{
    if (leftSiblingIndex >= ChildCount())
        leftSiblingIndex = ChildCount() - 1;
    if (! _childPtrs.Insert (node, leftSiblingIndex))
        return NULL;
    node->_indexInParent = leftSiblingIndex + 1;
    node->_parent = this;
    return node;
}




//
//-----------------CL_Tree ---------------------------------------
//

template <class ItemType>
CL_Tree<ItemType>::CL_Tree()
{
    _root = NULL;
}

    
template <class ItemType>
CL_Tree<ItemType>::CL_Tree(CL_TreeNodeLabel rt)
{
    _root = new CL_TreeNode<ItemType> (rt);
    _map.Add (rt, _root);
}








template <class ItemType>
CL_TreeNode<ItemType>* CL_Tree<ItemType>::AddChild
    (CL_TreeNodeLabel obj, CL_TreeNodeLabel parent, long i)
{
    CL_TreeNode<ItemType>* node, *child;

    if (!PrepareToChange())
        return NULL;
    node = Node (parent);
    if (node == NULL)
        return NULL;
    if (_map.IncludesKey (obj))
        return NULL;
    child = node->AddChild (obj, i);
    if (child && _map.Add (obj, child)) {
        Notify();
        return child;
    }
    else
        return NULL;
}     


    

template <class ItemType>
CL_TreeNode<ItemType>* CL_Tree<ItemType>::NewRoot
    (CL_TreeNodeLabel l)
{
    if (!PrepareToChange())
        return NULL;
    CL_TreeNode<ItemType>* root = new CL_TreeNode<ItemType> (l);
    if (!root)
        return NULL;
    if ( (_root == NULL || root->AddChild (_root)) &&  _map.Add (l, root)) {
        _root = root;
        _root->_indexInParent = 0;
        Notify();
        return _root;
    }
    return NULL;
}

    


template <class ItemType>
CL_Tree<ItemType>* CL_Tree<ItemType>::ExtractSubtree(CL_TreeNodeLabel obj)
{
    if (!PrepareToChange())
        return NULL;
    CL_TreeNode<ItemType>* node = Node (obj);
    if (node == NULL)
        return NULL;
    CL_TreeNode<ItemType>* parent = node->_parent;
    if (parent) {
        (parent->_childPtrs).Remove (node->_indexInParent);
        long n = parent->ChildCount();
        for (long i = node->_indexInParent; i < n; i++) {
            CL_TreeNode<ItemType>* sibling = parent->Child(i);
            sibling->_indexInParent--;
        }
    }
    CL_Tree<ItemType>* new_tree = new CL_Tree<ItemType>;
    new_tree->_root = node;
    node->_parent = NULL;
    node->_indexInParent = 0;
    new_tree->_BuildSubmap (new_tree->_root);
    Notify();
    return new_tree;
}
    
template <class ItemType>
CL_TreeNode<ItemType>* CL_Tree<ItemType>::Node (CL_TreeNodeLabel l) const
{
    return (CL_TreeNode<ItemType> *) (((CL_Tree<ItemType>*) this)->_map)[l];
    // --------------------------------^^^^^^^^^^^^^^^^^^^^ cast away const
}



template <class ItemType>
void CL_Tree<ItemType>::DestroySubtree(CL_TreeNodeLabel l)
{
    if (!PrepareToChange())
        return;
    CL_TreeNode<ItemType>* node = Node (l);
    if (node == NULL)
        return;
    CL_TreeNode<ItemType>* parent = node->_parent;
    if (parent) {
        (parent->_childPtrs).Remove (node->_indexInParent);
        long n = parent->ChildCount();
        for (long i = node->_indexInParent; i < n; i++) {
            CL_TreeNode<ItemType>* sibling = parent->Child(i);
            sibling->_indexInParent--;
        }
    }
    _DeleteSubtree (node);
    Notify ();
}




template <class ItemType>
long CL_Tree<ItemType>::PostOrderWalk (CL_TreeNodeLabel l,
                                        const CL_AbstractBinding& bind) const
{
    long count = 0;
    CL_TreeNode<ItemType>* node = Node (l);
    if (node == NULL)
        return 0;
    CL_Binding<CL_Object> nullBinding (0, 0);
    _Walk (node, nullBinding, bind, count, 0);
    return count;
}



template <class ItemType>
long CL_Tree<ItemType>::Traverse (CL_TreeNodeLabel l,
                                  const CL_AbstractBinding& b1,
                                  const CL_AbstractBinding& b2) const
{
    long count = 0;
    CL_TreeNode<ItemType>* node = Node (l);
    if (node == NULL)
        return 0;
    _Walk (node, b1, b2, count, 0);
    return count;
}


// Protected CL_Tree methods:

template <class ItemType>
void CL_Tree<ItemType>::_DeleteSubtree (CL_TreeNode<ItemType>* node)
{
    if (!node)
        return;
    long n = node->ChildCount ();
    for (long i = 0; i < n; i++) {
        _DeleteSubtree ((CL_TreeNode<ItemType>*) (node->_childPtrs)[i]);
    }
    _map.Remove (node->_label);
    delete node;
}



template <class ItemType>
void CL_Tree<ItemType>::_BuildSubmap (CL_TreeNode<ItemType>* node)
{
    if (!node)
        return;
    long n = node->ChildCount ();
    for (long i = 0; i < n; i++) {
        _BuildSubmap ((CL_TreeNode<ItemType>*) (node->_childPtrs)[i]);
    }
    _map.Add (node->_label, node);
}

template <class ItemType>
bool CL_Tree<ItemType>::_Walk (CL_TreeNode<ItemType>* node,
                               const CL_AbstractBinding& b1,
                               const CL_AbstractBinding& b2,
                               long& count, long depth) const
{
    if (!node)
        return TRUE;
    if (b1.Valid() && !b1.Execute (*node, depth))
        return FALSE;
    long n = node->ChildCount ();
    count++;
    for (long i = 0; i < n; i++) {
        if (!_Walk (node->Child(i), b1, b2, count, depth+1)) return FALSE;
    }
    return b2.Valid() ? b2.Execute (*node, depth) : TRUE;
}






#endif  /* _treeimp_cxx_ */
