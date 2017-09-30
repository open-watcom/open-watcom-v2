



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



#include "base/bytstrng.h"
#include "io/bytstore.h"


/* ------------------------------------------------------------------ */

// The data  structure  used is a   SlottedFile,  whose slots  are of  size
// CHUNK_SIZE. Each ByteString written into the SlottedFile is written in a
// linked list of  slots,  which we  will  call  the ByteString list.   The
// collection  of all ByteString    lists in  the  SlottedFile are   linked
// together into   a doubly-linked list,  which we  will  call the iterator
// list.  The first slot of the ByteString list contains  the handle of the
// next slot (zero if no other slots), the actual length of the ByteString,
// the handles of the first slots of the next and previous ByteString lists
// in  the   iterator  list,   followed    by   the  actual  data   of  the
// ByteString. Each subsequent  slot  of the  ByteString list contains  the
// handle of the next slot in the list, followed by the ByteString's data.
// 
// The iterator  list is used  to support  efficient  iteration through the
// ByteStrings  stored  in the file.    This necessitates the  head of  the
// iterator list to be stored in the user-header area of the slotted file.


#define CHUNK_SIZE 64 // Must be at least 20


struct PrimarySlot {
    CL_SlottedFileHandle next; // First secondary slot
    long                 byteStringSize;
    CL_SlottedFileHandle iter_next, iter_prev;
    uchar                data[1];
    long DataOffset () const
        {return (long) ((uchar*) &data - (uchar*) &next);};
};

struct SecondarySlot {
    CL_SlottedFileHandle next; // Next secondary slot
    uchar                data[1];
    long DataOffset () const
        {return (long) ((uchar*) &data - (uchar*) &next);};
};


struct HeaderForSlottedFile {
    CL_SlottedFileHandle iter_list_head;
    CL_SlottedFileHandle user_header;
};



CL_ByteStringStore::CL_ByteStringStore (const char* path, bool create, bool
                                      report_errors)
{
    if (create) {
        HeaderForSlottedFile hdr;
        hdr.iter_list_head = 0L;
        _file = new CL_PrivateSlottedFile
            (path, CHUNK_SIZE, sizeof (HeaderForSlottedFile), report_errors);
        if (_file) {
            // Create a dummy header string
            CL_ByteString dummy (12);
            hdr.user_header = Add (dummy);
            _file->WriteHeader ((uchar*) &hdr);
        }
    }
    else
        _file = new CL_PrivateSlottedFile (path, report_errors);
}



/* ------------------------------------------------------------------ */


CL_ByteStringStore::~CL_ByteStringStore ()
{
    if (_file)
        delete _file;
}



/* ------------------------------------------------------------------ */



    
// Add the given array of bytes into the store, and return the
// associated handle. Return 0 on failure.


CL_SlottedFileHandle CL_ByteStringStore::Add (const CL_ByteArray& data)
{
    CL_ByteString slot_data (CHUNK_SIZE);
    long data_size = data.Size();
    PrimarySlot* p = (PrimarySlot*) slot_data.AsPtr ();
    long offset = p->DataOffset();

    CL_SlottedFileHandle inext, iprev, ihead; // Iterator list ptrs
    HeaderForSlottedFile hdr;
    
    long used = minl (CHUNK_SIZE - offset, data_size);
    p->byteStringSize = data_size;
    p->next = (data_size - used > 0)
        ? _Add (data.AsPtr() + used, data_size - used)
        : 0L;
    slot_data.CopyFrom (data.AsPtr(), used, p->DataOffset());

    if (!_file->ReadHeader ((uchar*) &hdr))
        return 0;
    ihead = hdr.iter_list_head;
    CL_SlottedFileHandle h = _file->AllocateSlot ();

    // Insert the new string at the head of the iterator list: first change
    // the list head pointer, in the file header
    CL_SlottedFileHandle next = p->iter_next = ihead;
    p->iter_prev = 0;
    hdr.iter_list_head = ihead = h;
    if (!_file->ModifyRecord (h, slot_data) ||
        !_file->WriteHeader ((uchar*) &hdr))
        return 0;

    // Now set the previous pointer of the next string in the iterator list
    if (next) {
        if (!_file->RetrieveRecord (next, slot_data))
            return 0;
        p->iter_prev = h;
        if (!_file->ModifyRecord (next, slot_data))
            return 0;
    }
    return h;
}





