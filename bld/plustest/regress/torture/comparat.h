/****************************************************************************
File: Comparator.h

Description:  class JAM_Comparator<T>


Usage:

Notes:


History:
25 Mar 1992 Jam      created, referenced CPL2
06 Oct 1992 Jam      added specific defs for |char*| and |const char*|

****************************************************************************/
#ifndef JAM_Comparator_H
#define JAM_Comparator_H

#include <string.h>     // for strcmp()

//**************************************************************************
// JAM_Comparator definition
//**************************************************************************

template<class T> struct JAM_Comparator {
   inline static int lessthan(const T& a, const T& b) { return a<b; }
   inline static int equal(const T& a, const T& b)    { return a==b; }
};

struct JAM_Comparator<char*> {
   inline static int lessthan(char* const& a, char* const& b)
      { return strcmp(a,b)<0; }
   inline static int equal(char* const& a, char* const& b)
      { return strcmp(a,b)==0; }
};

struct JAM_Comparator<const char*> {
   inline static int lessthan(const char* const& a, const char* const& b)
      { return strcmp(a,b)<0; }
   inline static int equal(const char* const& a, const char* const& b)
      { return strcmp(a,b)==0; }
};

#endif // JAM_Comparator_H
