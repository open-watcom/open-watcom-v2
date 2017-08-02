



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



#ifndef _setimp_cxx_
#define _setimp_cxx_



#include "base/bytstrng.h"
#include "base/basicops.h"
#include "base/stream.h"

#include "base/set.h"

#define NEW_OP new


#ifdef __BORLANDC__
#pragma warn -lvc
#include <stdio.h>
#endif

#ifdef __GNUC__
#pragma implementation
#endif



template <class BaseType>
CL_Set<BaseType>::CL_Set(CL_ObjectIOFilter* filter)
{
    _idata = new CL_Sequence<BaseType>;
    _filter = filter;
    _null  = CL_Basics<BaseType>::NullValue ();
}


template <class BaseType>
void CL_Set<BaseType>::_Destructor()
{
    if (_idata)
        delete ((CL_Sequence<BaseType>*)_idata);
}


/*----------------------------------------------------------------------- */

template <class BaseType>
CL_Set<BaseType>::CL_Set(void* p)
{
    _idata = p;
    _filter = NULL;
}



/*----------------------------------------------------------------------- */



template <class BaseType>
CL_Set<BaseType>::CL_Set (const CL_Set<BaseType>& s)
{
    _idata = new CL_Sequence<BaseType>;
    _filter = s._filter;
    *this = s;
}






/*----------------------------------------------------------------------- */


//
// Add an object to the set. Return true on success.
//
template <class BaseType>
bool CL_Set<BaseType>::Add (const BaseType& o)
{
    if (!_idata)
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long index = 0;
    if (_data.BinarySearch (o, index))
        return FALSE;
    BaseType obj = o;
    if (_data.Insert (obj, index)) {
        Notify ();
        return TRUE;
    }
    return FALSE;
}





/*----------------------------------------------------------------------- */
// Remove the object equal to o from the set (if it's there). Return
// true on success.
template <class BaseType>
BaseType CL_Set<BaseType>::Remove (const BaseType& o)
{
    if (!_idata)
        return  CL_Basics<BaseType>::NullValue ();
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long i;
    BaseType p;
    if (!_data.BinarySearch (o, i))
        return  CL_Basics<BaseType>::NullValue ();
    p = _data.Remove (i);
    Notify ();
    return p;
}









/*----------------------------------------------------------------------- */
template <class BaseType>
void CL_Set<BaseType>::MakeEmpty ()
{
    if (!_idata)
        return;
    if (!PrepareToChange())
        return;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    _data.MakeEmpty ();
    Notify ();
}



/*----------------------------------------------------------------------- */


// Determine if o is in the set
template <class BaseType>
bool CL_Set<BaseType>::Includes (const BaseType& o) const
{
    if (!_idata)
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long i;
    return _data.BinarySearch (o, i);
}




template <class BaseType>
long CL_Set<BaseType>::RankOf (const BaseType& o) const
{
    if (!_idata)
        return 0;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long index;
    bool b = _data.BinarySearch (o, index);
    return b ? index : index+1;
}


template <class BaseType>
const BaseType& CL_Set<BaseType>::ItemWithRank (long i) const
{
    if (!_idata)
        return _null;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long n = _data.Size();
    if (n <= 0)
        return _null;
    i = maxl (0, minl (i, n-1));
    return _data[i];
}



/*----------------------------------------------------------------------- */

// Check if o is the same as this set
template <class BaseType>
bool CL_Set<BaseType>::_OpEqual (const CL_Set<BaseType>& o) const
{
    if (!IsA (o))
        return FALSE;
    if (!_idata)
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    if (Size() != ((const CL_Set<BaseType>&) o).Size())
        return FALSE;

    long n = ((const CL_Set<BaseType>&)o).Size();
    for (long i = 0; i < n; i++) {
        if (! ((const CL_Set<BaseType>&)o).Includes (_data[i]))
            return FALSE;
    }
    
    return TRUE;
}




/*----------------------------------------------------------------------- */


// Assignment

template <class BaseType>
void CL_Set<BaseType>::operator= (const CL_Set<BaseType>& s)
{
    if (this == &s || !_idata)
        return;
    if (!PrepareToChange())
        return;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    _data = *(CL_Sequence<BaseType>*)(s._idata);
    Notify ();
}



