

#ifndef _btree_h_ /* Sun Jan 17 20:31:53 1993 */
#define _btree_h_





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




// There are four classes related to the generic B-tree:
// \par\begin{tabular}{lp{3.5in}}
//   CL_GenericBTree&       a class that encapsulates B-tree algorithms\\
//   CL_GenericBTreeNode&   which defines the structure  of a node in a
//                          B-tree\\
//   CL_GenericBTreeIterator& an object that  allows  inspection  of  the
//                          items in a B-tree in ascending order\\
//   CL_BTreeNodeSpace&     which  defines  an  object  for  managing a
//                          repository of B-tree nodes\\
// \end{tabular}\par
// The  algorithms implemented  here  assume that items   are stored in the
// internal nodes  as  well as in  the leaves;  this  is therefore not  the
// B+-tree (the one in which items are stored only at the leaves).
// 
// To enhance reusability, the B-tree is viewed as operating on a NodeSpace
// object. The latter is an object that manages the  space of nodes for the
// tree via {\it node  handles.}  The   B-tree's  methods  do  not make any
// assumptions about the NodeSpace other than the  public protocol; thus it
// is possible to create in-memory or disk-based (and even shared-memory or
// remote-server-based)   B-trees  simply   by using   different  NodeSpace
// objects. The B-tree methods access the nodes via a BTreeNodeHandle.
//
// The generic B-tree does {\it not\/} own the objects that the pointers
// point to, and therefore does not delete them when it is destroyed. The
// BTreeIterator can be used to iterate over the tree's contents and
// destroy pointed-to objects.


//  Revision history:
//
//      M. A. Sridhar          April 28th, 1993
//
//      Completely redone:     October 25, 1993 -- MAS
//
 


#include "base/cmparatr.h"

#ifdef __GNUC__
#pragma interface
#endif




typedef long CL_BTreeNodeHandle;

class CL_BTreeNodeSpace;
class CL_GenericBTreeIterator;



class CL_GenericBTree {
 
public:


    // --------------------- Construction and destruction ------------------

    
    CL_GenericBTree (CL_AbstractComparator& cmp,
                     short order = 2, CL_BTreeNodeSpace* space = NULL);
    // Create a new B-tree of given order. Duplicate items are not
    // allowed. The NodeSpace may by created by the derived class and
    // passed to this constructor; if it is NULL, a default in-memory node
    // space is created. If the derived class passes a non-null NodeSpace,
    // it is the responsibility of the derived class to destroy the
    // NodeSpace object.
    //
    // The third parameter specifies the comparator to be used when
    // comparing two cells.
    //
    // The order must be at least 2; anything
    // less than 2 is taken to be 2.
    
 
    virtual ~CL_GenericBTree ();
    // Destructor.


    // ----------------------- Search and related methods ------------------

    CL_BTreeNodeSpace* NodeSpace () const
        {return _nodeSpace;};
    // Return the NodeSpace used by this tree.

    CL_AbstractComparator& Comparator () const {return _comparator;};
    // Return a reference to the comparator used by this tree.
    
    virtual CL_VoidPtr Find (CL_VoidPtr item) const;
    // Search the tree for the given item. Return a pointer to the
    // found item if the search was successful, as the function value. If
    // the search fails, the return value is NULL.

    CL_VoidPtr Smallest () const {return ItemWithRank (0);};
    // Find and return the minimum item. If the tree is empty,
    // the null pointer is returned. The implementation simply returns the
    // value {\tt ItemWithRank (0)}.

    CL_VoidPtr Largest () const {return ItemWithRank (Size()-1);};
    // Find and return the minimum item. If the tree is empty,
    // the null pointer is returned. The implementation simply returns the
    // value {\tt ItemWithRank (Size()-1)}.


    virtual CL_VoidPtr ItemWithRank (long i) const;
    // Given an index $i$ between 0 and Size()-1, return the element of rank
    // $i$, i.e., the element that has $i$ elements less than it in the tree.
    // If $i \le 0$, this returns the smallest element, and if $i \ge {\tt
    // Size()}$, this returns the largest element. If the tree is empty,
    // the null value of the base type is returned. The implementation
    // examines only the nodes on the path from the root to the one
    // containing the key sought, and therefore takes no more than $\log
    // n$ time steps with $n$ keys in the tree.
    //
    //   Note that it is possible to iterate through the elements of the tree
    // via calls to this method, varying the index from 0 to Size()-1;
    // however, this is much less efficient than using the BTreeIterator.

