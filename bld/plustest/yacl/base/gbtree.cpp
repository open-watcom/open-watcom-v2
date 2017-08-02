



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





#ifdef __GNUC__
#pragma implementation
#endif

#include "base/gbtree.h"
#ifdef DEBUG
#include "base/memory.h"
#endif

#define MAX_BTREE_HEIGHT 30 // Can't have trees higher than this



////////////////////////////////////////////////////////////////////////////
//         CL_GenericBTreeNode function definitions                       //
////////////////////////////////////////////////////////////////////////////



CL_GenericBTreeNode::CL_GenericBTreeNode
    (short order, CL_BTreeNodeSpace* space, CL_AbstractComparator& cmp)
:  _order (order), _cmp (cmp), _nodeSpace (space)
{
    _keyCount = 0;
    register short n = 2*order;
    _subtree = new CL_BTreeNodeHandle [n];
    for (short i = 0; i < n; i++)
        _subtree[i] = 0;
    _item = new CL_VoidPtr [n-1];
    _isLeaf = TRUE;
    _subtreeSize = 0;
    
}
 

CL_GenericBTreeNode::~CL_GenericBTreeNode()
{
    delete [] _item;
    delete [] _subtree ;
}
 
 
bool CL_GenericBTreeNode::Search (CL_VoidPtr itm, short& index) const
{
    if (!_item)
        return FALSE;
    long i;
    short result;
    if (_keyCount <= 7) { // Small number of keys, do a linear search
        if (_keyCount == 0) {
            index = -1;
            return FALSE;
        }
        for (i = 0; i < _keyCount; i++) {
            result = _cmp (_item[i], itm);
            if (result >= 0)
                break;
        }
        if (result == 0) {
            index = i;
            return TRUE;
        }
        else  {
            index = i-1;
            return FALSE;
        }
    }

    // Do a binary search
    long lo = 0, hi = _keyCount-1, mid;
    while (lo <= hi) {
        mid = (lo + hi)/2;
        result = _cmp (_item[mid], itm);
        if (result == 0) {
            index = mid;
            return TRUE;
        }
        if (result < 0)
            lo = mid+1;
        else
            hi = mid-1;
    }
    index = (result <= 0) ? (mid) :  mid-1;
    return FALSE;
}

void CL_GenericBTreeNode::ShiftRightAt (short pos, short amount)
{
    short i;
    for (i = _keyCount-1; i >= pos; i--) {
        _item[i+amount] = _item[i];
        _subtree[i+amount+1] = _subtree[i+1];
    }
    _subtree [pos+amount] = _subtree[pos];
    for (i = pos; i < pos+amount; i++) {
        _item[i] = 0;
    }
}


void CL_GenericBTreeNode::ShiftLeftAt (short pos, short amount)
{
    short i;
    for (i = pos; i < _keyCount; i++) {
        _item[i-amount] = _item[i];
        _subtree[i-amount] = _subtree[i];
    }
    // Now move the rightmost subtree
    _subtree [_keyCount-amount] = _subtree[_keyCount];
    for (i = _keyCount-amount+1; i <= _keyCount; i++)
        _subtree[i] = 0;
    for (i = _keyCount-amount; i < _keyCount; i++)
        _item[i] = 0;
    _keyCount -= amount;
}



void CL_GenericBTreeNode::MoveSubNode
    (const CL_GenericBTreeNode& x, short pos, short our_pos,
     short nkeys)
{
    short i, j;
    for (i = our_pos, j = pos; i < our_pos + nkeys; i++, j++) {
        _item[i] = x._item[j];
        _subtree[i] = x._subtree[j];
        x._item[j] = 0;
        x._subtree[j] = 0;
    }
    _subtree[our_pos+nkeys] = x._subtree[pos + nkeys];
}


 
////////////////////////////////////////////////////////////////////////////
//                  CL_BTreeNodeSpace methods                             //
////////////////////////////////////////////////////////////////////////////




 
void CL_BTreeNodeSpace::_Destroy (CL_GenericBTreeNode* node) const
{
    if (node) {
        register short n = node->Size();
        for (short i = 0; i < n; i++)
            DestroyItem (node->Item(i));
        delete node;
    }
}
    






