

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




#ifdef __BORLANDC__
#include <io.h>
#pragma warn -ncf
#endif
#ifdef __WATCOMC__
#include <unistd.h>
#endif

#if defined(__GNUC__)
extern "C" {
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __GNUC__
}
#endif

#if defined(__DEC_ULTRIX__)
extern "C" int ftruncate (int, int);
#endif

// #ifdef WINDOWS
// #include <windows.h>
// #endif
 
#include "io/binfile.h"
#include "base/error.h"
 

#define MAX_LINE_LENGTH 512





CL_BinaryFile::CL_BinaryFile (const char *pathname, bool createFlag)
    : _pathName (pathname)
{
    struct stat st_buf;

    _fd = -1;
    if (createFlag && !Create(_pathName)) {
        _MakeErrorString ("File: create call failed");
        return;
    }
    else if (! Exists (_pathName)) {
        _MakeErrorString ("File: file does not exist");
        return;
    }

    if (stat (_pathName, &st_buf) < 0)  {
        _MakeErrorString ("File: stat failed");
        return;
    }
    // #ifndef MS_WINDOWS
    _Open ();
    // #endif
} 
 
 
CL_BinaryFile::~CL_BinaryFile()
{
    _Close();
}
 
 
 
bool CL_BinaryFile::Create (const char* pathName)
{
    int fd;

    // #ifndef MS_WINDOWS
    if ((fd = creat (pathName, S_IREAD|S_IWRITE)) <= 0) {
        return FALSE;
    }
    close (fd);
// #else
//     OFSTRUCT    OfStruct;
//  
//     int mode =   OF_READWRITE | OF_CREATE;   //This will truncate the file
//     fd = OpenFile (_pathName, &OfStruct, mode);
//     _lclose (fd);
// #endif
    return TRUE;
}
 

long CL_BinaryFile::Size () const
{
// #ifdef __BORLANDC__
    long current = lseek (_fd, 0L, SEEK_CUR);
    if (current == -1)
        return -1;
    long size = lseek  (_fd, 0L, SEEK_END);
    if (size == -1)
        return -1;
    lseek (_fd, current, SEEK_SET);
    return size;
// #else
//     struct stat st_buf;
//     _MakeErrorString ("");
//     if (stat (_pathName, &st_buf) < 0)  {
//         _MakeErrorString ("Size: stat failed");
//         return -1;
//     }
//     unsigned long fileSize =  st_buf.st_size;
//     return fileSize;
// #endif
}
 

#if defined(__UNIX__)
#define CHANGESIZE ftruncate
#elif defined(__DOS__) || defined(__OS2__) || defined(__MS_WINDOWS__)
#define CHANGESIZE chsize
#endif

bool CL_BinaryFile::ChangeSize (long size)
{
    if (!PrepareToChange())
        return FALSE;
// #ifdef MS_WINDOWS
//     Open ();
// #endif
    _MakeErrorString ("");
    
    if (CHANGESIZE (_fd, size) == -1) {
        _MakeErrorString ("ChangeSize: chsize call failed");
        return FALSE;
    }
// #ifdef MS_WINDOWS
//     Close();
// #endif
    Notify ();
    return TRUE;
}

 
 
long CL_BinaryFile::Read (uchar *buffer, long num_bytes)  const
{
    int nn;

    _MakeErrorString ("");
// #ifdef MS_WINDOWS
//     Open();
// #endif
    
    // #ifndef MS_WINDOWS
    if ((nn = read (_fd, buffer, num_bytes)) < 0) {
// #else
//     if ((nn = _lread (fd, buffer, num_bytes)) < 0) {
// #endif
        _MakeErrorString ("Read: read call failed");
    }
// #ifdef MS_WINDOWS
//     Close();
// #endif
    return nn;
}
 


    
 
