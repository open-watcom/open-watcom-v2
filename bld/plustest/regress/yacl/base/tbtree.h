

#ifndef _tbtree_h_ /* Sun May 15 10:56:04 1994 */
#define _tbtree_h_

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



// This is a template-based, type-safe version of the Generic B-tree.
// Methods for this class are provided in the header file, so that
// type-safe instantiations of the {\small\tt CL_BTree} can be constructed
// with arbitrary base types. The lengths of these methods, however, are
// very small, so that no significant overhead is incurred.
//
// The ItemType class,  which  is the type  parameter for the BTree and
// the BTreeIterator, must support the  assignment operator and the comparison
// operators.  An ItemType may be  a composite object,  such as a key-value
// pair; it  is then the responsibility of  the ItemType  class to use only
// the key part for comparison, but both key and value for copying.




#include "base/gbtree.h"

template <class ItemType> class CL_BTreeIterator;

template <class ItemType>
class CL_BTree: public CL_Object {

public:
    CL_BTree (short order = 2, CL_BTreeNodeSpace* space = NULL);
    // Create a B-tree with given order. This must be at least 2; anything
    // less than 2 is taken to be 2. A NodeSpace may be specified in the
    // second parameter; if it is specified, the tree does not take
    // responsibility for it, and the caller must guarantee that it exists
    // for the lifetime of the tree and is destroyed afterwards. If no
    // NodeSpace is specified, a default in-memory NodeSpace is used.

    CL_BTree (CL_AbstractComparator& cmp, short order = 2,
              CL_BTreeNodeSpace* space = NULL);
    // Alternate constructor: use the given comparator. The comparator and
    // NodeSpace are assumed to be "borrowed" from the user of this object.
    
    ~CL_BTree ();
    // Destructor.
    
    //
    // ----------------------- Search and related methods ------------------

    virtual const ItemType& Find (const ItemType& item) const;
    // Search the tree for the given item. Return the
    // found item if the search was successful, as the function value. If
    // the search fails, the return value is the null value of the ItemType.


    const ItemType& Smallest () const {return ItemWithRank (0);};
    // Find and return the minimum item. If the tree is empty,
    // the null value is returned.

    const ItemType& Largest () const {return ItemWithRank (Size()-1);};
    // Find and return the minimum item. If the tree is empty,
    // the null value is returned.

    virtual const ItemType& ItemWithRank (long i) const;
    // Given an index $i$ between 0 and Size()-1, return the element of rank
    // $i$, i.e., the element that has $i$ elements less than it in the tree.
    // If $i \le 0$, this returns the smallest element, and if $i \ge {\tt
    // Size()}$, this returns the largest element. If the tree is empty,
    // the null value of the base type is returned. The implementation
    // examines only the nodes on the path from the root to the one
    // containing the key sought, and therefore takes no more than $\log
    // n$ time steps with $n$ keys in the tree.
    //
    // Note that it is possible to iterate through the elements of the tree
    // via calls to this method, varying the index from 0 to Size()-1;
    // however, this is much less efficient than using the BTreeIterator.
    
    virtual long RankOf (const ItemType& p) const;
    // Return the number of elements in the tree that are less than the
    // parameter.
    
    virtual long Size () const;
    // Return the size of the tree (number of items currently present).
    // The implementation needs constant time regardless of tree size.


    // ------------------------ Modification ------------------------------

    virtual bool Add  (const ItemType& item);
    // Add the item to the tree. Return TRUE if successfully added,
    // FALSE if the item was already in the tree.
 
    virtual ItemType Remove (const ItemType& item);
    // Remove the specified item from the tree. Return the removed item if
    // it was found in the tree, and the null value otherwise. The
    // implementation needs (in the worst case) two passes over the path
    // to the key, and so takes $2\log n$ time steps with $n$ keys in the
    // tree. It also coalesces any non-full nodes along the path from the
    // root to the deleted key.

    virtual ItemType ExtractMin ();
    // Remove and return the smallest item in the tree. Return the null
    // pointer if the tree is empty.


    // ---------------------- Basic methods ----------------------------

    void IntoStream (ostream& strm) const;
    // Override the method inherited from {\tt CL_Object}.
    // Dump the whole tree in indented form on {\tt strm}. This is for
    // debugging purposes only, and uses the {\tt AsString} method on the
    // {\tt ItemType}.

    virtual const char* ClassName () const { return "CL_BTree";};

    // --------------------- End public protocol -----------------------


protected:
    CL_Comparator<ItemType> _comparator;
    CL_GenericBTree         _tree;
    CL_HeapBTreeNodeSpace*  _space;
    
    void  PrintTree (CL_BTreeNodeHandle h, short level, ostream&) const;

    friend       CL_BTreeIterator<ItemType>;
};


// This is a template-based version of the iterator over a B-tree whose
// template base is {\small\tt ItemType}.

template <class ItemType>
class CL_BTreeIterator: public CL_Object {

public:
    CL_BTreeIterator (const CL_BTree<ItemType>& t);
    // Constructor: create a BTreeIterator for the given tree t.

    CL_BTreeIterator (const CL_BTreeIterator<ItemType>& itr);
    // Copy constructor. The copy inspects the same B-tree as {\tt itr}, and
    // (unless reset) begins  its iteration at the item at which {\tt itr}
    // is currently positioned.
    
    void Reset();
    // Reset the iterator to the leftmost (smallest) item.
    
    void BeginFrom (const ItemType&);
    // Begin the iteration from the given item (or the one immediately
    // larger, if the given item isn't in the tree).
    
    bool More();
    // Tell whether there are more items in the iteration.
    
    ItemType Next();
    // Return the next item in the iteration sequence.
    

    // ---------------------- Basic methods ----------------------------

    const char* ClassName () const { return "CL_BTreeIterator";};

    // --------------------- End public protocol -----------------------

 
protected:
    CL_GenericBTreeIterator    _iter;
};






#endif /* _tbtree_h_ */