////////////////////////////////////////////////////////////////////////////
//              CL_HeapBTreeNodeSpace methods                             //
////////////////////////////////////////////////////////////////////////////



CL_HeapBTreeNodeSpace::CL_HeapBTreeNodeSpace (short order, const
                                              CL_GenericBTree& tree,
                                              CL_AbstractComparator& cmp)
: CL_BTreeNodeSpace (order, tree, cmp)
{
    _root = CreateNode();
}



CL_HeapBTreeNodeSpace::~CL_HeapBTreeNodeSpace ()
{
    // Traverse the tree and get rid of all the nodes
    _DestroySubtree (_root);
    _root = NULL;
}



void CL_HeapBTreeNodeSpace::_DestroySubtree (CL_BTreeNodeHandle h)
{
    // Do a post-order walk, destroying nodes as we go along
    if (!h)
        return;
    CL_GenericBTreeNode* node = (CL_GenericBTreeNode*) h;
    register short n = node->Size();
    for (register short i = 0; i <= n; i++)
        _DestroySubtree (node->Subtree(i));
    _Destroy (node);
}


CL_BTreeNodeHandle CL_HeapBTreeNodeSpace::CreateNode ()
{
    CL_GenericBTreeNode* p = _BuildNode ();
    if (p)
        _SetHandle (*p, (CL_BTreeNodeHandle) p);
    return (CL_BTreeNodeHandle) p;
}


// static long time = 0; // DEBUG
// static FILE* theFile = fopen ("g:/logfile", "w"); // DEBUG

CL_GenericBTreeNode*  CL_HeapBTreeNodeSpace::BorrowNode
(CL_BTreeNodeHandle h) const
{
    // if (h) fprintf (theFile, "%8lx %08ld borrowed\n", h, time++); // DEBUG
    return (CL_GenericBTreeNode*) h;
}


void CL_HeapBTreeNodeSpace::ReturnNode (CL_GenericBTreeNode* /* h */) const
{
    // fprintf (theFile, "%8lx %08ld returned\n",  h, time++); // DEBUG
}


void CL_HeapBTreeNodeSpace::DestroyNode (CL_GenericBTreeNode* node)
{
    // fprintf (theFile, "%8lx %08ld destroyed\n", node, time++); // DEBUG
    _Destroy (node);
}




 
////////////////////////////////////////////////////////////////////////////
//                     CL_GenericBTree       definitions                  //
////////////////////////////////////////////////////////////////////////////



CL_GenericBTree::CL_GenericBTree (CL_AbstractComparator& cmp,
                                  short order, CL_BTreeNodeSpace* space)
: _comparator (cmp)
{
    _order = maxl (order, 2);
    if (space) {
        _nodeSpace = space;
        _ownNodeSpace = FALSE;
    }
    else {
        _nodeSpace = new CL_HeapBTreeNodeSpace (_order, *this, cmp);
        _ownNodeSpace = TRUE;
    }
}


CL_GenericBTree::~CL_GenericBTree ()
{
    if (_ownNodeSpace)
        delete _nodeSpace;
}


CL_VoidPtr CL_GenericBTree::Find (CL_VoidPtr item) const
{
    short pos;
    bool found;
    CL_VoidPtr ret_val = NULL;

    CL_BTreeNodeHandle tmp_handle = _nodeSpace->RootHandle ();
    CL_GenericBTreeNode* tmp_ptr;
    do {
        tmp_ptr = _nodeSpace->BorrowNode (tmp_handle);
        found = tmp_ptr->Search (item, pos);
    if (found || tmp_ptr->_isLeaf) break;
        tmp_handle = tmp_ptr->_subtree [pos+1];
        _nodeSpace->ReturnNode (tmp_ptr);
    } while (1);
    if (found)
        ret_val = tmp_ptr->_item [pos];
    _nodeSpace->ReturnNode (tmp_ptr);
    return ret_val;
}




