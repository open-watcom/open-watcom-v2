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


#include <iostream.h>

#ifndef _CVERROR_H_INCLUDED
#define _CVERROR_H_INCLUDED

class CVpackError {

    public :

        virtual void ErrorPrint() const;
};

class RetrieveError : public CVpackError {

    public :

        virtual void ErrorPrint() const;
};

class WriteError : public CVpackError {

    public :

        WriteError(const char* fileName) : _fileName(fileName) { }
        ~WriteError() { }
        virtual void ErrorPrint() const;

    private :

        const char*   _fileName;
};

class FileError : public RetrieveError {

    public :

        FileError(const char*);
        ~FileError() {}
        virtual void ErrorPrint() const;

    private :

        const char*   _fileName;
};

/*class SeekReadError : public RetrieveError {

    public :

        SeekReadError(const streampos, const int = 0);
        ~SeekReadError() {}
        virtual void ErrorPrint() const;

    private :

        const streampos _filePosition;
        const int       _length;
};

class BasePosError : public RetrieveError {

    public :

        virtual void ErrorPrint() const;
};

class DirPosError : public RetrieveError {

    public :

        virtual void ErrorPrint() const;
}; */

class DebugInfoError : public RetrieveError {

    public :

        DebugInfoError() { }
        ~DebugInfoError() { }

        virtual void ErrorPrint() const;

    private :

//        const char*     _errorMsg;
};

class CVSigError : public RetrieveError {

    public :

        virtual void ErrorPrint() const;
};

class OutOfMemory : public CVpackError {

    public :

        OutOfMemory(const size_t reqSize=0);
        ~OutOfMemory() {}
        virtual void ErrorPrint() const;

    private :

        const size_t  _requestSize;
};

class InternalError : public CVpackError {

    public :

        InternalError(const char *);
        ~InternalError() {}
        virtual void ErrorPrint() const;

    private :

        const char*     _debugMsg;
};

class MiscError : public CVpackError {

    public :

        MiscError(const char*);
        ~MiscError() { }
        virtual void ErrorPrint() const;

    private :

        const char*     _errorMsg;
};
#endif
