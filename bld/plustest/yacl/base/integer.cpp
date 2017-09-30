



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





#include <iostream.h>

#include "base/integer.h"
#include "base/bytarray.h"
#include "base/bytstrng.h"
#include "base/string.h"
#include "base/stream.h"


#ifdef __GNUC__
#pragma implementation
#endif


CL_DEFINE_CLASS(CL_Integer, _CL_Integer_CLASSID);


CL_String CL_Integer::AsString () const
{
    char buffer[15];
    short num_digits;
    long w;
    long v = _value;
    short sign = 1;

    if (_value == 0)
        return CL_String ("0");
    
    if (_value < 0) {
        sign = -1;
        v = -_value;
    }

    w = v;
    for (num_digits = 0; w > 0; num_digits++) {
        w /= 10;
    }
    if (sign == -1) num_digits++;
    buffer[num_digits] = '\0';
    for (w = v; w > 0; w /= 10) {
        buffer[--num_digits] = (char) ('0' + (w % 10));
    }
    return CL_String (buffer);
}





bool CL_Integer::WriteTo (CL_Stream& s) const
{
    return s.Write (ClassId())  && s.Write (_value);
}


bool CL_Integer::ReadFrom  (const CL_Stream& s)
{
    if (!PrepareToChange() || !ReadClassId (s) || !s.Read (_value))
        return FALSE;
    Notify ();
    return TRUE;
}

CL_Integer::~CL_Integer ()
{
}




CL_Integer& CL_Integer::operator= (const CL_Integer& i)
{
    if (this == &i || !PrepareToChange())
        return *this;
    _value = i._value;
    Notify ();
    return *this;
}


CL_Integer& CL_Integer::operator= (const long& i)
{
    if (!PrepareToChange())
        return *this;
    _value = i;
    Notify ();
    return *this;
}




void CL_Integer::FromStream (istream& stream)
{
    stream >> _value;
}



#if defined(__GNUC__) && __GNUC_MINOR__ >= 6
#include "base/cmparatr.h"
#include "base/basicops.h"
#include "base/binding.h"

template class CL_Binding<CL_Integer>;
template class CL_Comparator<CL_Integer>;
template class CL_Basics<CL_Integer>;
#endif
