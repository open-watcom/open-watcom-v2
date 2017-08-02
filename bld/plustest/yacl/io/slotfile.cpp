



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



// This may look like C code, but it is really -*- C++ -*-

#include "base/binding.h"
#include "base/bitset.h"
#include "base/bytestrm.h"
#include "io/slotfile.h"


#define NEW_OP new

// Notes on the SlottedFile implementation:
// 
// The  slotted file is viewed  as a tree  with three levels: the root, its
// children and its  grandchildren. The  root  has 2**14 =  16384 children,
// among which the  the leftmost 2**12 = 4096  are actual records, and  the
// remaining are bitmaps (instances of  CL_BitSet), called block    bitmaps.
// At  the root are  stored  two bitmaps, each  containing  16384 bits: the
// fullMap, in which a bit is on if the corresponding child  of the root is
// full, and  the nonemptyMap, in which  a bit is  on  if the corresponding
// child  of the root is  not empty. Note  that every element in the bitset
// fullMap also  occurs in nonemptyMap.    The former bitset is  useful for
// fast allocation, while the latter is useful for fast iteration.
// 
// This tree is stored in pre-order on the disk file.



#define ROOT_WIDTH  14           // 2**(ROOT_WIDTH) is the number of
                                 // children of the root; thus ROOT_WIDTH is
                                 // the number of bits needed to specify a
                                 // block bitmap
#define WIDTH       16           // #bits needed for specifying a
                                 // record in a block

#define RECORDS_PER_BLOCK  (((ulong) 1) << WIDTH)
                                 // #records in a block

#define UNUSED_BITS 4096         // #children of the root that are records
                                 // and not blocks.

// CL_SlottedFileHandle has the following layout:
//
//    ---------------------------------------------------------------
//    |01|<------   Block bitmap # ------>|<-- Record # in block -->|
//    ---------------------------------------------------------------
//        ^                                ^                       ^
//      Bit 29                           Bit 15                  Bit 0
//
// The 1 in bit 30 is to ensure that every handle is nonzero.
//


class CL_SlottedFileHeader: public CL_Object {

    // This is a private class encapsulating the header of a SlottedFile.
public:
    CL_SlottedFileHeader ();
    long StorableFormWidth () const;
    bool ReadFrom (const CL_Stream&);
    bool WriteTo (CL_Stream&) const;
    static long StoreWidth (); // Provided for the purpose of efficiency 

    const char* ClassName () const {return "CL_SlottedFileHeader";};

    void operator= (const CL_SlottedFileHeader& hdr);

    void operator= (const CL_Object& o)
        {*this = (const CL_SlottedFileHeader&) o;};

    // Instance data:
    CL_BitSet fullMap;
    CL_BitSet nonemptyMap;
    long allocCount; // #allocated slots in the file
    long slotSize;   // Size of each slot, in bytes
    short userHeaderSize;
};

void CL_SlottedFileHeader::operator= (const CL_SlottedFileHeader& hdr)
{
    fullMap        = hdr.fullMap;
    nonemptyMap    = hdr.nonemptyMap;
    allocCount     = hdr.allocCount;
    slotSize       = hdr.slotSize;
    userHeaderSize = hdr.userHeaderSize;
}

long CL_SlottedFileHeader::StoreWidth ()
{
    static long width = -1;
    if (width == -1) {
        CL_SlottedFileHeader hdr;
        width = hdr.StorableFormWidth();
    }
    return width;
}



CL_SlottedFileHeader::CL_SlottedFileHeader ()
: fullMap (1L << ROOT_WIDTH), nonemptyMap (1L << ROOT_WIDTH)
{
    userHeaderSize = 0;
}

long CL_SlottedFileHeader::StorableFormWidth() const
{
    return sizeof (short) + 2*sizeof (long) + 2*fullMap.StorableFormWidth();
}

bool CL_SlottedFileHeader::WriteTo (CL_Stream& s) const
{
    return s.Write (userHeaderSize) && s.Write (allocCount) && s.Write
        (slotSize) && fullMap.WriteTo (s) && nonemptyMap.WriteTo (s);
}