CL_VoidPtr CL_GenericBTree::ItemWithRank (long rank) const
{
    short pos;
    bool found;
    CL_VoidPtr itm;

    CL_GenericBTreeNode* tmp_ptr, *p1;
    tmp_ptr = _nodeSpace->BorrowRoot ();
    if (!tmp_ptr || tmp_ptr->_keyCount <= 0)
        return NULL;
    rank = minl (maxl (rank, 0), tmp_ptr->_subtreeSize-1);
    do {
        if (tmp_ptr->_isLeaf) {
            assert ((0 <= rank && rank <= tmp_ptr->_keyCount-1),
                    ("Internal error: CL_GenericBTree::ItemWithRank:"
                     "bad key count %d rank %ld", tmp_ptr->_keyCount, rank));
            CL_VoidPtr ret = tmp_ptr->_item[rank];
            _nodeSpace->ReturnNode (tmp_ptr);
            return ret;
        }
        // We're in a non-leaf, so find the subtree to descend into
        // (if any)
        short i;
        for (i = 0; i < tmp_ptr->_keyCount; i++) {
            p1 = _nodeSpace->BorrowNode (tmp_ptr->_subtree[i]);
            if (p1->_subtreeSize > rank)
                break;
            rank -= p1->_subtreeSize; // Account for i-th subtree
            _nodeSpace->ReturnNode (p1);
            if (rank == 0) {
                // We've got the item we wanted
                CL_VoidPtr ret = tmp_ptr->_item[i];
                _nodeSpace->ReturnNode (tmp_ptr);
                return ret;
            }
            rank--;               // Account for i-th key in node
        }
        if (i >= tmp_ptr->_keyCount) {
            // Descend into rightmost subtree
            p1 = _nodeSpace->BorrowNode (tmp_ptr->_subtree[i]);
        }
        _nodeSpace->ReturnNode (tmp_ptr);
        tmp_ptr = p1;
    } while (1);
}


long CL_GenericBTree::RankOf (CL_VoidPtr item) const
{
    short pos;
    bool found;
    CL_VoidPtr itm;
    long count = 0;

    CL_GenericBTreeNode* tmp_ptr, *p1;
    tmp_ptr = _nodeSpace->BorrowRoot ();
    if (!tmp_ptr || tmp_ptr->_keyCount <= 0)
        return 0;
    do {
        found = tmp_ptr->Search (item, pos);
        if (tmp_ptr->_isLeaf) {
            _nodeSpace->ReturnNode (tmp_ptr);
            count += found ? pos : pos+1;
            return count;
        }
        // We're in a non-leaf, so find the subtree to descend into
        short i;
        for (i = 0; i <= pos; i++) {
            p1 = _nodeSpace->BorrowNode (tmp_ptr->_subtree[i]);
            count += p1->_subtreeSize; // Account for i-th subtree
            _nodeSpace->ReturnNode (p1);
        }
        if (found)  {
            _nodeSpace->ReturnNode (tmp_ptr);
            return count + pos;
        }
        count += pos+1; // Account for the keys we compared
        p1 = _nodeSpace->BorrowNode (tmp_ptr->_subtree[i]);
        _nodeSpace->ReturnNode (tmp_ptr);
        tmp_ptr = p1;
    } while (1);
}



long CL_GenericBTree::Size() const
{
    CL_GenericBTreeNode* node = _nodeSpace->BorrowRoot ();
    long size = node->_subtreeSize;
    _nodeSpace->ReturnNode (node);
    return size;
}


   


bool CL_GenericBTree::Add (CL_VoidPtr item)
{
    bool        ans;
    CL_GenericBTreeNode* aNode, *tmpNode, *root;

    root = _nodeSpace->BorrowRoot ();
    if (root->_keyCount < (2*_order - 1)) {
        ans = _InsertNonFull (root, item);
        return ans;
    }

    // Root is full; create a new empty root
    aNode = _nodeSpace->MakeNode(); // aNode  will be the new root 
    CL_BTreeNodeHandle h = aNode->Handle();
    aNode->_subtree [0] = root->Handle();
    aNode->_isLeaf = FALSE;
    aNode->_subtreeSize = root->_subtreeSize;
    _SplitChild (aNode, 0, root);
    _nodeSpace->ReturnNode (root);

    _nodeSpace->NewRoot (h);

    // Now add the key 
    ans = _InsertNonFull (aNode, item);
    return ans;
}








 
 
