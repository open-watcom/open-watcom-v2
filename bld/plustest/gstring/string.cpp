//
//
// String class: string.c                             version: 0.9
//
// author: Uwe Steinmueller, SIEMENS NIXDORF Informationssysteme AG Munich
//         email: uwe.steinmueller@sniap.mchp.sni.de
//
// start: 28.08.92
//
// this source code is fully copyrighted but it is free in use for
// standardization purposes (X3J16 and ISO WG 21)
//

#include <gstring.h>
#include <iostream.h>
#include <assert.h>
#include <ctype.h>

#ifdef __TCPLUSPLUS__
#define     NORM_ADR(x)   ((char huge*)(x))
#else
#define     NORM_ADR(x)   (x)
#endif /* __TCPLUSPLUS__ */


//
//  overlapping copies, would be supplied with ANSI C memmove()
//  but this is not on all systems available
//

static  void *MemMove(void* s, const void* t, size_t len)
{
    char* target = (char*)s;
    char* p, *tt;

    if(s == t || len == 0)  // nothing to copy
        ;
    else if(len == 1)
        *target = *(char*)t;
    else if(s < t) {
        memcpy(s, t, len);
    }
    else {
        p = target + len - 1;
        tt = (char*)t + len - 1;
        for(; target <= p; p--, tt--)
            *p = *tt;
    }
    return s;
}

inline void
checkOverflow(size_t lower, size_t upper, const char *msg)
{
    if(upper < lower)
/* #####  E029: (col 17) symbol 'strError' has not been declared  */
        strError(msg, "LengthError");
}

//
// private member functions
//

/* #####  E241: (col 14) 'class String' has not been declared  */
void String::doReplace(size_t pos, size_t n, const char* cb, size_t len)
{
/* #####  E029: (col 22) symbol 'length' has not been declared  */
    size_t thisLen = length();
    size_t newlen;

    //
    // preconditions
    //

    assert(pos <= thisLen);   // if pos == thisLen it's like an append
    assert(cb != 0);
    assert(n <= (thisLen - pos));

    //
    // operations
    //

    newlen = len + (thisLen - n);
    checkOverflow(len, newlen, "String::doReplace");

    if(newlen == 0) {
/* #####  E029: (col 15) symbol 'refDec' has not been declared  */
        refDec();
/* #####  E029: (col 9) symbol 'srep' has not been declared  */
        srep = 0;
        return;
    }

    // prevent against hacks like: s.insert(1, s.cStr() + 2)
    //

/* #####  E029: (col 34) symbol 'srep' has not been declared  */
    if(srep && NORM_ADR(srep->str) <= NORM_ADR(cb)
            && NORM_ADR(cb) <= NORM_ADR(srep->str + thisLen)) {
/* #####  E029: (col 25) symbol 'String' has not been declared  */
        replace(pos, n, String(cb, len));
        return;
    }

/* #####  E029: (col 8) symbol 'needClone' has not been declared  */
    if(needClone(newlen)) {
/* #####  E241: (col 17) 'class String' has not been declared  */
/* #####  E006: (col 31) syntax error; probable cause: missing ';'  */
        String::StringRep *ps = StringRep::getNew(pos, newlen, srep->str);

        if(thisLen > 0)     // copy tail
/* #####  E029: (col 20) symbol 'ps' has not been declared  */
            memcpy(ps->str + pos + len, srep->str + pos + n,
                                      thisLen - (pos + n));
/* #####  E029: (col 9) symbol 'refDec' has not been declared  */
        refDec();
        srep = ps;
    }
    else {
        char* ptarget = srep->str;      // srep != 0

        if(n != len)  // move in place
            MemMove(ptarget + pos + len, ptarget + pos + n,
                thisLen - (pos + n));
    }
    if(len > 0)
        memcpy(srep->str + pos, cb, len);
    srep->setLen(newlen);
    assert(newlen == length());
}



/* #####  E241: (col 21) 'class String' has not been declared  */
const char* String::getStr() const
/* #####  E412: (col 1) only member functions can be declared 'const' or 'volatile'  */
{
    const char  *p;

/* #####  E029: (col 8) symbol 'srep' has not been declared  */
    if(srep == 0)
        p = "";
    else
        p = srep->str;
    return p;
}

//
//  private constructor
//

