



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




#ifndef _tbtreimp_cxx_ /* Wed May 18 15:39:57 1994 */
#define _tbtreimp_cxx_

#include "base/basicops.h"
#include "base/tbtree.h"
#ifdef DEBUG
#include "base/memory.h"
#endif


#include <iostream.h>


template <class ItemType>
class CL_TemplateNodeSpace: public CL_HeapBTreeNodeSpace {

    // The only purpose of this template class is to override the
    // DestroyItem method with the appropriate destruction code.
public:
    CL_TemplateNodeSpace (short order, const CL_GenericBTree& tree,
                          CL_AbstractComparator& cmp)
        : CL_HeapBTreeNodeSpace (order, tree, cmp) {};
    ~CL_TemplateNodeSpace ();
    void DestroyItem (CL_VoidPtr item) const
        {CL_Basics<ItemType>::Destroy (item);};
};


template <class ItemType>
CL_TemplateNodeSpace<ItemType>::~CL_TemplateNodeSpace ()
{
    _DestroySubtree (_root);
    _root = NULL; // This gets rid of the items also, because this class'
                  // version of the virtual DestroyItem is called.
}


template <class ItemType>
CL_BTree<ItemType>::CL_BTree (short order,  CL_BTreeNodeSpace* space)
: _tree (_comparator, order,
         space ? space : (_space = new CL_TemplateNodeSpace<ItemType>
                          (order, _tree, _comparator))
         )
{
    if (space)
        _space = NULL; // We don't own the node space
}


template <class ItemType>
CL_BTree<ItemType>::CL_BTree (CL_AbstractComparator& cmp,
                              short order,  CL_BTreeNodeSpace* space)
: _tree (cmp, order, space)
{
}


template <class ItemType>
CL_BTree<ItemType>::~CL_BTree ()
{
    if (_space)
        delete _space; // We own it if it's non-null
}


//
// ----------------------- Search and related methods ------------------


template <class ItemType>
const ItemType& CL_BTree<ItemType>::Find (const ItemType& item) const
{
    CL_VoidPtr p =  CL_Basics<ItemType>::MakePointer (item);
    CL_VoidPtr q = _tree.Find (p);
    if (q)
        return CL_Basics<ItemType>::Deref (q);
    static ItemType null = CL_Basics<ItemType>::NullValue();
    return null;
}




template <class ItemType>
const ItemType& CL_BTree<ItemType>::ItemWithRank (long i) const
{
    CL_VoidPtr p = _tree.ItemWithRank (i);
    if (p)
        return CL_Basics<ItemType>::Deref (p);
    static ItemType null = CL_Basics<ItemType>::NullValue();
    return null;
}



template <class ItemType>
long CL_BTree<ItemType>::RankOf (const ItemType& item) const
{
    CL_VoidPtr p = CL_Basics<ItemType>::MakePointer (item);
    return _tree.RankOf (p);
}


template <class ItemType>
long CL_BTree<ItemType>::Size () const
{
    return _tree.Size();
}


template <class ItemType>
bool CL_BTree<ItemType>::Add  (const ItemType& item)
{
    if (!PrepareToChange())
        return FALSE;
    CL_VoidPtr p = CL_Basics<ItemType>::MakeCopy (item);
    if (_tree.Add (p)) {
        Notify();
        return TRUE;
    }
    CL_Basics<ItemType>::Destroy (p);
    return FALSE;
}



template <class ItemType>
ItemType CL_BTree<ItemType>::Remove (const ItemType& item)
{
    if (!PrepareToChange())
        return CL_Basics<ItemType>::NullValue();
    CL_VoidPtr p = CL_Basics<ItemType>::MakePointer (item);
    CL_VoidPtr q = _tree.Remove (p);
    if (q) {
        ItemType r = CL_Basics<ItemType>::Deref(q);
        CL_Basics<ItemType>::Destroy (q);
        Notify ();
        return r;
    }
    return CL_Basics<ItemType>::NullValue();
}


template <class ItemType>
ItemType CL_BTree<ItemType>::ExtractMin ()
{
    if (!PrepareToChange())
        return CL_Basics<ItemType>::NullValue ();
    CL_VoidPtr p = _tree.ExtractMin ();
    if (p) {
        Notify();
        ItemType t = CL_Basics<ItemType>::Deref (p);
        CL_Basics<ItemType>::Destroy (p);
        return t;
    }
    return CL_Basics<ItemType>::NullValue ();
}




template <class ItemType>
void CL_BTree<ItemType>::IntoStream (ostream& strm) const
{
    CL_BTreeNodeSpace* space = _tree.NodeSpace ();
    if (space) {
        CL_BTreeNodeHandle h = space->RootHandle();
        PrintTree (h, 0, strm);
    }
}




template <class ItemType>
void CL_BTree<ItemType>::PrintTree (CL_BTreeNodeHandle h, short level,
                                    ostream& strm) const
{
    if (h == 0)
        return;
    CL_BTreeNodeSpace* space = _tree.NodeSpace ();
    CL_GenericBTreeNode* z = space->BorrowNode (h);

    for (short i = 0; i < z->Size(); i++) {
        PrintTree (z->Subtree(i), level+1, strm);
        for (short j = 0; j < 4*level; j++)
            strm << ' ';
        CL_VoidPtr q = z->Item(i);
        ItemType p = CL_Basics<ItemType>::Deref (q);
        CL_String st = CL_Basics<ItemType>::PrintableForm (p);
        strm << "------>" << (const char*) st << endl << flush;
    }
    PrintTree (z->Subtree(z->Size()), level+1, strm);
    space->ReturnNode (z);
}






// ---------------------------------------------------------------------
//                    CL_BTreeIterator methods
// ---------------------------------------------------------------------




template <class ItemType>
CL_BTreeIterator<ItemType>::CL_BTreeIterator(const CL_BTree<ItemType>& t)
: _iter (t._tree)
{
}

template <class ItemType>
CL_BTreeIterator<ItemType>::CL_BTreeIterator
    (const CL_BTreeIterator<ItemType>& itr) 
: _iter (itr._iter)
{
}


template <class ItemType>
void CL_BTreeIterator<ItemType>::Reset()
{
    _iter.Reset();
}



template <class ItemType>
void CL_BTreeIterator<ItemType>::BeginFrom (const ItemType& t)
{
    const CL_VoidPtr p = CL_Basics<ItemType>::MakePointer (t);
    _iter.BeginFrom (p);
}



template <class ItemType>
bool CL_BTreeIterator<ItemType>::More()
{
    return _iter.More ();
}


template <class ItemType>
ItemType CL_BTreeIterator<ItemType>::Next()
{
    CL_VoidPtr q = _iter.Next();
    ItemType p = CL_Basics<ItemType>::Deref (q);
    return p;
}



#endif /* _tbtreimp_cxx_ */
