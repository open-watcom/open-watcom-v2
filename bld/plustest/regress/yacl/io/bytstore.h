

#ifndef _bytstore_h_
#define _bytstore_h_


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



// This  is  ByteStringStore, a   class that  encapsulates a  facility  for
// storing byte strings  of arbitrary sizes into a  file, via a handle, and
// retrieving them for subsequent use. A user  of this class  can add a new
// byte string to the store, thus obtaining an associated handle (using the
// Add method), and later  retrieve the byte   string by specifying  that
// handle (using the Retrieve method). Byte strings currently stored in the
// store may be modified via  the Modify method; this modification includes
// changing either the data or the length (or both) of the byte string.
// 
// Byte strings may be removed from  the store via  the Remove method; this
// renders the associated handle invalid.
// 
// This class  also provides two methods,  ReadHeader  and WriteHeader, for
// manipulating a "user-defined" header. A  client of this object may store
// client-specific header information in this header.  Note that the header
// need not be of fixed length.




#include "base/bytstrng.h"
#include "io/slotfile.h"

class CL_ByteStringStore: public CL_Object {


public:

    // ----------------------- Constructors/destructors -----------------

    CL_ByteStringStore (const char* path, bool create = FALSE, bool
                       report_errors = TRUE);
    // Construct a CL_ByteStringStore on the given file. If {\tt create}
    // is TRUE, create a new file with the given name, overwriting any
    // existing one. If {\tt report_errors} is TRUE, then issue error
    // messages through {\tt CL_Error} when i/o errors occur.

    ~CL_ByteStringStore();
    // Destructor: close the ByteStringStore.

    // ------------------------ Storage and retrieval -------------------

    virtual CL_SlottedFileHandle Allocate ();
    // Allocate a handle at which a new ByteString can be stored, and
    // return it.
    
    virtual CL_SlottedFileHandle Add (const CL_ByteArray& data);
    // Add the given array of bytes into the store, and return the
    // associated handle. Return 0 on failure (e.g., due to I/O error).

    virtual bool Remove (CL_SlottedFileHandle h);
    // Remove the byte string associated with the given handle. Return
    // TRUE on success, FALSE if either the handle was invalid or an I/O
    // error occurred.

    virtual bool Retrieve  (CL_SlottedFileHandle h,
                            CL_ByteString& value) const;
    // Retrieve the byte string associated with the given handle (first
    // parameter), and return it in the second. Return TRUE as the
    // function value if successful, FALSE on I/O error or invalid handle.

    virtual bool Modify (CL_SlottedFileHandle h, const CL_ByteArray& b);
    // Modify the byte string associated with the given handle (first
    // parameter) to be equal to the second parameter. Note that the
    // length of the second parameter need not bear any relationship to
    // the length of the string currently associated with the given handle.
    //     Return TRUE as the function value if successful, FALSE on I/O
    // error or invalid handle.


    // ------------------- Manipulating user headers ------------------

    virtual bool ReadHeader (CL_ByteString& header) const;
    // Read and return the user-defined header in the file. Returns
    // TRUE on success, FALSE on i/o error.

    virtual bool WriteHeader (const CL_ByteString& header);
    // Write the parameter into the user-defined header. Returns
    // TRUE on success, FALSE on i/o error.


    // ---------------------- Basic methods ----------------------------

    const char* ClassName () const { return "CL_ByteStringStore";};

    // ---------------------- End public protocol -------------------

protected:
    CL_SlottedFileHandle _Add    (uchar* data, long data_size);
    
    bool                 _Remove (CL_SlottedFileHandle);
    
    bool                 _Retrieve  (CL_SlottedFileHandle h,
                                     CL_ByteString& value,
                                     CL_SlottedFileHandle& itr_next) const;
    

    class CL_SlottedFile* _file;
    friend class CL_ByteStoreIterator;
    
};



class CL_ByteStoreIterator {

public:
    CL_ByteStoreIterator (const CL_ByteStringStore&);
    // Constructor.

    CL_ByteStoreIterator (const CL_ByteStoreIterator&);
    // Copy constructor.

    // -------------------- Iteration methods -------------------------

    virtual void Reset ();
    // The Reset and Next methods retrieve the ByteStrings in the file, in
    // no particular order. The Reset method begins the iteration.

    virtual CL_SlottedFileHandle Next (CL_ByteString& value);
    // Return the next ByteString in sequence, in the parameter, and
    // return its handle as the function value. Return 0 when there are no
    // more ByteStrings.


protected:
    CL_SlottedFileHandle      _next;
    const CL_ByteStringStore& _store;
    
};


#endif
