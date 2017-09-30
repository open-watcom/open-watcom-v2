



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */





#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <iostream.h>

#include "base/string.h"
#include "base/strsplit.h"
#include "base/bytstrng.h"
#include "base/error.h"
#include "base/strgseq.h"
#include "base/binding.h"
#include "base/stream.h"




#ifdef DEBUG
#include "base/memory.h"
#endif

#if defined(__BORLANDC__) && !defined(__OS2__)
#define STRNCMP _fstrncmp
#else
#define STRNCMP strncmp
#endif


#ifdef __DEC_ULTRIX__
extern "C" int gcvt (double, int, char*);
#endif

#ifdef __GNUC__
#pragma implementation
#endif



#define NEW_OP new



const short DEFAULT_START_SIZE = 12; // Must be at least 10 to
                                     // accommodate long integers


// Some utility functions

void int_to_as (long val, char out[], short minwidth, char padChar = ' ')
{
    char tmp[50]; // Won't have more than this many digits
    short i, n, sign = 0;

    if (val == 0) {
        n = (short) maxl (minwidth, 1);
        for (i = 0; i < n-1; i++)
            out[i] = padChar;
        out[n-1] = '0';
        out[n] = '\0';
        return;
    }
    if (val < 0) {
        sign = -1;
        val = -val;
    }

    for (n = 0; val > 0; n++) {
        tmp[n] = (char) (val % 10 + '0');
        val /= 10;
    }
    if (sign < 0) minwidth--;
    if (padChar != ' ') {
        for (; n < minwidth; n++) {
            tmp[n] = padChar;
        }
        if (sign == -1) 
            tmp[n++] = '-';
    }
    else {
        if (sign == -1) 
            tmp[n++] = '-';
        for (; n < minwidth; n++) {
            tmp[n] = padChar;
        }
    }
    for (i = n-1; i >= 0; i--) {
        out[n-1-i] = tmp[i];
    }
    out [n] = '\0';
}


#ifdef __GNUC__
// G++ does not have the strstr function for finding substring
// occurrences. So here we implement the naive algorithm for string
// search.
static char* strstr (char* s1, const char* s2)
{
    if (!s1 || !s2)
        return NULL;
    long m = strlen (s1);
    long n = strlen (s2);
    long i, j;
    for (i = 0; i < m-n+1; i++) {
        for (j = 0; j < n; j++)
            if (s2[j] != s1[i+j])
                break;
        if (j == n)
            return s1+i;
    }
    return NULL;
}
#endif        

    
        
CL_DEFINE_CLASS(CL_String, _CL_String_CLASSID);

CL_String::CL_String () 
{
    _Init (DEFAULT_START_SIZE);
}


CL_String::CL_String (const char* strg)
{
    if (strg != NULL) {
        long n = strlen (strg);
        if (!_Init (n + DEFAULT_START_SIZE + 1))
            return;
        strcpy (_string, strg);
        _size = n;
    }
    else
        _Init (DEFAULT_START_SIZE+1);
}


CL_String::CL_String (const CL_String& strg)
{
    long n = strg.Size();
    if (!_Init (n + DEFAULT_START_SIZE + 1))
        return;
    *this = strg;
    _size = n;
}


CL_String::CL_String (long value, short minwidth, char padChar)
{
    if (!_Init ((minwidth > DEFAULT_START_SIZE ? minwidth :
                DEFAULT_START_SIZE) + 1))
        return; 
    int_to_as (value, _string, minwidth, padChar);
    _size = strlen (_string);
}


CL_String::CL_String (short value, short minwidth, char padChar)
{
    if (!_Init ((minwidth > DEFAULT_START_SIZE ? minwidth :
                DEFAULT_START_SIZE) + 1))
        return; 
    int_to_as ((long) value, _string, minwidth, padChar);
    _size = strlen (_string);
}


CL_String::CL_String (CL_ByteArray& b)
{
    char* b_ptr = (char*) b.AsPtr();
    char* p = b_ptr;
    long i;
    long n = b.Size();
    for (i = 0; i < n; i++, p++)
        if (*p == 0)
            break;
    if (!_Init (i))
        return;
    strncpy (_string, b_ptr, i);
    _string[i] = '\0';
    _size = i;
}




