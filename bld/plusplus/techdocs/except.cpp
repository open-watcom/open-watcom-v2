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


#include <iostream.h>

struct Database_Lock {
    Database_Lock( char *name )
    {
        cout << "lock database (" << name << ")" << endl;
    }
    ~Database_Lock()
    {
        cout << "unlock database" << endl;
    }
};

struct Application_Error {
    char *msg;
    Application_Error( char *p ) : msg(p)
    {
    }
};
struct Bad_Record : Application_Error {
    int rec;
    Bad_Record( int num, char *msg ) : Application_Error( msg ), rec(num)
    {
    }
};

void update_record( int number )
{
    struct my_class {
        my_class()
        {
            cout << "- my_class constructed" << endl;
        }
        ~my_class()
        {
            cout << "- my_class destructed" << endl;
        }
    };
    my_class my_variable;

    if( number != 0 ) {
        cout << "throwing an exception..." << endl;
        throw Bad_Record( number, "invalid record number" );
    }
}

void main()
{
    Database_Lock lock( "mydata" );

    try {
        update_record( 1 );
    } catch( Bad_Record &what ) {
        cout << what.msg << " " << what.rec << endl;
    } catch( Application_Error &what ) {
        cout << what.msg << endl;
    } catch( int &any_int_exception ) {
        cout << "caught an integral exception " << any_int_exception << endl;
    } catch( ... ) {
        cout << "caught an unknown exception" << endl;
    }
}
