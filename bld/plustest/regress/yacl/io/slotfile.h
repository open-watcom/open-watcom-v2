 
#ifndef _slotfile_h_
#define _slotfile_h_
 


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

 

// This is  a binary  file that manages   fixed-length records, called {\it
// slots.} Each slot contains  a fixed-length byte  string, whose length is
// determined  the  first  time  the  file  is  created.  Also, each   slot
// associated with a distinct file-specific {\it  handle\/} (a long value).
// {\tt SlottedFile} is an abstract class, and particular concrete versions
// of it may be constructed; {\tt PrivateSlottedFile} is one such.
// 
// The SlottedFile class provides for adding  and deleting records, as well
// as storing and retrieving the associated  data.  When a record is added,
// a file-specific  handle is  returned which  can  be used for  subsequent
// reference to that record. The returned handle is always nonzero.
//
// The class also provides methods for access and modification of a
// header area in the file  where a client of this class may maintain
// data. The size of this header area is determined by the derived
// class, such as {\tt PrivateSlottedFile}.
//
// A    user  of this   object  may   delete  a  record,   thus freeing the
// corresponding slot;  when this is done,  the  handle associated with the
// slot is invalid, but this does not affect the handles of other allocated
// slots. Once a slot  is freed, however,  it is illegal  to attempt to use
// its handle.
// 
// \paragraph {Implementation notes.}
// The implementation    uses a 2-level   bitmap-based  representation, and
// therefore  limits  the total  number of  records  that a SlottedFile can
// contain to  about   768,000,000. This   representation   is designed  to
// minimize I/O  on  the stream; it  guarantees  that every  method of this
// class requires  at most two  read and three write  operations (including
// read/write of  the record itself), regardless  of file  size.  For small
// files (with  fewer than 4096 records),  each method requires at most one
// read and two write operations. This class is  therefore  well suited for
// storing large tables in relational databases.
//
//    The implementation also  ensures  that the  file occupies  as  little
// additional  space  as  possible, by  truncating   the  file  after every
// deletion so that there is no unused space at its end.

#include "io/binfile.h"
#include "base/bitset.h"

 
typedef ulong CL_SlottedFileHandle;
class CL_SlottedFileHeader;

class CL_SlottedFile: public CL_Object {
 
public:
    
    CL_SlottedFile (CL_Stream& data_store, bool report_errors = TRUE);
    // Constructor: specifies the stream to use. This stream must
    // exist at the time this constructor is executed. The second
    // parameter specifies whether errors should be reported via the
    // {\tt CL_Error} class.

    ~CL_SlottedFile ();
    

    // ---------------------- Querying --------------------------
    
    virtual long SlotSize () const
        {return _slotSize;};
    // Return the slot size for this slotted file, in bytes.
 
    virtual bool IsValid (CL_SlottedFileHandle h) const;
    // Is the given handle a valid (allocated) one?

    virtual long SlotsAllocated () const;
    // Return the total number of slots currently allocated.

    short HeaderSize () const {return _userHeaderSize;};
    // Return the size (in bytes) of the user-defined header segment.
    
    // --------------------- Slot manipulation --------------------

    virtual CL_SlottedFileHandle AllocateSlot ();
    // Allocate a slot, and return its handle. The contents of the slot
    // are undefined.
    
    virtual CL_SlottedFileHandle AddRecord (uchar* record);
    // Allocate a slot, copy the given record into the slot, and return
    // the handle for the slot. Return a zero handle on error, e.g. I/O error
    // or unavailability of slots. 

    virtual bool RetrieveRecord (CL_SlottedFileHandle handle,
                                 uchar* record) const;
    // Retrieve the record at the given handle, and return it in the second
    // parameter; the latter is assumed to point to a byte block of size
    // at least as large as the record length. The function returns TRUE on
    // success and FALSE if an invalid handle was specified or an I/O error
    // occurred.

    virtual bool ModifyRecord (CL_SlottedFileHandle handle, uchar* record);
    // Overwrite the record at the given handle with the value specified in
    // the second parameter; the latter is assumed to point to a byte block
    // of size at least as large as the record length. The function returns
    // TRUE on success and FALSE if an invalid handle was specified or an
    // I/O error occurred.
    
    virtual bool DeleteRecord (CL_SlottedFileHandle handle);
    // Delete the record at the given handle. The handle must be currently
    // allocated. Return TRUE on success, FALSE on error (e.g., I/O error or
    // invalid handle). After the deletion, the handle becomes available for
    // allocation to a subsequent {\tt AddRecord} or {\tt AllocateSlot}
    // request.

    // ------------------- Manipulating user headers ------------------

    virtual bool ReadHeader (uchar* header) const;
    // Read and return the user-defined header in the file. The
    // parameter must point to a large-enough segment of memory. Returns
    // TRUE on success, FALSE on i/o error.

    virtual bool WriteHeader (uchar* header);
    // Write the parameter into the user-defined header. Returns
    // TRUE on success, FALSE on i/o error.

    // ------------------------ Iteration ---------------------------
    
    virtual CL_SlottedFileHandle FirstRecord (uchar* record) const;
    // Return the first record in series, and its handle.