//
// Private methods
//
 
bool CL_GenericBTree::_InsertNonFull (CL_GenericBTreeNode* x, CL_VoidPtr item)
{
    short pos;
    CL_GenericBTreeNode* y, *z = x;
    CL_GenericBTreeNode* stack[MAX_BTREE_HEIGHT];
    // We need a stack for updating the subtree sizes
    short sp = 0;
    bool found = FALSE;
    
    while (z && !(found = z->Search (item, pos))) {
        stack[sp++] = z;
    if (z->_isLeaf) break;
        pos++;
        y =  _nodeSpace->BorrowNode (z->_subtree[pos]);
        if (y->_keyCount == 2*_order-1) {
            _SplitChild (z, pos, y);
            if (_comparator (item, z->_item[pos]) >= 0) {
                pos++;
                _nodeSpace->ReturnNode (y);
                y = _nodeSpace->BorrowNode (z->_subtree[pos]);
            }
        }
        z = y;
    }

    if (!found) {
        short n = z->_keyCount;
        if (n > 0) {
            z->ShiftRightAt (pos+1);
            z->_item[pos+1] = item;
        }
        else 
            z->_item[0] = item;
        z->_keyCount++;
        for (short i = 0; i < sp; i++) {
            stack[i]->_subtreeSize++;
        }
    }
    for (short i = 0; i < sp; i++) {
        _nodeSpace->WriteBack (stack[i]);
    }
    return !found;
}  


void CL_GenericBTree::_SplitChild (CL_GenericBTreeNode* x,
                                   short i, CL_GenericBTreeNode* y) 
{
    CL_GenericBTreeNode* z = _nodeSpace->MakeNode();
    z->MoveSubNode (*y, _order, 0, _order-1);
    z->_isLeaf = y->_isLeaf;
    z->_keyCount = y->_keyCount = _order-1;
    x->ShiftRightAt (i); 
        // We shouldn't shift subtree[i], but it shouldn't matter
    x->_subtree[i+1] = z->Handle();
    x->_item [i] = y->_item [_order-1];
    x->_keyCount++;

    // Recompute _subtreeSize for y and z
    long size = 0;
    if (!z->_isLeaf) {
        for (short j = 0; j <= z->_keyCount; j++) {
            CL_GenericBTreeNode* p = _nodeSpace->BorrowNode
                (z->_subtree[j]);
            size += p->_subtreeSize;
            _nodeSpace->ReturnNode (p);
        }
    }
    size += z->_keyCount;
    z->_subtreeSize = size;
    y->_subtreeSize -= size+1;
    _nodeSpace->WriteBack (z);
    _nodeSpace->NodeModified (y);
    _nodeSpace->NodeModified (x);
}
 







 