/* #####  E241: (col 9) 'class String' has not been declared  */
String::String(const char* cb1, size_t n1, const char* cb2, size_t n2)
{
    assert(cb1 != 0 && cb2 != 0);

    //
    // operations
    //

    size_t newlen = n1 + n2;

    checkOverflow(n1, newlen, "operator+");   // unsigned overflow

    if(newlen == 0) {
/* #####  E029: (col 14) symbol 'srep' has not been declared  */
        srep = 0;
    }
    else {
/* #####  E241: (col 27) 'class StringRep' has not been declared  */
/* #####  E029: (col 27) symbol 'getNew' has not been declared  */
        srep = StringRep::getNew(n1, newlen, cb1);
/* #####  E133: (col 16) too many errors: compilation aborted  */
        memcpy(srep->str + n1, cb2, n2);
        srep->setLen(newlen);
    }

    //
    // post conditions
    //

    assert(newlen == 0 || srep != 0);
    assert(length() == newlen);
    assert(memcmp(getStr(), cb1, n1) == 0);
    assert(memcmp(getStr() + n1, cb2, n2) == 0);
}



//
//  constructors
//

String::String(const char* cb, size_t n)
{

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::String", "InvalidArgument");

    //
    // operations
    //

    if(n == NPOS)
        n = strlen(cb);

    if(n == 0) {
        srep = 0;
    }
    else {
        srep = StringRep::getNew(n, 0, cb);
    }

    //
    // post conditions
    //

    assert(n == 0 || srep != 0);
    assert(length() == n);
    assert(memcmp(cStr(), cb, length()) == 0);
}

String::String(char  c, size_t rep)
{
    //
    // preconditions
    //

    if(rep == NPOS)
        strError("String::String", "OutOfRange");

    //
    // operations
    //

    if(rep == 0) {
        srep = 0;
    }
    else {
        srep = StringRep::getNew(rep);
        memset(srep->str, c, rep);
    }

    //
    // post conditions
    //

    assert(rep == 0 || srep != 0);
    assert(length() == rep);
#ifndef NDEBUG
    for(int i = 0; i < rep; i++)
        assert(getAt(i) == c);
#endif /* NDEBUG */
}


char String::getAt(size_t pos) const
{
    //
    // preconditions
    //

    if(pos >= length())
        strError("String::getAt", "OutOfRange");

    //
    // operations
    //

    return getAtRaw(pos);
}


void String::putAt(size_t pos, char c)
{
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::putAt", "OutOfRange");

    //
    // operations
    //

    if(pos == thisLen)
        append(&c, 1);
    else                    // thisLen > 0 => srep != 0
        putAtRaw(pos, c);
}


//
// assignment operators
//
String& String::operator=(const String& s)
{
    //
    // operations
    //

    ((String&)s).refInc();    // casting away constness, as we change not
                              // the value of the string
    refDec();
    srep = s.srep;

    //
    // post conditions
    //

    assert(length() == s.length());
    assert(memcmp(cStr(), s.cStr(), length()) == 0);

    return *this;
}


String& String::operator=(char c)
{
    //
    // operations
    //

    assign(c, 1);

    return *this;
}

String& String::assign(char c, size_t rep)
{
    // could be implemented more efficiently
    //
    operator=(String(c, rep));

    //
    // post conditions
    //

    assert(length() == rep);
#ifndef NDEBUG
    for(int i = 0; i < rep; i++)
        assert(getAt(i) == c);
#endif /* NDEBUG */

    return *this;
}


String& String::assign(const char* cb, size_t n)
{
    //
    // preconditions
    //

    if(cb == 0)
        strError("String::assign", "InvalidArgument");

    //
    // operations
    //

    if(n == NPOS)
        n = strlen(cb);

    if(n == 0) {
        refDec();
        srep = 0;
    }
    else {
        if(needClone(n)) {
            String::StringRep* rep = StringRep::getNew(n, 0, cb);
            refDec();
            srep = rep;
        }
        else {
            srep->setLen(n);
            memcpy(srep->str, cb, n);
        }
    }

    //
    // post conditions
    //

    assert(length() == n);
    assert(memcmp(cStr(), cb, length()) == 0);

    return *this;
}

