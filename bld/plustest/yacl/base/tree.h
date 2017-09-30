



#ifndef _tree_h_ /* Mon Aug 23 17:09:26 1993 */
#define _tree_h_





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





// This is  a  tree class that maintains parent-child relationships among
// objects drawn from  a LabelType class.   The tree class is  a template
// class based  on the  LabelType parameter. Each  node  of the  tree  is
// labeled with an  element of type  LabelType, and all  the nodes of the
// tree are required to have distinct labels.  In addition,  each node of
// the tree may contain a pointer to a user-defined object.
// 
// The destructor of the tree destroys all the nodes in it, but  does not
// invoke the destructors of the user-defined objects stored in the nodes
// (i.e., the objects referenced by the Content() method of the Node). If
// the tree is instantiated with a pointer-based template base type (as is
// GenericTree), the pointed-to objects are {\it not\/} destroyed by the
// tree's destructor; a tree walker (see {\tt treewalk.h}) can be used to
// do so before destroying the tree.
// 
// Caveat: In the interests of future expandability, do NOT use templated
// names  for types; use only the  typedef names at  the end of this file
// (e.g., use VoidPtrTree but  not {\small\tt  CL_Tree <VoidPtr>}).  This
// will allow the implementation to be made more efficient (later) without
// affecting the  interface.


#include "base/object.h"
#include "base/sequence.h"
#include "base/map.h"
#include "base/binding.h"

#ifdef __GNUC__
#pragma interface
#endif


template <class ItemType> class CL_TreeNode;
    

typedef long CL_TreeNodeLabel;

template <class ItemType>
class __CLASSTYPE CL_Tree: public CL_Object {

public:

    // --------------- Construction and destruction ----------------

    CL_Tree ();
    // Default constructor: build an empty tree.

    CL_Tree (CL_TreeNodeLabel root);
    // Constructor: build a tree with root having the given label.

    ~CL_Tree()
    { _DeleteSubtree (_root);}
    // Destructor.
    
    // -------------------- Querying operations ------------------------

    CL_TreeNode<ItemType>* Root () const
        { return _root;};
    // Return the root of the tree.

    CL_TreeNode<ItemType>* Node (CL_TreeNodeLabel l) const;
    // Return the node with the given label, if it exists; return NULL
    // if no such node. The return value points to memory owned by the
    // tree, and must not be destroyed by the caller.
    
    

    // ---------- Addition and removal of nodes and subtrees -----------
    
    CL_TreeNode<ItemType>* AddChild (CL_TreeNodeLabel obj,
                                      CL_TreeNodeLabel parent,
                                      long index = 200000);
    // Add obj as the i-th child of the given parent; return the newly
    // created node. Return NULL on error (e.g. parent is not in tree,
    // invalid index specified, a node with the given label is already
    // in the tree, or a memory allocation error occurs). An index of
    // -1 specifies leftmost child. 
    // Specifying a very larger index (specifically, anything larger
    // than the number of siblings) causes addition as the rightmost
    // child. The default value of the index is large enough, in most
    // situations, to mean addition as rightmost child.

    CL_TreeNode<ItemType>* NewRoot (CL_TreeNodeLabel l);
    // Create a new root with given label, and make the current root
    // the (only) child of the new root. Return the new root if
    // successful, and NULL on failure.
    
    CL_Tree<ItemType>* ExtractSubtree (CL_TreeNodeLabel obj);
    // Remove and return the subtree rooted at the given node. The
    // returned tree must be destroyed by the caller of this method.
    
    void DestroySubtree (CL_TreeNodeLabel x);
    // Destroy the subtree rooted at x, and remove x from its
    // parent's set of children.

    // --------------------- Traversal -----------------------------

    long PostOrderWalk (CL_TreeNodeLabel l, const
                        CL_AbstractBinding& bind) const;
    // Perform a post-order traversal of the subtree rooted at the
    // node with the given label, and at each node in the traversal,
    // invoke the given method with  the first parameter being the
    // node and  the second parameter being the depth of the current
    // node in the subtree being traversed (note that this NOT the
    // depth in the entire tree unless the traversal starts at the
    // root of the tree!). The traversal continues while the called
    // method returns TRUE, and  stops when either the method returns
    // FALSE or the subtree has been completely visited. The called
    // method should NOT modify the tree. 
    //    The return value is the number of nodes traversed.