/*----------------------------------------------------------------------- */

template <class BaseType>
void CL_Set<BaseType>::operator= (const CL_Sequence<BaseType>& s)
{
    if (!_idata || !PrepareToChange())
        return;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    _data.MakeEmpty ();
    // Don't use this->MakeEmpty, because we don't want double notification
    register long n = s.Size();
    for (register long i = 0; i < n; i++)
        Add (s[i]);
    Notify ();
}



/*----------------------------------------------------------------------- */

template <class BaseType>
CL_Set<BaseType> CL_Set<BaseType>::operator* (const CL_Set<BaseType>& s) const
{
    CL_Set<BaseType> aSet;

    if (!_idata)
        return aSet;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    CL_Sequence<BaseType>& sdata = (* (CL_Sequence<BaseType>*) s._idata);
    CL_Sequence<BaseType>& adata = (* (CL_Sequence<BaseType>*) aSet._idata);
    long m = _data.Size();
    long n = s.Size();
    adata.ChangeSize (m+n);
    long i = 0, j = 0, count = 0;
    while (i < m && j < n) {
        short result = CL_Basics<BaseType>::Compare (_data[i], sdata[j]);
        if (result < 0)
            i++;
        else if (result == 0) {
            adata[count++] = _data[i];
            i++; j++;
        }
        else j++;
    }
    adata.ChangeSize (count);
    return aSet;
}





template <class BaseType>
bool CL_Set<BaseType>::IncludesAll (const CL_Set<BaseType>& s) const
{
    if (!_idata)
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    CL_Sequence<BaseType>& sdata = (* (CL_Sequence<BaseType>*) s._idata);
    register long m = _data.Size();
    register long n = sdata.Size();
    if (m < n)
        return FALSE;
    register long i = 0, j = 0, count = 0;
    while (i < m && j < n) {
        register short result = CL_Basics<BaseType>::Compare
            (_data[i], sdata[j]);
        if (result < 0)
            i++;
        else if (result == 0) {
            i++; j++; count++;
        }
        else j++;
    }
    return count == n;
}



template <class BaseType>
CL_Set<BaseType> CL_Set<BaseType>::operator+ (const CL_Set<BaseType>& s) const
{
    CL_Set<BaseType> aSet;

    if (!_idata)
        return aSet;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    CL_Sequence<BaseType>& sdata = (* (CL_Sequence<BaseType>*) s._idata);
    CL_Sequence<BaseType>& adata = (* (CL_Sequence<BaseType>*) aSet._idata);
    long m = _data.Size();
    long n = sdata.Size();
    adata.ChangeSize (m+n);
    long i = 0, j = 0, count = 0;
    while (i < m || j < n) {
        short result = (i >= m) ? 1 :
        ((j >= n) ? -1 : CL_Basics<BaseType>::Compare (_data[i], sdata[j]));
        if (result < 0) {
            adata[count] = _data[i];
            i++;
        }
        else if (result == 0) {
            adata[count] = _data[i];
            i++; j++;
        }
        else {
            adata[count] = sdata[j];
            j++;
        }
        count++;
    }
    adata.ChangeSize (count);
    return aSet;
}



template <class BaseType>
CL_Set<BaseType> CL_Set<BaseType>::operator- (const CL_Set<BaseType>& s) const
{
    CL_Set<BaseType> aSet;

    if (!_idata)
        return aSet;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    CL_Sequence<BaseType>& sdata = (* (CL_Sequence<BaseType>*) s._idata);
    CL_Sequence<BaseType>& adata = (* (CL_Sequence<BaseType>*) aSet._idata);
    long m = _data.Size();
    long n = sdata.Size();
    adata.ChangeSize (m+n);
    long i = 0, j = 0, count = 0;
    while (i < m) {
        short result = (j >= n) ? -1 :
            CL_Basics<BaseType>::Compare (_data[i], sdata[j]);
        if (result < 0) {
            adata[count++] = _data[i];
            i++;
        }
        else if (result == 0) {
            i++; j++;
        }
        else {
            j++;
        }
    }
    adata.ChangeSize (count);
    return aSet;
}