CL_VoidPtr CL_GenericBTree::Remove (CL_VoidPtr key)
{
    CL_GenericBTreeNode* root = _nodeSpace->BorrowRoot();
    CL_GenericBTreeNode* node = root;
    CL_VoidPtr retVal;
    
    if (!node || node->_keyCount == 0) // Empty root
        return (CL_VoidPtr) NULL;
    short pos;
    if (node->_keyCount == 1 && node->_isLeaf) {
        // Tree has only one key
        if (_comparator (key, node->_item[0]) == 0) {
            node->_keyCount = node->_subtreeSize = 0;
            _nodeSpace->WriteBack (node);
            return node->_item[0];
        }
        return NULL;
    }
    CL_GenericBTreeNode* stack[MAX_BTREE_HEIGHT];
    // We need a stack for updating the subtree sizes
    short sp = 0;
    short index = 0;
    bool found = FALSE;

    CL_GenericBTreeNode* q;
    // stack[sp++] = node;
    enum {SEARCHING, DESCENDING} state = SEARCHING;
    DeleteActionEnum action;
    while (1) {
        if (state == SEARCHING) {
            found = node->Search (key, index);
            if (found)
                retVal = node->_item[index];
        }
        q = _DescendInto (node, index+1, action);
        if (node == root &&  node->_keyCount == 0) {
            _nodeSpace->DestroyNode (node);
        }
        else {
            // We should add the root to the stack only if it wasn't
            // already destroyed
            stack [sp++] = node;
        }            
    if (!q) break;
        // _DescendInto may have caused our key to be copied into q.
        // If so, it would be copied into either q->_item[0] or
        // q->_item[_order-1]  (because of a right or left rotation,
        // respectively) or into q->_item[_order-1] (because of a merge).
        if (found) {
            state = DESCENDING;
            if (action == RotateRight) {
                index = 0;
            }
            else if (action == RotateLeft || action == Merge) {
                index = _order-1;
            }
            else // No rotation or merge was done
                break;
        }
        node = q;
    }
    if (!found) {
        // The key is not in the tree
        for (short i = 0; i < sp; i++)
            _nodeSpace->WriteBack(stack[i]);
        return (CL_VoidPtr) NULL;
    }
    if (node->_isLeaf) {
        // Key found in leaf
        node->ShiftLeftAt (index+1);
    }
    else {
        // The key is in an internal node, so we'll replace it by its
        // inorder successor:
        CL_GenericBTreeNode* p = q;
        while (1) {
            stack[sp++] = p;
        if (p->_isLeaf) break;
            p = _DescendInto (p, 0, action);
        }
        node->_item[index] = p->_item[0];
        p->ShiftLeftAt(1);
    }

    // Now update subtree sizes along search path
    short i = 0;
    if (stack[0]->_keyCount == 0) {
        i = 1;
    }
    for (; i < sp; i++) {
        stack[i]->_subtreeSize--;
        _nodeSpace->WriteBack (stack[i]);
    }
    return retVal;
}


CL_GenericBTreeNode* CL_GenericBTree::_DescendInto
    (CL_GenericBTreeNode* node, short subtreeIndex, DeleteActionEnum& action)
{
    CL_GenericBTreeNode* child, *sibling, *p;
    child = _nodeSpace->BorrowNode (node->_subtree[subtreeIndex]);
    if (!child || child->_keyCount >= _order) {
        action = NoAction;
        return child;
    }
    if (subtreeIndex == 0) {
        sibling = _nodeSpace->BorrowNode (node->_subtree[1]);
        p = _Adjust (node, 0, child, sibling, action);
    }
    else {
        sibling = _nodeSpace->BorrowNode
            (node->_subtree[subtreeIndex-1]);
        p = _Adjust (node, subtreeIndex-1, sibling, child, action);
    }
    if (action != Merge)
        _nodeSpace->ReturnNode (sibling);
    return p;
}



