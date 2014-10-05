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


/*
BTREES:  WinHelp-style B-tree implementation.
*/
#include "btrees.h"
#include <stdlib.h>


//
//  BtreePage   --A b-tree 'node'.  Used only by other
//                b-tree functions.
//

struct BtreePage
{
    uint_16     _numEntries;    // # of BtreeData in this page.
    BtreePage   *_prevPage;     // Prev page in this level.
    BtreePage   *_nextPage;     // Next page in this level.
    uint_16     _thisPage;      // Index # of this page.
    uint_32     _size;          // Size of the page
    uint_32     _maxSize;       // The page size; used for dumping
    BtreeData   *_entries;              // Pointer to BtreeData list.
    BtreePage   *_parent;               // Parent page.
    BtreePage   *_firstChild;           // First child page.

    // Dump this page to an output file.
    int         dump( OutFile *dest );

    // Split a page which has gotten too large into two pages.
    int         split();

    int         needSplit( uint_32 size );

    BtreePage( uint_32 max_size, BtreePage *ancestor, BtreePage *descendant=NULL );
    ~BtreePage();
};


//  BtreeData::BtreeData

BtreeData::BtreeData()
        : _bnext( NULL ),
          _bprev( NULL ),
          _child( NULL )
{
    // empty
}


//  BtreeData::insertSelf       --Insert this object into the specified page.

void BtreeData::insertSelf( BtreePage *dest )
{
    BtreeData   *current;
    BtreeData   *temp;

    temp = NULL;
    for( current = dest->_entries; current != NULL; current = current->_bnext ) {
        if( !current->lessThan( this ) )
            break;
        temp = current;
    }
    if( temp == NULL ) {
        if( current != NULL ) {
            _bnext = current;
            current->_bprev = this;
        }
        dest->_entries = this;
    } else {
        _bnext = temp->_bnext;
        _bprev = temp;
        temp->_bnext = this;
        if( _bnext != NULL ) {
            _bnext->_bprev = this;
        }
    }
    dest->_size += size();
    dest->_numEntries += 1;

    return;
}


//  BtreeDate::seekNext         --Find the child of the specified
//                                page where this data belongs.

BtreePage *BtreeData::seekNext( BtreePage *first )
{
    BtreePage   *result;
    BtreeData   *current;

    result = first->_firstChild;
    for( current = first->_entries; current != NULL; current = current->_bnext ) {
        if( lessThan( current ) )
            break;
        result = current->_child;
    }
    return result;
}


//  BtreePage::BtreePage

#define TREEPAGE_HEADER_SIZE    4

BtreePage::BtreePage( uint_32 max_size, BtreePage *ancestor, BtreePage *descendant )
        : _numEntries( 0 ),
          _prevPage( NULL ),
          _nextPage( NULL ),
          _maxSize( max_size ),
          _entries( NULL ),
          _parent( ancestor ),
          _firstChild( descendant )
{
    if( descendant == NULL ) {
        // Leaf pages start with a header and indices to the previous
        // and next leaf pages.
        _size = TREEPAGE_HEADER_SIZE+2*sizeof( uint_16 );
    } else {
        // Index pages start with a header and index to the leftmost
        // child.
        _size = TREEPAGE_HEADER_SIZE+sizeof( uint_16 );
    }
}


//  BtreePage::~BtreePage       --Deletes all of its contents as well.

BtreePage::~BtreePage()
{
    BtreeData   *current;
    BtreeData   *next;

    for( current = _entries; current != NULL; current = next ) {
        next = current->bnext();
        delete current;
    }
}


//  BtreePage::dump             --Dump the page to an output file.

int BtreePage::dump( OutFile *dest )
{
    uint_32     amount_left = _maxSize;

    // Spit out the page header.
    dest->write( (uint_8)0 );
    dest->write( (uint_8)0 );
    dest->write( _numEntries );
    amount_left -= TREEPAGE_HEADER_SIZE;

    // If this a leaf node, print the indices of the previous
    // and next pages.
    if( _firstChild == NULL ) {
        if( _prevPage ) {
            dest->write( _prevPage->_thisPage );
        } else {
            dest->write( (uint_16)~0 );
        }
        if( _nextPage ) {
            dest->write( _nextPage->_thisPage );
        } else {
            dest->write( (uint_16)~0 );
        }
        amount_left -= 2 * sizeof( uint_16 );
    } else {
        // If this is a tree node, print the index of the first child.
        dest->write( _firstChild->_thisPage );
        amount_left -= sizeof( uint_16 );
    }

    for( BtreeData *i = _entries; i != NULL; i = i->bnext() ) {

        // Dump the data.
        i->dump( dest );
        amount_left -= i->size();

        // if this is a tree node, print the index of the child page
        // corresponding to this data.

        if( _firstChild != NULL ) {
            dest->write( i->child()->_thisPage );
            amount_left -= sizeof( uint_16 );
        }
    }

    // Pad out the remaining space.
    while( amount_left ) {
        dest->write( (uint_8)0 );
        amount_left--;
    }
    return 1;
}


//  BtreePage::split    --Break the page in two.  Assumes the parent
//                        page is non-empty.

