// This is in the public domain without warranties expressed or implied.
//
// Created: JAM 08/21/92 -- Initial design and implementation (ref. Stroustrup)
//
// The Comparator class template allows other class templates (eg, Range)
// to compare values of the type they are instantiated with without
// requiring that the values have the "<" and ">" operators defined.
//

#ifndef COMPARATORH
#define COMPARATORH

#include <string.h>        // for strcmp()

template<class T> struct CoolComparator {
   inline static int lessthan(const T& a, const T& b) { return a<b; }
   inline static int equal(const T& a, const T& b)    { return a==b; }
};

// define char* Comparator since so common
template<>
struct CoolComparator<char*> {
   static int lessthan(char* a, char* b) { return strcmp(a,b)<0; }
   static int equal(char* a, char* b) { return strcmp(a,b)==0; }
};

#endif // COMPARATORH

