#ifndef OIO_H
#define OIO_H


/* OIO.h -- declarations for Object I/O abstract classes

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

log:    OIO.H $
Revision 1.2  95/01/29  13:27:26  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:24  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:23  kgorlen
 * Release for 1st edition.
 * 
*/
#include "Object.h"

class OIOin : public NIHCL {
protected:
        Object* _readObject(const Class& cl)    { return cl.readObject(*this); }
        virtual Object* readObject(const Class&) = 0;
        friend Object* Class::readFrom(OIOin&) const;
public:
        OIOin() {}
        virtual OIOin& operator>>(char*) = 0;
        virtual OIOin& operator>>(char&) = 0;
        virtual OIOin& operator>>(unsigned char&) = 0;
        virtual OIOin& operator>>(short&) = 0;
        virtual OIOin& operator>>(unsigned short&) = 0;
        virtual OIOin& operator>>(int&) = 0;
        virtual OIOin& operator>>(unsigned int&) = 0;
        virtual OIOin& operator>>(long&) = 0;
        virtual OIOin& operator>>(unsigned long&) = 0;
        virtual OIOin& operator>>(float&) = 0;
        virtual OIOin& operator>>(double&) = 0;
        virtual OIOin& operator>>(streambuf*) = 0;
        virtual int get() = 0;
        virtual OIOin& get(char&) = 0;
        virtual OIOin& get(unsigned char&) = 0;
        virtual OIOin& get(char*, unsigned size) = 0;
        virtual OIOin& get(unsigned char*, unsigned size) = 0;
        virtual OIOin& get(short*, unsigned size) = 0;
        virtual OIOin& get(unsigned short*, unsigned size) = 0;
        virtual OIOin& get(int*, unsigned size) = 0;
        virtual OIOin& get(unsigned int*, unsigned size) = 0;
        virtual OIOin& get(long*, unsigned size) = 0;
        virtual OIOin& get(unsigned long*, unsigned size) = 0;
        virtual OIOin& get(float*, unsigned size) = 0;
        virtual OIOin& get(double*, unsigned size) = 0;
        virtual OIOin& get(streambuf&, char delim ='\n') = 0;
        virtual OIOin& getCString(char*, unsigned maxlen) = 0;
        virtual int precision() = 0;
        virtual int precision(int) = 0;
        virtual int width() = 0;
        virtual int width(int) = 0;
};

class OIOout : public NIHCL {
protected:
        void _storer(const Object& ob)  { ob.storer(*this); }
        virtual void storeObject(const Object&) = 0;
        friend void Object::storeOn(OIOout&) const;
public:
        OIOout() {};
        virtual OIOout& operator<<(const char*) = 0;
        virtual OIOout& operator<<(char) = 0;
        virtual OIOout& operator<<(unsigned char) = 0;
        virtual OIOout& operator<<(short) = 0;
        virtual OIOout& operator<<(unsigned short) = 0;
        virtual OIOout& operator<<(int) = 0;
        virtual OIOout& operator<<(unsigned) = 0;
        virtual OIOout& operator<<(long) = 0;
        virtual OIOout& operator<<(unsigned long) = 0;
        virtual OIOout& operator<<(float) = 0;
        virtual OIOout& operator<<(double) = 0;
        virtual OIOout& put(char) = 0;
        virtual OIOout& put(const char* val, unsigned size) = 0;
        virtual OIOout& put(const unsigned char* val, unsigned size) = 0;
        virtual OIOout& put(const short* val, unsigned size) = 0;
        virtual OIOout& put(const unsigned short* val, unsigned size) = 0;
        virtual OIOout& put(const int* val, unsigned size) = 0;
        virtual OIOout& put(const unsigned int* val, unsigned size) = 0;
        virtual OIOout& put(const long* val, unsigned size) = 0;
        virtual OIOout& put(const unsigned long* val, unsigned size) = 0;
        virtual OIOout& put(const float* val, unsigned size) = 0;
        virtual OIOout& put(const double* val, unsigned size) = 0;
        virtual OIOout& putCString(const char*) = 0;
        virtual int precision() = 0;
        virtual int precision(int) = 0;
        virtual int width() = 0;
        virtual int width(int) = 0;
};

#endif /* OIO_H */
