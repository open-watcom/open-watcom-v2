#ifndef JAM_STRING_H
#define JAM_STRING_H

#include <iostream.h>
#include <string.h>
#include <assert.hpp>

//************************************************************************
// class JAM_String definition
//************************************************************************

class JAM_String {
public:
   JAM_String();

   JAM_String(const JAM_String& other); 

   JAM_String(const char* string); 

   ~JAM_String(); 

   JAM_String& operator=(const JAM_String& other); 
   JAM_String& operator=(const char* string); 

   JAM_String& operator+=(const JAM_String& other); 
   JAM_String& operator+=(const char* string); 

   friend int operator==(const JAM_String& s1, const JAM_String& s2);
   friend int operator==(const JAM_String& s1, const char* s2);

   friend int operator!=(const JAM_String& s1, const JAM_String& s2);
   friend int operator!=(const JAM_String& s1, const char* s2);

   friend int operator<(const JAM_String& s1, const JAM_String& s2);
   friend int operator<(const JAM_String& s1, const char* s2);

   friend int operator>(const JAM_String& s1, const JAM_String& s2);
   friend int operator>(const JAM_String& s1, const char* s2);

   friend JAM_String operator+(const JAM_String& s1, const JAM_String& s2);
   friend JAM_String operator+(const JAM_String& s1, const char* s2);

   const char& operator[](int index) const; 

   int length() const;

   int isblank() const { return length()==0; }

   const char* gets() const { return rep->s; }

   // use with caution -- do not modify returned string!
   char* cstr() const { return (char*)rep->s; }

   // input string until delim or EOF; delim is extracted but not stored
   static JAM_String getline(istream& is, int delim = '\n');

   // skip whitespace and read string until [^A-Za-z0-9_-]
   static JAM_String get_identifier(istream& is);

   friend ostream& operator<<(ostream& os, const JAM_String& string);
      
   friend istream& operator>>(istream& is, JAM_String& string);

protected:
   struct SRep {
      char* s;
      int num_refs;
      SRep() : num_refs(1) {}
      SRep(char* literal_string); 
   };
   static SRep nil_string;
   SRep* rep;
};


//************************************************************************
// JAM_String inlines
//************************************************************************

inline JAM_String::JAM_String()
{
   nil_string.num_refs++;
   rep = &nil_string;
}

inline JAM_String::JAM_String(const JAM_String& other)
{
   other.rep->num_refs++;
   rep = other.rep;
}

inline JAM_String::JAM_String(const char* string)
{
   rep = new SRep;
   rep->s = new char[strlen(string)+1];
   strcpy(rep->s, string);
}

inline JAM_String::~JAM_String()
{
   if (--rep->num_refs == 0) {
      delete [] rep->s;
      delete rep;
      }
}

inline JAM_String& JAM_String::operator=(const JAM_String& other)
{
   other.rep->num_refs++;        // latch to other rep first (s1=s1)
   if (--rep->num_refs == 0) {   // unlink from current rep
      delete [] rep->s;          // delete SRep if last user
      delete rep;
      }
   rep = other.rep;
   return *this;
}

inline JAM_String& JAM_String::operator+=(const JAM_String& other)
{
   return (*this) += other.rep->s;
}

inline const char& JAM_String::operator[](int index) const
{
   JAM_assert(index>=0 && index<length());
   return rep->s[index];
}

inline int JAM_String::length() const
{ return strlen(rep->s);
}

inline int operator==(const JAM_String& s1, const JAM_String& s2)
{ return s1.rep==s2.rep || strcmp(s1.rep->s, s2.rep->s)==0;
}

inline int operator==(const JAM_String& s1, const char* s2)
{ return strcmp(s1.rep->s, s2)==0;
}

inline int operator!=(const JAM_String& s1, const JAM_String& s2)
{ return !(s1==s2);
}

inline int operator!=(const JAM_String& s1, const char* s2)
{ return !(s1==s2);
}

inline int operator<(const JAM_String& s1, const JAM_String& s2)
{ return strcmp(s1.rep->s, s2.rep->s)<0;
}

inline int operator<(const JAM_String& s1, const char* s2)
{ return strcmp(s1.rep->s, s2)<0;
}

inline int operator>(const JAM_String& s1, const JAM_String& s2)
{ return strcmp(s1.rep->s, s2.rep->s)>0;
}

inline int operator>(const JAM_String& s1, const char* s2)
{ return strcmp(s1.rep->s, s2)>0;
}

inline ostream& operator<<(ostream& os, const JAM_String& string)
{ os << string.rep->s; return os;
}

#endif   // JAM_STRING_H

