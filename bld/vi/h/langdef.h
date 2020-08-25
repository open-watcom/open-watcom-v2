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
* Description:  Definition of source language constants.
*
****************************************************************************/

/*    enum      enumrc  name        namej       fname          descr (filter)        filter   */
pick( NONE,     LANG0,  "None",     "None",     "",            "All Files",          "*.*"                            )
pick( C,        LANG1,  "C",        "C",        "c.dat",       "C Files",            "*.c;*.h"                        )
pick( CPP,      LANG2,  "C++",      "C++",      "cpp.dat",     "C++ Files",          "*.cpp;*.hpp;*.cxx;*.hxx;*.inl"  )
pick( FORTRAN,  LANG3,  "Fortran",  "Fortran",  "fortran.dat", "Fortran Files",      "*.for;*.fi;*.f;*.inc"           )
pick( JAVA,     LANG4,  "Java",     "Java",     "java.dat",    "Java(Script) Files", "*.java;*.js"                    )
pick( SQL,      LANG5,  "SQL",      "SQL",      "sql.dat",     "SQL Files",          "*.sql"                          )
pick( BAT,      LANG6,  "Batch",    "Batch",    "bat.dat",     "Batch Files",        "*.bat;*.cmd"                    )
pick( BASIC,    LANG7,  "Basic",    "Basic",    "basic.dat",   "Basic",              "*.bas;*.frm;*.cls"              )
pick( PERL,     LANG8,  "Perl",     "Perl",     "perl.dat",    "Perl Files",         "*.pl;*.cgi"                     )
pick( HTML,     LANG9,  "HTML",     "HTML",     "html.dat",    "HTML Files",         "*.htm;*.html;*.xhtml"           )
pick( WML,      LANG10, "WML",      "WML",      "wml.dat",     "WML Files",          "*.wml"                          )
pick( GML,      LANG11, "GML",      "GML",      "gml.dat",     "GML Files",          "*.gml"                          )
pick( DBTEST,   LANG12, "DBTest",   "DBTest",   "dbtest.dat",  "DBTest",             "*.tst"                          )
pick( MAKEFILE, LANG13, "Makefile", "Makefile", "mif.dat",     "Makefiles",          "makefile;*.mk;*.mif;*.mak"      )
pick( RC,       LANG14, "Resource", "Resource", "rc.dat",      "Resource Files",     "*.rc;*.rh;*.dlg"                )
pick( AWK,      LANG15, "AWK",      "AWK",      "awk.dat",     "AWK Files",          "*.awk"                          )
#ifndef VI_LANG_FILTER
pick( USER,     LANG16, "User",     "User",     "user.dat",    "",                   ""                               )
#endif
