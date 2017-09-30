




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



#include "base/bytestrm.h"

#include "io/dskbtree.h"
#include "io/bytstore.h"


#include <iostream.h>







class CL_DiskBTreeNode: public CL_GenericBTreeNode, public CL_Object {

public:
    CL_DiskBTreeNode (short order, const CL_GenericBTree& tree,
                      CL_AbstractComparator& cmp);

    bool ReadFrom (const CL_Stream&);

    bool WriteTo  (CL_Stream&) const;

    const char* ClassName () const { return "CL_DiskBTreeNode";};

};



class CL_DiskBTreeNodeSpace: public CL_BTreeNodeSpace {

public:
    CL_DiskBTreeNodeSpace (short order, CL_ByteStringStore& store,
                           const CL_DiskBTree& tree,
                           CL_AbstractComparator& cmp,
                           CL_ObjectBuilder* f, bool create = FALSE);

    ~CL_DiskBTreeNodeSpace ();
    
    // --------------- Override all virtual functions ----------------
    
    CL_BTreeNodeHandle RootHandle () const;

    void NewRoot (CL_BTreeNodeHandle h);
    
    CL_BTreeNodeHandle CreateNode ();

    CL_GenericBTreeNode* BorrowNode (CL_BTreeNodeHandle h) const;
    
    void ReturnNode (CL_GenericBTreeNode* ) const;
    
    void NodeModified (CL_GenericBTreeNode*);
    
    void DestroyNode (CL_GenericBTreeNode* node);

    virtual void DestroyItem (CL_VoidPtr p) const
        {if (p) delete (CL_ObjectPtr) p;};
    
    CL_ObjectBuilder* ItemBuilder () const {return _builder;};

protected:
    CL_ByteStringStore&  _store;
    CL_ObjectBuilder*    _builder;
    CL_GenericBTreeNode* _tmp;  // A (hopefully temporary) hack to remember
                                // the most recently returned node.
};



static const short DEFAULT_SIZE = 70;
// This value is used as the starting size of ByteStrings that serve as
// ByteStreams for  storing representations of nodes. Its purpose is to
// speed up the algorithms by obviating the need for repeated resizing of
// the ByteString.


// --------------------- DiskBTreeNodeSpace methods -----------------------


struct DiskBTreeHeader {
    CL_SlottedFileHandle root;
    short order;
};

CL_DiskBTreeNodeSpace::CL_DiskBTreeNodeSpace
    (short order, CL_ByteStringStore& store,
     const CL_DiskBTree& tree, CL_AbstractComparator& cmp,
     CL_ObjectBuilder* f, bool create)
:CL_BTreeNodeSpace (order, tree._tree, cmp), _store (store)
{
    if (!f)
        CL_Error::Warning ("DiskBTree constructor: null builder function");
    _builder = f;
    if (create) {
        CL_BTreeNodeHandle h = CreateNode ();
        NewRoot (h);
    }
    _tmp = NULL;
}

CL_DiskBTreeNodeSpace::~CL_DiskBTreeNodeSpace ()
{
    if (_tmp)
        _Destroy (_tmp);
}




/* ------------------------------------------------------------------ */

CL_BTreeNodeHandle CL_DiskBTreeNodeSpace::RootHandle () const
{
    CL_ByteString hdr (sizeof (DiskBTreeHeader));
    if (!_store.ReadHeader (hdr))
        return 0;
    DiskBTreeHeader* p = (DiskBTreeHeader*) hdr.AsPtr();
    return p->root;
}



/* ------------------------------------------------------------------ */


void CL_DiskBTreeNodeSpace::NewRoot (CL_BTreeNodeHandle h)
{
    CL_ByteString hdr (sizeof (DiskBTreeHeader));
    DiskBTreeHeader* p = (DiskBTreeHeader*) hdr.AsPtr();
    p->root = h;
    p->order = _order;
    _store.WriteHeader (hdr);
}



/* ------------------------------------------------------------------ */


CL_BTreeNodeHandle CL_DiskBTreeNodeSpace::CreateNode ()
{
    CL_ByteString dummy (DEFAULT_SIZE);
    CL_ByteStream s (dummy);
    CL_DiskBTreeNode node (_order, _tree, _cmp);
    CL_BTreeNodeHandle h = _store.Allocate ();
    _SetHandle (node, h);
    node.WriteTo (s);
    _store.Modify (h, dummy);
    return h;
}



/* ------------------------------------------------------------------ */


CL_GenericBTreeNode* CL_DiskBTreeNodeSpace::BorrowNode
    (CL_BTreeNodeHandle h) const
{
    CL_ByteString node_data (DEFAULT_SIZE);
    if (!_store.Retrieve (h, node_data))
        return NULL;
    CL_ByteStream s (node_data);
    CL_DiskBTreeNode* n = new CL_DiskBTreeNode (_order, _tree, _cmp);
    if (n) {
        n->ReadFrom (s);
        _SetHandle (*n, h);
    }
    return n;
}