bool CL_SlottedFileHeader::ReadFrom (const CL_Stream& s)
{
    return s.Read (userHeaderSize) && s.Read (allocCount) && s.Read
        (slotSize) && fullMap.ReadFrom (s) && nonemptyMap.ReadFrom (s);
}    



// ------------------------ IO_Iter structure ---------------------
// 
// struct IO_Iter {
//     IO_Iter () : _rootIter (root), _blockIter (block) {};
//     CL_BitSet root, block;
//     CL_BitSetIterator _rootIter, _blockIter;
//     short _blockIndex;
// };



/* ---------------------- Handle manipulation functions ------------- */

inline CL_SlottedFileHandle __MakeHandle (long record_no,
                                          long block_map_index)
{
    return record_no | (((ulong) block_map_index) <<  WIDTH)
        | (((ulong) 1) << (ROOT_WIDTH + WIDTH));
}

inline long __RecordNumber (CL_SlottedFileHandle h)
{
    return ((((ulong) 1) << WIDTH) - 1) & h;
}

inline long __BlockMapIndex (CL_SlottedFileHandle h)
{
    return (h  >> WIDTH) & ((((ulong) 1) << ROOT_WIDTH) - 1);
}





/* ------------------------ CL_SlottedFile methods --------------------- */


CL_SlottedFile::CL_SlottedFile (CL_Stream& s, bool report)
: _file (s), _reportErrors (report)
{
    _header = new CL_SlottedFileHeader;
}

/* ------------------------------------------------------------------ */

CL_SlottedFile::~CL_SlottedFile ()
{
    if (_header) {
        delete _header;
    }
}


/* ------------------------------------------------------------------ */

CL_SlottedFileHandle CL_SlottedFile::AllocateSlot ()
{
    if (!PrepareToChange())
        return 0;
    CL_SlottedFileHandle h =  _GetSlot ();
    Notify();
    return h;
}

/* ------------------------------------------------------------------ */

CL_SlottedFileHandle CL_SlottedFile::AddRecord (uchar* record)
{
    if (!PrepareToChange())
        return 0;
    CL_SlottedFileHandle h = _GetSlot ();
    if (h)
        if (!_file.Write (_RecordOffset (h), record, _slotSize))
            _DoError ("AddRecord", h);
    Notify ();
    return h;
}


/* ------------------------------------------------------------------ */


bool CL_SlottedFile::IsValid (CL_SlottedFileHandle h) const
{
    if (!_ReadHeader (*_header)) {
        _DoError ("IsValid: header read");
        return FALSE;
    }
    long rec   = __RecordNumber (h);
    long index = __BlockMapIndex (h);
    if (index < UNUSED_BITS) {
        ulong hi_bits = h & (3L << (WIDTH + ROOT_WIDTH));
        return (rec == index)
            && (hi_bits == (1L << (WIDTH + ROOT_WIDTH)))
            && _header->fullMap.Includes (rec);
    }
    CL_BitSet block_map;
    if (!_file.Read (_BlockBitmapOffset (index), block_map)) {
        _DoError ("IsValid: block map read");
        return FALSE;
    }
    return block_map.Includes (rec);
}


/* ------------------------------------------------------------------ */

long CL_SlottedFile::SlotsAllocated () const
{
    if (!_ReadHeader (*_header)) {
        _DoError ("SlotsAllocated: header read");
        return 0;
    }
    return _header->allocCount;
}


/* ------------------------------------------------------------------ */


bool CL_SlottedFile::RetrieveRecord (CL_SlottedFileHandle h,
                                     uchar* record) const
{
    if (!IsValid (h))
        return FALSE;
    if (_file.Read (_RecordOffset (h), record, _slotSize) != _slotSize) {
        _DoError ("RetrieveRecord: record read failed", h);
        return FALSE;
    }
    return TRUE;
}


