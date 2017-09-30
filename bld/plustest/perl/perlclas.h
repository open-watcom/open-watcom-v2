/*
 * Version 1.6
 * Written by Jim Morris,  jegm@sgi.com
 * Kudos to Larry Wall for inventing Perl
 * Copyrights only exist on the regex stuff, and all have been left intact.
 * The only thing I ask is that you let me know of any nifty fixes or
 * additions.
 * 
 * Credits:
 * I'd like to thank Michael Golan <mg@Princeton.EDU> for his critiques
 * and clever suggestions. Some of which have actually been implemented
 */

#ifndef _PERL_H
#define _PERL_H

#include <string.h>
#include "regexp.h"

#if     DEBUG
#include        <stdio.h>
#endif

#define INLINE  inline

// This is the base class for PerlList, it handles the underlying
// dynamic array mechanism

template<class T>
class PerlListBase
{
private:
    enum{ALLOCINC=20};
    T *a;
    int cnt;
    int first;
    int allocated;
    int allocinc;
    void grow(int amnt= 0, int newcnt= -1);

protected:
    void compact(const int i);

public:
#ifdef  USLCOMPILER
    // USL 3.0 bug with enums losing the value
    PerlListBase(int n= 20)
#else
    PerlListBase(int n= ALLOCINC)
#endif
    {
        a= new T[n];
        cnt= 0;
        first= n>>1;
        allocated= n;
        allocinc= n;
#       ifdef   DEBUG
        fprintf(stderr, "PerlListBase(int %d) a= %p\n", allocinc, a);
#       endif
    }

    PerlListBase(const PerlListBase<T>& n);
    PerlListBase<T>& PerlListBase<T>::operator=(const PerlListBase<T>& n);
    virtual ~PerlListBase(){
#       ifdef   DEBUG
        fprintf(stderr, "~PerlListBase() a= %p, allocinc= %d\n", a, allocinc);
#       endif
        delete [] a;
    }

    INLINE T& operator[](const int i);
    INLINE const T& operator[](const int i) const;

    int count(void) const{ return cnt; }

    void add(const T& n);
    void add(const int i, const T& n);
    void erase(void){ cnt= 0; first= (allocated>>1);}
};

// PerlList
class PerlStringList;

template <class T>
class PerlList: private PerlListBase<T>
{
public:

    PerlList(int sz= 10): PerlListBase<T>(sz){}

    // stuff I want public to see from PerlListBase
    T& operator[](const int i){return PerlListBase<T>::operator[](i);}
    const T& operator[](const int i) const{return PerlListBase<T>::operator[](i);}
    PerlListBase<T>::count;

    // add perl-like synonym
    void reset(void){ erase(); }
    int scalar(void) const { return count(); }

    operator void*() { return count()?this:0; } // so it can be used in tests
    int isempty(void) const{ return !count(); } // for those that don't like the above (hi michael)

    T pop(void)
    {
        T tmp;
        int n= count()-1;
        if(n >= 0){
            tmp= (*this)[n];
            compact(n);
        }
        return tmp;
    }

    void push(const T& a){ add(a);}
    void push(const PerlList<T>& l);

    T shift(void)
    {
        T tmp= (*this)[0];
        compact(0);
        return tmp;
    }
    
    int unshift(const T& a)
    {
        add(0, a);
        return count();
    }
    
    int unshift(const PerlList<T>& l);

    PerlList<T> reverse(void);
    PerlList<T> sort();
    
    PerlList<T> splice(int offset, int len, const PerlList<T>& l);
    PerlList<T> splice(int offset, int len);
    PerlList<T> splice(int offset);
};

// just a mechanism for self deleteing strings which can be hacked
class TempString
{
private:
    char *str;
public:
    TempString(const char *s)    
    {
        str= new char[strlen(s) + 1];
        strcpy(str, s);
    }
    
    TempString(const char *s, int len)    
    {
        str= new char[len + 1];
        if(len) strncpy(str, s, len);
        str[len]= '\0';
    }

    ~TempString(){ delete [] str; }

    operator char*() const { return str; }
};

/*
 * This class takes care of the mechanism behind variable length strings
 */

class VarString
{
private:
    enum{ALLOCINC=32};
    char *a;
    int len;
    int allocated;
    int allocinc;
    INLINE void grow(int n= 0);

public:
#ifdef  USLCOMPILER
    // USL 3.0 bug with enums losing the value
    INLINE VarString(int n= 32);
#else
    INLINE VarString(int n= ALLOCINC);
#endif

    INLINE VarString(const VarString& n);
    INLINE VarString(const char *);
    INLINE VarString(const char* s, int n);
    INLINE VarString(char);

    ~VarString(){
#       ifdef   DEBUG
        fprintf(stderr, "~VarString() a= %p, allocinc= %d\n", a, allocinc);
#       endif
        delete [] a;
    }

