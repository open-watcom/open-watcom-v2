#ifndef _BINFILE_H_
#define _BINFILE_H_




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


// This is an abstract interface to a binary file, essentially a wrapper around
// the platform's file system calls. The strategy for deciding when to
// open and close the file is determined by the implementation of this
// class, and cannot be relied upon by the user of this class.
//
// The ``core'' methods for reading and writing unsigned char arrays and
// for seeking to a given position, seeking to end of file and changing
// file size are declared virtual; the default implementations for all
// other methods are provided in terms of the core methods. Thus a
// derived class need only override the core methods.


#include "base/string.h"
#include "base/bytstrng.h"
#include "base/stream.h"


class CL_BinaryFile: public CL_Stream {
 
public:
 
    CL_BinaryFile (const char* pathName, bool create = FALSE);
    // Constructor: instantiate a BinaryFile object with the given path
    // name. If the second parameter is TRUE, create the file, overwriting
    // any existing one. If it is FALSE, and the file does not exist,
    // issue a runtime error.

    ~CL_BinaryFile();
    // Destructor.

    // ----------------- Error state methods ---------------------

    inline bool Valid() const;
    // Tell whether the state of the file is ok.

    CL_String ErrorString () const;
    // Return the (platform-specific) error message string associated
    // with the most recent error.
    

    // ----------------- Read operations -------------------------

    virtual long Read (uchar* buffer, long num_bytes) const; 
    // Read from current position. Returns number of bytes read, 0 on eof, -1
    // on error.
 
    bool Read (CL_Object& obj) const
        {return CL_Stream::Read (obj);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    bool Read (CL_ObjectPtr& obj) const
        {return CL_Stream::Read (obj);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    // ----------------- Write operations ------------------------

    virtual bool Write (uchar* buffer, long num_bytes);
    // Write at current position.

    bool Write (const CL_Object& p)
        {return CL_Stream::Write (p);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    bool Write (CL_ObjectPtr p)
        {return CL_Stream::Write (p);};
    // Override the inherited method; forward the call to parent class.
    // This is to prevent the "hides virtual" warning.
    
    
    // ----------------- Seek operations -------------------------
    
    virtual bool SeekTo (CL_Offset position) const;
    // Change the current position. Returns TRUE on success.
 
    virtual bool SeekToEnd () const;

    virtual bool SeekRelative (long change) const;
    
    virtual bool ChangeSize (CL_Offset size);
    // Change the size of file

    // ---------------------- Querying ----------------------

    virtual bool Eof () const;
    // Are we at the end of the file?
    
    virtual long Offset () const;
    // Return the current position
    
    CL_String FileName() const;
    // Return the pathname of the file (the name it was initialized with)
    
    virtual long Size () const;
    // Return the size of the file in bytes
 
    // ------------------ Static functions ------------------------------

    static bool Exists (const char *pathName);
    // Return 1 if the file with given name exists, 0 if not.
 
    static bool Create (const char* pathName);     
    // Create the file; truncate any existing file with
    // the same name. Return TRUE on success, FALSE on failure.

    // ------------------ Basic functions ------------------------------
    
    const char* ClassName() const {return "CL_BinaryFile";};

    CL_ClassId ClassId() const { return _CL_BinaryFile_CLASSID;};

    // ------------------- End public protocol ------------------------ 
 
protected:
 
    //  Instance variables:

    CL_String    _pathName;     // Path name
    int          _fd;           // The file descriptor
    CL_String    _errorString;

    // Protected methods:
    
    virtual bool _Open ();

    virtual void _Close ();

    void         _MakeErrorString (const char*) const;

};
 
 


inline bool CL_BinaryFile::Valid() const
{
    return _errorString.Size() == 0;
}





inline CL_String CL_BinaryFile::FileName () const
{
    return _pathName;
}


#endif