CL_GenericBTreeNode* CL_GenericBTree::_Adjust
  (CL_GenericBTreeNode* node, short index,
   CL_GenericBTreeNode* c0, CL_GenericBTreeNode* c1, DeleteActionEnum& action)
{
    assert ((c0 != NULL && c1 != NULL),
            ("BTree::Adjust: assertion failed: line %d\n", __LINE__));
    assert ((c0->_keyCount == _order-1 || c1->_keyCount == _order-1),
            ("BTree::Adjust: assertion failed: line %d\n", __LINE__));
            
    if (c0->_keyCount == _order-1 && c1->_keyCount == _order-1) {
        // Merge the two nodes
        c0->_item[_order-1] = node->_item[index];
        c0->MoveSubNode (*c1, 0, _order, _order-1);
        c0->_keyCount = 2*_order-1;
        c0->_subtreeSize += c1->_subtreeSize+1;
        
        _nodeSpace->DestroyNode (c1);
        if (node->_keyCount > 1) {
            node->ShiftLeftAt (index+1);
            node->_subtree[index] = c0->Handle();
        }
        else {
            _nodeSpace->NewRoot (c0->Handle());
        }
        action = Merge;
        return c0;
    }
    if (c0->_keyCount >= _order) {
        // Rotate right
        c1->ShiftRightAt (0);
        c1->_item[0] = node->_item[index];
        c1->_subtree[0] = c0->_subtree[c0->_keyCount];
        node->_item[index] = c0->_item[c0->_keyCount-1];
        c0->_keyCount--;
        c1->_keyCount++;
        CL_GenericBTreeNode* p = _nodeSpace->BorrowNode
            (c1->_subtree[0]);
        long xfr = (p) ? p->_subtreeSize+1 : 1;
        c1->_subtreeSize += xfr;
        c0->_subtreeSize -= xfr;
        if (p)
            _nodeSpace->ReturnNode (p);
        _nodeSpace->NodeModified (c0);
        action = RotateRight;
        return c1;
    }
    else {
        // c1->keyCount >= order, so rotate left
        c0->_item[_order-1] = node->_item[index];
        c0->_subtree[_order] = c1->_subtree[0];
        c0->_keyCount++;
        node->_item[index] = c1->_item[0];
        CL_GenericBTreeNode* p = _nodeSpace->BorrowNode
            (c0->_subtree[_order]);
        long xfr = (p) ? p->_subtreeSize+1 : 1;
        c1->_subtreeSize -= xfr;
        c0->_subtreeSize += xfr;
        c1->ShiftLeftAt(1);
        if (p)
            _nodeSpace->ReturnNode (p);
        _nodeSpace->NodeModified (c1);
        action = RotateLeft; 
        return c0;
    }
}

            
        
    


CL_VoidPtr CL_GenericBTree::ExtractMin ()
{
    CL_GenericBTreeNode* stack[MAX_BTREE_HEIGHT];
    // We need a stack for updating the subtree sizes
    short sp = 0;
    CL_GenericBTreeNode* node = _nodeSpace->BorrowRoot();
    if (node->_keyCount == 0)
        return NULL;
    stack[sp++] = node;
    DeleteActionEnum action;
    while (!node->_isLeaf) {
        node = _DescendInto (node, 0, action);
        stack[sp++] = node;
    }
    CL_VoidPtr item = node->_item[0];
    node->ShiftLeftAt(1);
    for (short i = 0; i < sp; i++) {
        stack[i]->_subtreeSize--;
        _nodeSpace->WriteBack (stack[i]);
    }
    return item;
}




///////////////////////////////////////////////////////////////////
//                  BTreeIterator methods                        //
///////////////////////////////////////////////////////////////////


 


// The  BTreeIterator  remembers and   manipulates  the  search path  to  a
// particular key in the tree.
// 
// A search path is a sequence of pairs of the form <node#, subtree#>, with
// the  first pair <root,  subtree#> and the   last pair being  of the form
// <node#,  key#>. It completely   specifies the path   from the root  to a
// particular key in the tree.
//
// The Iterator maintains the invariant that the path specified by the
// current values in the array represents the path to the key that was
// returned by the most recent call to Next().

 
struct PathStruct {
public:
    CL_BTreeNodeHandle _handle;
    short              _indexInNode;
};


CL_GenericBTreeIterator::CL_GenericBTreeIterator
(const CL_GenericBTree& tree) :_tree (tree)
{
    _path = new PathStruct [MAX_BTREE_HEIGHT];
    _length = 0;
    Reset();
}

CL_GenericBTreeIterator::CL_GenericBTreeIterator
    (const CL_GenericBTreeIterator& itr)
: _tree (itr._tree)
{
    _path = new PathStruct [MAX_BTREE_HEIGHT];
    if (_path) {
        _length = itr._length;
        for (register short i = 0; i < _length; i++)
            ((PathStruct*) _path)[i] = ((PathStruct*) itr._path)[i];
    }
    else
        _length = 0;
    _index = itr._index;
}


CL_GenericBTreeIterator::~CL_GenericBTreeIterator()
{
    if (_path)
        delete [] (PathStruct*) _path;
}