    virtual CL_SlottedFileHandle NextRecord (CL_SlottedFileHandle h,
                                             uchar* record) const;
    // Return the next record in series after the one with the given handle,
    // in the parameter, and
    // return its handle as the function value. Return 0 when there are no
    // more records. 

    // ------------------------ Static functions -----------------------

    static long Valid (const char* path);
    // Determine whether the given path specifies a valid slotted file,
    // and if so, return its slot size; if not, return 0.
    

    // ------------------ Basic functions ------------------------------
    
    const char* ClassName () const {return "CL_SlottedFile";};

    CL_ClassId ClassId () const { return _CL_SlottedFile_CLASSID;};

    // ------------------- End public protocol ------------------------ 
 

protected:

    // ----- Instance variables -------
    
    CL_SlottedFileHeader* _header;
    CL_Stream&            _file;           // The file we use
    long                  _slotSize;       // Slot size: set by derived class
    short                 _userHeaderSize; // User-defined header's size:
                                           // set by derived class
    bool                  _reportErrors;

    
    // ------ Protected methods ----
    
    CL_Offset             _BlockBitmapOffset (ushort index) const;

    CL_Offset             _RecordOffset (CL_SlottedFileHandle) const;

    CL_SlottedFileHandle  _GetSlot();

    void                  _DoError (const char*,
                                    CL_SlottedFileHandle h = 0) const;

    virtual bool         _ReadHeader  (CL_SlottedFileHeader& hdr) const = 0;
    // Read and write the header: for derived classes to override. These are
    // const methods because they are used in most methods, but do not
    // affect the abstract object's state, since they are only used for
    // bookkeeping.

    virtual bool         _WriteHeader (const CL_SlottedFileHeader&) const = 0;
    
    friend class CL_SlottedFileIterator;
};



// SlottedFileIterator allows iteration over the given file.

class CL_SlottedFileIterator {

public:
    CL_SlottedFileIterator (const CL_SlottedFile& file);
    // Constructor: tell which SlottedFile to inspect.

    CL_SlottedFileIterator (const CL_SlottedFileIterator& iter);
    // Copy constructor.

    ~CL_SlottedFileIterator ();
    // Destructor.
    
    virtual void Reset ();
    // Reset the iteration to the beginning of the file.

    virtual CL_SlottedFileHandle Next (uchar* record);
    // Return the next record in sequence, in the parameter, and
    // return its handle as the function value. Return 0 when there are no
    // more records. The records are returned in order of their occurrence
    // in the file.

protected:
    CL_BitSet               _root, _block;
    CL_BitSetIterator       _rootIter, _blockIter;
    short                   _blockIndex;
    const CL_SlottedFile&   _sfile;
    CL_SlottedFileHeader*   _header;

};



// The PrivateSlottedFile is an implementation of the SlottedFile that
// assumes that the file will be used exclusively by one process and not
// shared by multiple processes.

class CL_PrivateSlottedFile: public CL_SlottedFile {

public:
    // ---------------- Construction and destruction -------------------
    
    CL_PrivateSlottedFile (const char* path, bool report_errors = TRUE);
    // Open an existing slotted file with the given path name. This
    // constructor exits with a fatal error if either the file does not
    // exist or does not contain the correct headers; therefore, it is
    // advisable to use the {\tt Valid()} method before invoking it.
    //
    //     The second parameter specifies what to to do in case of I/O
    // errors. The default is to use the {\tt CL_Error::Warning} call to issue
    // error messages; if reportErrors is FALSE, such warnings will not
    // be issued.
 
    CL_PrivateSlottedFile (const char* path, long record_size,
                           short user_header_size = 0,
                           bool report_errors = TRUE);
    // Create a new slotted file with specified path name and
    // record size. Overwrite any existing file with the same path name.
    // The third parameter specifies the size of the user header area to
    // be set apart in the file.

    CL_PrivateSlottedFile (CL_Stream& file, bool report_errors = TRUE);
    // Alternate constructors: similar to the above, except that the given
    // stream is used as data repository; this stream is {\it not\/}
    // owned by the SlottedFile, and will not be destroyed by the
    // SlottedFile's destructor.
 
    CL_PrivateSlottedFile (CL_Stream& file, long record_size,
                           short user_header_size = 0,
                           bool report_errors = TRUE);

    virtual ~CL_PrivateSlottedFile();
    // Destructor.
 

protected:
    CL_BinaryFile*        _ourFile; // Created by the first two constructors
    bool                  _weOwnIt; // TRUE if one of the first two
                                    // constructors was used

    bool _ReadHeader  (CL_SlottedFileHeader&) const;

    bool _WriteHeader (const CL_SlottedFileHeader&) const;

};




// class CL_SharedSlottedFile:  public CL_SlottedFile {
// 
// public:
//     CL_SharedSlottedFile () {NotImplemented ("SharedSlottedFile");};
// 
//     ~CL_SharedSlottedFile ();
// 
//     char* ClassName () const {return "CL_SharedSlottedFile";};
//     
// protected:
//     bool _ReadHeader  () const;
// 
//     bool _WriteHeader () const;
// 
// };

    
#endif