/* ------------------------------------------------------------------ */
CL_SlottedFileHandle CL_ByteStringStore::Allocate ()
{
    CL_ByteString s (10); // Small byte string
    return Add (s);
}





/* ------------------------------------------------------------------ */


// Remove the byte string associated with the given handle. Return
// TRUE on success, FALSE if either the handle was invalid or an I/O
// error occurred.
bool CL_ByteStringStore::Remove (CL_SlottedFileHandle h)
{
    CL_ByteString slot (CHUNK_SIZE), tmp_slot (CHUNK_SIZE);
    // Retrieve the primary slot
    if (!_file->RetrieveRecord (h, slot))
        return FALSE;
    // Remove it from the iterator list
    CL_SlottedFileHandle inext, iprev;
    PrimarySlot* p = (PrimarySlot*) slot.AsPtr();
    PrimarySlot* q = (PrimarySlot*) tmp_slot.AsPtr();
    inext = p->iter_next;
    iprev = p->iter_prev;
    if (inext) {
        if (!_file->RetrieveRecord (inext, tmp_slot))
            return FALSE;
        q->iter_prev = iprev;
        if (!_file->ModifyRecord (inext, tmp_slot))
            return FALSE;
    }
    if (iprev) {
        if (!_file->RetrieveRecord (iprev, tmp_slot))
            return FALSE;
        q->iter_next = inext;
        if (!_file->ModifyRecord (iprev, tmp_slot))
            return FALSE;
    }
    else {
        // No previous element, this must be the head of the iterator list
        HeaderForSlottedFile hdr;
        if (!_file->ReadHeader ((uchar*) &hdr))
            return FALSE;
        hdr.iter_list_head = p->iter_next; 
        if (!_file->WriteHeader ((uchar*) &hdr))
            return FALSE;
    }
    return _Remove (p->next) && _file->DeleteRecord (h);
}


/* ------------------------------------------------------------------ */


// Retrieve the byte string associated with the given handle (first
// parameter), and return it in the second. Return TRUE as the
// function value if successful, FALSE on I/O error or invalid handle.
bool CL_ByteStringStore::Retrieve  (CL_SlottedFileHandle h, CL_ByteString&
                                   value) const
{
    CL_SlottedFileHandle next;
    
    return _Retrieve (h, value, next);
}



/* ------------------------------------------------------------------ */

// Modify the byte string associated with the given handle (first
// parameter) to be equal to the second. Note that the length of the
// second parameter need not bear any relationship to the length of
// the string currently associated with the given handle.
//     Return TRUE as the function value if successful, FALSE on I/O
// error or invalid handle.
bool CL_ByteStringStore::Modify (CL_SlottedFileHandle h, const CL_ByteArray& b)
{
    CL_ByteString slot (CHUNK_SIZE);
    CL_SlottedFileHandle current = h;
    // Read the first chunk
    if (!_file->RetrieveRecord (h, slot.AsPtr()))
        return FALSE;
    PrimarySlot* p = (PrimarySlot*) slot.AsPtr();
    long i = 0; // Count of #bytes in b that have been written out
    long new_size = b.Size();
    p->byteStringSize = new_size;
    long offset = p->DataOffset();
    uchar* data = b.AsPtr ();
    SecondarySlot* q = (SecondarySlot*) slot.AsPtr(); 
    while (i < new_size) {
        long used = minl (CHUNK_SIZE - offset, new_size - i);
        slot.CopyFrom (data, used, offset);
        data += used;
        i += used;
    if (p->next == 0) break;
        if (!_file->ModifyRecord (current, slot))
            return 0L;
        current = p->next;
        offset = q->DataOffset();
        if (!_file->RetrieveRecord (current, slot))
            return FALSE;
    }
    if (i < new_size) {
        // We've hit the end of the old list
        q->next = _Add (data, new_size - i);
    }
    else {
        // We've run out of data in the parameter array
        _Remove (q->next);
        q->next = 0L;
    }
    if (!_file->ModifyRecord (current, slot))
        return FALSE;
    return TRUE;
}




/* ------------------------------------------------------------------ */





bool CL_ByteStringStore::ReadHeader (CL_ByteString& header) const
{
    HeaderForSlottedFile hdr;
    if (_file) {
        _file->ReadHeader ((uchar*) &hdr);
        return Retrieve (hdr.user_header, header);
    }
    return FALSE;
}



/* ------------------------------------------------------------------ */


