//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Created: MBN 04/04/89 -- Initial design and implementation
// Updated: MBN 12/15/89 -- Made case-flag optional on is_equal, is_not_equal
// Updated: LGO 01/05/90 -- Split into seperate files
// Updated: MJF 05/22/90 -- Fixed is_eqaul to return properly
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #include
//

#include <ctype.h>              // Include character processing macros
#include <cool/char.h>          // Include char* specification header file

Boolean is_equal (const char* c1, const char* c2, Boolean case_flag) {
  if (case_flag == SENSITIVE) {                 // Case sensitive
    for ( ; *c1 == *c2; c1++, c2++)             // For each character in string
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    return (*c1 == *c2) ? TRUE : FALSE;         // Return match result
  }
  else {                                        // Case insensitive
    for ( ; *c1 == *c2 || TO_UPPER (*c1) == TO_UPPER (*c2); c1++, c2++)
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    // Return match result
    return (*c1 == *c2 || TO_UPPER (*c1) == TO_UPPER (*c2)) ? TRUE : FALSE;
  }
}


Boolean is_not_equal (const char* c1, const char* c2, Boolean case_flag) {
  return (!is_equal (c1, c2, case_flag));
}


Boolean is_equal_n (const char* c1, const char* c2, int n, Boolean case_flag) {
  if (case_flag == SENSITIVE) {                 // Case sensitive
    for ( ; --n >= 0 && *c1 == *c2; c1++, c2++) // For each character in string
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    return n<0;                                 // Return match result
  }
  else {                                        // Case insensitive
    for ( ; --n >= 0 && *c1 == *c2 || TO_UPPER (*c1) == TO_UPPER (*c2);
        c1++, c2++)
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    return n<0;                                 // Return match result
  }
}


Boolean is_ge (const char* c1, const char* c2, Boolean case_flag) {
  if (case_flag == SENSITIVE) {                 // Case sensitive
    for (; *c1 == *c2; c1++, c2++)              // For each character in string
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    return (*c1 > *c2) ? TRUE : FALSE;          // Return match result
  }
  else {                                        // Case insensitive
    for (;*c1 == *c2 || TO_UPPER (*c1) == TO_UPPER (*c2); c1++, c2++)
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    // Return match result
    return (TO_UPPER (*c1) > TO_UPPER (*c2)) ? TRUE : FALSE;
  }
}


Boolean is_lt (const char* c1, const char* c2, Boolean case_flag) {
  return (!is_ge (c1, c2, case_flag));
}


Boolean is_le (const char* c1, const char* c2, Boolean case_flag) {
  if (case_flag == SENSITIVE) {                 // Case sensitive
    for (; *c1 == *c2; c1++, c2++)              // For each character in string
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    return (*c1 < *c2) ? TRUE : FALSE;          // Return match result
  }
  else {                                        // Case insensitive
    for (; *c1 == *c2 || TO_UPPER (*c1) == TO_UPPER (*c2); c1++, c2++) 
      if (*c1 == END_OF_STRING)                 // If end of first string
        return TRUE;                            // Match found, return TRUE
    // Return match result
    return (TO_UPPER (*c1) < TO_UPPER (*c2)) ? TRUE : FALSE;
  }
}


Boolean is_gt (const char* c1, const char* c2, Boolean case_flag) {
  return (!is_le (c1, c2, case_flag));
}

// c_capitalize -- Capitalize all words in a string. A word is defined as
//                 a sequence of characters separated by non-alphanumerics
// Input:          Character string
// Output:         Updated string

char* c_capitalize (char* s) {          // Capitalize each word in string
  char* p = s;                          // Point to beginning of string
  for (;;) {                            // Infinite loop
    for (; *p && !isalnum(*p); p++);    // Skip to first alphanumeric
    if (*p == END_OF_STRING)            // If end of string
      return s;                         // Return string
    *p = TO_UPPER(*p);                  // Convert character
    while(*++p && isalnum (*p));        // Search for next word
  }
}


