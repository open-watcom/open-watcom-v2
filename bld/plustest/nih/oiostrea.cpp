/* OIOstream.c -- implementation of stream Object I/O abstract classes

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        May, 1989

Function:
        
Abstract classes OIOistream and OIOostream for stream Object I/O.

Modification History:

log:    OIOSTREA.C $
Revision 1.1  93/08/05  11:19:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:31  kgorlen
 * Release for 1st edition.
 * 
*/

#include <ctype.h>
#include "OIOstream.h"
#include "OIOTbl.h"

static char rcsid[] = "header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\OIOSTREA.C 1.1 93/08/05 11:19:58 Anthony_Scian Exp Locker: NT_Test_Machine $";

extern const int NIHCL_RDEOF,NIHCL_RDFAIL,NIHCL_RDSYNERR,NIHCL_STROV;
        
const unsigned OIOostream::MAXCOL = 80;

void OIOistream::checkRead() const      // readFrom input error checking 
{
        char context[31];
        if (strm->good()) return;
        if (strm->eof()) setError(NIHCL_RDEOF,DEFAULT);
        strm->clear();
        context[0] = '\0';
        strm->get(context,sizeof(context),-1);
        setError(NIHCL_RDFAIL,DEFAULT,context);
}

void OIOistream::syntaxErr(const char* expect, char was) const
/*
        Report readFrom syntax error.
*/
{
        char context[31];
        strm->putback(was);
        context[0] = '\0';
        strm->get(context,sizeof(context));
        setError(NIHCL_RDSYNERR,DEFAULT,expect,context);
}

OIOin& OIOistream::operator>>(char* t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(char& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(unsigned char& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(short& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(unsigned short& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(int& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(unsigned int& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(long& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(unsigned long& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(float& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(double& t) { *strm >> t; return *this; }
OIOin& OIOistream::operator>>(streambuf* t) { *strm >> t; return *this; }

int OIOistream::get() { return strm->get(); }

OIOin& OIOistream::get(char& c) { strm->get(c); return *this; }

OIOin& OIOistream::get(unsigned char& c) { strm->get(c); return *this; }

OIOin& OIOistream::get(char* s, unsigned size)
{
        long f = strm->flags();
        *strm >> hex;
        int i;
        while (size--) {
                *strm >> i;
                *s++ = i;
        }
        strm->flags(f);
        return *this;
}

OIOin& OIOistream::get(unsigned char* s, unsigned size)
{
        return get((char*)s,size);
}

OIOin& OIOistream::get(short* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(unsigned short* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(int* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(unsigned int* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(long* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(unsigned long* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(float* val, unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(double* val,unsigned size)
{
        while (size--) *strm >> *val++;
        return *this;
}

OIOin& OIOistream::get(streambuf& b, char delim)
{
        strm->get(b,delim);
        return *this;
}

OIOin& OIOistream::getCString(char* s, unsigned maxlen)
// Read characters in C string format from strm into buffer area s.
// maxlen is the length of s, so at most maxlen-1 characters can be
// read due to the byte required for the null terminator.
{
        unsigned char c;
        char* p = s;
        unsigned n = maxlen;
        *strm >> ws;
        strm->get(c);
        checkRead();
        if (c != '"') syntaxErr("\"",c);
        while ((strm->get(c), c != '"')) {
                if (c == '\n') continue;    // skip newlines
                if (n-- == 0) setError(NIHCL_STROV,DEFAULT,maxlen,s,maxlen);
                if (c != '\\') *p++ = c;
                else switch ((strm->get(c), c)) {
                        case 'n' : { *p++ = '\n'; break; }
                        case 't' : { *p++ = '\t'; break; }
                        case 'b' : { *p++ = '\b'; break; }
                        case 'r' : { *p++ = '\r'; break; }
                        case 'f' : { *p++ = '\f'; break; }
                        case '\\' : { *p++ = '\\'; break; }
                        case '"' : { *p++ = '"'; break; }
                        case '[' : { *p++ = '{'; break; }
                        case ']' : { *p++ = '}'; break; }
                        case 'x' : {        // hex character code
                                char buf[4];
                                int i;
                                strm->get(buf,3);
                                sscanf(buf,"%2x",&i);
                                *p++ = i;
                                break;
                                }
                        default  : {        // decimal character code for backward compatibility
                                int i;
                                strm->putback(c);
                                *strm >> i; *p++ = i; strm->get(c);
                                if (c != '\\') syntaxErr("\\",c);
                                }
                        };
        }
        if (n-- == 0) setError(NIHCL_STROV,DEFAULT,maxlen,s,maxlen);
        *p++ = '\0';
        return *this;
}

int OIOistream::precision()         { return strm->precision(); }

int OIOistream::precision(int p)    { return strm->precision(p); }

int OIOistream::width()             { return strm->width(); }

int OIOistream::width(int w)        { return strm->width(w); }

OIOout& OIOostream::operator<<(const char* t) { *strm << t; return *this; }
OIOout& OIOostream::operator<<(char t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(unsigned char t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(short t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(unsigned short t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(int t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(unsigned t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(long t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(unsigned long t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(float t) { *strm << t << '\n'; return *this; }
OIOout& OIOostream::operator<<(double t) { *strm << t << '\n'; return *this; }

OIOout& OIOostream::put(char c) { strm->put(c); return *this; }

OIOout& OIOostream::put(const char* s, unsigned size)
{
        long f = strm->flags();
        *strm << hex;
        while (size--) *strm << (int)*s++ << '\n';
        strm->flags(f);
        return *this;
}

OIOout& OIOostream::put(const unsigned char* s, unsigned size)
{
        return put((const char*)s,size);
}

OIOout& OIOostream::put(const short* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const unsigned short* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const int* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const unsigned int* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const long* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const unsigned long* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const float* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

OIOout& OIOostream::put(const double* val, unsigned size)
{
        while (size--) *strm << *val++ << '\n';
        return *this;
}

void OIOostream::putwrap(char c)
{
        if (++col > MAXCOL) {
                *strm << '\n';
                col = 1;
        }
        *strm << c;
}

void OIOostream::putwrap(const char* s, unsigned len)
{
        col += len;
        if (col > MAXCOL) {
                *strm << '\n';
                col = len;
        }
        *strm << s;
}

OIOout& OIOostream::putCString(const char* s)
{
        register unsigned char c;
        putwrap("\"",1);
        while (c = *s++) {
                char* p = 0;
                switch (c) {            // check for escape sequence
                        case '\n' : { p = "\\n"; break; }       // line feed
                        case '\t' : { p = "\\t"; break; }       // horizontal tab
                        case '\b' : { p = "\\b"; break; }       // backspace
                        case '\r' : { p = "\\r"; break; }       // carriage return
                        case '\f' : { p = "\\f"; break; }       // form feed
                        case '\\' : { p = "\\\\"; break; }
                        case '"'  : { p = "\\\""; break; }
                        case '{'  : { p = "\\["; break; }
                        case '}'  : { p = "\\]"; break; }
                }
                if (p) putwrap(p,2);    // put 2-character escape sequence
                else {
                        if (isprint(c)) putwrap(c);     // printable character
                        else {          // hex character code
                                char buf[8];
                                sprintf(buf,"\\x%02x",c);
                                putwrap(buf,4);
                        }
                }
        }
        putwrap('\"');
        *strm << '\n';
        col = 0;
        return *this;
}

int OIOostream::precision()         { return strm->precision(); }

int OIOostream::precision(int p)    { return strm->precision(p); }

int OIOostream::width()             { return strm->width(); }

int OIOostream::width(int w)        { return strm->width(w); }
