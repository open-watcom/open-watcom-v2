

#ifndef _dskbtree_h_ /* Thu Feb 24 15:01:49 1994 */
#define _dskbtree_h_





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

// The DiskBTree owns its contained objects, unlike other containers that
// use reference semantics. A client of this object is required to ``hand
// over'' the stored objects to the DiskBTree, which then takes care of
// destroying it.
//
// An object that needs to be stored in a B-tree on disk must implement
// the virtual ReadFrom and WriteTo methods of CL_Object. The
// ObjectBuilder passed to the constructor of the DiskBTree must
// reconstruct an instance of the B-tree item and return it.

#include "base/stream.h"
#include "base/tbtree.h"
#include "base/builder.h"
#include "io/bytstore.h"

class CL_DiskBTreeNodeSpace;

class CL_DiskBTree: public  CL_BTree<CL_ObjectPtr> {

public:
    CL_DiskBTree (CL_ByteStringStore& store,
                  CL_ObjectBuilder* f, short order, bool create = FALSE);
    // The parameter f is an ObjectBuilder that can reconstruct an item of
    // the B-tree from its passive representation in a Stream, and return
    // this item (see {\tt builder.h}). The (memory for the) object f is
    // assumed to be owned by the user of this tree, and is not tampered
    // with by this tree.
    //
    // The present implementation of the DiskBTreeNodeSpace uses no
    // caching; it simply maps nodes to byte strings and stores them in
    // the ByteStringStore.

    
    ~CL_DiskBTree();
    // Destructor.
    
    // ---------------------- Basic methods ----------------------------


    virtual const char* ClassName () const { return "CL_DiskBTree";};

    // --------------------- End public protocol -----------------------
protected:
    CL_Comparator<CL_ObjectPtr> _comparator;
    CL_ByteStringStore&         _store;
    CL_DiskBTreeNodeSpace*      _diskNodeSpace;
    CL_ObjectBuilder*           _builder;

    friend CL_DiskBTreeNodeSpace;

};





#endif /* _dskbtree_h_ */