// c_downcase -- Convert all alphabetical characters to lowercase
// Input:        Character string
// Output:       Updated string

char* c_downcase (char* s) {    // Convert entire string to lower case
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (isupper (*p))           // if this is upper case
      *p = TO_LOWER (*p);       // convert to lowercase 
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}



// c_left_trim -- Removes any occurrence of the character(s) in "rem" from
//                "str" that appear as a prefix to the string. The first
//                non-matching character encountered terminates the remove
//                operation and the rest of the string is copied intact.
// Input:         Source string and token string
// Output:        Modified string "str" (string modified in place)

char* c_left_trim (char* str, const char* rem) { // Trim prefix from string
  char* result = str;
  char* s;
  register char c;
  for (s=str; (c=*s) != END_OF_STRING; s++) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  if (s != result)                                // when characters trimed
    while ((*result++ = *s++) != END_OF_STRING); // shift string down
  return str;                                     // Return pointer to string
}


// c_right_trim -- Removes any occurrence of the character(s) in "rem" from
//                 "str" that appear as a suffix to the string. The first
//                 non-matching character encountered terminates the remove
//                 operation and the rest of the string is copied intact.
// Input:          Source string and token string
// Output:         Modified string "str" (string modified in place)

char* c_right_trim (char* str, const char* rem) { // Trim suffix from string
  char* s = str + strlen(str) - 1;                // last character of str
  for (; s >= str; s--) {
    register const char* r = rem;
    register char t;
    register char c = *s;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      break;
  }
  *(s+1) = END_OF_STRING;
  return str;                                     // Return pointer to string
}


// c_trim -- Removes any occurrence of the character(s) in "rem" from "str"
// Input:    Source string and token string
// Output:   Source string (string is modified in place)

char* c_trim (char* str, const char* rem) {     // Trim characters from string
  char* s = str;
  char* result = str;
  register char c;
  while ((c=*s++) != END_OF_STRING) {
    register const char* r = rem;
    register char t;
    while ((t=*r++) != END_OF_STRING && t != c); // Scan for match
    if (t == END_OF_STRING)                      // If no match found
      *result++ = c;
  }
  *result = END_OF_STRING;                      // NULL terminate string
  return str;                                   // Return pointer to string
}


// c_upcase -- Convert all alphabetical characters to uppercase
// Input:      Character string 
// Output:     Updated string
//

char* c_upcase (char* s) {      // Convert entire string to upper case
  char* p = s;                  // Point to beginning of string
  while (*p) {                  // While there are still valid characters
    if (islower (*p))           // if this is lower case
      *p = TO_UPPER (*p);       // convert to uppercase 
    p++;                        // Advance pointer
  }
  return s;                     // Return reference to modified string
}

// strfind -- finds the pattern in the source.  Sets start and
// end accordingly and returns a pointer to the beginning of the
// string found, or NULL.  Uses * to mean any number of any characters,
// and ? to mean one of any character.  \? and \* are used in the 
// pattern to look for the actual symbols ? and * in the source.
// Finds the first occurance from the beginning of the source string.
//


const char* strfind (const char* string, const char* pattern,
                     long* start, long* end)
{
  const char* stringp = string;
  const char* startp = string;
  for (; *pattern != '\0'; pattern++, string++) {
    if (*pattern == '\\') {                     // Special case \* and \?
      pattern++;
      if (*pattern == '*' || *pattern == '?') {
        if (*string != *pattern)
          return NULL;
      }
      else if (*string != '\\')
        return NULL;
    }
    else if (*pattern == '*') {
      long endp;
      pattern++;
      if (*pattern == '\0')
        goto match;
      while (strfind(string, pattern, (long *)NULL, &endp) == NULL) {
        if (*string++ == '\0')
          return NULL;
      }
      startp = string;
      string += endp;
      goto match;
    }
    else if (*string == '\0')
      return NULL;
    else if ((*pattern != '?') && (*pattern != *string))
      return NULL;
  }
  if (*string != '\0') return NULL;
 match:                                         // Match found
  if (start != NULL) *start = startp - stringp;
  if (end != NULL) *end = string - stringp;
  return(startp);
}


