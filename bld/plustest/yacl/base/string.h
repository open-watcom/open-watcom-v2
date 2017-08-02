

#ifndef _string_h_
#define _string_h_





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



// This class provides the basic functionality for manipulating a
// printable string of characters.



#include "base/object.h"

#ifdef __GNUC__
#pragma interface
#endif


class CL_StringSequence;
class CL_Substring;
class CL_StringSplitter;

class __CLASSTYPE CL_String: public CL_Object {

public:
    //
    // ----------------------------- Constructors -------------------------
    //

    CL_String ();
    // Default: construct the null string.

    CL_String (const char* strg);
    // Construct a string from the given null-terminated char array.

    CL_String (long value, short minwidth = 1, char padChar = ' ');
    // Construct a string containing the digits of 'value', in a field
    // at least minwidth characters wide. Pad, if necessary, with as many of
    // the given padding characters as necessary on the left.

    CL_String (short value, short minwidth = 1, char padChar = ' ');
    // Construct a string containing the digits of 'value', in a field
    // at least minwidth characters wide. Pad, if necessary, with as many of
    // the given padding characters as necessary on the left.


    CL_String (CL_ByteArray& b);
    // Copy b's contents and append a null byte.
    
    CL_String (char c, short count);
    // Produce a string with c  repeated count times.
                
    CL_String (const CL_String& strg);
    // Copy constructor.
    
    
    ~CL_String();
    // Destructor.

    //
    // -------------------------- Conversion -------------------------------
    //

    operator const char* () const;
    // Return pointer to null-terminated char array representation.
    // The caller of this method should not modify the returned char
    // array.

    const char* AsPtr() const;
    // Synonym for {\tt operator const char*}.

    long AsLong() const;
    // Find the longest prefix that is all digits, and return them as
    // a long value.

    double AsDouble () const;
    // Find our longest prefix that looks like a double number, and
    // return it as a double value.
    

    // ----------- Structural manipulations, substrings ------------------

    long Size() const {return _size;} ;
    // Return the size of the string.

    long Length() const { return _size; };
    // Return the size of the string.
    
    char operator[] (long index) const;
    // Return character at given index.


    CL_Substring operator() (long position, long size);
    // Return the substring beginning at the given position and of the
    // given size. Note that this is not a const method, because the
    // returned substring can be assigned to.

    CL_Substring Suffix (long position);
    // Return our suffix beginning at the given position. Note that this
    // is not a const method, because the returned substring can be
    // assigned to.

    // -------------------- Search-related methods ---------------------


    // --- Character searches --
    
    long CharIndex (char c, long pos = 0L) const;
    // Return the index of the first position, at or right of 'pos', that
    // contains the character c. Return -1 if no such position.

    long NCharIndex (char c, long pos = 0L) const;
    // Return the index of the first position, at or right of 'pos', that
    // does not contain the character c. Return -1 if no such position.

         
    long CharIndex (const char* s, long pos = 0L) const;
    // Return the index of the first position, at or right of 'pos', that
    // contains any one of the characters in s. Return -1 if no such position.

    long NCharIndex (const char* s, long pos = 0L) const;
    // Return the index of the first position, at or right of 'pos', that
    // contains none of the characters in s. Return -1 if no such position.

    
    // ---- String searches ---

    
    bool IsPrefixOf (const char* p) const;
    // Are we a prefix of the given string?

    long Index (const char* s, long pos = 0, long n = 1) const;
    // Return the left end of the n-th occurrence of the string s in
    // our string; begin the search for s at position pos. Return -1 if 
    // no more than n-1 such occurrences of s were found.

    long Replace (const char* s1, const char* s2, long n = 1, long pos = 0);
    // Scan our string beginning at position pos, and replace the
    // first n occurrences of s1 in our string with s2. 
    // Return the number of successful replacements that occurred.
    // Note that if s2 is NULL or points to a null string, the
    // occurrences of s1 are removed. If s1 is null, no replacements
    // will be made.
    //    This method returns 0 if memory allocation fails.

    long ReplaceAll (const char* s1, const char* s2, long pos = 0);
    // Beginning at position pos, replace all occurrences of s1 with
    // s2, as in Replace. Return the number of replacements that took
    // place.

    

    // ------------------------ Decomposition ----------------------------
    //
    
    CL_String Field (long n, const char field_seps[] = " \t") const;
    // View our string as being made up of fields, separated by one or
    // more characters from the null-terminated string field_seps. Return
    // the n-th field. Fields are indexed from 1, i.e., n == 1 returns the
    // leftmost field. If n is 0, the entire string is returned. (This
    // function is similar to the functionality in awk.)

    CL_StringSequence Split (const char field_seps[] = " \t") const;
    // View our string as being made up of fields, separated by one or
    // more characters from the null-terminated string field_seps. Return
    // the sequence of fields in us.
    