//
//  appending operators
//
String& String::operator+=(const String& s)
{
    size_t sLen;
    KEEPOLD;

    //
    // operations
    //

    if(this == &s)              // append to self
        return operator+=(String(s));
    sLen = s.length();
    if(sLen > 0)
        doReplace(length(), 0, s.srep->str, sLen);

    //
    // post conditions
    //

    assert((OLD.length() + s.length()) == length());
    assert(memcmp(cStr() + OLD.length(), s.cStr(), s.length()) == 0);

    return *this;
}

String& String::operator+=(const char *cs)
{
    KEEPOLD;
#ifndef NDEBUG
    char *pOLD = 0;
    if(cs != 0) {
        pOLD = new char[strlen(cs) + 1];
        assert(pOLD != 0);
        strcpy(pOLD, cs);
    }
#endif

    //
    // preconditions
    //

    if(cs == 0)
        strError("String::operator+=", "InvalidArgument");

    //
    // operations
    //

    doReplace(length(), 0, cs, strlen(cs));

    //
    // post conditions
    //

#ifndef NDEBUG
    //
    // this asertion is so complicated
    // because of hacks like: s += s.cStr();
    //
    assert((OLD.length() + strlen(pOLD)) == length());
    assert(memcmp(cStr() + OLD.length(), pOLD, strlen(pOLD)) == 0);
    delete [] pOLD;
#endif

    return *this;
}

String& String::operator+=(char c)
{
    KEEPOLD;

    //
    // operations
    //

    doReplace(length(), 0, &c, 1);

    //
    // post conditions
    //

    assert((OLD.length() + 1) == length());
    assert(getAt(OLD.length()) == c);

    return *this;
}

String& String::append(const char* cb, size_t n)
{
    KEEPOLD;

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::append", "InvalidArgument");

    //
    // operations
    //

    if(n == NPOS)
        n = strlen(cb);

    if(n > 0)
        doReplace(length(), 0, cb, n);

    //
    // post conditions
    //

    assert((OLD.length() + n) == length());
    assert(memcmp(cStr() + OLD.length(), cb, n) == 0);

    return *this;
}


String& String::append(char c, size_t rep)
{
    KEEPOLD;

    //
    // preconditions
    //

    if(rep == NPOS)
        strError("String::append", "OutOfRange");

    //
    // operations
    //

    if(rep == 1)
        doReplace(length(), 0, &c, 1);
    else if(rep > 0)
        operator+=(String(c, rep));

    //
    // post conditions
    //

    assert((OLD.length() + rep) == length());
#ifndef NDEBUG
    for(int i = 0; i < rep; i++) {
        int j = i + OLD.length();
        assert(getAt(j) == c);
    }
#endif /* NDEBUG */

    return *this;
}

int String::compare(const char* cb, size_t n) const
{
    size_t  thisLen  = length();
    int  res;

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::compare", "InvalidArgument");

    //
    // operations
    //

    if(n == NPOS)
        n = strlen(cb);

    res = memcmp(getStr(), cb, thisLen > n ? n : thisLen);
    if(res == 0 && n != thisLen) {
        if(thisLen > n)
            res = 1;
        else
            res = -1;
    }
    return res;
}

//
//  search member functions
//
int String::find(const String& s, size_t& fpos, size_t pos) const
{
    return find(s.getStr(), fpos, pos, s.length());
}

int String::rfind(const String& s, size_t& fpos, size_t pos) const
{
    return rfind(s.getStr(), fpos, pos, s.length());
}