    virtual long RankOf (CL_VoidPtr p) const;
    // Return the number of elements in the tree that are less than the
    // parameter.
    
    virtual long Size () const;
    // Return the size of the tree (number of items currently present).
    // The implementation needs constant time regardless of tree size.


    // ------------------------ Modification ------------------------------

    virtual bool Add  (CL_VoidPtr item);
    // Add the item to the tree. Return TRUE if successfully added,
    // FALSE if the item was already in the tree.
 
    virtual CL_VoidPtr Remove (CL_VoidPtr item);
    // Remove the specified item from the tree. Return NULL if the
    // item was not found in the tree, and the found item otherwise. The
    // implementation needs (in the worst case) two passes over the path
    // to the key, and so takes $2\log n$ time steps with $n$ keys in the
    // tree. It also coalesces any non-full nodes along the path from the
    // root to the deleted key.

    virtual CL_VoidPtr ExtractMin ();
    // Remove and return the smallest item in the tree. Return the null
    // pointer if the tree is empty.


    
    // --------------------- End public protocol -----------------------

 
        
protected:

    //------------------- Protected helper methods ---------------------


    enum DeleteActionEnum {NoAction, RotateLeft, RotateRight, Merge};
    
    bool                 _InsertNonFull (class CL_GenericBTreeNode* n1,
                                        CL_VoidPtr p);
    
    void                 _SplitChild (CL_GenericBTreeNode* n,
                                     short i, CL_GenericBTreeNode*);

    CL_GenericBTreeNode* _DescendInto (CL_GenericBTreeNode*, short,
                                       DeleteActionEnum&);

    CL_GenericBTreeNode* _Adjust (CL_GenericBTreeNode* node, short index,
                                  CL_GenericBTreeNode* c0,
                                  CL_GenericBTreeNode* c1, DeleteActionEnum&);

    // --------------------- Friend declarations --------------------

    //    friend CL_GenericBTreeNode;

    
    //------------ Instance data -----------------------------
    short                   _order;
    CL_BTreeNodeSpace*      _nodeSpace;
    CL_AbstractComparator&  _comparator;

private:
    bool  _ownNodeSpace;
};




// This class encapsulates a single node of the B-tree.


class CL_GenericBTreeNode {

public:

    // ------------------ Access and Manipulation -----------------

    long Size() const {return _keyCount;};
    // Return the number of items in this node.

    CL_VoidPtr Item (short i) const {return _item[i];};
    // Return the i-th item.  The value i must be such that
    // $0 \le i < {\tt Size()}$.

    CL_BTreeNodeHandle Subtree (short i) const {return _subtree[i];};
    // Return the handle of the i-th subtree. The value i must be such that
    // $0 \le i \le {\tt Size()}$.

    long SubtreeSize() const {return _subtreeSize;};
    // Return the number of keys in the subtree rooted at this node. This
    // method consults an instance variable, and therefore takes constant
    // time; it does not need to  traverse the subtree.
    
    CL_BTreeNodeHandle Handle() {return _handle;};
    // Return a reference to the handle for this node.

    CL_BTreeNodeSpace* NodeSpace () const {return _nodeSpace;};
    // Return the NodeSpace in which this node lives.
    
    virtual bool Search (CL_VoidPtr key, short& position) const;
    // Search the node for the given key; return greatest $i$ such that
    // {\tt key[i] <= key}. Return TRUE if {\tt key[i] == key}, FALSE
    // otherwise.

    virtual void ShiftRightAt (short pos, short amount = 1);
    // Shift all the keys and subtrees, beginning at position pos,
    // right by the given amount. Note that the subtree to the left of
    // key[pos] is {\it also\/} moved.

    virtual void ShiftLeftAt (short pos, short amount = 1);
    // Shift all the keys and subtrees, beginning at position pos,
    // left by the given amount. Note that the subtree to the left of
    // key[pos] is {\it also\/} moved.