/*----------------------------------------------------------------------- */
    
    

template <class BaseType>
void CL_Set<BaseType>::operator += (const CL_Set<BaseType>& s)
{
    // Notification done by operator=
    *this = (*this) + s;
}



/*----------------------------------------------------------------------- */


// Set intersection


    


/*----------------------------------------------------------------------- */
    

template <class BaseType>
void CL_Set<BaseType>::operator *= (const CL_Set<BaseType>& s)
{
    (*this) = (*this) * s;
}



/*----------------------------------------------------------------------- */


// Set difference


/*----------------------------------------------------------------------- */
    

template <class BaseType>
void CL_Set<BaseType>::operator -= (const CL_Set<BaseType>& s)
{
    (*this) = (*this) - s;
}



/*----------------------------------------------------------------------- */



template <class BaseType>
bool CL_Set<BaseType>::_ReadElement (const CL_Stream& s,
                                     BaseType& element)
{
    return CL_RestoreFrom (element, s, _filter);
}




template <class BaseType>
bool CL_Set<BaseType>::_ReadFrom (const CL_Stream& s)
{
    if (!PrepareToChange())
        return FALSE;
    CL_ClassId id;
    if (!s.Read (id) || id != ClassId())
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    long n;
    if (!s.Read (n))
        return FALSE;
    if (!_data.ChangeSize (n))
        return FALSE;
    for (register long i = 0; i < n; i++) {
        if (!_ReadElement (s, _data[i]))
            return FALSE;
    }
    _data.Sort ();
    Notify ();
    return TRUE;
}


template <class BaseType>
bool CL_Set<BaseType>::_WriteElement (CL_Stream& s, const BaseType& e) const
{
    return CL_SaveTo (e, s, _filter);
}




template <class BaseType>
bool CL_Set<BaseType>::_WriteTo  (CL_Stream& s) const
{
    if (!_idata)
        return FALSE;
    CL_Sequence<BaseType>& _data = (* (CL_Sequence<BaseType>*) _idata);
    register long n = Size();
    if (!s.Write (ClassId()) || !s.Write (n) )
        return FALSE;
    for (register long i = 0; i < n; i++) {
        if (!_WriteElement (s, _data[i]))
            return FALSE;
    }
    return TRUE;
}








    

//
// Protected CL_Set methods
//










    
/*----------------------------------------------------------------------- */


//
//------------------------------------------------------------
//

// CL_SetIterator methods


template <class BaseType>
CL_SetIterator<BaseType>::CL_SetIterator (const CL_Set<BaseType>& o)
    :_set (o) 
{
    _index = 0;
}



/*----------------------------------------------------------------------- */

template <class BaseType>
CL_SetIterator<BaseType>::CL_SetIterator (const CL_SetIterator<BaseType>& s)
    :_set (s._set), _index (s._index)
{
}



/*----------------------------------------------------------------------- */

template <class BaseType>
void CL_SetIterator<BaseType>::Reset ()
{   
    _index = 0;
}


/*----------------------------------------------------------------------- */

template <class BaseType>
void CL_SetIterator<BaseType>::BeginFromRank (long l)
{   
    _index = maxl (0, l);
}


/*----------------------------------------------------------------------- */

template <class BaseType>
const BaseType& CL_SetIterator<BaseType>::Next ()
{
    register CL_Sequence<BaseType>* data = (CL_Sequence<BaseType>*)
        _set._idata;
    if (data && (_index < data->Size()) )
        return (*data)[_index++];
    return _set._null; 
}



/*----------------------------------------------------------------------- */

template <class BaseType>
bool CL_SetIterator<BaseType>::More () const
{
    return _index < _set.Size();
}



/*----------------------------------------------------------------------- */

// template <class BaseType>
// bool CL_SetIterator<BaseType>::_OpEqual
//     (const CL_SetIterator<BaseType>& o) const
// {
//     return (((const CL_SetIterator<BaseType> &) o)._index == _index && 
//             &((const CL_SetIterator<BaseType> &) o)._set == &_set);
// }





/*----------------------------------------------------------------------- */






#endif /* _setimp_cxx_ */