    VarString& operator=(const VarString& n);
    VarString& operator=(const char *);

    INLINE const char operator[](const int i) const;
    INLINE char& operator[](const int i);

    operator const char *() const{ return a; }

    int length(void) const{ return len; }

    void add(char);
    void add(const char *);
    void add(int, const char *);
    void remove(int, int= 1);

    void erase(void){ len= 0; }
};

class PerlStringList;
//
// Implements the perl specific string functionality 
//
class PerlString
{
private:
    VarString pstr;  // variable length string mechanism
    class substring;
    
public:
    
    PerlString():pstr(){}
    PerlString(const PerlString& n) : pstr(n.pstr){}     
    PerlString(const char *s) : pstr(s){}
    PerlString(const char c) : pstr(c){}
    PerlString(const substring& sb) : pstr(sb.pt, sb.len){}
    
    PerlString& operator=(const char *s){pstr= s; return *this;}        
    PerlString& operator=(const PerlString& n); 
    PerlString& operator=(const substring& sb);

    operator const char*() const{return pstr;}
    const char operator[](int n) const{ return pstr[n]; }

    int length(void) const{ return pstr.length(); }
    
    char chop(void);
    
    int index(const PerlString& s, int offset= 0);    
    int rindex(const PerlString& s, int offset= -1);
    substring substr(int offset, int len= -1);
    substring substr(const Range& r){ return substr(r.start(), r.length());}
        
    int m(const char *, const char *opts=""); // the regexp match m/.../ equiv
    int m(Regexp&);
    int m(const char *, PerlStringList&, const char *opts="");
    int m(Regexp&, PerlStringList&);
   
    int tr(const char *, const char *, const char *opts="");
    int s(const char *, const char *, const char *opts="");

    PerlStringList split(const char *pat= "[ \t\n]+", int limit= -1);
    
    int operator<(const PerlString& s) const { return (strcmp(pstr, s) < 0); }
    int operator>(const PerlString& s) const { return (strcmp(pstr, s) > 0); }
    int operator<=(const PerlString& s) const { return (strcmp(pstr, s) <= 0); }
    int operator>=(const PerlString& s) const { return (strcmp(pstr, s) >= 0); }
    int operator==(const PerlString& s) const { return (strcmp(pstr, s) == 0); }
    int operator!=(const PerlString& s) const { return (strcmp(pstr, s) != 0); }
    PerlString operator+(const PerlString& s) const;
    PerlString operator+(const char *s) const;
    PerlString operator+(char c) const;
    friend PerlString operator+(const char *s1, const PerlString& s2);

    PerlString& operator+=(const PerlString& s){pstr.add(s); return *this;}
    PerlString& operator+=(const char *s){pstr.add(s); return *this;}
    PerlString& operator+=(char c){pstr.add(c); return *this;}
    friend class substring;

private:
    void insert(int pos, int len, const char *pt, int nlen);

    // This idea lifted from NIH class library -
    // to handle substring LHS assignment
    // Note if subclasses can't be used then take external and make
    // the constructors private, and specify friend PerlString
    class substring
    {
    public:
        int pos, len;
        PerlString& str;
        char *pt;
    public:
        substring(PerlString& os, int p, int l) : str(os)
        {
            if(p > os.length()) p= os.length();
            if((p+l) > os.length()) l= os.length() - p;
            pos= p; len= l;
            if(p == os.length()) pt= 0; // append to end of string
            else pt= &os.pstr[p];
        }

        void operator=(const PerlString& s)
        {
            if(&str == &s){ // potentially overlapping
                VarString tmp(s);
                str.insert(pos, len, tmp, strlen(tmp));
            }else str.insert(pos, len, s, s.length());
        }
        
        void operator=(const substring& s)
        {
            if(&str == &s.str){ // potentially overlapping
                VarString tmp(s.pt, s.len);
                str.insert(pos, len, tmp, strlen(tmp));
            }else str.insert(pos, len, s.pt, s.len);
        }

        void operator=(const char *s)
        {
            str.insert(pos, len, s, strlen(s));
        }
    };
};

class PerlStringList: public PerlList<PerlString>
{
public:
    PerlStringList(int sz= 6):PerlList<PerlString>(sz){}
    // copy lists, need to duplicate all internal strings
    PerlStringList(const PerlStringList& n);

    PerlStringList& operator=(const PerlList<PerlString>& n);
 
    int split(const char *str, const char *pat= "[ \t\n]+", int limit= -1);
    PerlString join(const char *pat= " ");
    int m(const char *rege, const char *targ, const char *opts=""); // makes list of sub exp matches
    PerlStringList grep(const char *rege, const char *opts=""); // trys rege against elements in list
};

