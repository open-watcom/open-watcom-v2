#ifndef OIOSTREAM_H
#define OIOSTREAM_H


/* OIOstream.h -- declarations for stream Object I/O abstract classes

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

log:    OIOSTREA.H $
Revision 1.2  95/01/29  13:27:28  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:36  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:34  kgorlen
 * Release for 1st edition.
 * 
*/
#include "OIO.h"

class OIOistream : public OIOin {
protected:
        istream* strm;          // input stream
        void checkRead() const;
        OIOistream& putback(char c) { strm->putback(c); return *this; }
        void syntaxErr(const char* expect, char was) const;
        virtual Object* readObject(const Class&) = 0;
public:
        OIOistream(istream& s)  { strm = &s; }
        operator istream&()     { return *strm; }
        virtual OIOin& operator>>(char*);
        virtual OIOin& operator>>(char&);
        virtual OIOin& operator>>(unsigned char&);
        virtual OIOin& operator>>(short&);
        virtual OIOin& operator>>(unsigned short&);
        virtual OIOin& operator>>(int&);
        virtual OIOin& operator>>(unsigned int&);
        virtual OIOin& operator>>(long&);
        virtual OIOin& operator>>(unsigned long&);
        virtual OIOin& operator>>(float&);
        virtual OIOin& operator>>(double&);
        virtual OIOin& operator>>(streambuf*);
        virtual int get();
        virtual OIOin& get(char&);
        virtual OIOin& get(unsigned char&);
        virtual OIOin& get(char*, unsigned size);
        virtual OIOin& get(unsigned char*, unsigned size);
        virtual OIOin& get(short*,unsigned size);
        virtual OIOin& get(unsigned short*,unsigned size);
        virtual OIOin& get(int*,unsigned size);
        virtual OIOin& get(unsigned int*,unsigned size);
        virtual OIOin& get(long*,unsigned size);
        virtual OIOin& get(unsigned long*,unsigned size);
        virtual OIOin& get(float*,unsigned size);
        virtual OIOin& get(double*,unsigned size);
        virtual OIOin& get(streambuf&, char delim ='\n');
        virtual OIOin& getCString(char*, unsigned maxlen);
        virtual int precision();
        virtual int precision(int);
        virtual int width();
        virtual int width(int);
};

class OIOostream : public OIOout {
protected:
        ostream* strm;                  // output stream
        unsigned col;                   // current column
        static const unsigned MAXCOL;   // max columns in output line
        void putwrap(char);
        void putwrap(unsigned char c) { putwrap((char)c); }
        void putwrap(const char* s, unsigned len);
        virtual void storeObject(const Object&) = 0;
public:
        OIOostream(ostream& s)  { strm = &s; col = 0; }
        operator ostream&()     { return *strm; }
        virtual OIOout& operator<<(const char*);
        virtual OIOout& operator<<(char);
        virtual OIOout& operator<<(unsigned char);
        virtual OIOout& operator<<(short);
        virtual OIOout& operator<<(unsigned short);
        virtual OIOout& operator<<(int);
        virtual OIOout& operator<<(unsigned);
        virtual OIOout& operator<<(long);
        virtual OIOout& operator<<(unsigned long);
        virtual OIOout& operator<<(float);
        virtual OIOout& operator<<(double);
        virtual OIOout& put(char);
        virtual OIOout& put(const char* val, unsigned size);
        virtual OIOout& put(const unsigned char* val, unsigned size);
        virtual OIOout& put(const short* val, unsigned size);
        virtual OIOout& put(const unsigned short* val, unsigned size);
        virtual OIOout& put(const int* val, unsigned size);
        virtual OIOout& put(const unsigned int* val, unsigned size);
        virtual OIOout& put(const long* val, unsigned size);
        virtual OIOout& put(const unsigned long* val, unsigned size);
        virtual OIOout& put(const float* val, unsigned size);
        virtual OIOout& put(const double* val, unsigned size);
        virtual OIOout& putCString(const char*);
        virtual int precision();
        virtual int precision(int);
        virtual int width();
        virtual int width(int);
};

#endif /* OIOSTREAM_H */