bool CL_BinaryFile::Write (uchar *buffer, long num_bytes)
{

// #ifdef MS_WINDOWS
//     Open();
// #endif

    if (!PrepareToChange())
        return FALSE;
    _MakeErrorString ("");
    // #ifndef MS_WINDOWS
    if ((write (_fd, buffer, num_bytes)) < 0) {
        _MakeErrorString ("Write: write call failed");
        return FALSE;
    }
// #else
//     if ((_lwrite (fd, buffer, num_bytes)) < 0) {
//         CL_Error::Fatal ("CL_BinaryFile::Write: write failed: %s",
//             strerror (errno));
//     }
// #endif
// #ifdef MS_WINDOWS
//     Close();
// #endif
    Notify ();
    return TRUE;

}
 
 


bool CL_BinaryFile::SeekTo (long position) const
{
    int mode;

    _MakeErrorString ("");
// #ifdef MS_WINDOWS
//     if ((retval = _llseek (fd, position, SEEK_SET)) < 0) {
// #else
    if (lseek (_fd, position, SEEK_SET) == -1L) {
        // #endif
        _MakeErrorString ("SeekTo: lseek call failed");
        return FALSE;
    }
    return TRUE;
}
 

bool CL_BinaryFile::SeekRelative (long position) const
{
    int mode;

    _MakeErrorString ("");
// #ifdef MS_WINDOWS
//     if ((retval = _llseek (fd, position, SEEK_CUR)) < 0) {
// #else
    if (lseek (_fd, position, SEEK_CUR) == -1L) {
        // #endif
        _MakeErrorString ("SeekRelative: lseek call failed");
        return FALSE;
    }
    return TRUE;
}
 

bool CL_BinaryFile::Eof () const
{
#if defined(__UNIX__)
    long current = lseek (_fd, 0L, SEEK_CUR);
    if (current == -1)
        return -1;
    long size = lseek  (_fd, 0L, SEEK_END);
    if (size == -1)
        return -1;
    lseek (_fd, current, SEEK_SET);
    return size == current;
#elif defined(__DOS__) || defined(__OS2__) || defined(__MS_WINDOWS__)
    return eof (_fd);
#endif
}


long CL_BinaryFile::Offset () const
{
    return lseek (_fd, 0, SEEK_CUR);
}

 
bool CL_BinaryFile::Exists (const char *pathname)
{
    return (access (pathname, 0) == 0);
}  
 
 
 

// Protected methods
 
bool CL_BinaryFile::_Open()
{
    _MakeErrorString ("");
    // #ifndef MS_WINDOWS
    int mode = O_RDWR;
    
#if defined(__DOS__) || defined(__MS_WINDOWS__) || defined(__OS2__)
    mode |= O_BINARY;
#endif
    _fd = open (_pathName.AsPtr(), mode, S_IREAD | S_IWRITE);
// #else
//     OFSTRUCT    OfStruct;
// 
//     int mode =   OF_READWRITE;
//     fd = OpenFile (_pathName.AsPtr(), &OfStruct, mode);
// #endif
    if (_fd < 0) 
        _MakeErrorString ("_Open: open call failed");
    return (_fd >= 0);
}  // End of function Open
 
 
void CL_BinaryFile::_Close()
{

    if (_fd < 0)
        return;
// #ifndef MS_WINDOWS
    close (_fd);
// #else
//     _lclose (fd);
// #endif
    _fd = -1;
}  // End of function Close




bool CL_BinaryFile::SeekToEnd () const
{
    int mode;
    long retval;

    _MakeErrorString ("");
// #ifdef MS_WINDOWS
//     if ((retval = _llseek (fd, 0L, SEEK_END)) < 0) {
// #else
    if ((retval = lseek (_fd, 0L, SEEK_END)) < 0) {
        // #endif
        _MakeErrorString  ("SeekToEnd: seek call failed");
    }
    return retval >= 0;
}



CL_String CL_BinaryFile::ErrorString() const
{
    return _errorString;
}




void CL_BinaryFile::_MakeErrorString (const char* msg) const
{
    if (CL_String (msg).Length() > 0) {
        ((CL_BinaryFile*) this)->_errorString.AssignWithFormat
            ("File %s: %s: %s", _pathName.AsPtr(), msg, strerror (errno));
        // Cast away const
        CL_Error::Warning (_errorString);
    }
    else
        ((CL_BinaryFile*) this)->_errorString = "";
        
}


#ifdef __BORLANDC__
#pragma warn .ncf
#endif