void CL_GenericBTreeIterator::BeginFrom (CL_VoidPtr item)
{
    short pos;
    bool found;

    if (!_path) // Memory allocation failed?
        return;
    _length = 0;
    _index  = -1;
    if (_tree.Size() <= 0)
        return;
    PathStruct* path = (PathStruct*) _path;

    register CL_BTreeNodeSpace* space =  _tree.NodeSpace();
    CL_BTreeNodeHandle tmp_handle = space->RootHandle ();
    CL_GenericBTreeNode* tmp_ptr, *p;
    do {
        tmp_ptr = space->BorrowNode (tmp_handle);
        found = tmp_ptr->Search (item, pos);
        path[_length]._handle = tmp_handle;
        _index += path[_length]._indexInNode = found ? pos : pos+1;
        _length++;
    if (tmp_ptr->_isLeaf) break;
        for (register long i = 0; i <= pos; i++) {
            CL_GenericBTreeNode* p = space->BorrowNode
                (tmp_ptr->_subtree[i]);
            _index += p->_subtreeSize;
            space->ReturnNode (p);
        }
    if (found) break;
        tmp_handle = tmp_ptr->_subtree [pos+1];
        space->ReturnNode (tmp_ptr);
    } while (1);
    if (!tmp_ptr->_isLeaf) {
        // We're in an internal node; so move down to the leaf
        tmp_handle = tmp_ptr->_subtree[pos];
        do {
            p = space->BorrowNode (tmp_handle);
            path[_length]._handle = tmp_handle;
            path[_length]._indexInNode = p->_keyCount;
            _length++;
            tmp_handle = p->_subtree[p->_keyCount]; // Rightmost subtree
            space->ReturnNode (p);
        } while (tmp_handle);
    }
    path[_length-1]._indexInNode--;  // So that the first call to Next gives
                                     // the nearest key >= the given key
    space->ReturnNode (tmp_ptr);
}





void CL_GenericBTreeIterator::Reset ()
{
    if (!_path) // Memory allocation failed?
        return;
    _length = 1;
    PathStruct* path = (PathStruct*) _path;
    path[0]._handle = _tree.NodeSpace()->RootHandle();
    path[0]._indexInNode = -1;
    _index = -1;
}





CL_VoidPtr CL_GenericBTreeIterator::Next ()
{
    if (_index >= _tree.Size())
        return NULL;
    CL_VoidPtr retVal;
    
    if (!_path || _length == 0)
        return NULL;
    PathStruct* path = (PathStruct*) _path;
    CL_GenericBTreeNode* node;
    short  ndx = path[_length-1]._indexInNode;
    register CL_BTreeNodeSpace* space =  _tree.NodeSpace();
    node = space->BorrowNode (path[_length-1]._handle);

    _index++;
    if (! node->_isLeaf) {
        // Move to the next right subtree
        path[_length-1]._indexInNode++;
        CL_BTreeNodeHandle handle = node->_subtree [ndx+1];
        while (!node->_isLeaf) {
            path[_length]._handle = handle;
            path[_length]._indexInNode = 0;
            _length++;
            space->ReturnNode (node);
            node = space->BorrowNode (handle);
            handle = node->_subtree[0];
        };
        retVal = node->_item[0];
        space->ReturnNode (node);
        return retVal;
    }
    
    // We're in a leaf
    if (ndx >= node->_keyCount-1) {
            // We're at far right of the leaf, so move up
        do {
            _length--;
            space->ReturnNode (node);
        if (_length <= 0) break;
            node = space->BorrowNode (path[_length-1]._handle);
            ndx = path[_length-1]._indexInNode;
        } while (ndx >= node->_keyCount);
        if (_length) {
            retVal = node->_item[ndx];
            space->ReturnNode (node);
        }
        else
            retVal = NULL;
        return retVal;
    }
    // We're in the middle or at left end of a leaf
    path[_length-1]._indexInNode++;
    retVal = node->_item[path[_length-1]._indexInNode];
    space->ReturnNode (node);
    return retVal;
}




bool CL_GenericBTreeIterator::More ()
{
    return _index < _tree.Size()-1;
}