/* ------------------------------------------------------------------ */


bool CL_SlottedFile::ModifyRecord (CL_SlottedFileHandle h,
                                   uchar* record)
{
    if (!PrepareToChange())
        return FALSE;
    if (!IsValid (h))
        return FALSE;
    if (!_file.Write (_RecordOffset (h), record, _slotSize)) {
        _DoError ("ModifyRecord: record write failed", h);
        return FALSE;
    }
    Notify ();
    return TRUE;
}




/* ------------------------------------------------------------------ */



bool CL_SlottedFile::DeleteRecord (CL_SlottedFileHandle handle)
    // Free a previously-allocated slot
{
    if (!PrepareToChange())
        return FALSE;
    ulong hi_bits = handle & (3L << (WIDTH + ROOT_WIDTH));
    if (hi_bits != (1L << (WIDTH + ROOT_WIDTH)))
        return FALSE;
    long rec = __RecordNumber (handle);
    long index = __BlockMapIndex (handle);
    long new_file_size = -1L; // Will be changed appropriately if the file
                              // needs to be truncated
    CL_BitSet block_map;
    if (!_ReadHeader (*_header))
        return FALSE;
    if (index < UNUSED_BITS) {
        // The index specifies a record, not a bitmap
        _header->fullMap.Remove (index);
        _header->nonemptyMap.Remove (index);
        if (_header->nonemptyMap.IsEmpty()) {
            // There are no more records in the file after this deletion
            new_file_size = CL_SlottedFileHeader::StoreWidth();
        }
        else {
            long l = _header->nonemptyMap.Largest();
            if (l < index) {
                // We're deleting the record at the end of the file
                new_file_size = _BlockBitmapOffset (l) + _slotSize;
            }
        }
    }
    else {
        // The index specifies a bitmap
        CL_Offset block_offset = _BlockBitmapOffset (index);
        long file_size = _file.Size();
        if (block_offset < 0 || block_offset >= file_size)
            return FALSE;
        if (!_file.Read (block_offset, block_map)) {
            _DoError ("DeleteRecord: Level-1 map read failed");
            return FALSE;
        }
        block_map.Remove (rec);
        _header->fullMap.Remove (index);
        register long rec_end = _RecordOffset (handle) + _slotSize;
        if (block_map.IsEmpty()) {
            _header->nonemptyMap.Remove (index);
            if (file_size <= rec_end) {
                // We're removing the only allocated record in the block
                new_file_size = block_offset;
            }
        }
        else {
            if (file_size <= rec_end) {
                // We're removing the record at the end of the file
                new_file_size = rec_end - _slotSize;
            }
        }
        if (new_file_size != block_offset &&
            !_file.Write (block_offset, block_map)) {
            _DoError ("DeleteRecord: block map write failed", handle);
            return FALSE;
        }
    }
    _header->allocCount--;
    if (!_WriteHeader (*_header))
        return FALSE;
    if (new_file_size != -1L && !_file.ChangeSize (new_file_size)) {
        _DoError ("DeleteRecord: ChangeSize failed");
        return FALSE;
    }
    Notify ();
    return TRUE;
}



/* ------------------------------------------------------------------ */
bool CL_SlottedFile::ReadHeader (uchar* header) const
{
    return (_userHeaderSize > 0)
        ? _file.Read (CL_SlottedFileHeader::StoreWidth(), header,
                      _userHeaderSize)
        : FALSE;
}

        

// Write the parameter into the user-defined header. Returns
// TRUE on success, FALSE on i/o error.
bool CL_SlottedFile::WriteHeader (uchar* header)
{
    return (_userHeaderSize > 0)
        ? _file.Write (CL_SlottedFileHeader::StoreWidth(), header,
                       _userHeaderSize)
        : FALSE;
}