CL_String::CL_String (char c, short count)
{
    if (!_Init (count+1))
        return;
    for (short i=0; i < count; i++)
        _string[i] = c;
    _string[count] = '\0';
    _size = strlen (_string); // Might not equal count, if
                              // c is the null character!
}



    
    
CL_String::~CL_String()
{
    if (_string)
        delete [] _string;
}



long CL_String::AsLong() const
{
    short i = 0, sign = 1;
    long value = 0;

    while (i < Size() && _string[i] == ' ') i++;
    if (i >= Size())
        return 0;
    if (_string[i] == '-') {
        i++;
        sign = -1;
    }
    while (_string[i] >= '0' && _string[i] <= '9') {
        value = value * 10 + _string[i] - '0';
        i++;
    }
    return value * sign;
}


double CL_String::AsDouble () const
{
    return atof (_string);
}



bool CL_String::Insert (const char* p, long pos)
{
    if (!PrepareToChange())
        return FALSE;
    if (!_DoInsert (p, pos))
        return FALSE;
    Notify();
    return TRUE;
}


bool CL_String::_DoInsert (const char* p, long pos)
{
    if (pos < -1 || pos >= _size)
        return FALSE;
    long n = strlen (p);
    if (n <= 0)
        return TRUE;
    if (n + _size >= _capacity) {
        // Not enough room: expand the string
        long new_cap = _capacity + n + DEFAULT_START_SIZE;
        char* new_data = new char[new_cap];
        if (!new_data)
            return FALSE;
        _capacity = new_cap;
        strncpy (new_data, _string, pos+1);
        strncpy (new_data + pos + 1, p, n);
        if (pos < _size - 1)
            strcpy  (new_data + pos + n + 1, _string + pos + 1);
        else
            new_data [_size + n] = '\0';
        delete [] _string;
        _string = new_data;
    }
    else {
        if (pos < _size - 1) {
            memmove (_string+pos+n+1, _string+pos+1, _size-pos-1);
            memcpy  (_string+pos+1, p, n);
        }
        else
            strcpy (_string + _size, p);
    }
    _size += n;
    return TRUE;
}


bool CL_String::Insert (char c, long pos)
{
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    return Insert (buf, pos);
}



bool CL_String::PadTo (long num_chars, char pad_char,
                       bool left_justified)
{
    if (!PrepareToChange())
        return FALSE;
    if (num_chars <= _size)
        return FALSE;
    char* pad_strg = new char [_capacity = num_chars+DEFAULT_START_SIZE+1];
    if (!pad_strg)
        return FALSE;
    memset (pad_strg, pad_char, num_chars);
    pad_strg[num_chars]  = '\0';
    if (left_justified) 
        strncpy (pad_strg, _string, _size);
    else
        strncpy (pad_strg + num_chars - _size,
                 _string, _size);
    delete _string;
    _string = pad_strg;
    _size = num_chars;
    Notify ();
    return TRUE;
}



CL_String CL_String::InUpperCase () const
{
    CL_String t (*this);
    t.ToUpper ();
    return t;
}


CL_String CL_String::InLowerCase () const
{
    CL_String t (*this);
    t.ToLower ();
    return t;
}


long CL_String::ToUpper ()
{
    if (!PrepareToChange())
        return 0;
    long count = 0;
    for (long i = 0; i < _size; i++)
        if (islower (_string[i])) {
            _string[i] = toupper (_string[i]);
            count++;
        };
    Notify ();
    return count;
}


long CL_String::ToLower ()
{
    if (!PrepareToChange())
        return 0;
    long count = 0;
    for (long i = 0; i < _size; i++)
        if (isupper (_string[i])) {
            _string[i] = tolower (_string[i]);
            count++;
        };
    Notify ();
    return count;
}


bool CL_String::WordCapitalize ()
{
    if (!PrepareToChange())
        return FALSE;
    if (_size <= 0 || !isalpha (_string[0]))
        return FALSE;
    if (islower (_string[0]))
        _string[0] = toupper (_string[0]);
    for (long i = 1; i < _size; i++) {
        if (!isalpha (_string[i]))
            break;
        if (isupper (_string[i]))
            _string[i] = tolower (_string[i]);
    }
    Notify ();
    return TRUE;
}


        

//
// Substructure extraction
//


