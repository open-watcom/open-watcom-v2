/*
 * Version 1.6
 * Feeble attempt to duplicate perl associative arrays
 * So feeble I won't even call it PerlAssoc!
 * Anyway the key can only be a string, the value can be anything.
 * Written by Jim Morris,  jegm@sgi.com
 */
#ifndef _SPASSOC_H
#define _SPASSOC_H

#include <iostream.h>
#include "splash.h"

template<class T>
class Binar
{
private:
    SPString k;
    T v;

public:
    Binar(SPString a, T b) : k(a), v(b){}
    Binar(SPString a) : k(a){}
    Binar(){}
    
    Binar<T>& operator=(const Binar<T>& n){ k= n.k; v= n.v; return *this; }
    SPString& key(void){ return k; }
    const SPString& key(void) const { return k; }
    T& value(void){ return v; }
    const T& value(void) const { return v; }
    int operator==(const Binar<T>& b) const{return ((k == b.k) && (v == b.v));}    
    int operator<(const Binar& b) const {return v < b.v;} // to keep sort quiet
};

template<class T>
class Assoc
{
private:
    SPList<Binar<T> > dat;
    Binar<T> def;
    
public:
    Assoc():def(""){}
    Assoc(SPString dk, T dv) : def(dk, dv){}

    int scalar(void) const { return dat.scalar(); }
    
    SPStringList keys(void);
    SPList<T> values(void);
    
    int isin(const SPString& k) const;
    T adelete(const SPString& k);
        
    T& operator()(const SPString& k);
    Binar<T>& operator[](int i){ return dat[i]; }
};

template<class T>
SPStringList Assoc<T>::keys(void)
{
    SPStringList r;
    for(int i=0;i<dat.scalar();i++)
        r.push(dat[i].key());
    return r;
}    

template<class T>
SPList<T> Assoc<T>::values(void)
{
    SPList<T> r;
    for(int i=0;i<dat.scalar();i++)
        r.push(dat[i].value());
    return r;
}

template<class T>
T& Assoc<T>::operator()(const SPString& k)
{
    int i;
    for(i=0;i<dat.scalar();i++){
        if(k == dat[i].key()) return dat[i].value();
    }
    
    dat.push(Binar<T>(k, def.value()));
    return dat[i].value();
}

template<class T>
T Assoc<T>::adelete(const SPString& k)
{
    for(int i=0;i<dat.scalar();i++){
        if(k == dat[i].key()){
            T r= dat[i].value();
            dat.splice(i, 1);
            return r;
        }
    }
    
    return def.value();
}

template<class T>
int Assoc<T>::isin(const SPString& k) const
{
    for(int i=0;i<dat.scalar();i++){
        if(k == dat[i].key()) return i+1;
    }
    return 0;
}

template<class T>
ostream& operator<<(ostream& os, Binar<T>& a)
{
    os << "(" << a.key() << ", " << a.value() << ")";
    return os;
}

template<class T>
ostream& operator<<(ostream& os, Assoc<T>& a)
{
    for(int i=0;i<a.scalar();i++){
#ifdef  TEST
        os << "[" << i << "] " << a[i] << endl;
#else
        os << a[i] << endl;
#endif
    }
    return os;
}

#if 0
template<class T>
istream& operator>>(istream& s, Binar<T>& a)
{
char c= 0;

    s >> c;
    if(c == '('){
        s >> a.key()
    }
    os << "(" << a.key() << ", " << a.value() << ")";
    return os;
}
#endif
#endif