    long Split (CL_String fld[], long n, const char field_seps[] = " \t")
        const;
    // View our string as being made up of fields, separated by one or
    // more characters from the string field_seps. Return the value of
    // fld as follows:
    //      fld[0..n-2]    contains the first n-1 fields, if there are
    //                     that many fields
    //      fld[n-1]       contains the remaining part of the string
    // Return the actual number of cells used in fld as the function
    // value.
    // Note that fld is an array of CL_String objects provided by the
    // caller, and must have at least n CL_Strings in it.

    
    //
    // ------------------------ Comparison -----------------------------------
    
    bool operator<  (const CL_Object& strg) const;

    bool operator<= (const CL_Object& strg) const;

    bool operator>  (const CL_Object& strg) const;

    bool operator>= (const CL_Object& strg) const;

    bool operator== (const CL_Object& strg) const;

    bool operator!= (const CL_Object& strg) const;

    

    virtual bool operator<  (const char* strg) const;

    virtual bool operator<= (const char* strg) const;

    virtual bool operator>  (const char* strg) const;

    virtual bool operator>= (const char* strg) const;

    virtual bool operator== (const char* strg) const;

    virtual bool operator!= (const char* strg) const;

    short Compare (const CL_Object& obj) const;
    // Compare returns -1 if we are less than the given string, 0 if
    // the two are equal, and +1 if we are greater. The method issues a
    // runtime error message if the given object does not have the class id
    // of a string.
    
    short Compare (const CL_String& s) const;
    // Compare returns -1 if we are less than the given string, 0 if
    // the two are equal, and +1 if we are greater.

    bool CompareWith (const CL_String& obj,
                      CL_Object::ComparisonOperator op) const;

    bool CompareWith (const CL_Object& obj,
                      CL_Object::ComparisonOperator op) const;
    // Overrides CL_Object's CompareWith method. Checks that obj's class id
    // is the same as ours and then invokes Compare with obj cast down to
    // CL_String. (Returns FALSE if class id does not match.)



    // --------------------------String modification ----------------------

    virtual bool Insert (char c, long position = -1);
    // Insert a character into the string, immediately to the right of the
    // given position, thus expanding the string. The default position of
    // -1 specifies insertion at the left end of the string. 
    // Return TRUE if successful, FALSE if either an invalid position is
    // specified, a pre-change dependent refused change, or else memory
    // allocation failure occurred.

    virtual bool Insert (const char* p, long position = -1);
    // Insert a null-terminated character string into ourselves. Note that the
    // effect is equivalent to assigning to the null substring at the
    // given position; i.e., {\tt s.Insert (p, pos)} is the same as {\tt s
    // (position+1, 0) = p;}

    bool Append (char c);
    // Append the given character to the string. This method is equivalent
    // to using the += operator, but is a little more efficient for single
    // character appends.

    bool AssignWithFormat (const char* format, ...);
    // Create a char buffer whose contents are identical to those produced
    // if the parameters were passed to printf; assign the result to this
    // string. Return TRUE if successful, FALSE if the result was too big
    // (i.e., > 1K).
    
    // --------- Assignments of various kinds
    
    virtual void operator= (const char* strg);

    virtual void operator= (const CL_String& strg);

    virtual void operator= (const CL_Object& strg);
    // Check that the given parameter is really a string (via the
    // inherited {\tt IsA} method), and
    // assign its value to this string.
    
    virtual void operator= (long l);
    // Convert the given long value into a string, and assign it to
    // this string.

    virtual void operator= (double d);
    // Convert the given double value into a string and assign it to
    // this string.

    

    // Concatenations of various kinds
    
    virtual CL_String operator+  (const char* strg) const;
    // Return the string obtained by concatenating the null-terminated
    // parameter strg to this string.

    virtual CL_String operator+  (const CL_String& strg) const;
    // Return the string obtained by concatenating strg to our value.

    virtual CL_String& operator+= (const char* strg);
    // Append the given (null-terminated) string to this string, and then
    // return this string.

    virtual CL_String& operator+= (const CL_String& strg);

    CL_String operator+ (long v) const;
    // Convert the long value into a decimal string and return the result of
    // appending it to ourselves.

    CL_String& operator+= (long v);
    // Append the string representation of the long value to ourselves.
    
    virtual CL_String operator/ (const CL_String& s) const;
    // "Division" operator:
    // Return the string resulting from the removal of the suffix
    // beginning with the first occurrence of the given string.
    // Thus "Alpha Beta Gamma" / "Beta" yields "Alpha ". If the given
    // string does not occur in us, simply return (a copy of) ourselves.

    void operator/= (const CL_String& s)
        { *this = *this / s;};
    

    // --------------------- Storage and restoration -------------------
    
    virtual long StorableFormWidth () const;

    virtual bool ReadFrom (const CL_Stream&);
    // Read and reconstruct ourselves from the binary representation in
    // the given stream. Return TRUE on success, FALSE if failed for any
    // reason, including when a pre-change dependent disallows the change.