// Write the parameter into the user-defined header. Returns
// TRUE on success, FALSE on i/o error.
bool CL_ByteStringStore::WriteHeader (const CL_ByteString& header)
{
    HeaderForSlottedFile hdr;
    if (_file) {
        _file->ReadHeader ((uchar*) &hdr);
        return Modify (hdr.user_header, header);
    }
    return FALSE;
}





/* ------------------------------------------------------------------ */



// Protected methods:


/* ------------------------------------------------------------------ */


CL_SlottedFileHandle CL_ByteStringStore::_Add (uchar* data, long data_size)
{
    if (data_size <= 0)
        return 0;
    CL_ByteString slot_data (CHUNK_SIZE);
    SecondarySlot* p = (SecondarySlot*) slot_data.AsPtr();
    CL_SlottedFileHandle ret_val = _file->AllocateSlot();
    long i = 0;
    long available = CHUNK_SIZE - p->DataOffset();
    CL_SlottedFileHandle current = ret_val;
    while (i < data_size) {
        long leftover_data = data_size - i;
        long used;
        CL_SlottedFileHandle next;
        if (leftover_data > available) {
            used = available;
            next = _file->AllocateSlot();
        }
        else {
            used = leftover_data;
            next = 0L;
        }
        p->next = next;
        slot_data.CopyFrom (data, used, p->DataOffset());
        if (!_file->ModifyRecord (current, slot_data))
            return 0L;
        current = next;
        data += used;
        i += used;
    }
    return ret_val;
}


/* ------------------------------------------------------------------ */

bool CL_ByteStringStore::_Remove (CL_SlottedFileHandle h)
{
    CL_ByteString slot (CHUNK_SIZE);
    SecondarySlot* p = (SecondarySlot*) slot.AsPtr();
    while (h != 0L) {
        if (!_file->RetrieveRecord (h, slot))
            return FALSE;
        if (!_file->DeleteRecord (h))
            return FALSE;
        h = p->next;
    }
    return TRUE;
}



/* ------------------------------------------------------------------ */

bool CL_ByteStringStore::_Retrieve  (CL_SlottedFileHandle h,
                                    CL_ByteString& value,
                                    CL_SlottedFileHandle& itr_next) const
{
    CL_ByteString slot (CHUNK_SIZE);
    PrimarySlot* p = (PrimarySlot*) slot.AsPtr();

    // Read and copy the first chunk
    if (!_file->RetrieveRecord (h, slot.AsPtr()))
        return FALSE;
    long size = p->byteStringSize;
    if (!value.ChangeSize (size))
        return FALSE;
    itr_next = p->iter_next;
    h = p->next;

    long count = minl (CHUNK_SIZE - p->DataOffset(), size);
    value.CopyFrom ((uchar*) &p->data, count);


    // Now read the remaining (secondary) chunks, if any:
    SecondarySlot* q = (SecondarySlot*) slot.AsPtr();
    uchar* out = value.AsPtr () + count;
    CL_ByteArray in (q->data, CHUNK_SIZE - q->DataOffset());
    while (h != 0L && count < size) {
        if (!_file->RetrieveRecord (h, slot.AsPtr()))
            return FALSE;
        // Compute #usable bytes in this chunk:
        long usable  = minl (size - count,  in.Size());
        // Copy the data segment over
        in.CopyTo (out, usable);
        count += usable;
        out += usable;
        h = q->next;
    }
#ifdef ERROR_CHECKING
    if (i < size || h != 0L)
        CL_Error::Warning ("CL_ByteStringStore::_Retrieve: internal error:"
                           " length mismatch");
#endif
    return TRUE;
}






CL_ByteStoreIterator::CL_ByteStoreIterator (const CL_ByteStringStore& s)
: _store (s)
{
}
CL_ByteStoreIterator::CL_ByteStoreIterator (const CL_ByteStoreIterator&
                                            itr)
: _store (itr._store)
{
}





/* ------------------------------------------------------------------ */

void CL_ByteStoreIterator::Reset ()
{
    HeaderForSlottedFile hdr;
    if (_store._file) {
        _store._file->ReadHeader ((uchar*) &hdr);
        _next = hdr.iter_list_head;
    }
    else
        _next = 0;
}



/* ------------------------------------------------------------------ */

CL_SlottedFileHandle CL_ByteStoreIterator::Next (CL_ByteString& b)
{
    if (_next == 0 || !_store._file)
        return 0;
    CL_SlottedFileHandle h = _next;
    if (!_store._Retrieve (h, b, _next))
        return FALSE;
    return h;
}


