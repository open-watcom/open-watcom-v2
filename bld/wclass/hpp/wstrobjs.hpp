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


#ifndef wstringobjectlist_class
#define wstringobjectlist_class

#include <ctype.h>
#include <string.h>

#include "wvlist.hpp"

template<class T> class WStringObjectList : public WVList {
    public:
        WStringObjectList( const char *str=NULL );
        ~WStringObjectList();

        T &stringAt( int index );
        const char *cStringAt( int index );
        const char *cString( int first=0, int last=-1 );
        int parseIn( const char *str );
};


#if defined( __WStringObjectList__ )

// Possible states for finite state machine while parsing
#define BETWEEN_TOKENS  0
#define IN_TOKEN        1
#define END_TOKEN       2
#define IN_FILENAME     3
#define END_FILENAME    4



template<class T> WStringObjectList<T>::WStringObjectList( const char *str ) {
/****************************************************************************/

    parseIn( str );
}


template<class T> WStringObjectList<T>::~WStringObjectList() {
/************************************************************/

    deleteContents();
}


template<class T> T &WStringObjectList<T>::stringAt( int index ) {
/****************************************************************/

    static T str( "" );
    if( index < count() ) {
        T *s = (T *)(*this)[index];
        return( *s );
    }
    return( str );
}


template<class T> const char *WStringObjectList<T>::cStringAt( int index ) {
/**************************************************************************/

    if( index < count() ) {
        T* s = (T *)(*this)[index];
        return( *s );
    }
    return( "" );
}


template<class T> const char *WStringObjectList<T>::cString( int first,
                                                             int last ) {
/***********************************************************************/

    static T str;
    str = "";
    if( last < 0 ) last = 32767;
    int icount = count();
    for( int i = 0; i < icount; ++i ) {
        T* s = (T *)(*this)[i];
        if( (i >= first) && (i <= last) ) {
            if( str.size() > 0 ) str.concat( ' ' );
            str.concat( s->gets() );
        }
    }
    return( str );
}


template<class T> int WStringObjectList<T>::parseIn( const char *str ) {
/**********************************************************************/
    // This function will implement a "half" finite state machine to parse
    // the given string, i.e. final states for acceptance/rejection of tokens
    // is not considered; the input string will simply be parsed for tokens
    // and each token will be added to the list.  This FSM takes tokens to
    // be delimited by spaces or double quotes (").

    int state = BETWEEN_TOKENS;         //initial state

    if( str != NULL ) {
        T *temp = new T();
        int i = 0;
        char ch = str[i];

        for( ; ch != '\0'; ) {
            switch( state ) {
                case BETWEEN_TOKENS: {  //between "words" or tokens
                    switch( ch ) {
                        case ' ': {     //leading/trailing blanks, therefore ignore
                            i++;
                            break;
                        }
                        case '"':  {    //beginning of filename
                            temp->concat( ch );
                            state = IN_FILENAME;
                            i++;
                            break;
                        }
                        case '\0': {    //end of input string
                            state = END_TOKEN;
                            break;
                        }
                        default: {      //beginning of a "word" or token
                            temp->concat( ch );
                            state = IN_TOKEN;
                            i++;
                            break;
                        }
                    }
                    break;
                }
                case IN_TOKEN: {        //in the middle of a "word" or token
                     switch( ch ) {
                        case '"': {     //start of filename
                            temp->concat( ch );
                            state = IN_FILENAME;
                            i++;
                            break;
                        }
                        case ' ': {     //end of "word" or token
                            state = END_TOKEN;
                            break;
                        }
                        case '\0': {    //end of input string
                            state = END_TOKEN;
                            break;
                        }
                        default: {      //any other char, therefore add
                            temp->concat( ch );
                            i++;
                            break;
                        }
                    }
                    break;
                }
                case IN_FILENAME: {     //in the middle of a filename
                    switch( ch ) {
                        case '"': {     //end of filename reached? need to check
                            temp->concat( ch );
                            state = END_FILENAME;
                            break;
                        }
                        default: {      //anything else is allowed
                            temp->concat( ch );
                            break;
                        }
                    }
                    i++;
                    break;
                }
                case END_FILENAME: {    //has the end of the filename been reached?
                    switch( ch ) {
                        case ' ': {     //end of filename (and token)
                            state = END_TOKEN;
                            break;
                        }
                        case '\0': {    //end of input string
                            state = END_TOKEN;
                            break;
                        }
                        case '"': {     //check again for end of filename
                            temp->concat( ch ); //used to check for consecutive "
                            i++;
                            break;
                        }
                        default: {      //anything else -> still in middle of filename
                            temp->concat( ch );
                            state = IN_FILENAME;
                            i++;
                            break;
                        }
                    }
                    break;
                }
                case END_TOKEN: {       //end of "word" or token, therefore add to list
                    if( temp->size() > 0 ) {
                        add( temp );
                        temp = new T();
                    }
                    state = BETWEEN_TOKENS;
                    i++;
                    break;
                }
            }
            ch = str[i];                //get the next char from input string
        }
        if( temp->size() > 0 ) {        //add last item, if necessary
            add( temp );
            temp = new T();
        }
        delete temp;
    }
    return( count() );
}

#endif

#endif
