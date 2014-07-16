//
// String class: strngrep.c                             version: 0.9
//
// author: Uwe Steinmueller, SIEMENS NIXDORF Informationssysteme AG Munich
//         email: uwe.steinmueller@sniap.mchp.sni.de
//
// start: 28.08.92
//
// this source code is fully copyrighted but it is free in use for
// standardization purposes (X3J16 and ISO WG 21)
//
#include <stdlib.h>
#include <gstring.h>

//
// implementation String::StringRep
//

//
//  note: String::StringRep::~StringRep() is never called
//
void String::StringRep::deleteSelf()
{
    delete (void *) this;
}

//
// this defines one possible strategy for getting memory chunks
//
// this function is inlined to avoid macros and to seperate the
// chunk size strategy from the rest of the code
//
inline
static size_t chunkSize(size_t n)
{
    size_t newval;

    if(n < 64)
        newval = n * 2;         // have 100% reserved for appends
    else if(n < 256)
        newval = n * n/2;       // have 50% reserved for appends
    else
        newval = n + n/4;       // have 25% reserve space for appends

    if(newval < FIXLEN)
        newval = FIXLEN;
    if(newval < n)              // unsigned overflow !!
        newval = n;
    return newval;
}

String::StringRep* 
String::StringRep::getNew(size_t len, size_t allocLen, const char* source)
{
    String::StringRep *rep;
    size_t realAllocLength;

    assert(len < NPOS);         // should be checked by the clients of
                                // this function
    if(allocLen <= len)
        allocLen = len + 1;

    allocLen = chunkSize(allocLen);    // calculate chunk size

    realAllocLength =  allocLen + sizeof(String::StringRep) - FIXLEN;

    if(realAllocLength <= allocLen) {                 // unsigned overflow
        strError("String::StringRep::getNewStringRep", "OutOfMemory");
    }
    rep = (String::StringRep*) new char[realAllocLength];
    if(rep == 0)
        strError("String::StringRep::getNewStringRep", "OutOfMemory");
    rep->alloc  = allocLen;
    rep->refCount = 1;
    rep->len = len;
    if(source != 0)
        memcpy(rep->str, source, len);
    assert(rep != 0 && len < rep->alloc);
    return rep;
}