/* ------------------------------------------------------------------ */


void CL_DiskBTreeNodeSpace::ReturnNode (CL_GenericBTreeNode* n) const
{
    if (_tmp)
        _Destroy (_tmp);
    ((CL_DiskBTreeNodeSpace*) this)->_tmp = n; // cast away const
}



/* ------------------------------------------------------------------ */


void CL_DiskBTreeNodeSpace::NodeModified (CL_GenericBTreeNode* node)
{
    if (!node)
        return;
    CL_ByteString node_data (DEFAULT_SIZE);
    CL_ByteStream s (node_data);
    ((CL_DiskBTreeNode*) node)->WriteTo (s);
    _store.Modify (node->Handle(), node_data);
}


/* ------------------------------------------------------------------ */

void CL_DiskBTreeNodeSpace::DestroyNode (CL_GenericBTreeNode* node)
{
    _store.Remove (node->Handle());
}




// --------------------- CL_DiskBTreeNode methods ----------------------


CL_DiskBTreeNode::CL_DiskBTreeNode (short order,
                                    const CL_GenericBTree& tree,
                                    CL_AbstractComparator& cmp)
: CL_GenericBTreeNode (order, tree.NodeSpace(), cmp)
{
}


/* ------------------------------------------------------------------ */

bool CL_DiskBTreeNode::ReadFrom (const CL_Stream& s)
{
    if (s.Read (_keyCount) && s.Read ((short&) _isLeaf) && s.Read
        (_subtreeSize)) {
        if (!_subtree)
            return FALSE;
        // _subtree is already allocated in the BTreeNode constructor
        if (_keyCount <= 0)
            return TRUE;
        short i;
        for (i = 0; i <= _keyCount; i++)
            if (!s.Read (_subtree[i]))
                return FALSE;
        CL_ObjectBuilder* bld =
            ((CL_DiskBTreeNodeSpace*) _nodeSpace)->ItemBuilder ();
        for (i = 0; i < _keyCount; i++) {
            CL_Object* p = bld->BuildFrom (s);
            if (!p)
                return FALSE;
            _item[i] = p;
        }
        return TRUE;
    }
    return FALSE;
}


/* ------------------------------------------------------------------ */

bool CL_DiskBTreeNode::WriteTo  (CL_Stream& s) const
{
    if (s.Write (_keyCount) && s.Write ((short) _isLeaf) && s.Write
        (_subtreeSize)) {
        short i;
        for (i = 0; i <= _keyCount; i++)
            if (!s.Write (_subtree[i]))
                return FALSE;
        for (i = 0; i < _keyCount; i++) {
            if (!((CL_Object*) _item[i])->WriteTo (s))
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}



/* ------------------------------------------------------------------ */





static short _ReadOrder (CL_ByteStringStore& store)
{
    // This doesn't seem to serve its purpose, because of
    // order-of-initialization problems. Will try to fix it later.
    // -- MAS 8/22/94
    CL_ByteString hdr (sizeof (DiskBTreeHeader));
    if (!store.ReadHeader (hdr))
        return 0;
    DiskBTreeHeader* p = (DiskBTreeHeader*) hdr.AsPtr();
    return p->order;
}


CL_DiskBTree::CL_DiskBTree (CL_ByteStringStore& store,
                            CL_ObjectBuilder* f, short order, bool create)
: CL_BTree<CL_ObjectPtr>
  (order,
   (_diskNodeSpace = new CL_DiskBTreeNodeSpace (order, store, *this,
                                                _comparator, f, create))
  ),
  _store (store)
{
}


CL_DiskBTree::~CL_DiskBTree ()
{
    delete _diskNodeSpace;
}






// void CL_DiskBTree::IntoStream (ostream& strm) const
// {
//     CL_BTreeNodeSpace* space = NodeSpace ();
//     if (space) {
//         CL_BTreeNodeHandle h = space->RootHandle();
//         PrintTree (h, 0, strm);
//     }
// }
// 
// 
// 
// 
// 
// void CL_DiskBTree::PrintTree (CL_BTreeNodeHandle h, short level,
//                               ostream& strm) const
// {
// 
//     if (h == 0)
//         return;
//     CL_BTreeNodeSpace* space = NodeSpace ();
//     CL_GenericBTreeNode* z = space->BorrowNode (h);
// 
//     for (short i = 0; i < z->Size(); i++) {
//         PrintTree (z->Subtree(i), level+1, strm);
//         for (short j = 0; j < 4*level; j++)
//             strm << ' ';
//         CL_ObjectPtr q = (CL_ObjectPtr) z->Item(i);
//         CL_String s;
//         if (q)
//             s = q->AsString();
//         strm << "------>" << s << endl << flush;
//     }
//     PrintTree (z->Subtree(z->Size()), level+1, strm);
//     space->ReturnNode (z);
// }