// This doesn't belong in any class
inline PerlStringList m(const char *pat, const char *str, const char *opts="")
{
PerlStringList l;
    
    l.m(pat, str, opts);
    l.shift(); // remove the first element which would be $&
    return l;
}

// Streams operators
template <class T>
istream& operator>>(istream& ifs, PerlList<T>& arr)
{
T a;
    // Should I reset arr first?
    arr.reset(); // I think so, to be consistent
    
    while(ifs >> a){
        arr.push(a);
//      cout << "<" << a << ">" << endl;
    };
    return ifs;    
}

template <class T>
ostream& operator<<(ostream& os,  const PerlList<T>& arr)
{

    for(int i=0;i<arr.count();i++){
#ifdef  TEST
        os << "[" << i << "]" << arr[i] << " ";
    }
    os << endl; 
#else
        os << arr[i] << endl;
    }
#endif
    return os;   
}

template <class T>
ostream& operator<<(ostream& os,  PerlList<T>& arr)
{

    for(int i=0;i<arr.count();i++){
#ifdef  TEST
        os << "[" << i << "]" << arr[i] << " ";
    }
    os << endl; 
#else
        os << arr[i] << endl;
    }
#endif
    return os;   
}

istream& operator>>(istream& ifs, PerlString& s);
istream& operator>>(istream& ifs, PerlStringList& sl);
ostream& operator<<(ostream& os,  const PerlString& arr);
ostream& operator<<(ostream& os,  const PerlStringList& arr);

// Implementation of template functions for perllistbase
template <class T>
INLINE T& PerlListBase<T>::operator[](const int i)
{
    assert((i >= 0) && (first >= 0) && ((first+cnt) <= allocated));
    int indx= first+i;
        
    if(indx >= allocated){  // need to grow it
        grow((indx-allocated)+allocinc, i+1); // index as yet unused element
        indx= first+i;                    // first will have changed in grow()
    }
    assert(indx >= 0 && indx < allocated);

    if(i >= cnt) cnt= i+1;  // it grew
    return a[indx];
}

template <class T>
INLINE const T& PerlListBase<T>::operator[](const int i) const
{
     assert((i >= 0) && (i < cnt));
     return a[first+i];
}

template <class T>
PerlListBase<T>::PerlListBase(const PerlListBase<T>& n)
{
    allocated= n.allocated;
    allocinc= n.allocinc;
    cnt= n.cnt;
    first= n.first;
    a= new T[allocated];
    for(int i=0;i<cnt;i++) a[first+i]= n.a[first+i];
#ifdef  DEBUG
    fprintf(stderr, "PerlListBase(PerlListBase&) a= %p, source= %p\n", a, n.a);
#endif

}

template <class T>
PerlListBase<T>& PerlListBase<T>::operator=(const PerlListBase<T>& n){
//  cout << "PerlListBase<T>::operator=()" << endl;
    if(this == &n) return *this;
#ifdef  DEBUG
    fprintf(stderr, "~operator=(PerlListBase&) a= %p\n", a);
#endif
    delete [] a; // get rid of old one
    allocated= n.allocated;
    allocinc= n.allocinc;
    cnt= n.cnt;
    first= n.first;
    a= new T[allocated];
    for(int i=0;i<cnt;i++) a[first+i]= n.a[first+i];
#ifdef  DEBUG
    fprintf(stderr, "operator=(PerlListBase&) a= %p, source= %p\n", a, n.a);
#endif
    return *this;
}

template <class T>
void PerlListBase<T>::grow(int amnt, int newcnt){
    if(amnt <= 0) amnt= allocinc; // default value
    if(newcnt < 0) newcnt= cnt;   // default
    allocated += amnt;
    T *tmp= new T[allocated];
    int newfirst= (allocated>>1) - (newcnt>>1);
    for(int i=0;i<cnt;i++) tmp[newfirst+i]= a[first+i];
#ifdef  DEBUG
    fprintf(stderr, "PerlListBase::grow() a= %p, old= %p, allocinc= %d\n", tmp, a, allocinc);
    fprintf(stderr, "~PerlListBase::grow() a= %p\n", a);
#endif
    delete [] a;
    a= tmp;
    first= newfirst;
}

template <class T>
void PerlListBase<T>::add(const T& n){
    if(cnt+first >= allocated) grow();
    a[first+cnt]= n;
    cnt++;
}

template <class T>
void PerlListBase<T>::add(const int ip, const T& n){
    assert(ip >= 0);
    if(first == 0 || (first+cnt) >= allocated) grow();
    assert((first > 0) && ((first+cnt) < allocated));
    if(ip == 0){ // just stick it on the bottom
        first--;
        a[first]= n;
    }else{
        for(int i=cnt;i>ip;i--) // shuffle up
            a[first+i]= a[(first+i)-1];
        a[first+ip]= n;
    }
    cnt++;
}