long CL_String::CharIndex (char c, long pos) const
{
    for (long i=pos; i < _size; i++) {
        if (_string[i] == c) return i;
    }
    return -1;
}



long CL_String::NCharIndex (char c, long pos) const
{
    for (long i=pos; i < _size; i++) {
        if (_string[i] != c) return i;
    }
    return -1;
}


CL_Substring CL_String::operator () (long position, long size)
{
    if (position >= _size)
        return CL_Substring (*this, _size, 0);
    long len = minl (size,  _size-position);
    return CL_Substring (*this, position, len);
    // return sub;
}



         
long CL_String::CharIndex (const char* s, long pos)  const
{
    if (!s)
        return 0;
    long n = strlen(s);

    for (long i=pos; i < _size; i++) {
        for (long j = 0; j < n; j++)
            if (_string[i] == s[j]) return i;
    }
    return -1;
}



long CL_String::NCharIndex (const char* s, long pos) const
{
    if (!s)
        return 0;
    long n = strlen(s);
    for (long i = pos; i < _size; i++) {
        long j;
        for (j = 0; j < n; j++) {
            if (_string[i] == s[j]) break;
        }
        if (j >= n) return i;
    }
    return -1;
}
    

bool CL_String::IsPrefixOf (const char* p) const 
{
    if (!p)
        return FALSE;
    long l = strlen (p);
    
    if (_size > l)
        return FALSE;
    short b = STRNCMP (_string, p, _size);
    return (b == 0) ? TRUE : FALSE;
}


long CL_String::Index (const char* p, long pos, long n) const
{
    if (!p || !_string)
        return -1;
    long len = strlen(p);
    if (pos > _size - len)
        return -1;
    char* q = _string + pos;
    for (; n > 0; n--) {
        q = strstr (q, p);
        if (q == NULL)
            return -1;
        if (n > 1) q++;
    }
    return (q == NULL) ? -1 : (q - _string);
}




long CL_String::Replace (const char* s1, const char* s2, long n, long pos)
{
    if (!PrepareToChange())
        return 0;
    long len1, len2;
    if (!s1 || (len1 = strlen (s1)) == 0)
        return 0;
    const char* repl = (s2) ?  s2 : "";
    len2 = strlen (repl);
    long new_cap = maxl (_capacity + n* (len2 - len1), _capacity);
    char* new_array = new char [new_cap];
    if (!new_array)
        return 0;
    char* in = _string+pos;
    char* out = new_array;
    strncpy (out, _string, pos);
    out += pos;
    char* q;
    long repl_count = 0;
    while (repl_count < n && in - _string < _size) {
        q = strstr (in, s1);
        if (!q) break;
        long copy_len = q-in;
        strncpy (out, in, copy_len);
        strncpy (out + copy_len, repl, len2);
        out += copy_len + len2;
        in = q+len1;
        repl_count ++;
    }
    strcpy (out, in);
    _size = strlen (new_array);
    _capacity = new_cap;
    delete [] _string;
    _string = new_array;
    Notify();
    return repl_count;
}


// Beginning at position pos, replace all occurrences of s1 with
// s2, as in Replace. Return the number of replacements that took
// place.
long CL_String::ReplaceAll (const char* s1, const char* s2, long pos)
{
    return Replace (s1, s2, _size, pos);
}



CL_String CL_String::Field (long n, const char field_seps[]) const
{
    if (_size <= 0)
        return "";
    if (n == 0)
        return *this;
    CL_StringSplitter splitter (*this, field_seps);
    CL_String s;
    for (splitter.Reset(); n > 0; n--) {
        s = splitter.Next();
        if (s.Length() == 0) break;
    }
    return s;
}


CL_StringSequence CL_String::Split (const char field_seps[]) const
{
    CL_StringSplitter splitter (*this, field_seps);
    CL_StringSequence seq;
    CL_String s;

    do {
        s = splitter.Next();
    if (s.Length() == 0) break;
        seq.Add (s);
    } while (1);
    return seq;
}



long CL_String::Split (CL_String fld[], long n, const char field_seps[]) const
{
    CL_StringSplitter splitter (*this, field_seps);
    long i;
    for (i = 0; i < n-1; i++) {
        fld[i] = splitter.Next();
        if (fld[i].Length() == 0) break;
    }
    if (i >= n-1) {
        fld[i] = splitter.Remaining();
        return fld[i].Size() > 0 ? n : n-1;
    }
    return i;
}