    virtual bool WriteTo  (CL_Stream&) const;
    // Write the passive binary representation of this object into the
    // given stream. Return TRUE if successful, FALSE otherwise.

    CL_String AsString () const
        { return *this; };
    // Override the method inherited from {\small\tt CL_Object}.

    void FromStream (istream& stream);
    // Override the method inherited from {\small\tt CL_Object}. The
    // implementation skips any occurrences of the current fill character
    // of the stream, and then collects non-fill characters into this
    // string.

    virtual istream& ReadLine (istream& stream);
    // Read everything upto and including the next end-of-line marker (or
    // end-of-file) on the given stream, and set our value to the result.

    
    // ------------------------ Miscellaneous methods ----------------
    
    virtual bool PadTo (long num_chars, char pad_char = ' ',
                        bool left_justified = TRUE);
    // Pad ourselves (if necessary)  to fill out  to  the given number of
    // characters.   The padding  character is the second parameter, and  the
    // third parameter specifies whether the padding occurs on the right side
    // (i.e.,  left   justification)   or   the   left   side   (i.e.,  right
    // justification).   If  num_chars is smaller than our size, we  remain
    // unchanged. Return TRUE if padding occurred, FALSE if either the string
    // remained unchanged or memory allocation failed.

    CL_String InUpperCase () const;
    // Return this string with all lower-case letters replaced by upper-case
    // ones.

    CL_String InLowerCase () const;
    // Return this string with all upper-case letters replaced by
    // lower-case ones.

    virtual long ToUpper ();
    // Convert all lower-case characters to upper case. Return the number of
    // characters converted. Note that this can be combined with the
    // operator() to operate on substrings; e.g.,
    //               myString (10,3).ToUpper();
    // converts all lower-case characters between positions 10 and 12 to
    // upper-case.

    virtual long ToLower ();
    // Convert all upper-case characters to lower case. Return the number of
    // characters converted.

    virtual bool WordCapitalize ();
    // If this string begins with an alphabet, render the first letter
    // of the first word upper case, and all other letters in that
    // word lower case. Return TRUE if this change was effected, and
    // FALSE otherwise.
    
    //
    // ------------- Basic methods -------------------------
    //

    const char* ClassName() const {return "CL_String";};

    CL_ClassId ClassId() const { return _CL_String_CLASSID;};

    CL_Object* Clone() const {return new CL_String (*this);};

    // -------------------- End public protocol ------------------


    
protected:

    CL_String (const char* s, long len); // Protected constructor

    virtual bool _Init (long size);

    bool _DoInsert (const char*, long);
    // Do insertion without notification
    
    friend CL_Substring;
    friend CL_StringSplitter;
    
    char* _string;
    long  _capacity;
    long  _size;
};





class CL_Substring: public CL_String {

public:

    virtual void operator= (const char* strg)
        {CL_String::operator= (strg);};
    // Ensure that all assignment operators are inherited.
    
    virtual void operator= (const CL_String& strg)
        {CL_String::operator= (strg);};

    virtual void operator= (const CL_Object& strg)
        {CL_String::operator= (strg);};

    virtual void operator= (long l)
        {CL_String::operator= (l);};

    virtual void operator= (double d)
        {CL_String::operator= (d);};
    
    ~CL_Substring ();

protected:
    
    CL_Substring (CL_String& s, long pos, long length);
    // The constructor is protected, so that a Substring cannot be directly
    // constructed.

    CL_Substring (const CL_Substring&);

    bool _Modified (CL_Object&, long);
    // The method called to notify us when our value changes, so that we may
    // modify the real client.

    friend CL_String;
    CL_String& _client; // The string that we are a substring of
    long       _pos;    // Where the substring begins
    long       _len;    // How long the substring was when constructed
};



// Return pointer to char array representation
inline const char* CL_String::AsPtr() const
{
    return _string;
}

inline CL_String::operator const char* () const
{
    return _string;
}



// Return character at given index
inline char CL_String::operator[] (long index) const
{
    assert (index >= 0 && index < _size,
            ("CL_String::operator[]:"
            " invalid index %ld string size %ld", index, Size()));
    return _string[index];
}



inline void CL_String::operator= (const CL_Object& s)
{
    if (CheckClassType (s, "CL_String::op="))
        *this = (const CL_String&) s;
}


inline CL_Substring CL_String::Suffix (long position)
{
    return (*this) (position, Size() - position);
}


inline CL_String operator+ (const char* s1, const CL_String& s2)
{
    return CL_String (s1) + s2;
}

inline bool CL_String::Append (char c)
{
    long pos = Size() - 1;
    return Insert (c, pos);
}


inline CL_String& CL_String::operator+= (long v)
{
    *this = *this + v;
    return *this;
}


inline bool CL_String::CompareWith (const CL_Object& obj,
                                    CL_Object::ComparisonOperator op) const
{
    return ClassId() == obj.ClassId() ?
        CompareWith ((const CL_String&) obj, op) : FALSE;
}

#endif
