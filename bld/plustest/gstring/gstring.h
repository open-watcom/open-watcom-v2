#ifndef _GSTRING_H
#define _GSTRING_H
//
//
// String class: gstring.h                            version: 0.9
//
// author: Uwe Steinmueller, SIEMENS NIXDORF Informationssysteme AG Munich
//         email: uwe.steinmueller@sniap.mchp.sni.de
//
// start: 28.08.92
//
// this source code is fully copyrighted but it is free in use for
// standardization purposes (X3J16 and ISO WG 21)
//

// called gstring.h as there is a string.h and uppercase letters
// cannot be used with DOS

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <assert.h>
#include <limits.h>

//
//  debug support
//

#ifndef NDEBUG

//  KEEPOLD stores the this string on entry to allow its usage
//  in post conditions
//
//  KEEPOLD uses this constructor not to interfere with the reference
//  counting, otherwise the code would behave very different with and
//  without NDEBUG defined

#define KEEPOLD   String OLD(this->getStr(), this->length())

#else

#define KEEPOLD

#endif /* NDEBUG */

//
//  error routine (will be replaced by exceptions)
//

inline
void strError(const char *fct, const char * msg)
{
    cerr << "STR_ERROR in " << fct << ", reason: " << msg << endl;
    exit(1);
}

#ifdef UNIX
const size_t NPOS = 0xffffffff;  // some value indicating an invalid size_t
#else
const size_t NPOS = UINT_MAX;    // some value indicating an invalid size_t
#endif /* UNIX */

class Size_T                     // wrapper for size_t
{
public:
    Size_T(size_t n) : val(n) {}
    size_t value() { return val; }
    ~Size_T() {}
private:
    size_t  val;
};

const size_t FIXLEN  =  8;

//
// declaration for class String
//

class String
{
    //
    // Exceptions: OutOfMemory, OutOfRange, InvalidArgument
    //

    class StringRep {
        friend class String;
    private:
        //
        // member variables
        //
        size_t   len;
        size_t   alloc;
        size_t   refCount;
        char     str[FIXLEN];
        //
        //  private access functions only for class String to use
        //
        void         refInc();
        void         refDec();
        void         deleteSelf();
        char*        cPtr();
        size_t       getLen();
        void         setLen(size_t nlen);
        //
        //  not used, but private
        //  to protect against usage outside of string
        //
        StringRep();
        ~StringRep();
        StringRep(const StringRep&);
        void operator=(const StringRep&);
        //
        // friends
        //
        static StringRep* getNew(size_t  len, size_t allocLen = 1,
                                          const char* source=0);
    };
public:
    //
    // constructors
    //

    String();
    String(Size_T ic);
    String(const String& s);
    String(const char* cb, size_t n = NPOS);
    String(char c, size_t rep = 1);

    //
    // destructor (non virtual, have care using String as a base class)
    //

    ~String();

    //
    // Assignment (value semantics)
    //

    String& operator=(const String& s);

        // needed for convenience and efficiency

        String& operator=(const char* cs);
        String& assign(char c, size_t rep = 1);
        String& assign(const char* cb, size_t n = NPOS);
        String& operator=(char c);


    //
    // Concatenation
    //

    String& operator+=(const String& s);

        // needed for convenience and efficiency

        String& operator+=(const char* cs);
        String& append(const char* cb, size_t n = NPOS);
        String& append(char c, size_t rep = 1);
        String& operator+=(char c);

    friend String operator+(const String& s1, const String& s2);

        // needed for convenience and efficiency

        friend String operator+(const char* cs, const String& s);
        friend String operator+(const String& s, const char* cs);
        friend String operator+(char c, const String& s);
        friend String operator+(const String& s, char c);

    //
    // Compare / Predicates
    //

    int compare(const String& s) const;

    friend int operator==(const String& s1, const String& s2);
    friend int operator!=(const String& s1, const String& s2);


        // needed for convenience and efficiency

        int compare(const char* cb, size_t n = NPOS) const;

        friend int compare(const String& s1, const String& s2);
        friend int operator==(const String& s1, const String& s2);
        friend int operator!=(const String& s1, const String& s2);

        friend int compare(const char* cs, const String& s2);
        friend int operator==(const char* cs, const String& s2);
        friend int operator!=(const char* cs, const String& s2);

        friend int compare(const String& s1, const char* cs);
        friend int operator==(const String& s1, const char* cs);
        friend int operator!=(const String& s1, const char* cs);

    //
    // Insertion at some pos
    //

    String& insert(size_t pos, const String& s);

        // needed for convenience and efficiency

        String& insert(size_t pos, const char* cb, size_t n = NPOS);
        String& insert(size_t pos, char c, size_t rep = 1);

    //
    // Removal
    //

    String& remove(size_t pos, size_t n = NPOS);

        // needed for convenience and efficiency

        String& getRemove(char& c,size_t pos);
        String& getRemove(String &s, size_t pos, size_t n = NPOS);

    //
    // Replacement at some pos
    //

    String& replace(size_t pos, size_t n, const String& s);

        // needed for convenience and efficiency

        String& replace(size_t pos, size_t n, const char* cb,
                                    size_t l = NPOS);
        String& replace(size_t pos, size_t n, char c, size_t rep = 1);

    //
    // Subscripting
    //

    char getAt(size_t pos) const;
    void putAt(size_t pos, char c);

