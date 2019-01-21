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

/*    enum           enumrc          name        namej       fname          descr (filter)        filter   */
pick( LANG_NONE,     VI_LANG_LANG0,  "None",     "None",     "",            "All Files",          "*.*"                            )
pick( LANG_C,        VI_LANG_LANG1,  "C",        "C",        "c.dat",       "C Files",            "*.c;*.h"                        )
pick( LANG_CPP,      VI_LANG_LANG2,  "C++",      "C++",      "cpp.dat",     "C++ Files",          "*.cpp;*.hpp;*.cxx;*.hxx;*.inl"  )
pick( LANG_FORTRAN,  VI_LANG_LANG3,  "Fortran",  "Fortran",  "fortran.dat", "Fortran Files",      "*.for;*.fi;*.f;*.inc"           )
pick( LANG_JAVA,     VI_LANG_LANG4,  "Java",     "Java",     "java.dat",    "Java(Script) Files", "*.java;*.js"                    )
pick( LANG_SQL,      VI_LANG_LANG5,  "SQL",      "SQL",      "sql.dat",     "SQL Files",          "*.sql"                          )
pick( LANG_BAT,      VI_LANG_LANG6,  "Batch",    "Batch",    "bat.dat",     "Batch Files",        "*.bat;*.cmd"                    )
pick( LANG_BASIC,    VI_LANG_LANG7,  "Basic",    "Basic",    "basic.dat",   "Basic",              "*.bas;*.frm;*.cls"              )
pick( LANG_PERL,     VI_LANG_LANG8,  "Perl",     "Perl",     "perl.dat",    "Perl Files",         "*.pl;*.cgi"                     )
pick( LANG_HTML,     VI_LANG_LANG9,  "HTML",     "HTML",     "html.dat",    "HTML Files",         "*.htm;*.html;*.xhtml"           )
pick( LANG_WML,      VI_LANG_LANG10, "WML",      "WML",      "wml.dat",     "WML Files",          "*.wml"                          )
pick( LANG_GML,      VI_LANG_LANG11, "GML",      "GML",      "gml.dat",     "GML Files",          "*.gml"                          )
pick( LANG_DBTEST,   VI_LANG_LANG12, "DBTest",   "DBTest",   "dbtest.dat",  "DBTest",             "*.tst"                          )
pick( LANG_MAKEFILE, VI_LANG_LANG13, "Makefile", "Makefile", "mif.dat",     "Makefiles",          "makefile;*.mk;*.mif;*.mak"      )
pick( LANG_RC,       VI_LANG_LANG14, "Resource", "Resource", "rc.dat",      "Resource Files",     "*.rc;*.rh;*.dlg"                )
pick( LANG_AWK,      VI_LANG_LANG15, "AWK",      "AWK",      "awk.dat",     "AWK Files",          "*.awk"                          )
#ifndef LANG_FILTER
pick( LANG_USER,     VI_LANG_LANG16, "User",     "User",     "user.dat",    "",                   ""                               )
#endif