CL_SlottedFileHandle CL_SlottedFile::FirstRecord (uchar* record) const
{
    if (!_ReadHeader (*_header)) {
        _DoError ("FirstRecord: header read");
        return 0;
    }
    if (_header->allocCount <= 0)
        return 0;
    long index = _header->nonemptyMap.Smallest ();
    CL_SlottedFileHandle ret;
    if (index < UNUSED_BITS) 
        ret = __MakeHandle (index, index);
    else {
        CL_BitSet block_map;
        if (!_file.Read (_BlockBitmapOffset (index), block_map)) {
            _DoError ("FirstRecord:  block bitmap read", 0);
            return 0;
        }
        ret = __MakeHandle (block_map.Smallest(), index);
    }
    return RetrieveRecord (ret, record) ? ret : 0;
    
}

CL_SlottedFileHandle CL_SlottedFile::NextRecord (CL_SlottedFileHandle h,
                                                 uchar* record) const
{
    if (!_ReadHeader (*_header)) {
        _DoError ("NextRecord: header read");
        return 0;
    }
    long rec   = __RecordNumber (h);
    long index = __BlockMapIndex (h);

    CL_Offset offset = _BlockBitmapOffset (index);
    if (offset >= _file.Size()) {
        return 0; 
    }
    CL_BitSet block_map;
    CL_SlottedFileHandle ret;
    if (rec >= UNUSED_BITS) {
        // The given handle is that of a record in some block; read its map
        if (!_file.Read (offset, block_map)) {
            _DoError ("_NextRecord:  block bitmap read", 0);
            return 0;
        }
        long rec2 = block_map.Successor (rec);
        if (rec2 >= 0) {
            ret = __MakeHandle (rec2,  index);
            return RetrieveRecord (ret, record) ? ret : 0;
        }
        // No more allocated records in the block; fall through...
    }
    long l = _header->nonemptyMap.Successor (index);
    if (l < 0)
        return 0L;
    if (l < UNUSED_BITS)
        ret = __MakeHandle (l, l);
    else {
        if (!_file.Read (_BlockBitmapOffset (l), block_map)) {
            _DoError ("_NextRecord:  block bitmap read 2", 0);
            return 0;
        }
        ret = __MakeHandle (block_map.Smallest(), l);
    }
    return RetrieveRecord (ret, record) ? ret : 0;
}







/* ------------------------------------------------------------------ */



// Protected methods:
 
static long __BlockMapWidth ()
{
    static long width = -1;
    if (width == -1) {
        CL_BitSet  block_map (RECORDS_PER_BLOCK);
        width = block_map.StorableFormWidth ();
    }
    return width;
}


/* ------------------------------------------------------------------ */

CL_Offset CL_SlottedFile::_BlockBitmapOffset (ushort index) const
{
    long n = CL_SlottedFileHeader::StoreWidth() + _userHeaderSize;
    if (index < UNUSED_BITS) {
        // We're talking about a record, not a bitmap
        return  n + index * _slotSize;
    }
    return n + UNUSED_BITS * _slotSize
        + (index - UNUSED_BITS) * (__BlockMapWidth() +
                                   RECORDS_PER_BLOCK * _slotSize);
}


/* ------------------------------------------------------------------ */


CL_Offset        CL_SlottedFile::_RecordOffset
    (CL_SlottedFileHandle h) const
{
    long n = __BlockMapIndex(h);
    CL_Offset offset = _BlockBitmapOffset (n);
    if (n >= UNUSED_BITS) {
        CL_BitSet block_map (RECORDS_PER_BLOCK);
        offset += _slotSize * __RecordNumber (h)
            + block_map.StorableFormWidth();
    }
    return offset;
}


/* ------------------------------------------------------------------ */