    long Traverse (CL_TreeNodeLabel l, const CL_AbstractBinding& action1,
                   const CL_AbstractBinding& action2) const;
    // This is a generalized traversal of the tree,
    // incorporating both preorder and postorder as special cases.
    // It traverses the subtree rooted at the node with the given label,
    // using the following algorithm:
    // 
    //        Invoke {\tt action1} at current node $v$ \\
    //        For each child $w$ of current node $v$, in left-to-right order,
    //            recursively traverse the subtree rooted at $w$ \\
    //        Invoke {\tt action2} at current node $v$ \\
    //
    // The parameter values to the methods of {\tt action1} and {\tt
    // action2}, and 
    // the return value of the method, are as in the method PostOrderWalk.
    // The traversal terminates when either the subtree has been
    // completed, or when either of the action calls returns FALSE.
    // Note that there are {\it two\/} calls at each node: once when entering
    // the node and once when backing up into the node. This is true
    // even for leaf nodes.
    //
    // If either the object or the method of a given binding is NULL,
    // the action is not invoked.
    //
    // Neither of the action methods should modify the tree.
    
    // --------------------- Basic methods ---------------------------

    const char* ClassName() const { return "CL_Tree";};
    
    CL_ClassId ClassId() const   { return _CL_Tree_CLASSID;};


    // --------------------- End public protocol ----------------------
    
    
    
protected:

    void _DeleteSubtree (CL_TreeNode<ItemType>* node);

    void _BuildSubmap   (CL_TreeNode<ItemType>* node);

    bool _Walk          (CL_TreeNode<ItemType>* node,
                         const CL_AbstractBinding& b1,
                         const CL_AbstractBinding& b2,
                         long& count, long depth) const;
    
    CL_TreeNode<ItemType>* _root;
    CL_IntPtrMap           _map; // The map of label values to node pointers
};





// The class TreeNode is intended to be used only in conjunction with the
// class Tree; nodes of the tree may be accessed via the Node() method of
// the tree. Do not use node methods for modifying the node directly.

template <class ItemType>
class __CLASSTYPE CL_TreeNode: public CL_Object {
    
public:
    CL_TreeNode (CL_TreeNodeLabel lbl);

    ~CL_TreeNode();

    // ------------------ Querying functions -------------------------

    CL_TreeNodeLabel Label () const
    { return _label; }

    CL_TreeNode<ItemType>* Parent () const
    { return _parent; };
    
    const CL_ObjectSequence& Children () const {return  _childPtrs;};

    CL_TreeNode<ItemType>* Child (long childIndex) const;

    long IndexInParent () const
    { return _indexInParent; };
    
    long ChildCount () const
    { return _childPtrs.Size();};

    bool IsLeaf () const
    {return ChildCount() == 0;};

    ItemType& Content ();
    // Return the user-specified content of this node. Note that a
    // reference is returned, which can be modified by the user of
    // this class.

protected:
    friend class CL_Tree<ItemType>;

    // ------------------- Modification functions -------------------

    CL_TreeNode<ItemType>* AddChild (CL_TreeNodeLabel lbl, long
                                     leftSiblingIndex = 200000L);
    // Add a child to this node, immediately to the right of the child
    // with given index. Index of -1 specifies leftmost child.
    // Specifying a very larger index (specifically, anything larger
    // than the number of siblings) causes addition as the rightmost
    // child. The default value of the index is large enough, in most
    // situations, to mean addition as rightmost child.
    // If successful, return a pointer to the new child node; otherwise,
    // return NULL.

    
    CL_TreeNode<ItemType>* AddChild (CL_TreeNode<ItemType>* node,
                                      long leftSiblingIndex = 200000L);



    CL_TreeNodeLabel        _label;
    CL_TreeNode<ItemType>*  _parent;
    CL_ObjectSequence       _childPtrs;      // Ptrs to nodes
    long                    _indexInParent;  // Child number of this
                                             // node
    CL_VoidPtr              _content;

private:
    void                    _Destructor(); // Sidestep gcc bug
    
};




template <class BaseType>
inline CL_TreeNode<BaseType>::~CL_TreeNode()
{
    _Destructor ();
}





#ifndef _no_cl_tree_typedefs_
#include "base/treedef.h"
#endif



#endif