int String::find(const char* cb, size_t& fpos, size_t pos, size_t n) const
{
    size_t thisLen  = length();

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::find", "InvalidArgument");

    //
    // operations
    //

    fpos = NPOS;                            //  not found fpos


    if(pos >= thisLen)
        return 0;

    if(n == NPOS)
        n = strlen(cb);

    if(thisLen == 0 || n > (thisLen - pos))    // not found empty or too long
        return 0;
    if(n == 0) {                             // empty string is always found
        fpos = pos;
        return 1;
    }
    // pointers are valid because thisLen > 0

    char *pStart  = srep->str + pos;
    char *pEnd    = srep->str + thisLen - n;

    for(; pStart <= pEnd; pos++, pStart++) {
        if(*pStart != *cb)
            continue;
        if(memcmp(pStart, cb, n) == 0) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

int String::rfind(const char* cb, size_t& fpos, size_t pos, size_t n) const
{
    size_t thisLen  = length();

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::rfind", "InvalidArgument");

    //
    // operations
    //

    fpos = NPOS;                            //  not found fpos

    if(n == NPOS)
        n = strlen(cb);

    if(thisLen == 0 || n > thisLen)            // not found empty or too long
        return 0;

    if(pos >= thisLen)
        pos = thisLen - 1;

    if(n == 0) {                             // empty string is always found
        fpos = pos;
        return 1;
    }
    if((thisLen - n) < pos)
        pos = thisLen - n;

    // pointers are valid because thisLen > 0

    char *pStart  = srep->str + pos;
    char *pBegin  = srep->str;

    for(; pStart >= pBegin; pos--, pStart--) {
        if(*pStart != *cb)
            continue;
        if(memcmp(pStart, cb, n) == 0) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

int String::find(char c, size_t& fpos, size_t pos) const
{
    size_t thisLen = length();

    //
    // operations
    //

    fpos = NPOS;

    if(pos >= thisLen || thisLen == 0)
        return 0;

    // pointers are valid because thisLen > 0

    char *pStart  = srep->str + pos;
    char *pEnd    = srep->str + thisLen;

    for(; pStart <  pEnd; pos++, pStart++) {
        if(*pStart == c) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

int String::rfind(char c, size_t& fpos, size_t pos) const
{
    size_t thisLen = length();

    //
    // operations
    //

    fpos = NPOS;

    if(thisLen == 0)
        return 0;

    if(pos >= thisLen)
        pos = thisLen - 1;

    // pointers are valid because thisLen > 0

    char *pStart  = srep->str + pos;
    char *pBegin  = srep->str;

    for(; pStart >= pBegin; pos--, pStart--) {
        if(*pStart == c) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

//
// insert
//
String& String::insert(size_t pos, const String& s)
{
    KEEPOLD;

    //
    // preconditions
    //

    if(pos > length())
        strError("String::insert", "OutOfRange");

    //
    // operations
    //

    if(this == &s)
        return insert(pos, String(s));          // insert into itself
    if(s.srep)
        doReplace(pos, 0, s.srep->str, s.srep->getLen());

    //
    // post conditions
    //

    assert(length() == (s.length() + OLD.length()));
    assert(memcmp(cStr() + pos, s.cStr(), s.length()) == 0);

    return *this;
}

String& String::insert(size_t pos, const char* cb, size_t n)
{
    KEEPOLD;

#ifndef NDEBUG
    char *pOLD = 0;
    if(cb != 0) {
        if(n == NPOS)
            n = strlen(cb);
        pOLD = new char[n];
        assert(pOLD != 0);
        memcpy(pOLD, cb, n);
    }
#endif

    //
    // preconditions
    //

    if(pos > length())
        strError("String::insert", "OutOfRange");
    if(cb == 0)
        strError("String::insert", "InvalidArgument");

    //
    // operations
    //

    if(n == NPOS)
        n = strlen(cb);

    if(n > 0)
        doReplace(pos, 0, cb, n);

    //
    // post conditions
    //

    assert(length() == (n + OLD.length()));

#ifndef NDEBUG
    //
    // this asertion is so complicated
    // because of hacks like: s.insert(1, s.cStr() + 2)
    //
    assert(memcmp(cStr() + pos, pOLD, n) == 0);
    delete [] pOLD;
#endif

    return *this;
}

String& String::insert(size_t pos, char c, size_t rep)
{
    KEEPOLD;

    //
    // preconditions
    //

    if(pos > length())
        strError("String::insert", "OutOfRange");
    //
    // operations
    //

    if(rep == 1)
        doReplace(pos, 0, &c, 1);
    else if(rep > 0)
        insert(pos, String(c, rep));

    //
    // post conditions
    //

    assert(length() == (rep + OLD.length()));
#ifndef NDEBUG
    for(int i = 0; i < rep; i++)
        assert(getAt(i + pos) == c);
#endif /* NDEBUG */

    return *this;
}

//
//  replace
//

String& String::replace(size_t pos, size_t n, const char *cb, size_t len)
{
    KEEPOLD;
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::replace", "OutOfRange");
    if(cb == 0)
        strError("String::replace", "InvalidArgument");

    //
    // operations
    //

    if(len == NPOS)
        len = strlen(cb);

    if((pos + n) > thisLen)
        n = thisLen - pos;

    if(n > 0 || len > 0)
        doReplace(pos, n, cb, len);

    //
    // post conditions
    //

    assert(length() == (OLD.length() - n + len));

    return *this;
}

String& String::replace(size_t pos, size_t n, char c, size_t rep)
{
    // this could be implemented more efficiently

    // we get all assertions and checks in
    // replace(size_t, size_t, const String&);

    replace(pos, n, String(c,rep));
    return *this;
}

String& String::replace(size_t pos, size_t len, const  String& s)
{
    KEEPOLD;
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::replace", "OutOfRange");

    //
    // operations
    //

    if(this == &s) {                         // if argument string is target
        return replace(pos, len, String(s)); // get a copy and call it
                                             // recursively
    }

    if((pos + len) > thisLen)
        len = thisLen - pos;

    doReplace(pos, len, s.getStr(), s.length());

    //
    // post conditions
    //

    assert(length() == (OLD.length() - len + s.length()));

    return *this;
}


//
// remove
//

String& String::remove(size_t pos, size_t n)
{
    KEEPOLD;
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > length())
        strError("String::remove", "OutOfRange");

    //
    // operations
    //

    if(n == NPOS || (pos + n) > thisLen)
        n = thisLen - pos;

    if(n > 0)
        doReplace(pos, n,  "", 0);

    //
    // post conditions
    //

    assert(length() == (OLD.length() - n));

    return *this;
}

String& String::getRemove(char& c, size_t pos)
{
    KEEPOLD;

    //
    // preconditions
    //

    if(pos >= length())
        strError("String::getRemove", "OutOfRange");

    //
    // operations
    //

    c = getAtRaw(pos);

    doReplace(pos, 1,  "", 0);

    //
    // post conditions
    //

    assert(length() == (OLD.length() - 1));
    assert(OLD.getAt(pos) == c);

    return *this;
}

String& String::getRemove(String& s, size_t pos, size_t n)
{
    KEEPOLD;
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::getRemove", "OutOfRange");

    //
    // operations
    //

    if(n == NPOS || (pos + n) > thisLen)
        n = thisLen - pos;

    s = substr(pos, n);
    doReplace(pos, n,  "", 0);

    //
    // post conditions
    //

    assert(length() == (OLD.length() - s.length()));
    assert(s == OLD.substr(pos, n));

    return *this;
}

//
// substring
//
String String::substr(size_t pos, size_t n) const
{
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::substr", "OutOfRange");

    //
    // operations
    //

    if(n == NPOS || (pos + n) > thisLen)
        n = thisLen - pos;

    if(n == 0)
        return String();
    else
        return String(srep->str + pos, n);
}

//
// ANSI C functionality
//
int String::findFirstOf(const String& s, size_t& fpos, size_t pos) const
{
    return findFirstOf(s.getStr(), fpos, pos, s.length());
}

int String::findFirstOf(const char *cb, size_t& fpos, size_t pos, size_t n)
                                                                const
{
    size_t thisLen = length();

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::findFirstOf", "InvalidArgument");

    //
    // operations
    //

    fpos = NPOS;

    if(thisLen == 0 || pos >= thisLen)
        return 0;

    if(n == NPOS)
        n = strlen(cb);

    // pointer are valid because thisLen > 0 => srep != 0

    char *pStart  = srep->str + pos;
    char *pEnd    = srep->str + thisLen;

    for(; pStart <  pEnd; pos++, pStart++) {
        if(memchr(cb, *pStart, n) != 0) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

int String::findFirstNotOf(const String& s, size_t& fpos, size_t pos) const
{
    return findFirstNotOf(s.getStr(), fpos, pos, s.length());
}

int String::findFirstNotOf(const char *cb, size_t& fpos, size_t pos, size_t n)
                                                                       const
{
    size_t thisLen = length();

    //
    // preconditions
    //

    if(cb == 0)
        strError("String::findFirstOf", "InvalidArgument");

    //
    // operations
    //

    fpos = NPOS;

    if(thisLen == 0 || pos >= thisLen)
        return 0;
    if(n == NPOS)
        n = strlen(cb);

    // pointer are valid because thisLen > 0 => srep != 0

    char *pStart  = srep->str + pos;
    char *pEnd    = srep->str + thisLen;

    for(; pStart <  pEnd; pos++, pStart++) {
        if(memchr(cb, *pStart, n) == 0) {
            fpos = pos;
            return 1;
        }
    }
    return 0;
}

//
// copy to C buffer
//
size_t String::copy(char *cb, size_t n, size_t pos, size_t len) const
{
    size_t thisLen = length();

    //
    // preconditions
    //

    if(pos > thisLen)
        strError("String::copy", "OutOfRange");
    if(cb == 0)
        strError("String::copy", "InvalidArgument");

    //
    // operations
    //

    if(len > (thisLen - pos))
        len = thisLen - pos;

    if(n < len)
        len = n;
    if(len > 0)
        memcpy(cb, srep->str + pos, len);

    return (len);
}

//
//  get pointer to internal C-String
//
const char* String::cStr() const
{
    const char *p = "";

    //
    // operations
    //

    if(srep)
         p = srep->cPtr();
    return p;
}

//
// capacity
//

void  String::reserve(size_t n) const
{
    size_t thisLen = length();
    String::StringRep* rep;

    // guarantees uniqueness !!

    if(n < thisLen)
        n = thisLen;
    if(n > 0)
        rep = StringRep::getNew(thisLen, n, srep->str);
    else
        rep = 0;
    ((String*)this)->refDec();
    ((String*)this)->srep = rep;  // casting constness away to use
                                  // meaningwise constness
}

//
// friend functions
//
ostream& operator<<(ostream& os, const String& s)
{
    //
    // operations
    //

    if(s.srep) {
        os.write(s.getStr(), s.length());
    }
    return os;
}

const int lBufSize = 32;

istream& operator>>(istream& is, String& s)
{
    char aChar;
    char buffer[lBufSize];
    int bufpos = 0;

    // I am nor very experienced in iostream
    // please give me some comments

    s = "";
    if(is.good()) {
        if(is.flags() & ios::skipws)
            is >> ws;
        if(is) {
            for(;;) {
                is.get(aChar);
                if(!is)
                    break;
                if(isspace(aChar)) {
                    is.putback(aChar);
                    break;
                }
                if(bufpos >= lBufSize) {
                    s.append(buffer, lBufSize);
                    bufpos = 0;
                }
                buffer[bufpos++] = aChar;
            }
        }
    }
    if(bufpos > 0)
        s.append(buffer, bufpos);
    if(is.eof() && s.length() != 0)
        is.clear(0);                    // ??
    return is;
}

istream& getline(istream& is, String& s, char c)
{
    char aChar;
    char buffer[lBufSize];
    int bufpos = 0;

    s = "";
    if(is.good()) {
        for(;;) {
            is.get(aChar);
            if(!is)
                break;
            if(aChar == c) {       // get and consume eol character !!
                break;
            }
            if(bufpos >= lBufSize) {
                s.append(buffer, lBufSize);
                bufpos = 0;
            }
            buffer[bufpos++] = aChar;
        }
    }
    if(bufpos > 0)
        s.append(buffer, bufpos);
    if(is.eof() && s.length() != 0)
        is.clear(0);                // ??
    return is;
}

//
// string add functions
//
String operator+(const String& s1, const String& s2)
{
    //
    // operations
    //

    return String(s1.getStr(), s1.length(), s2.getStr(), s2.length());
}

String operator+(const String& s, const char* cs)
{
    //
    // preconditions
    //

    if(cs == 0)
        strError("operator+", "InvalidArgument");

    //
    // operations
    //
    return String(s.getStr(), s.length(), cs, strlen(cs));
}

String operator+(const char* cs, const String& s)
{
    //
    // preconditions
    //

    if(cs == 0)
        strError("operator+", "InvalidArgument");

    //
    // operations
    //
    return String(cs, strlen(cs), s.getStr(), s.length());
}

String operator+(const String& s, char c)
{
    //
    // operations
    //
    return String(s.getStr(), s.length(), &c, 1);
}

String operator+(char c, const String& s)
{
    //
    // operations
    //
    return String(&c, 1, s.getStr(), s.length());
}

int compare(const String& s1, const String& s2)
{
    return s1.compare(s2);
}

int compare(const String& s, const char* cs)
{
    //
    // preconditions
    //

    if(cs == 0)
        strError("compare", "InvalidArgument");

    //
    // operations
    //

    return s.compare(cs, strlen(cs));
}

int compare(const char *cs, const String& s)
{
    //
    // preconditions
    //

    if(cs == 0)
        strError("compare", "InvalidArgument");

    //
    // operations
    //

    return -s.compare(cs, strlen(cs));
}

