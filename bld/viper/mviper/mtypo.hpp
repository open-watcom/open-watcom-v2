/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef wtypo_class
#define wtypo_class

WCLASS WTypo : public WObject
{
    public:
        WEXPORT WTypo();
        WEXPORT WTypo( const WTypo& t );
        WEXPORT WTypo( const WString& good, const WString& bad ) { _good = good; _bad = bad; };
        virtual bool WEXPORT isEqual( const WTypo* t ) const { return( *t == _bad );  };
        virtual bool WEXPORT isEqual( const WObject *o ) const { return( WObject::isEqual( o ) ); };
        WString good() { return( _good ); };
        WString bad() { return( _bad ); };
    private:
        WString _good;
        WString _bad;
};

#endif //wtypo_class

void AddTypo( WString &good, WString &bad );
void FixTypo( WString &word );