//
// Comparison
//
bool CL_String::operator<  (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) < 0);
}

bool CL_String::operator<= (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) <= 0);
}

bool CL_String::operator== (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) == 0);
}

bool CL_String::operator>  (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) > 0);
}

bool CL_String::operator>= (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) >= 0);
}

bool CL_String::operator!= (const CL_Object& strg) const
{
    if (!IsA (strg))
        return FALSE;
    return (strcmp (_string, ((const CL_String&) strg)._string) != 0);
}





bool CL_String::operator<  (const char* strg) const
{
    if (!strg)
        return FALSE;
    return (strcmp (_string, strg) < 0);
}

bool CL_String::operator<= (const char* strg) const
{
    if (!strg)
        return (_size == 0);
    return (strcmp (_string, strg) <= 0);
}

bool CL_String::operator== (const char* strg) const
{
    if (!strg)
        return (_size == 0);
    return (strcmp (_string, strg) == 0);
}

bool CL_String::operator>  (const char* strg) const
{
    if (!strg)
        return (_size > 0);
    return (strcmp (_string, strg) > 0);
}

bool CL_String::operator>= (const char* strg) const
{
    if (!strg)
        return TRUE;
    return (strcmp (_string, strg) >= 0);
}

bool CL_String::operator!= (const char* strg) const
{
    if (!strg)
        return (_size > 0);
    return (strcmp (_string, strg) != 0);
}



short CL_String::Compare (const CL_Object& obj) const
{
    if (!IsA (obj))
        return (this < (CL_String*) &obj ? -1 : 1);
    return (strcmp (_string, ((const CL_String&) obj)._string));
}


short CL_String::Compare (const CL_String& obj) const
{
    return (strcmp (_string,  obj._string));
}


bool CL_String::CompareWith (const CL_String& obj,
                             CL_Object::ComparisonOperator op) const
{
    switch (op) {
    case CL_Object::OP_EQUAL:
        return (strcmp (_string, obj._string) == 0);
        
    case CL_Object::OP_LESSTHAN:
        return (strcmp (_string, obj._string) < 0);
        
    case CL_Object::OP_GTRTHAN:
        return (strcmp (_string, obj._string) > 0);
        
    case CL_Object::OP_LESSEQ:
        return (strcmp (_string, obj._string) <= 0);
        
    case CL_Object::OP_GTREQ:
        return (strcmp (_string, obj._string) >= 0);
        
    case CL_Object::OP_NOTEQUAL:
        return (strcmp (_string, obj._string) != 0);
        
    case CL_Object::OP_PREFIX:
        return (obj._size <= _size &&
                strncmp (_string, obj._string, obj._size) == 0);
        
    case CL_Object::OP_CONTAINS:
        return (obj._size <= _size &&
                strstr (_string, obj._string) != NULL);


    default:
        CL_Error::Warning ("CL_String::CompareWith: bad operator %d",
                           (short) op);
    }
    return FALSE;
}



//
// Assignment
//
void CL_String::operator= (const char* strg)
{
    if (!PrepareToChange())
        return;
    if (!strg) {
        _size = 0;
        _string[0] = '\0';
        Notify();
        return;
    }
    long n = strlen (strg);

    if (strg == _string) return; // Nothing to do
    if (_capacity >= n+1) {
        strcpy (_string, strg);
        _size = n;
    }
    else {
        char* p = NEW_OP char [_capacity = n + DEFAULT_START_SIZE + 1];
        if (!p)
            return; // No memory -- return without notifying dependents
        strcpy (p, strg);
        _size = n;
        delete [] _string;
        _string = p;
    }
    Notify ();
}

    
void CL_String::operator= (const CL_String& strg)
{
    if (this == &strg || !PrepareToChange())
        return;
    long n = strg.Size();
    if (strg._string == _string) return; // Nothing to do

    if (_capacity >= strg.Size() + 1) {
        strcpy (_string, strg._string);
        _size = n;
    }
    else {
        char* p = NEW_OP char [_capacity = n + DEFAULT_START_SIZE + 1];
        if (!p)
            return; // No memory
        strcpy (p, strg._string);
        _size = n;
        delete [] _string;
        _string = p;
    }
    Notify ();
}