// strrfind -- finds the pattern in the source.  Sets start and
// end accordingly and returns a pointer to the beginning of the
// string found, or NULL.  Uses * to mean any number of any characters,
// and ? to mean one of any character.  \? and \* are used in the 
// pattern to look for the actual symbols ? and * in the source.
// Finds the last occurance from the end of the source string.


static const char* _strrfind(const char* strstart,
                             const char* string,
                             const char* patstart,
                             const char* pattern,
                             long* end)
{
  const char* startp = string;
  for (; pattern >= patstart; pattern--, string--) {
    if (*pattern == '*') {              // '*' matches anything or nothing
      --pattern;
      if (pattern >= patstart && *pattern == '\\') { // Special case \*
        if (string < strstart || *string != '*')
          return NULL;
      }
      else {
        const char* res;
        if (pattern < patstart) {
          string++;
          goto match;
        }
        while ((res=_strrfind(strstart,string,patstart,pattern,end)) == NULL) {
          if (--string < strstart)
            return NULL;
        }
        startp = string;
        string = res;
        goto match;
      }
    }
    else if (string < strstart)                 // check end of string
      return NULL;
    else if (*pattern == '?') {                 // '?' matches any character
      if (pattern > patstart && *(pattern-1) == '\\') { // Special case \?
        pattern--;
        if (*string != '?')
          return NULL;
      }
    }
    else if (*pattern != *string)
      return NULL;
  }
  if (++string != strstart) return NULL;
 match:                                         // Match found
  *end = startp - strstart + 1;
  return(string);
}


const char* strrfind (const char* string, const char* pattern,
                      long* start, long* end)
{
  long endp;
  const char* res = _strrfind(string, string + strlen(string) - 1,
                              pattern, pattern + strlen(pattern) - 1, &endp);
  if (res != NULL) {
    if (start != NULL) *start = res - string;
    if (end != NULL) *end = endp;
  }
  return(res);
}

// strndup -- creates a new string and copies everything between
// zero and length of the given char* into it.  Returns the new
// string.
//

char* strndup (const char* s, long length) {
  if (length<0) return(NULL);
  char* ret = (char*) new char[length+1];
  char* retp = ret;  
  while(length-- > 0 && *s != END_OF_STRING)
    *retp++ = *s++;
  *retp = END_OF_STRING;
  return(ret);
}


// strnremove -- removes everything between zero and end from 
// the given string.  Returns a pointer to the new string.

char* strnremove (char* s, long end) {
  int len = strlen(s);
  if (end < 0 ) return(NULL);
  if (end > len) end = len;
  for(int ind=0; s[ind]=s[end]; ind++) end++;
  return(s);
}

//
// strnyank -- the equivalent of doing a strndup and a strnremove.
// Returns a char* to the copy of zero-end elements of the given string.
// Modifies the given string by removing the zero-end elements.
//

char* strnyank (char* s, long end) {
  int len = strlen(s);
  if (end>len) end=len;
  long mark = end;
  char* ret = (char*) new char[end+1];
  if (end < 0) return(NULL);
  for(int ind=0; ind <= len; ind++) {
    if (ind<mark) ret[ind]=s[ind];
    s[ind]=s[end];
    end++;
  }
  ret[mark]=END_OF_STRING;
  return(ret);
}


// reverse -- Reverse the order of the characters in char*
// Input:     char* 
// Output:    char* with character order reversed

void reverse (char* c) {                // Reverse the order of characters
  int length = strlen (c);              // Number of characters in string
  char temp;

  for (int i = 0, j = length-1;         // Counting from front and rear
       i < length / 2; i++, j--) {      // until we reach the middle
    temp = c[i];                        // Save front character
    c[i] = c[j];                        // Switch with rear character
    c[j] = temp;                        // Copy new rear character
  }
}