CL_SlottedFileHandle CL_SlottedFile::_GetSlot ()
{
    if (!_ReadHeader (*_header)) {
        _DoError ("_GetSlot: header read", 0);
        return 0;
    }
    if (_header->fullMap.IsUniversal ()) {
        // No more slots available!
        return 0;
    }

    long index = _header->fullMap.SmallestNonMember ();
    long record_no;
    if (index <  UNUSED_BITS) {
        // We're allocating in the low end of the file
        record_no = index;
        _header->fullMap.Add (index);
        _header->nonemptyMap.Add (index);
    }
    else {
        // We're allocating in the high end
        CL_Offset offset = _BlockBitmapOffset (index);
        if (offset >= _file.Size()) {
            // File too small, create the level-1 map
            CL_BitSet block_map (RECORDS_PER_BLOCK);
            block_map.Add (0);
            if (!_file.Write (offset, block_map)) {
                _DoError ("_GetSlot: block_map write", 0);
                return 0;
            }
            record_no = 0;
            _header->nonemptyMap.Add (index);
        }
        else {
            // File large enough, level-1 map exists
            CL_BitSet block_map;
            if (!_file.Read (offset, block_map)) {
                _DoError ("_GetSlot:  block bitmap read", 0);
                return 0;
            }
#ifdef ERROR_CHECKS
            if (block_map.IsUniversal ()) {
                // Something wrong
                CL_Error::Warning ("_GetSlot: internal error: block_map full");
                return 0;
            }
#endif
            record_no = block_map.SmallestNonMember ();
            block_map.Add (record_no);
            if (!_file.Write (offset, block_map)) {
                _DoError ("_GetSlot: block_map write back", 0);
                return 0;
            }
            _header->nonemptyMap.Add (index);
            if (block_map.IsUniversal ()) 
                _header->fullMap.Add (index);
        }
    }
    _header->allocCount++;
    if (!_WriteHeader (*_header)) {
        _DoError ("_GetSlot: header write back", 0);
        return 0;
    }
    CL_SlottedFileHandle h;
    h = __MakeHandle (record_no, index);
    return h;
}

/* ------------------------------------------------------------------ */


void CL_SlottedFile::_DoError (const char* caller,
                               CL_SlottedFileHandle h) const
{
    if (_reportErrors)
        CL_Error::Warning ("CL_SlottedFile::%s: handle %ld:"
                           "I/O error %s", caller, h,
                           _file.ErrorString().AsPtr());
}
    


 









// ------------------------ CL_PrivateSlottedFile methods ----------------



CL_PrivateSlottedFile::CL_PrivateSlottedFile (const char* path, bool report)
    : CL_SlottedFile ( * (_ourFile = new CL_BinaryFile (path)), report)
{
    if (!_header || !_ourFile)
        return; // No memory
    _weOwnIt = TRUE;
    if (!_file.SeekTo (0) || !_header->ReadFrom (_file)) {
        CL_Error::Fatal
            ("CL_PrivateSlottedFile constructor: "
             "file '%s' is not a slotted file", path);
    }
    _slotSize = _header->slotSize;
    _userHeaderSize = _header->userHeaderSize;
}


/* ------------------------------------------------------------------ */


 
CL_PrivateSlottedFile::CL_PrivateSlottedFile
    (const char* path, long record_size, short user_hdr, bool report)
    : CL_SlottedFile (* (_ourFile =
                         new CL_BinaryFile (path, TRUE)),
                      report)
{
    if (!_header)
        return; // No memory
    _header->allocCount = 0;
    _header->slotSize   = record_size;
    _userHeaderSize = _header->userHeaderSize = user_hdr;
    _slotSize      = record_size;
    if (!_WriteHeader (*_header)) {
        _DoError ("constructor: header write", 0);
        return;
    }
    if (user_hdr > 0) {
        CL_ByteString dummy (user_hdr);
        WriteHeader (dummy);
    }
    _weOwnIt = TRUE;
}


/* ------------------------------------------------------------------ */
 
CL_PrivateSlottedFile::CL_PrivateSlottedFile (CL_Stream& file,
                                              bool report)
: CL_SlottedFile (file, report)
{
    _weOwnIt = FALSE;
    _ourFile = NULL;
    if (!_header)
        return; // No memory
    if (!_file.SeekTo (0) || !_header->ReadFrom (_file)) {
        CL_Error::Fatal ("CL_PrivateSlottedFile constructor: "
                         "stream is not a slotted file");
    }
    _slotSize = _header->slotSize;
    _userHeaderSize = _header->userHeaderSize;
}

