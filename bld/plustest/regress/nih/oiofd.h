#ifndef OIOFD_H
#define OIOFD_H


/* IOfd.h -- declarations for NIHCL file Object I/O classes

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
        May, 1989

Modification History:

log:    OIOFD.H $
Revision 1.2  95/01/29  13:27:28  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/09  16:30:48  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:28  kgorlen
 * Release for 1st edition.
 * 
*/
#include "Object.h"
#include <string.h>
#include <osfcn.h>

class OIOifd : public NIHCL {
        int fd;                 // file descriptor
protected:
        virtual Object* readObject(const Class&);
        friend Object* Class::readFrom(OIOifd&) const;
public:
        OIOifd(int filedesc)    { fd = filedesc; }
        operator int()          { return fd; }
        OIOifd& operator>>(char& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(unsigned char& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(short& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(unsigned short& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(int& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(unsigned int& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(long& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(unsigned long& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(float& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        OIOifd& operator>>(double& val) {
                get((char*)&val,sizeof(val));
                return *this;
        }
        int get()                       { char c; get(c); return c; }
        OIOifd& get(char& c)            { get(&c,1); return *this; }
        OIOifd& get(unsigned char& c)   { get(&c,1); return *this; }
        OIOifd& get(char*, unsigned size);
        OIOifd& get(unsigned char* val, unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(short* val, unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(unsigned short* val, unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(int* val, unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(unsigned int* val, unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(long* val,unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(unsigned long* val,unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(float* val,unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& get(double* val,unsigned size) {
                get((char*)val,size*sizeof(*val));
                return *this;
        }
        OIOifd& getCString(char*, unsigned maxlen);
};

class OIOofd : public NIHCL {
        int fd;                 // file descriptor
        void writeErr();
protected:
        virtual void storeObject(const Object&);
        friend void Object::storeOn(OIOofd&) const;
public:
        OIOofd(int filedesc)    { fd = filedesc; }
        operator int()          { return fd; }
        OIOofd& operator<<(const char* val) {
                *this << (unsigned)strlen(val);
                put(val,strlen(val));
                return *this;
        }
        OIOofd& operator<<(char val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(unsigned char val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(short val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(unsigned short val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(int val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(unsigned int val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(long val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(unsigned long val) {
                put((char*)&val,sizeof(val));
                return *this;
        }
        OIOofd& operator<<(float val) {
                put((char*)&val,sizeof(float));
                return *this;
        }
        OIOofd& operator<<(double val) {
                put((char*)&val,sizeof(double));
                return *this;
        }
        OIOofd& put(const char* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const unsigned char* val,unsigned size) {
                return put((const char*)val, size);
        }
        OIOofd& put(const short* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const unsigned short* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const int* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const unsigned int* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const long* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const unsigned long* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const float* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& put(const double* val,unsigned size) {
                write((const char*)val,size*sizeof(*val));
                return *this;
        }
        OIOofd& putCString(const char*);
        OIOofd& write(const char* buf,unsigned nbyte) {
                if (::write(fd,(void *)buf,nbyte) < 0) 
                    writeErr();
                return *this;
        }
public:                         // type definitions
        enum oioRecordTy {              // binary object I/O record type codes
                storeOnClassRef,        // class reference and object stored
                storeOnClass,           // class and object stored
                storeOnObjectRef        // object reference stored
        };

};

#endif /* OIOFD_H */