    virtual void MoveSubNode (const CL_GenericBTreeNode& x, short pos,
                      short our_pos, short nkeys);
    // MoveSubNode: Move {\it nkeys\/} keys, and their left and right
    // subtrees, beginning from position pos in node $x$ into ourselves
    // beginning at position {\it our_pos}.


    // --------------------- End public protocol -----------------------


protected:

    CL_AbstractComparator& _cmp;      // Comparator must be initialized
                                      // BEFORE NodeSpace is initialized
    CL_BTreeNodeSpace*   _nodeSpace;  
    CL_BTreeNodeHandle* _subtree;     // Pointer to array of subtree handles
    CL_VoidPtr*         _item;        // Pointer to array of items
    short               _keyCount;    // # keys in node
    bool                _isLeaf;      // Is this node a leaf?
    long                _subtreeSize; // # keys in subtree rooted at this node

    CL_BTreeNodeHandle  _handle;      // Our handle
    short               _order;

    friend CL_GenericBTree;
    friend CL_GenericBTreeIterator;
    friend CL_BTreeNodeSpace;
 
    // ---------------- Construction and destruction ---------------
    
    CL_GenericBTreeNode (short order, CL_BTreeNodeSpace* space,
                         CL_AbstractComparator& cmp);
    // Constructor: create a node of the B-tree with given order.

    virtual ~CL_GenericBTreeNode();
    // Destructor.

};




// The NodeSpace is the space in which tree nodes are stored. Nodes in
// this space are accessed via handles encapsulated by the
// SubTreeHandle class. The NodeSpace functions as a kind of "node
// library" from which nodes can be "borrowed" and "returned."
// Borrowed nodes can be modified, in which case the NodeSpace must be
// informed of the modification via the NodeModified method.
//
// The BTree class assumes that the NodeSpace class provides the
// following primitives:
// \par\begin{tabular}{lp{3.5truein}}
//    RootHandle, & which returns the handle of the root of the
//                  tree \\
//    Root, &       which returns a raw C++ pointer to the root \\
//    NewRoot, &    which modifies the root handle\\
//    CreateNode, & which creates a new node in the NodeSpace and
//                  returns a handle to it. \\
//    BorrowNode, & which returns a raw C++ pointer to the node
//                  specified by the given handle. This pointer
//                  points to space owned by NodeSpace. \\
//    ReturnNode, & which informs the NodeSpace that the caller
//                  done with the node \\
//    NodeModified,&which tells the NodeSpace that the caller
//                  has modified the specified node (but is not
//                  necessarily done with that node yet) \\
//    DestroyNode,& which destroys the given node, and invalidates
//                  its handle. \\
// \end{tabular}\par
// The default implementation is for the node space on the heap, so that
// translation between node handles and pointers is trivial.


class CL_BTreeNodeSpace  {

public:
    CL_BTreeNodeSpace (short order, const CL_GenericBTree& tree,
                       CL_AbstractComparator& cmp);
    // Construct a new NodeSpace.

    virtual ~CL_BTreeNodeSpace () {};
    
    // --------------- Essential (core) functions ----------------
    
    virtual CL_BTreeNodeHandle   RootHandle () const = 0;

    virtual void                 NewRoot (CL_BTreeNodeHandle h) = 0;
    
    virtual CL_BTreeNodeHandle   CreateNode () = 0;

    virtual void                 DestroyNode (CL_GenericBTreeNode*) = 0;

    virtual CL_GenericBTreeNode* BorrowNode (CL_BTreeNodeHandle h) const = 0;
    //  Return a null pointer if the handle h is zero.
    
    virtual void                 ReturnNode (CL_GenericBTreeNode* ) const = 0;
    
    virtual void                 NodeModified (CL_GenericBTreeNode*) = 0;
    

    // -------------------- Convenience functions --------------------
    
    CL_GenericBTreeNode* BorrowRoot () const
        {return BorrowNode (RootHandle());};
    // Convenience function to borrow the root of the tree.

    void WriteBack (CL_GenericBTreeNode* node)
        { NodeModified (node); ReturnNode (node);};
    // Convenience function that calls {\small\tt NodeModified} and then
    // {\small\tt ReturnNode}.