void CL_String::operator= (long value)
{
    if (!PrepareToChange())
        return;
    if (_capacity < DEFAULT_START_SIZE) {
        delete [] _string;
        _string = new char [_capacity = DEFAULT_START_SIZE];
        if (!_string) {
            _capacity = 0; // No memory
            return;
        }
    }
    int_to_as (value, _string, 1);
    _size = strlen (_string);
    Notify ();
}




void CL_String::operator= (double d)
{
    if (!PrepareToChange())
        return;
    if (_capacity < DEFAULT_START_SIZE) {
        delete [] _string;
        _string = new char [_capacity = DEFAULT_START_SIZE];
        if (!_string) {
            _capacity = 0; // No memory
            return;
        }
    }
    gcvt (d, _capacity-1, _string);
    _size = strlen (_string);
    Notify();
}



//
// Concatenation
//
CL_String CL_String::operator+  (const char* strg) const
{
    long len;
    if (!strg || (len = strlen(strg)) == 0)
        return *this;
    long n = _size + len + 1;
    CL_String q;
    if (q._capacity < n) {
        delete [] q._string;
        q._string = NEW_OP char [q._capacity = n +
                                 DEFAULT_START_SIZE + 1];
        if (!q._string)
            return ""; // No memory
    }
    strcpy (q._string, _string);
    strcpy (q._string + _size, strg);
    q._size = n-1;
    return q;
}

        
CL_String CL_String::operator+  (const CL_String& strg) const
{
    long n = _size + strg.Size() + 1;
    CL_String q;
    if (q._capacity < n) {
        delete [] q._string;
        q._string = NEW_OP char [q._capacity = n +
                                 DEFAULT_START_SIZE + 1];
        if (!q._string)
            return ""; // No memory
    }
    strcpy (q._string, _string);
    strcpy (q._string + _size, strg._string);
    q._size = n-1;
    return q;
}

CL_String CL_String::operator+ (long v) const
{
    return *this + CL_String (v);
}

        
CL_String& CL_String::operator+= (const char* strg)
{
    if (!PrepareToChange())
        return *this;
    long len;
    if (!strg || (len = strlen(strg)) == 0)
        return *this;
    long n = _size + len + 1;
    if (_capacity >= n) {
        strcpy (_string + _size, strg);
        _size += strlen (strg);
    }
    else {
        char* p = NEW_OP char [_capacity = n + DEFAULT_START_SIZE + 1];
        if (!p) // No memory
            return *this;
        strcpy (p, _string);
        strcpy (p + _size, strg);
        delete [] _string;
        _string = p;
        _size = n-1;
    }
    Notify ();
    return *this;
}

CL_String& CL_String::operator+= (const CL_String& strg)
{
    if (!PrepareToChange())
        return *this;
    long n = _size + strg.Size() + 1;
    if (_capacity >= n) {
        strcpy (_string + _size, strg._string);
        _size += strg._size;
    }
    else {
        char* p = NEW_OP char [_capacity = n + DEFAULT_START_SIZE + 1];
        if (!p)
            return *this;
        strcpy (p, _string);
        strcpy (p + _size, strg);
        delete [] _string;
        _string = p;
        _size = n-1;
    }
    Notify ();
    return *this;
}

    




bool CL_String::AssignWithFormat (const char* fmt, ...)
{
    va_list args;
    CL_ByteString msg (1024);

    va_start (args, fmt);
    int v = vsprintf ((char*) msg.AsPtr(), fmt,  args);
    if (v >= 1024 || v == EOF)
        return FALSE;
    *this = (const char*) msg.AsPtr(); // This does the notification
    return TRUE;
}



CL_String CL_String::operator/ (const CL_String& s) const
{
    long ndx = Index (s._string);
    if (ndx >= 0)
        return CL_String (_string, ndx);
    else return *this;
}



bool CL_String::ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange() || !ReadClassId (s) )
        return FALSE;
    long len;
    if (!s.Read (len) || len <= 0)
        return FALSE;
    long cap = len + DEFAULT_START_SIZE;
    char* p = new char [cap];
    if (!p)
        return FALSE;
    if (s.Read ((uchar*) p, len) != len)
        return FALSE;
    _capacity = cap;
    _size = len - 1;
    if (_string)
        delete [] _string;
    _string = p;
    Notify ();
    return TRUE;
}

