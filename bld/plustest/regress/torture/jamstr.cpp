#include <iostream.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <jamstr.h>


//************************************************************************
// JAM_String::SRep definitions
//************************************************************************

JAM_String::SRep JAM_String::nil_string("");

// currently only used by nil_string to force s to never be deleted
JAM_String::SRep::SRep(char* literal_string)
   : s(literal_string), num_refs(2)
{
}

//************************************************************************
// JAM_String definitions
//************************************************************************

JAM_String& JAM_String::operator=(const char* string)
{
   char* new_string = new char[strlen(string) + 1];
   strcpy(new_string, string);
   if (rep->num_refs > 1) {
      rep->num_refs--;
      rep = new SRep;
      }
   else {
      delete [] rep->s;
      }
   rep->s = new_string;
   return *this;
}


JAM_String& JAM_String::operator+=(const char* string)
{
   char* new_string = new char[length() + strlen(string) + 1];
   sprintf(new_string, "%s%s", rep->s, string);
   if (rep->num_refs > 1) {
      rep->num_refs--;
      rep = new SRep;
      }
   else {
      delete [] rep->s;
      }
   rep->s = new_string;
   return *this;
}

istream& operator>>(istream& is, JAM_String& string)
{
   is >> ws;                     // eat white space
   if (!is.good()) return is;    // return if no hope of inputting from is

   const int BUFFER_SIZE = 81;

   char* first_char = new char[BUFFER_SIZE];
   if (first_char==0) return is; // should set error state
   char* current = first_char;
   char* last_char = current + BUFFER_SIZE - 1;
   *current = '\0';
   char c;

   while (is.get(c)) {
      if (isspace(c)) {    // encountered terminator
         is.putback(c);    // leave stream at terminator
         break;
         }
      else {               // encountered string character
         if (current==last_char) {
            *current = '\0';
            ptrdiff_t current_size = last_char - first_char + 1;
            char* new_string = new char[current_size+BUFFER_SIZE];
            if (new_string==0) {
               delete [] first_char;
               return is; // should set error state
               }
            strcpy(new_string, first_char);
            delete [] first_char;
            first_char = new_string;
            current = first_char + current_size - 1;
            last_char = current + BUFFER_SIZE;
            }
         *current = c;
         ++current;
         }
      }
   *current = 0;

   if (is.bad()) {      // if unsuccessful read delete newstring
      delete [] first_char;
      }
   else {               // else if successful read, change string
      if (string.rep->num_refs > 1) {
         string.rep->num_refs--;
         string.rep = new JAM_String::SRep;
         }
      else {
         delete [] string.rep->s;
         }
      string.rep->s = first_char;
      }

   return is;
}

JAM_String JAM_String::getline(istream& is, int delim)
{
   const int BUFFER_SIZE = 81;

   char* first_char = new char[BUFFER_SIZE];
   if (first_char==0) return JAM_String(); // should set error state
   char* current = first_char;
   char* last_char = current + BUFFER_SIZE - 1;
   *current = '\0';
   char c;

   while (is.get(c)) {
      if (c==delim) {      // encountered and ate terminator
         break;
         }
      else {               // encountered string character
         if (current==last_char) {
            *current = '\0';
            ptrdiff_t current_size = last_char - first_char + 1;
            char* new_string = new char[current_size+BUFFER_SIZE];
            if (new_string==0) {
               delete [] first_char;
               return JAM_String(); // should set error state
               }
            strcpy(new_string, first_char);
            delete [] first_char;
            first_char = new_string;
            current = first_char + current_size - 1;
            last_char = current + BUFFER_SIZE;
            }
         *current = c;
         ++current;
         }
      }
   *current = 0;

   JAM_String string;

   if (is.bad()) {      // if unsuccessful read delete newstring
      delete first_char;
      }
   else {               // else if successful read, change string
      if (string.rep->num_refs > 1) {
         string.rep->num_refs--;
         string.rep = new SRep;
         }
      else {
         delete [] string.rep->s;
         }
      string.rep->s = first_char;
      }
   return string;
}

JAM_String JAM_String::get_identifier(istream& is)
{
   is >> ws;               // eat white space

   const int BUFFER_SIZE = 32;

   char* first_char = new char[BUFFER_SIZE];
   if (first_char==0) return JAM_String(); // should set error state
   char* current = first_char;
   char* last_char = current + BUFFER_SIZE - 1;
   *current = '\0';
   char c;

   while (is.get(c)) {
      if (!(isalnum(c) || c=='_' || c=='-')) {     // encountered terminator
         is.putback(c);    // leave stream at terminator
         break;
         }
      else {               // encountered string character
         if (current==last_char) {
            *current = '\0';
            size_t current_size = last_char - first_char + 1;
            char* new_string = new char[current_size+BUFFER_SIZE];
            if (new_string==0) {
               delete [] first_char;
               return JAM_String(); // should set error state
               }
            strcpy(new_string, first_char);
            delete [] first_char;
            first_char = new_string;
            current = first_char + current_size - 1;
            last_char = current + BUFFER_SIZE;
            }
         *current = c;
         ++current;
         }
      }
   *current = 0;

   JAM_String string;

   if (is.bad()) {      // if unsuccessful read delete newstring
      delete first_char;
      }
   else {               // else if successful read, change string
      if (string.rep->num_refs > 1) {
         string.rep->num_refs--;
         string.rep = new SRep;
         }
      else {
         delete [] string.rep->s;
         }
      string.rep->s = first_char;
      }
   return string;
}

JAM_String operator+(const JAM_String& s1, const JAM_String& s2)
{
   JAM_String result(s1);
   result += s2;
   return result;
}

JAM_String operator+(const JAM_String& s1, const char* s2)
{
   JAM_String result(s1);
   result += s2;
   return result;
}