    CL_GenericBTreeNode*  MakeNode () 
        { return BorrowNode (CreateNode());};
    // Convenience function that creates a new node and then borrows it.

    virtual void DestroyItem (CL_VoidPtr) const {};
    // Destroy the item pointed to by the parameter. This method is called
    // for each item of a node when the node is being destroyed. The default
    // implementation does nothing; derived classes may override this method.

    // --------------------- End public protocol -----------------------

protected:
    short _order;
    CL_AbstractComparator& _cmp;    // Declare _cmp before _tree, to
                                    // initialize in correct order
    const CL_GenericBTree& _tree;

    virtual CL_GenericBTreeNode* _BuildNode () const
        {return new CL_GenericBTreeNode (_order, (CL_BTreeNodeSpace*)
                                         this, _cmp);};
    // Create an empty node and return it. This method is
    // meant for use by derived classes, since the BTreeNode constructor is
    // protected.
    
    virtual void _Destroy (CL_GenericBTreeNode* node) const;
    // Call {\tt delete} on the parameter. The default implementation
    // invokes DestroyItem on each contained item, and then destroys the
    // node. This method is
    // meant for use by derived classes, since the BTreeNode destructor is
    // protected.

    virtual void _SetHandle (CL_GenericBTreeNode& node,
                             CL_BTreeNodeHandle h) const
        {node._handle = h;};
    // Set the handle of {\tt node} to {\tt h}. This method is
    // meant for use by derived classes.
    

};



inline CL_BTreeNodeSpace::CL_BTreeNodeSpace
    (short order, const CL_GenericBTree& tree, CL_AbstractComparator& cmp)
: _order (order), _cmp (cmp), _tree (tree)
{
};


 

class CL_GenericBTreeIterator {

public:
    CL_GenericBTreeIterator (const CL_GenericBTree& t);
    // Constructor: create a BTreeIterator for the given tree t.

    CL_GenericBTreeIterator (const CL_GenericBTreeIterator& itr);
    // Copy constructor. The copy inspects the same B-tree as {\tt itr}, and
    // (unless reset) begins  its iteration at the item at which {\tt itr}
    // is currently positioned.
    
    virtual ~CL_GenericBTreeIterator();
    // Destructor.
    
    void Reset();
    // Reset the iterator to the leftmost (smallest) item.
    
    void BeginFrom (CL_VoidPtr p);
    // Begin the iteration from the given item (or the one immediately
    // larger, if the given item isn't in the tree).
    
    bool More();
    // Tell whether there are more items in the iteration.
    
    CL_VoidPtr Next();
    // Return the next item in the iteration sequence. Return the NULL
    // pointer if no more items.

    long CurrentRank () const {return _index;};
    // Return the rank of the element that was returned by the most recent
    // call to Next().
    
    // --------------------- End public protocol -----------------------

 
protected:
    CL_VoidPtr                  _path;      // Stack containing path to
                                            // current element
    short                       _length;    // Length of stack
    long                        _index;     // Rank of  element most recently
                                            // returned by Next
    const      CL_GenericBTree& _tree;      // The tree being inspected
    
};










class CL_HeapBTreeNodeSpace: public CL_BTreeNodeSpace {

public:
    CL_HeapBTreeNodeSpace (short order, const CL_GenericBTree& tree,
                           CL_AbstractComparator& cmp);

    ~CL_HeapBTreeNodeSpace ();
    
    virtual CL_BTreeNodeHandle RootHandle () const { return _root; };

    virtual void NewRoot (CL_BTreeNodeHandle h) { _root = h; };
    
    virtual CL_BTreeNodeHandle CreateNode ();

    virtual CL_GenericBTreeNode* BorrowNode (CL_BTreeNodeHandle h) const;
    
    virtual void ReturnNode (CL_GenericBTreeNode* ) const;
    
    virtual void NodeModified (CL_GenericBTreeNode*) {};
    
    virtual void DestroyNode (CL_GenericBTreeNode* node);

    // --------------------- End public protocol -----------------------

protected:
    CL_BTreeNodeHandle _root;

    void               _DestroySubtree (CL_BTreeNodeHandle h);
    
};




#endif


