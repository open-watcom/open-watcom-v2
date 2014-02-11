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
* Description:  ide2make utility to convert ide project to makefiles
*
****************************************************************************/


#include "mconfig.hpp"
#include "mproject.hpp"
#include "mcompon.hpp"
#include "wfilenam.hpp"
#include "wstrlist.hpp"
#include "wobjfile.hpp"
#include "wrect.hpp"
#include "mstate.hpp"
#include "mvstate.hpp"
#include "mrstate.hpp"
#include "mcstate.hpp"

extern "C" {
    #include <stdio.h>
};

// Force the state classes to be linked in
MTool   footool( "footool", "footool" );
MVState foovstate( &footool );
MCState foocstate( &footool );
MRState foorstate( &footool );

static char ProjectIdent[] = { "projectIdent" };


WCLASS VpeMain : public WObject
{
    Declare( VpeMain )
    public:
        VpeMain( int argc, char** argv );
        ~VpeMain();
    private:
        MProject*       _project;
        bool loadProject( const WFileName& fn );
};

static const char *usage[] = {
    "Usage: ide2make [*options] [tgtfile]*",
    "",
    "  ide2make loads an IDE project file (-p switch) and, using the associated",
    "  .tgt files, generates make files that can be invoked with WMAKE.",
    "  If the .wpj file does not exist, a default project is used.  If any",
    "  tgtfile(s) are specified, they are used as the targets in creating the",
    "  make files.",
    "",
    "options:",
    "  -p wpjfile       - loads wpjfile.wpj (project.wpj by default).",
    "  -c cfgfile       - loads cfgfile instead of ide.cfg.",
    "  -i incdir        - directory to search configuration files.",
    "  -d               - generate makefiles using development switch set.",
    "  -r               - generate makefiles using release switch set.",
    "  -h host type     - generate makefiles for selected host.",
    "                     (default is current host)",
    NULL
};

static const char *usage_hosts[] = {
    #undef pick
    #define pick(enum,type,batchserv,editor,DLL,parms,pathsep,descr) descr,
    #include "hosttype.h"
    NULL
};

int main( int argc, char** argv )
{
    if( argc <= 1 ) {
        for( int i = 0; usage[i] != NULL; i++ ) {
            puts( usage[i] );
        }
        for( int i = 0; usage_hosts[i] != NULL; i++ ) {
            printf( "                     %d - %s\n", i, usage_hosts[i] );
        }
    } else {
        VpeMain app( argc, argv );
    }
    return 0;
}

Define( VpeMain )

WEXPORT VpeMain::VpeMain( int argc, char** argv )
    : _project( NULL )
{
    WFileName pfile( "project" );;
    WFileName cfg;
    WStringList parms;
    char sMode = 0;
    bool debug = FALSE;
    HostType host = HOST_UNDEFINED;
    int  i;
    char *incdir = NULL;

    for( i=1; i<argc; i++ ) {
        if( streq( argv[i], "-c" ) ) {
            if( i+1 < argc ) {
                i++;
                cfg = argv[i];
            }
        } else if( streq( argv[i], "-p" ) ) {
            if( i+1 < argc ) {
                i++;
                pfile = argv[i];
            }
        } else if( streq( argv[i], "-d" ) ) {
            sMode = 'd';
        } else if( streq( argv[i], "-i" ) ) {
            if( i+1 < argc ) {
                i++;
                incdir = argv[i];
            }
        } else if( streq( argv[i], "-r" ) ) {
            sMode = 'r';
        } else if( streq( argv[i], "-x" ) ) {
            debug = TRUE;
        } else if( streq( argv[i], "-h" ) ) {
            if( i+1 < argc ) {
                i++;
                host = (HostType)atoi( argv[i] );
            }
        } else {
            parms.add( new WString( argv[i] ) );
        }
    }
    if( strlen( pfile.ext() ) == 0 ) {
        pfile.setExt( ".wpj" );
    }

    new MConfig( cfg, debug, host, incdir );
    WString err;
    if( !_config->ok() ) {
        printf( "ide2make: %s\n", (const char*)_config->errMsg() );
    } else {
        if( pfile.attribs() ) {
            if( !loadProject( pfile ) ) {
                delete _project;
                _project = NULL;
            }
            if( parms.count() > 0 ) {
                for( i=_project->components().count(); i>0; ) {
                    i--;
                    MComponent* comp = (MComponent*)_project->components()[i];
                    delete _project->removeComponent( comp );
                }
            }
        } else {
            _project = new MProject( pfile );
        }
        if( _project ) {
            int icount = parms.count();
            for( int i=0; i<icount; i++ ) {
                WFileName f( parms.cStringAt(i) );
                f.absoluteTo(); f.setExt( ".tgt" );
                _project->attachComponent( f );
            }
            if( sMode ) {
                SwMode mode = (sMode=='d') ? SWMODE_DEBUG : SWMODE_RELEASE;
                for( int i=0; i<_project->components().count(); i++ ) {
                    MComponent* comp = (MComponent*)_project->components()[i];
                    comp->setMode( mode );
                }
            }
            _project->makeMakeFile();
        }
    }
}

WEXPORT VpeMain::~VpeMain()
{
    delete _project;
    delete _config;
}

#ifndef NOPERSIST
VpeMain* WEXPORT VpeMain::createSelf( WObjectFile& )
{
    return NULL;
}

void WEXPORT VpeMain::readSelf( WObjectFile& p )
{
    WRect r; p.readObject( &r );
    _project = (MProject*)p.readObject();
}

void WEXPORT VpeMain::writeSelf( WObjectFile& )
{
}
#endif

bool VpeMain::loadProject( const WFileName& fn )
{
    WObjectFile of;
    if( of.open( fn, OStyleReadB ) ) {
        fn.setCWD();    //so the load will work for relative filenames
        if( of.version() <= 22 ) {
            of.close();
            printf( "Project '%s' format is obsolete; you must create the project again or use an older version of the IDE.", (const char*)fn );
            return FALSE;
        }
        if( of.version() >= 34 ) {
            char ident[ sizeof( ProjectIdent ) + 1 ];
            of.readObject( ident, sizeof( ident ) - 1 );
            if( !streq( ident, ProjectIdent ) ) {
                //bad file format
                of.close();
                printf( "Error: Project '%s' - Bad File Format.", (const char*)fn );
                return FALSE;
            }
        }
        of.readObject( this );
        of.close();
        return TRUE;
    }
    printf( "Unable to open project '%s'", (const char*)fn );
    return FALSE;
}