/* ------------------------------------------------------------------ */
 
CL_PrivateSlottedFile::CL_PrivateSlottedFile (CL_Stream& file,
                                              long record_size,
                                              short user_hdr,
                                              bool report)
: CL_SlottedFile (file, report)
{
    _weOwnIt = FALSE;
    _ourFile = NULL;
    if (!_header)
        return; // No memory
    _header->allocCount = 0;
    _header->slotSize   = record_size;
    _slotSize           = record_size;
    _userHeaderSize     = _header->userHeaderSize = user_hdr;
    if (!_WriteHeader (*_header)) {
        _DoError ("constructor: header write", 0);
    }
}


/* ------------------------------------------------------------------ */
 
CL_PrivateSlottedFile::~CL_PrivateSlottedFile()
{
    if (!_file.SeekTo (0) || !_header->WriteTo (_file)) {
        CL_Error::Warning ("CL_PrivateSlottedFile destructor: "
                         "header write failed");
    }
    if (_weOwnIt)
        delete _ourFile;
}


/* ------------------------------------------------------------------ */
 

bool CL_PrivateSlottedFile::_ReadHeader (CL_SlottedFileHeader& hdr) const
{
    if (&hdr != _header)
        hdr = *_header;
    return (_header != NULL);
}



/* ------------------------------------------------------------------ */
 
bool CL_PrivateSlottedFile::_WriteHeader (const CL_SlottedFileHeader&) const
{
//     if (_header != NULL && _file.SeekToBegin ()) {
//         _file << (*_header);
//         return TRUE;
//     }
//     return FALSE;
    return TRUE;
}










CL_SlottedFileIterator::CL_SlottedFileIterator (const CL_SlottedFile& file)
: _rootIter (_root), _blockIter (_block), _sfile (file)
{
    _header = new CL_SlottedFileHeader;
}


CL_SlottedFileIterator::CL_SlottedFileIterator
    (const CL_SlottedFileIterator& iter)
: _rootIter (_root), _blockIter (_block), _sfile (iter._sfile)
{
    _header = new CL_SlottedFileHeader;
}

CL_SlottedFileIterator::~CL_SlottedFileIterator ()
{
    delete _header;
}

void CL_SlottedFileIterator::Reset ()
{
    if (!_sfile._ReadHeader (*_header)) {
        _sfile._DoError ("Reset: root bitmap read", 0);
        return;
    }
    _root = _header->nonemptyMap;
    _block.MakeEmpty ();
    _rootIter.Reset ();
    _blockIter.Reset ();
    _blockIndex = -1;
}


CL_SlottedFileHandle CL_SlottedFileIterator::Next (uchar* record)
{
    // At the end of execution of this method, _blockIndex will be the
    // block-map index of the record being returned (or the record index
    // itself, if the value is less than UNUSED_BITS).
    CL_SlottedFileHandle ret_val;
    short rec_no;
    if (!_blockIter.More()) {
        if (!_rootIter.More()) {
            // Iteration finished
            return 0;
        }
        _blockIndex = _rootIter.Next();
        if (_blockIndex < UNUSED_BITS)
            rec_no = _blockIndex;
        else {
            if (!_sfile._file.Read (_sfile._BlockBitmapOffset (_blockIndex),
                                    _block)) {
                _sfile._DoError ("Next: level-1 map read failed");
                return 0;
            }
            _blockIter.Reset ();
            rec_no = _blockIter.Next();
        }
    }
    else
        rec_no = _blockIter.Next();
    long slot_size = _sfile._slotSize;
    ret_val = __MakeHandle (rec_no, _blockIndex);
    if (_sfile._file.Read (_sfile._RecordOffset (ret_val), record,
                           slot_size) != 
        slot_size) {
        _sfile._DoError ("Next: record read failed", ret_val);
        return 0;
    }
    return ret_val;
}






