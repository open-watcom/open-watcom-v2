#ifndef FDSET_H
#define FDSET_H


/* FDSet.h -- Class for manipulating fd_set objects used by select(2)

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

Modification History:

log:    FDSET.H $
Revision 1.2  95/01/29  13:27:08  NT_Test_Machine
*** empty log message ***

Revision 1.1  93/02/16  16:03:20  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:35  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"
#include <libc.h>
#include <sys/types.h>

#ifdef SYSV
#include <memory.h>

int bcmp(const void* p, const void* q, int n)
{
        return memcmp(p,q,n);
}

#endif

#ifndef FD_SETSIZE

#include "nihclconfig.h"

const unsigned FD_SETSIZE = sizeof(fd_set) * 8;

inline void FD_SET(int  fd, fd_set* p)  { ((int*)p)[div_bitsize_int(fd)] |= (1 << mod_bitsize_int(fd)); }
inline void FD_CLR(int  fd, fd_set* p)  { ((int*)p)[div_bitsize_int(fd)] &= ~(1 << mod_bitsize_int(fd)); }
inline bool FD_ISSET(int fd, fd_set* p) { return ((int*)p)[div_bitsize_int(fd)] & (1 << mod_bitsize_int(fd)); }
inline void FD_ZERO(fd_set* p)          { memset((void*)p, 0, sizeof(fd_set)); }

#endif

class FDSet;

class FDSetRef: public NIHCL {
        fd_set* p;      // pointer to word containing bit
        int fd;         // file descriptor
        FDSetRef(fd_set& fdset, int fdesc) {
                p = &fdset; fd = fdesc;
        }
        FDSetRef(const FDSetRef& s) {
                p = s.p; fd = s.fd;
        }
        friend FDSet;
public:
        operator bool() const   { return FD_ISSET(fd,p); }
        bool operator=(bool b) {
                if (b) FD_SET(fd,p);
                else FD_CLR(fd,p);
                return b;
        }
        void operator&=(bool b) {
                if (b && FD_ISSET(fd,p)) FD_SET(fd,p);
                else FD_CLR(fd,p);
        }
        void operator|=(bool b) {
                if (b || FD_ISSET(fd,p)) FD_SET(fd,p);
                else FD_CLR(fd,p);
        }
        void operator^=(bool b) {
                if ((b != 0) != (FD_ISSET(fd,p) !=0 )) FD_SET(fd,p);
                else FD_CLR(fd,p);
        }
};

class FDSet: public VIRTUAL Object {
        DECLARE_MEMBERS(FDSet);
        static unsigned dtablesz;   // size of file descriptor table
public:
        static unsigned dtablesize() { return dtablesz; }
private:
        fd_set fs;                  // system-defined file descriptor set
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        FDSet()                         { FD_ZERO(&fs); }
        operator fd_set&()              { return fs; }
        operator const fd_set&() const  { return fs; }
        operator fd_set*()              { return &fs; }
        operator const fd_set*() const  { return &fs; }
        FDSetRef operator[](int fd)             { return FDSetRef(fs,fd); }
        const FDSetRef operator[](int fd) const { return FDSetRef(((FDSet*)this)->fs,fd); }
        FDSet operator-(const FDSet&) const;
        FDSet operator&(const FDSet&) const;
        FDSet operator|(const FDSet&) const;
        FDSet operator^(const FDSet&) const;
        bool operator==(const FDSet& a) const   { return !bcmp(&fs,&a.fs,sizeof(fs)); }
        bool operator!=(const FDSet& a) const   { return !(*this==a); }
        void operator-=(const FDSet&);
        void operator&=(const FDSet&);
        void operator|=(const FDSet&);
        void operator^=(const FDSet&);
        void clr(int fd)        { FD_CLR(fd,&fs); }
        bool includes(unsigned fd) const { return FD_ISSET(fd,&fs); }
        bool isSet(int fd) const        { return FD_ISSET(fd,&fs); }
        void set(int fd)        { FD_SET(fd,&fs); }
        void zero()             { FD_ZERO(&fs); }
        virtual unsigned capacity() const;  // returns descriptor table size
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();   // {}
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;      // returns # of fd bits set
        virtual const Class* species() const;
};

#endif
