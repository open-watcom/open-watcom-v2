/*
 * Version 1.4
 * Feeble attempt to duplicate perl associative arrays
 * So feeble I won't even call it PerlAssoc!
 * Anyway the key can only be a string, the value can be anything.
 * Written by Jim Morris,  jegm@sgi.com
 */
#ifndef _PERLASSOC_H
#define _PERLASSOC_H

#include <iostream.h>
#include "perlclas.h"

template<class T>
class Binar
{
private:
    PerlString k;
    T v;

public:
    Binar(PerlString a, T b) : k(a), v(b){}
    Binar(PerlString a) : k(a){}
    Binar(){}
    
    Binar<T>& operator=(const Binar<T>& n){ k= n.k; v= n.v; return *this; }
    PerlString& key(void){ return k; }
    const PerlString& key(void) const { return k; }
    T& value(void){ return v; }
    const T& value(void) const { return v; }
    int operator==(const Binar<T>& b) const{return ((k == b.k) && (v == b.v));}    
    int operator<(const Binar& b) const {return v < b.v;} // to keep sort quiet
};

template<class T>
class Assoc
{
private:
    PerlList<Binar<T> > dat;
    Binar<T> def;
    
public:
    Assoc():def(""){}
    Assoc(PerlString dk, T dv) : def(dk, dv){}

    int scalar(void) const { return dat.scalar(); }
    
    PerlStringList keys(void);
    PerlList<T> values(void);
    
    int isin(const PerlString& k) const;
    T adelete(const PerlString& k);
        
    T& operator()(const PerlString& k);
    Binar<T>& operator[](int i){ return dat[i]; }
};

template<class T>
PerlStringList Assoc<T>::keys(void)
{
    PerlStringList r;
    for(int i=0;i<dat.scalar();i++)
        r.push(dat[i].key());
    return r;
}    

template<class T>
PerlList<T> Assoc<T>::values(void)
{
    PerlList<T> r;
    for(int i=0;i<dat.scalar();i++)
        r.push(dat[i].value());
    return r;
}

template<class T>
T& Assoc<T>::operator()(const PerlString& k)
{
    int i;
    for(i=0;i<dat.scalar();i++){
        if(k == dat[i].key()) return dat[i].value();
    }
    
    dat.push(Binar<T>(k, def.value()));
    return dat[i].value();
}

template<class T>
T Assoc<T>::adelete(const PerlString& k)
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
int Assoc<T>::isin(const PerlString& k) const
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
        os << "[" << i << "] " << a[i] << endl;
    }
    return os;
}
#endif