    char getAtRaw(size_t pos) const;    // without range check
    void putAtRaw(size_t pos, char c);  // without range check

    //
    // Search
    //

    int find(char c, size_t& fpos, size_t pos = 0) const;
    int find(const String& s, size_t& fpos, size_t pos = 0) const;
    int find(const char* cb, size_t& fpos, size_t pos = 0,
                 size_t n = NPOS) const;

    int rfind(char c, size_t& fpos, size_t pos = NPOS) const;
    int rfind(const String& s, size_t& fpos, size_t pos = NPOS) const;
    int rfind(const char* cb, size_t& fpos, size_t pos = NPOS,
                 size_t n = NPOS) const;

    //
    // Substring
    //

    String substr(size_t pos, size_t n = NPOS) const;

    //
    // I/O
    //

    friend ostream& operator<<(ostream& os, const String& s);
    friend istream& operator>>(istream& is, String& s);
    friend istream& getline(istream& is, String& s, char c = '\n');

    // ANSI C functionality

    // functionality of strpbrk() and strcspn()

    int findFirstOf(const String &s, size_t& fpos, size_t pos = 0) const;
    int findFirstOf(const char* cb, size_t& fpos, size_t pos = 0,
                    size_t n = NPOS) const;

    // functionality of strspn()

    int findFirstNotOf(const String& s, size_t& fpos, size_t pos = 0) const;
    int findFirstNotOf(const char* cb, size_t& fpos, size_t pos = 0,
                    size_t n = NPOS) const;

    // an equivalent to strtok is not provided, as this should be
    // the task of more powerful special classes

    //
    // Miscellaneous
    //

    // length

    size_t length() const;

    // copy to C buffer

    size_t copy(char* cb, size_t n, size_t pos = 0,
                    size_t len = NPOS) const;

    // get pointer to internal character array

    const char* cStr() const;

    // Capacity

    size_t reserve() const;
    void   reserve(size_t ic) const;
private:
    //
    //  private member variables
    //

    StringRep           *srep;

    //
    //  private member functions
    //

    void                doReplace(size_t pos, size_t len, const char *cb,
                        size_t n);
    const char*         getStr() const;
    int                 needClone(size_t n) const;
    void                refInc();
    void                refDec();
    //
    // private constructor for use in operator+
    //
                        String(const char*, size_t, const char*, size_t);
};


//
// inline section
//
inline
int String::needClone(size_t n) const
{
    int res = 0;

    if(srep == 0 || srep->refCount > 1 || n >= srep->alloc)
        res = 1;
    return res;
}

inline
size_t String::StringRep::getLen()
{
    assert(len < alloc);

    return len;
}

inline
void String::StringRep::setLen(size_t nlen)
{
    assert(nlen < alloc);

    len = nlen;
}

inline
void String::StringRep::refInc()
{
    refCount++;

    // assert(refCount != 0);

    // refCount is an size_t, so an overrun will rarely happen
    // with 16 bit size_t there can be more then 60000 shared strings
    // with 32 bit ints it can't really happen
}

inline
void String::StringRep::refDec()
{
    if(--refCount == 0)
        deleteSelf();
}
inline
void String::refInc()
{
    if(srep != 0)
        srep->refInc();
}

inline
void String::refDec()
{
    if(srep != 0)
        srep->refDec();
}

inline
size_t  String::length()  const
{
    return srep ? srep->getLen() : 0;
}

inline
String::String(const String& s)
{
    srep = s.srep;

    refInc();

    assert(length() == s.length());
    assert(memcmp(cStr(), s.cStr(), length()) == 0);
}

inline
char String::getAtRaw(size_t pos) const
{
    assert(pos < length());

    // the client knows exactly what he does !!

    return *(srep->str + pos);
}

inline
void String::putAtRaw(size_t pos, char c)
{
    assert(pos < length());

    // the client knows exactly what he does !!

    *(srep->str + pos) = c;
}



inline
String::String(Size_T ic)
{
    int len = ic.value();

    if(len == 0)
        srep = 0;
    else
        srep = StringRep::getNew(0, len + 1, 0);

    assert(length() == 0);
}

inline
String::String() : srep(0)
{
    assert(length() == 0);
}

inline
String::~String()
{
    refDec();
}

inline
String& String::operator=(const char* cs)
{
    return assign(cs);
}

inline
size_t  String::reserve() const
{
    return srep ? srep->alloc : 0;
}

inline
int String::compare(const String& s) const
{
    return compare(s.getStr(), s.length());
}

// == convenience inlines
//
inline int operator==(const String& s1, const String& s2)
{
    return s1.compare(s2) == 0;
}

inline int operator==(const String& s, const char* cs)
{
    return s.compare(cs, strlen(cs)) == 0;
}

inline int operator==(const char* cs, const String& s)
{
    return  s == cs;
}

// !=  convenience inlines
//
inline int operator!=(const String& s1, const String& s2)
{
    return s1.compare(s2) != 0;
}

inline int operator!=(const char* cs, const String& s)
{
    return s.compare(cs) != 0;
}

inline int operator!=(const String& s, const char* cs)
{
    return s.compare(cs) != 0;
}

inline char* String::StringRep::cPtr()
{
    assert(alloc > len);

    *(str + len) = '\0';

    return str;
}

#endif /* _GSTRING_H */ /* do not add stuff below this line */