int BtreePage::split()
{
//    int         i=0;
    uint_16     limit = (uint_16)( ( _numEntries + 1 ) / 2 );
    uint_32     cur_size;
    uint_16     cur_num;
    BtreeData   *current;
    BtreePage   *sibling;

    // Search for the point to break the page at.
    cur_size = 0;
    cur_num = 0;
    for( current = _entries; current != NULL; current = current->bnext() ) {
        if( cur_num >= limit )
            break;
        cur_size += current->size();
        cur_num++;
    }
    if( current == NULL )
        return 0;

    // Create a new page, and modify the assorted pointers.
    sibling = new BtreePage( _maxSize, _parent, current->child() );
    sibling->_prevPage = this;
    sibling->_nextPage = _nextPage;
    if( _nextPage != NULL ) {
        _nextPage->_prevPage = sibling;
    }
    _nextPage = sibling;

    // Give some of our data to the new page by modifying our data list.
    if( _firstChild != NULL ) {
        sibling->_entries = current->bnext();
        if( current->bprev() != NULL ) {
            current->bprev()->bnext( NULL );
        } else {
            _entries = NULL;
        }
        if( current->bnext() != NULL ) {
            current->bnext()->bprev( NULL );
        }
        sibling->_numEntries = (uint_16) (_numEntries-cur_num-1);
        sibling->_size = _size-cur_size-current->size();
    } else {
        sibling->_entries = current;
        if( current->bprev() != NULL ) {
            current->bprev()->bnext( NULL );
        } else {
            _entries = NULL;
        }
        current->bprev( NULL );
        sibling->_numEntries = (uint_16) (_numEntries-cur_num);
        sibling->_size = _size-cur_size;
    }

    // Record the fact that we've lost data.
    _numEntries = cur_num;
    _size = cur_size + TREEPAGE_HEADER_SIZE + sizeof(uint_16);
    if( _firstChild == NULL ) {
        _size += sizeof( uint_16 );
    }

    if( _parent == NULL ) {

        // Die in flames!!!
        HCError( BTREE_ERR );

    } else {

        // Notify the parent page of the split by passing a new data item.
        BtreeData *newdata;
        if( _firstChild == NULL ) {

            // If this is a leaf page, we generate a new item.
            newdata = current->myKey();

        } else {

            // If this is a tree page, we pass an existing item.
            newdata = current;
            newdata->bnext( NULL );
            newdata->bprev( NULL );

        }
        newdata->child( sibling );
        newdata->insertSelf( _parent );
        _parent->_size += sizeof( uint_16 );
    }

    // Update the parent field of 'sibling's children.
    if( _firstChild != NULL ) {
        sibling->_firstChild->_parent = sibling;
        for( current = sibling->_entries; current != NULL; current = current->bnext() ) {
            current->child()->_parent = sibling;
        }
    }

    // And we're done!
    return 1;
}


//  BtreePage::needSplit    --Check if page size overflow.

int BtreePage::needSplit( uint_32 size )
{
    uint_32     cur_size = _size + size;
    uint_32     num      = _numEntries;


    if( size )
        ++num;
    if( _firstChild != NULL ) {
        cur_size += num * sizeof( uint_16 );
    }
    return( cur_size > _maxSize || num >= (uint_16)~1 );
}



//  Btree::Btree

Btree::Btree( char const *magnum, uint_32 max_size )
        : _numLevels( 1 ),
          _totalEntries( 0 ),
          _numPages( 0 ),
          _numSplits( 0 ),
          _size( 0 ),
          _maxSize( max_size ),
          _magic( magnum )
{
    _root = new BtreePage( max_size, NULL );
}


//  Btree::~Btree

Btree::~Btree()
{
    killPage( _root );
}


//  Btree::insert       --Insert a data element into a tree.

void Btree::insert( BtreeData *newdata )
{
    BtreePage   *nextlevel = _root;
    BtreePage   *curpage;

    // Search for the leaf page where this data will go.
    for( ;; ) {
        curpage = nextlevel;
        nextlevel = newdata->seekNext( curpage );
        if( nextlevel == NULL )
            break;

        // As we search, split any full pages we see on the way down.
        if( nextlevel->needSplit( newdata->size() ) ) {
            ++_numSplits;
            nextlevel->split();
            if( nextlevel->_nextPage->_entries->lessThan( newdata ) ) {
                nextlevel = nextlevel->_nextPage;
            }
        }
    }

    // Insert the data in the appropriate page.
    newdata->child( NULL );
    newdata->insertSelf( curpage );

    // If the _root page has to be split, do it now.
    if( _root->needSplit( 0 ) ) {
        BtreePage *newroot = new BtreePage( _maxSize, NULL, _root );
        _root->_parent = newroot;
        _root->split();
        _root = newroot;
        ++_numSplits;
        ++_numLevels;
    }

    // Increment the number of entries;
    _totalEntries++;
}


//  Btree::killPage     --Delete a tree of pages recursively.

void Btree::killPage( BtreePage *start )
{
    if( start == NULL )
        return;
    if( start->_firstChild != NULL ) {  // if this page has children,
        killPage( start->_firstChild );
        BtreeData *current;
        for( current = start->_entries; current != NULL; current = current->bnext() ) {
            killPage( current->child() );
        }
    }

    // Now delete the page itself.
    delete start;
}