template <class T>
void PerlListBase<T>::compact(const int n){ // shuffle down starting at n
int i;
    assert((n >= 0) && (n < cnt));
    if(n == 0) first++;
    else for(i=n;i<cnt-1;i++){
            a[first+i]= a[(first+i)+1];
    }
    cnt--;
}


// implementation of template functions for perllist

template <class T>
void PerlList<T>::push(const PerlList<T>& l)
{
    for(int i=0;i<l.count();i++)
        add(l[i]);
}

template <class T>
int PerlList<T>::unshift(const PerlList<T>& l)
{
    for(int i=l.count()-1;i>=0;i--)
        unshift(l[i]);
    return count();
}

template <class T>
PerlList<T> PerlList<T>::reverse(void)
{
    PerlList<T> tmp;
    for(int i=count()-1;i>=0;i--)
        tmp.add((*this)[i]);
    
    return tmp; 
}

template <class T>
PerlList<T> PerlList<T>::sort(void)
{
PerlList<T> tmp(*this);
int n= tmp.scalar();

    for(int i=0;i<n-1;i++)
        for(int j=n-1;i<j;j--)
            if(tmp[j] < tmp[j-1]){
                T temp = tmp[j];
                tmp[j] = tmp[j-1];
                tmp[j-1]= temp;
            }
    
    return tmp; 
}

template <class T>
PerlList<T> PerlList<T>::splice(int offset, int len, const PerlList<T>& l)
{
PerlList<T> r= splice(offset, len);

    if(offset > count()) offset= count();
    for(int i=0;i<l.count();i++){
        add(offset+i, l[i]);    // insert into list
    }
    return r;
}

template <class T>
PerlList<T>  PerlList<T>::splice(int offset, int len)
{
PerlList<T> r;

    if(offset >= count()) return r;
    int i;
    for(i=offset;i<offset+len;i++){
        r.add((*this)[i]);
    }

    for(i=offset;i<offset+len;i++)
        compact(offset);
    return r;
}

template <class T>
PerlList<T>  PerlList<T>::splice(int offset)
{
PerlList<T> r;

    if(offset >= count()) return r;
    int i;
    for(i=offset;i<count();i++){
        r.add((*this)[i]);
    }

    int n= count(); // count() will change so remember what it is
    for(i=offset;i<n;i++)
        compact(offset);
    return r;
}

// VarString Implementation
INLINE VarString::VarString(int n)
{
    a= new char[n];
    *a= '\0';
    len= 0;
    allocated= n;
    allocinc= n;
#   ifdef       DEBUG
    fprintf(stderr, "VarString(int %d) a= %p\n", allocinc, a);
#   endif
}

INLINE VarString::VarString(const char* s)
{
    int n= strlen(s) + 1;
    a= new char[n];
    strcpy(a, s);
    len= n-1;
    allocated= n;
    allocinc= ALLOCINC;
#   ifdef       DEBUG
    fprintf(stderr, "VarString(const char *(%d)) a= %p\n", allocinc, a);
#   endif
}

INLINE VarString::VarString(const char* s, int n)
{
    a= new char[n+1];
    if(n) strncpy(a, s, n);
    a[n]= '\0';
    len= n;
    allocated= n+1;
    allocinc= ALLOCINC;
#   ifdef       DEBUG
    fprintf(stderr, "VarString(const char *, int(%d)) a= %p\n", allocinc, a);
#   endif
}

INLINE VarString::VarString(char c)
{
    int n= 2;
    a= new char[n];
    a[0]= c; a[1]= '\0';
    len= 1;
    allocated= n;
    allocinc= ALLOCINC;
#   ifdef       DEBUG
    fprintf(stderr, "VarString(char (%d)) a= %p\n", allocinc, a);
#   endif
}


INLINE ostream& operator<<(ostream& os,  const VarString& arr)
{
#ifdef TEST
    os << "(" << arr.length() << ")" << (const char *)arr;
#else
    os << (const char *)arr;
#endif
        
    return os;    
}

INLINE const char VarString::operator[](const int i) const
{
     assert((i >= 0) && (i < len) && (a[len] == '\0'));
     return a[i];
}

INLINE char& VarString::operator[](const int i)
{
     assert((i >= 0) && (i < len) && (a[len] == '\0'));
     return a[i];
}

INLINE VarString::VarString(const VarString& n)
{
    allocated= n.allocated;
    allocinc= n.allocinc;
    len= n.len;
    a= new char[allocated];
    strcpy(a, n.a);
#ifdef  DEBUG
    fprintf(stderr, "VarString(VarString&) a= %p, source= %p\n", a, n.a);
#endif

}
#endif