bool CL_String::WriteTo (CL_Stream& s) const
{
    return s.Write (ClassId())  &&
        s.Write (_size+1) && s.Write ((uchar*) _string, _size+1);
}


long CL_String::StorableFormWidth () const
{
    return sizeof (CL_ClassId) + (sizeof _size) + (_size + 1);
}





void CL_String::FromStream (istream& stream)
{
    if (!PrepareToChange ())
        return;
    register char fill_char = stream.fill ();
    CL_String tmp;
    char c;
    while (stream.get (c) && c == fill_char); // Skip fill characters
    while (c != fill_char && stream.good ()) {
        tmp.Append (c);
        stream.get (c);
    }
    if (!stream.eof())
        stream.putback (c);
    // Now a dirty trick:
    delete _string;
    _string = tmp._string;
    _capacity = tmp._capacity;
    _size = tmp._size;
    tmp._string = 0;
    Notify ();
}


istream& CL_String::ReadLine (istream& stream)
{
    if (!PrepareToChange ())
        return stream;
    CL_String tmp;
    char c;
    while (stream.get (c) && c != '\n') {
        tmp.Append (c);
    }

    // Now a dirty trick:
    delete _string;
    _string = tmp._string;
    _capacity = tmp._capacity;
    _size = tmp._size;
    tmp._string = 0;
    Notify ();
    return stream;
}



//
// Protected methods
//




bool CL_String::_Init (long size)
{
    _capacity = size;
    _string = NEW_OP char [size];
    if (!_string) {
        _capacity = _size = 0L;
        return FALSE;
    }
    _size = 0;
    _string[0] = '\0';
    return TRUE;
}



CL_String::CL_String (const char* s, long len)
{
    if (_Init (len + DEFAULT_START_SIZE)) {
        strncpy (_string, s, len);
        _string[len] = '\0';
        _size = len;
    }
}




// ------------------- CL_Substring methods ----------------------



typedef CL_Binding<CL_Substring> Bind;
CL_Substring::CL_Substring (CL_String& s, long pos, long length)
: CL_String (s), _client (s), _len (length)
{
    Bind b (this, &CL_Substring::_Modified);
    AddDependent (b, 1);
    _pos = maxl (0, minl (pos, s.Size()));
    if (pos >= 0 && pos < s.Size()) {
        _size = minl (length, s.Size() - pos);
        strncpy (_string, s._string + pos, _size);
        _string[_size] = '\0';
    }
    else { 
        _size = 0;
        _string[0] = '\0';
    }
}

CL_Substring::CL_Substring (const CL_Substring& s)
: CL_String (s), _client (s._client), _pos (s._pos), _len (s._len)
{
    Bind b (this, &CL_Substring::_Modified);
    AddDependent (b, 1);
}


    

// Modified: the method called to notify us when our value changes
bool CL_Substring::_Modified (CL_Object&, long)
{
    if (!_client.PrepareToChange())
        return FALSE;
    // Find the new size of the client
    long new_size = _client._size + _size - _len;
    if (new_size == _client._size) { // No change in client size
        strncpy (_client._string + _pos, _string, _len);
    }
    else if (new_size < _client._capacity) {
        // Enough room for modification
        memmove (_client._string + _pos + _size,
                 _client._string + _pos + _len,
                 _client._size - _pos - _len);
        memcpy (_client._string + _pos, _string, _size);
        _client._size += _size - _len;
    }
    else {
        // Not enough room: have to reallocate space
        long new_cap = new_size + 1 + DEFAULT_START_SIZE;
        char* p = new char [new_cap];
        if (!p)
            return FALSE; // No memory
        strncpy (p, _client._string, _pos);
        strncpy (p + _pos, _string, _size);
        strncpy (p + _pos + _size, _client._string + _pos + _len,
                 _client._size - _pos - _len);
        _client._size += _size - _len;
        _client._capacity = new_cap;
        delete [] _client._string;
        _client._string = p;
    }
    _client._string[_client._size] = '\0';
    _client.Notify();
    return TRUE;
}



CL_Substring::~CL_Substring ()
{
}




#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#include "base/cmparatr.h"
#include "base/basicops.h"

template class CL_Binding<CL_Substring>;
template class CL_Comparator<CL_String>;
template class CL_Basics<CL_String>;
#endif