//  Btree::size         --Overrides Dumpable::size.

uint_32 Btree::size()
{
    if( !_size ) {
        labelPages( _root );
        _size = ( _numPages * _maxSize );
        _size += 38;    // The size of the b-tree header.
    }
    return _size;
}


//  Btree::dump         --Overrides Dumpable::dump.
//                        A few magic #'s involved here; see "b_tree.doc".

int Btree::dump( OutFile *dest )
{
    static const int hsize = 6;
    uint_16     header[hsize] = {  0x0000, _numSplits, _root->_thisPage,
                                   0xFFFF, _numPages, _numLevels };

    // Write the magic number and header information.
    dest->write( _magic, 1, _magNumSize );
    dest->write( header, sizeof( uint_16 ), hsize );
    dest->write( _totalEntries );

    // Dump all of the pages recursively.
    dumpPage( dest, _root );
    return 1;
}


//  Btree::labelPages   --Assign indices to a tree of pages recursively.

void Btree::labelPages( BtreePage *start )
{
    if( start == NULL )
        return;
    start->_thisPage = _numPages++;
    labelPages( start->_firstChild );
    BtreeData   *current;
    for( current = start->_entries; current != NULL; current = current->bnext() ) {
        labelPages( current->child() );
    }
    return;
}


//  Btree::dumpPage     --Dump a tree of pages recursively.

void Btree::dumpPage( OutFile *dest, BtreePage *start )
{
    if( start == NULL )
        return;

    // Dump this page.
    start->dump( dest );

    // Recursively dump any pages below this page.
    if( start->_firstChild != NULL ) {
        dumpPage( dest, start->_firstChild );
        BtreeData       *current;
        for( current = start->_entries; current != NULL; current = current->bnext() ) {
            dumpPage( dest, current->child() );
        }
    }
    return;
}


//  Btree::findNode     --Find an element of the tree.

BtreeData *Btree::findNode( BtreeData &keyval )
{
    BtreeData   *result;

    if( _root == NULL )
        return NULL;
    BtreePage   *cur_page;
    BtreeData   *current;
    BtreePage   *nextpage = _root;
    do {
        cur_page = nextpage;
        nextpage = keyval.seekNext( cur_page );
    } while( nextpage != NULL );

    for( current = cur_page->_entries; current != NULL; current = current->bnext() ) {
        if( !current->lessThan( &keyval ) ) {
            break;
        }
    }
    if( current == NULL || keyval.lessThan( current ) ) {
        result = NULL;
    } else {
        result = current;
    }
    return result;
}


//  BtreeIter::BtreeIter

BtreeIter::BtreeIter( Btree &t )
{
    _tree = &t;
    init();
}


//  BtreeIter::init     --Point this iterator at the leftmost
//                        leaf of the tree.

void BtreeIter::init()
{
    _page = _tree->_root;
    if( _page == NULL ) {
        _data = NULL;
        return;
    }

    while( _page->_firstChild != NULL ) {
        _page = _page->_firstChild;
    }
    _data = _page->_entries;
    return;
}


//  BtreeIter::goPrev   --Find the previous data element.

void BtreeIter::goPrev()
{
    if( _data->bprev() == NULL ) {
        _page = _page->_prevPage;
        if( _page == NULL ) {
            _data = NULL;
        } else {
            _data = _page->_entries;
            while( _data->bnext() != NULL ) {
                _data = _data->bnext();
            }
        }
    } else {
        _data = _data->bprev();
    }
    return;
}


//  BtreeIter::goNext   --Find the next data element.

void BtreeIter::goNext()
{
    if( _data->bnext() == NULL ) {
        _page = _page->_nextPage;
        if( _page == NULL ) {
            _data = NULL;
        } else {
            _data = _page->_entries;
        }
    } else {
        _data = _data->bnext();
    }
    return;
}


// Buncha overloaded operators for iterators.

BtreeIter& BtreeIter::operator--()
{
    goPrev();
    return *this;
}

void BtreeIter::operator--(int)
{
    goPrev();
    return;
}

BtreeIter& BtreeIter::operator++()
{
    goNext();
    return *this;
}

void BtreeIter::operator++(int)
{
    goNext();
    return;
}


//  BtreeIter::pageEntries      --Access function.

uint_16 BtreeIter::pageEntries()
{
    uint_16 result = 0;
    if( _page != NULL ) {
        result = _page->_numEntries;
    }
    return result;
}


//  BtreeIter::thisPage         --Return the index of the
//                                current page.

uint_16 BtreeIter::thisPage()
{
    uint_16 result = (uint_16) ~0;
    if( _page != NULL ) {
        result = _page->_thisPage;
    }
    return result;
}


//  BtreeIter::nextPage --Go to the first data element on the next page.

void BtreeIter::nextPage()
{
    if( _page != NULL ) {
        _page = _page->_nextPage;
        if( _page == NULL ) {
            _data = NULL;
        } else {
            _data = _page->_entries;
        }
    }
}
