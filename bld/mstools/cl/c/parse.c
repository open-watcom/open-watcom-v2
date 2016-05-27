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
* Description:  Command line parsing for CL clone tool.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "cl.h"
#include "cmdline.h"
#include "cmdscan.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "macro.h"
#include "memory.h"
#include "message.h"
#include "parse.h"
#include "cmdlnprs.h"

#ifdef __WATCOMC__
#pragma disable_message (202);
#endif

/*
 * Initialize the OPT_STORAGE structure.
 */
void InitParse( OPT_STORAGE *cmdOpts )
/************************************/
{
    OPT_INIT( cmdOpts );
}


/*
 * Destroy the OPT_STORAGE structure.
 */
void FiniParse( OPT_STORAGE *cmdOpts )
/************************************/
{
    OPT_FINI( cmdOpts );
}


/*
 * Gripe about a command line error.
 */
static void cmd_line_error( void )
/********************************/
{
    char *              str;

    GoToMarkContext();
    str = CmdScanString();
    Warning( "Ignoring invalid option '%s'", str );
}


/*
 * Parse the command string contained in the current context.
 */
void CmdStringParse( OPT_STORAGE *cmdOpts, int *itemsParsed )
/***********************************************************/
{
    char                ch;
    char *              filename;

    for( ;; ) {
        /*** Find the start of the next item ***/
        CmdScanWhitespace();
        ch = GetCharContext();
        if( ch == '\0' )  break;
        MarkPosContext();               /* mark start of switch */

        /*** Handle switches, command files, and input files ***/
        if( ch == '-'  ||  ch == '/' ) {        /* switch */
            if( OPT_PROCESS( cmdOpts ) ) {
                cmd_line_error();
            }
        } else if( ch == '@' ) {                /* command file */
            filename = CmdScanFileNameWithoutQuotes();
            PushContext();
            if( OpenFileContext( filename ) ) {
                FatalError( "Cannot open '%s'.", filename );
            }
            FreeMem( filename );
            CmdStringParse( cmdOpts, itemsParsed );
            PopContext();
        } else if( ch == '"' ) {                /* quoted option or file name */
            ch = GetCharContext();
            if( ch == '-' ) {
                Quoted = true;
                if( OPT_PROCESS( cmdOpts ) ) {
                    cmd_line_error();
                }
            } else {
                UngetCharContext();
                UngetCharContext();
                filename = CmdScanFileName();
                AddFile( TYPE_DEFAULT_FILE, filename );
                FreeMem( filename );
            }                
        } else {                                /* input file */
            UngetCharContext();
            filename = CmdScanFileName();
            AddFile( TYPE_DEFAULT_FILE, filename );
            FreeMem( filename );
        }
        (*itemsParsed)++;
    }
    CloseContext();
}


/*
 * Parse the /D option.
 */
static int parse_D( OPT_STRING **p )
/**********************************/
{
    char *              str;
    char *              eq;

    p = p;
    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/D requires an argument" );
        return( 0 );
    }
    for( ;; ) {                 /* convert all '#' chars to '=' chars */
        eq = strchr( str, '#' );
        if( eq == NULL )  break;
        *eq = '=';
    }
    if( DefineMacro( str ) ) {
        return( 1 );
    } else {
        Warning( "Ignoring invalid macro definition '%s'", str );
        return( 0 );
    }
}


/*
 * Add another string to an OPT_STRING.
 */
static void add_string( OPT_STRING **p, char *str )
/*************************************************/
{
    OPT_STRING *        buf;
    OPT_STRING *        curElem;

    /*** Make a new list item ***/
    buf = AllocMem( sizeof(OPT_STRING) + strlen(str) );
    strcpy( buf->data, str );
    buf->next = NULL;

    /*** Put it at the end of the list ***/
    if( *p == NULL ) {
        *p = buf;
    } else {
        curElem = *p;
        while( curElem->next != NULL )  curElem = curElem->next;
        curElem->next = buf;
    }
}


/*
 * Parse the /F option.
 */
static int parse_F( OPT_STRING **p )
/**********************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/F requires an argument" );
        return( 0 );
    }
    add_string( p, str );
    return( 1 );
}


/*
 * Parse the /FI option.
 */
static int parse_FI( OPT_STRING **p )
/***********************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/FI requires an argument" );
        return( 0 );
    }
    add_string( p, str );
    return( 1 );
}


/*
 * Destroy an OPT_STRING.
 */
void OPT_CLEAN_STRING( OPT_STRING **p )
/*************************************/
{
    OPT_STRING *        s;

    while( (s = *p) != NULL ) {
        *p = s->next;
        FreeMem( s );
    }
}


/*
 * Parse the /Fm option.
 */
static int parse_Fm( OPT_STRING **p )
/***********************************/
{
    char *              str;

    str = CmdScanString();
    if( str == NULL ) {
        OPT_CLEAN_STRING( p );
    } else {
        if( *p != NULL ) {
            Warning( "Overriding /Fm%s with /Fm%s", (*p)->data, str );
        }
        add_string( p, str );
    }
    return( 1 );
}


/*
 * Parse the /Gs option.
 */
static int parse_Gs( OPT_STRING **p )
/***********************************/
{
    char *              str;

    str = CmdScanString();
    if( str != NULL ) {
        add_string( p, str );
    }
    return( 1 );
}


/*
 * Parse the /I option.
 */
static int parse_I( OPT_STRING **p )
/**********************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/I requires an argument" );
        return( 0 );
    }
    add_string( p, str );
    return( 1 );
}


/*
 * Parse the /o option.
 */
static int parse_o( OPT_STRING **p )
/**********************************/
{
    char *              str;

    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/o requires an argument" );
        return( 0 );
    }
    add_string( p, str );
    return( 1 );
}


/*
 * Parse the /link option.
 */
static int parse_link( OPT_STRING **p )
/*************************************/
{
    char *              str;
    bool                gotOne = false;

    if( !CmdScanRecogChar( ' ' )  &&  !CmdScanRecogChar( '\t' ) ) {
        FatalError( "Whitespace required after /link" );
        return( 0 );
    }
    for( ;; ) {
        CmdScanWhitespace();
        str = CmdScanString();
        if( str == NULL ) {
            if( !gotOne ) {
                FatalError( "/link requires at least one argument" );
                return( 0 );
            } else {
                break;
            }
        }
        add_string( p, str );
        gotOne = true;
    }
    return( 1 );
}


/*
 * Parse the /passwopts option.
 */
static int parse_passwopts( OPT_STRING **p )
{
    char *str;
    char *src;
    char *dst;

    if( !CmdScanRecogChar( ':' ) )
    {
        FatalError("/passwopts:{argument} requires an argument");
        return 0;
    }

    str = CmdScanString();
    if (str == NULL)
    {
        FatalError("/passwopts requires an argument");
        return 0;
    }

    /*
     * If quoted, stip out the quote characters.
     */
    if (*str == '\"')
    {
        for (dst = str, src = str + 1; *src && (*src != '\"'); )
        {
            *dst++ = *src++;
        }

        if (*src != '\"')
        {
            FatalError("/passwopts argument is missing closing quote");
            return 0;
        }

        *dst = 0x00;
    }

    add_string( p, str );
    return( 1 );
} /* parse_passwopts() */


/*
 * Scan a filename.  No leading whitespace is permitted.
 */
bool OPT_GET_FILE( OPT_STRING **p )
/*********************************/
{
    char *              filename;

    filename = CmdScanFileName();
    if( filename != NULL ) {
        add_string( p, filename );
        return( true );
    } else {
        OPT_CLEAN_STRING( p );
        return( false );
    }
}


/*
 * Parse the /Tc option.
 */
static int parse_Tc( OPT_STRING **p )
/***********************************/
{
    CmdScanWhitespace();
    if( OPT_GET_FILE( p ) ) {
        AddFile( TYPE_C_FILE, (*p)->data );
        return( 1 );
    } else {
        FatalError( "/Tc requires an argument" );
        return( 0 );
    }
}


/*
 * Parse the /Tp option.
 */
static int parse_Tp( OPT_STRING **p )
/***********************************/
{
    CmdScanWhitespace();
    if( OPT_GET_FILE( p ) ) {
        AddFile( TYPE_CPP_FILE, (*p)->data );
        return( 1 );
    } else {
        FatalError( "/Tp requires an argument" );
        return( 0 );
    }
}


/*
 * Parse the /U option.
 */
static int parse_U( OPT_STRING **p )
/**********************************/
{
    char *              str;

    p = p;
    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/U requires an argument" );
        return( 0 );
    }
    UndefineMacro( str );
    return( 1 );
}


/*
 * Parse the /V option.
 */
static int parse_V( OPT_STRING **p )
/**********************************/
{
    char *              str;

    p = p;
    CmdScanWhitespace();
    str = CmdScanString();
    if( str == NULL ) {
        FatalError( "/V requires an argument" );
        return( 0 );
    }
    /* it's unsupported, so just skip over it; error msg will come later */
    return( 1 );
}


/*
 * Ensure the parameter to /Ob is valid.
 */
static void check_inlining_level( unsigned *p )
/*********************************************/
{
    if( *p != 0  &&  *p != 1  &&  *p != 2 ) {
        Warning( "Invalid value '%d' for /Ob -- assuming '0'", *p );
        *p = 0;
    }
}


/*
 * Ensure the parameter to /Zp is valid.
 */
static void check_packing( unsigned *p )
/**************************************/
{
    if( *p != 1  &&  *p != 2  &&  *p != 4  &&  *p != 8  &&  *p != 16 ) {
        Warning( "Invalid value '%d' for /Zp -- assuming '8'", *p );
        *p = 8;
    }
}


/*
 * Ensure the parameter to /Zm is valid.
 */
static void check_maxmem( unsigned *p )
/*************************************/
{
    if( *p > 2000 ) {
        Warning( "Invalid value '%d' for /Zm -- assuming '100'", *p );
        *p = 100;
    }
}


/*
 * Ensure the parameter to /W is valid.
 */
static void check_warn_level( unsigned *p )
/*****************************************/
{
    if( *p != 0  &&  *p != 1  &&  *p != 2  &&  *p != 3  &&  *p != 4 ) {
        Warning( "Invalid value '%d' for /W -- assuming '1'", *p );
        *p = 1;
    }
}


#ifdef __TARGET_386__
/*
 * Warn when one of /G3, /G4, /G5, and /GB overrides another.
 */
static void handle_arch_i86( OPT_STORAGE *cmdOpts, int x )
/********************************************************/
{
    static bool         hasBeenCalled;
    static unsigned     prevValue;
    char                oldCpu, newCpu;

    x = x;
    oldCpu = 0;
    newCpu = 0;
    if( hasBeenCalled ) {
        if( prevValue != cmdOpts->arch_i86 ) {
            switch( prevValue ) {           /* what is the old CPU? */
              case OPT_arch_i86_G3:
                oldCpu = '3';
                break;
              case OPT_arch_i86_G4:
                oldCpu = '4';
                break;
              case OPT_arch_i86_G5:
                oldCpu = '5';
                break;
              case OPT_arch_i86_GB:
                oldCpu = 'B';
                break;
              default:
                Zoinks();
            }
            switch( cmdOpts->arch_i86 ) {   /* what is the new CPU? */
              case OPT_arch_i86_G3:
                newCpu = '3';
                break;
              case OPT_arch_i86_G4:
                newCpu = '4';
                break;
              case OPT_arch_i86_G5:
                newCpu = '5';
                break;
              case OPT_arch_i86_GB:
                newCpu = 'B';
                break;
              default:
                Zoinks();
            }
            Warning( "Overriding /G%c with /G%c", oldCpu, newCpu );
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->arch_i86;
}
#endif


/*
 * Warn when one of /Z7, /Zd, and /Zi overrides another.
 */
static void handle_debug_info( OPT_STORAGE *cmdOpts, int x )
/**********************************************************/
{
    static bool         hasBeenCalled;
    static int          prevValue;

    x = x;
    if( cmdOpts->debug_info == OPT_debug_info_Zi ) {
        Warning( "Replacing unsupported /Zi with /Z7" );
        cmdOpts->debug_info = OPT_debug_info_Z7;
    }
    if( hasBeenCalled ) {
        if( prevValue == OPT_debug_info_Z7 ) {
            if( cmdOpts->debug_info == OPT_debug_info_Zd ) {
                Warning( "Overriding /Z7 with /Zd" );
            }
        } else if( prevValue == OPT_debug_info_Zd ) {
            if( cmdOpts->debug_info == OPT_debug_info_Z7 ) {
                Warning( "Overriding /Zd with /Z7" );
            }
        }
    } else {
        hasBeenCalled = true;
    }
    Warning( "Using Dwarf debugging information" );
    prevValue = cmdOpts->debug_info;
}


/*
 * Warn when the previous /F value is overridden.
 */
static void handle_F( OPT_STORAGE *cmdOpts, int x )
/*************************************************/
{
    static bool         hasBeenCalled;
    OPT_STRING *        p;

    x = x;
    if( hasBeenCalled ) {
        Warning( "Overriding /F%s with /F%s", cmdOpts->F_value->data,
                 cmdOpts->F_value->next->data );
        p = cmdOpts->F_value->next;
        FreeMem( cmdOpts->F_value );
        cmdOpts->F_value = p;
    } else {
        hasBeenCalled = true;
    }
}


/*
 * Warn when the previous /Fe value is overridden.
 */
static void handle_Fe( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;
    OPT_STRING *        p;

    x = x;
    if( hasBeenCalled ) {
        Warning( "Overriding /Fe%s with /Fe%s", cmdOpts->Fe_value->data,
                 cmdOpts->Fe_value->next->data );
        p = cmdOpts->Fe_value->next;
        FreeMem( cmdOpts->Fe_value );
        cmdOpts->Fe_value = p;
    } else {
        hasBeenCalled = true;
    }
}


/*
 * Warn when the previous /Fp value is overridden.
 */
static void handle_Fp( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;
    OPT_STRING *        p;

    x = x;
    if( hasBeenCalled ) {
        Warning( "Overriding /Fp%s with /Fp%s", cmdOpts->Fp_value->data,
                 cmdOpts->Fp_value->next->data );
        p = cmdOpts->Fp_value->next;
        FreeMem( cmdOpts->Fp_value );
        cmdOpts->Fp_value = p;
    } else {
        hasBeenCalled = true;
    }
}


/*
 * Warn when the previous /FR value is overridden.
 */
static void handle_FR( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;
    OPT_STRING *        p;

    x = x;
    if( hasBeenCalled ) {
        Warning( "Overriding /FR%s with /FR%s", cmdOpts->FR_value->data,
                 cmdOpts->FR_value->next->data );
        p = cmdOpts->FR_value->next;
        FreeMem( cmdOpts->FR_value );
        cmdOpts->FR_value = p;
    } else {
        Warning( "Browsing information will be output to .mbr file(s)" );
        hasBeenCalled = true;
    }
}


/*
 * Suppress warning messages.
 */
static void handle_nowwarn( OPT_STORAGE *cmdOpts, int x )
/*******************************************************/
{
    x = x;
    cmdOpts = cmdOpts;
    DisableWarnings( true );
}


/*
 * Warn when one of /Ob0, /Ob1, and /Ob2 overrides another.
 */
static void handle_inlining_level( OPT_STORAGE *cmdOpts, int x )
/**************************************************************/
{
    static bool         hasBeenCalled;
    static unsigned     prevValue;

    x = x;
    if( hasBeenCalled ) {
        if( prevValue != cmdOpts->Ob_value ) {
            Warning( "Overriding /Ob%d with /Ob%d", prevValue,
                     cmdOpts->Ob_value );
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->Ob_value;
}


/*
 * Warn when one of /O1, /O2, and /Ox overrides another.  Note that, for
 * MS compatability, no warning is issued when one of /O2 and /Ox overrides
 * the other.
 */
static void handle_opt_level( OPT_STORAGE *cmdOpts, int x )
/*********************************************************/
{
    static bool         hasBeenCalled;
    static int          prevValue;

    x = x;
    if( hasBeenCalled ) {
        if( prevValue == OPT_opt_level_O1 ) {
            if( cmdOpts->opt_level == OPT_opt_level_O2 ) {
                Warning( "Overriding /O1 with /O2" );
            } else if( cmdOpts->opt_level == OPT_opt_level_Ox ) {
                Warning( "Overriding /O1 with /Ox" );
            }
        } else if( prevValue == OPT_opt_level_O2 ) {
            if( cmdOpts->opt_level == OPT_opt_level_O1 ) {
                Warning( "Overriding /O2 with /O1" );
            }
        } else if( prevValue == OPT_opt_level_Ox ) {
            if( cmdOpts->opt_level == OPT_opt_level_O1 ) {
                Warning( "Overriding /Ox with /O1" );
            }
        } else if( prevValue == OPT_opt_level_Od ) {
            if( cmdOpts->opt_level == OPT_opt_level_O1 ) {
                Warning( "Overriding /Od with /O1" );
            } else if( cmdOpts->opt_level == OPT_opt_level_O2 ) {
                Warning( "Overriding /Od with /O2" );
            } else if( cmdOpts->opt_level == OPT_opt_level_Ox ) {
                Warning( "Overriding /Od with /Ox" );
            }
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->opt_level;
}


/*
 * Warn when one of /Os and /Ot overrides the other.
 */
static void handle_opt_size_time( OPT_STORAGE *cmdOpts, int x )
/*************************************************************/
{
    static bool         hasBeenCalled;
    static int          prevValue;

    x = x;
    if( hasBeenCalled ) {
        if( prevValue == OPT_opt_size_time_Os ) {
            if( cmdOpts->opt_size_time == OPT_opt_size_time_Ot ) {
                Warning( "Overriding /Os with /Ot" );
            }
        } else if( prevValue == OPT_opt_size_time_Ot ) {
            if( cmdOpts->opt_size_time == OPT_opt_size_time_Os ) {
                Warning( "Overriding /Ot with /Os" );
            }
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->opt_size_time;
}


/*
 * Warn when one of /Ge and /Gs overrides the other.
 */
static void handle_stack_probes( OPT_STORAGE *cmdOpts, int x )
/************************************************************/
{
    static bool         hasBeenCalled;
    static int          prevValue;

    x = x;
    if( cmdOpts->Gs_value != NULL ) {
        Warning( "Ignoring unsupported parameter '%s' to /Gs",
                 cmdOpts->Gs_value->data );
        OPT_CLEAN_STRING( &(cmdOpts->Gs_value) );
    }

    if( hasBeenCalled ) {
        if( prevValue == OPT_stack_probes_Ge ) {
            if( cmdOpts->stack_probes == OPT_stack_probes_Gs ) {
                Warning( "Overriding /Ge with /Gs" );
            }
        } else if( prevValue == OPT_stack_probes_Gs ) {
            if( cmdOpts->stack_probes == OPT_stack_probes_Ge ) {
                Warning( "Overriding /Gs with /Ge" );
            }
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->stack_probes;
}


/*
 * Warn when one of /MD, /ML, and /MT overrides another.
 */
static void handle_threads_linking( OPT_STORAGE *cmdOpts, int x )
/***************************************************************/
{
    static bool         hasBeenCalled;
    static unsigned     prevValue;
    char                oldType, newType;

    x = x;
    oldType = 0;
    newType = 0;
    if( hasBeenCalled ) {
        if( prevValue != cmdOpts->threads_linking ) {
            switch( prevValue ) {                   /* what's the old type? */
              case OPT_threads_linking_MD:
                oldType = 'D';
                break;
              case OPT_threads_linking_ML:
                oldType = 'L';
                break;
              case OPT_threads_linking_MT:
                oldType = 'T';
                break;
              default:
                Zoinks();
            }
            switch( cmdOpts->threads_linking ) {    /* what's the new type? */
              case OPT_threads_linking_MD:
                newType = 'D';
                break;
              case OPT_threads_linking_ML:
                newType = 'L';
                break;
              case OPT_threads_linking_MT:
                newType = 'T';
                break;
              default:
                Zoinks();
            }
            Warning( "Overriding /M%c with /M%c", oldType, newType );
        }
    } else {
        hasBeenCalled = true;
    }
    prevValue = cmdOpts->threads_linking;
}


/*
 * Warn when one of /w and /W overrides the other.
 */
static void handle_warn_level( OPT_STORAGE *cmdOpts, int x )
/**********************************************************/
{
    static bool         hasBeenCalled;
    static int          prevType;
    static unsigned     prevLevel;

    x = x;
    if( hasBeenCalled ) {
        if( prevType == OPT_warn_level_w ) {
            if( cmdOpts->warn_level == OPT_warn_level_W ) {
                if( cmdOpts->W_value != 0 ) {
                    Warning( "Overriding /w with /W%d", cmdOpts->W_value );
                }
            }
        } else if( prevType == OPT_warn_level_W ) {
            if( cmdOpts->warn_level == OPT_warn_level_w ) {
                if( prevLevel != 0 ) {
                    Warning( "Overriding /W%d with /w", prevLevel );
                }
            } else if( cmdOpts->warn_level == OPT_warn_level_W ) {
                if( cmdOpts->W_value != prevLevel ) {
                    Warning( "Overriding /W%d with /W%d", prevLevel,
                             cmdOpts->W_value );
                }
            }
        }
    } else {
        hasBeenCalled = true;
    }

    prevType = cmdOpts->warn_level;
    if( prevType == OPT_warn_level_W ) {
        prevLevel = cmdOpts->W_value;
    } else {
        prevLevel = 0;
    }
}


/*
 * Ensure that /Yc has no filename parameter.
 */
static void handle_precomp_headers( OPT_STORAGE *cmdOpts, int x )
/***************************************************************/
{
    x = x;
    if( cmdOpts->precomp_headers == OPT_precomp_headers_Yc ) {
        if( cmdOpts->Yc_value != NULL ) {
            Warning( "Ignoring unsupported parameter '%s' to /Yc",
                     cmdOpts->Yc_value->data );
            OPT_CLEAN_STRING( &(cmdOpts->Yc_value) );
        }
    } else if( cmdOpts->precomp_headers == OPT_precomp_headers_Yu ) {
        if( cmdOpts->Yu_value != NULL ) {
            Warning( "Ignoring unsupported parameter '%s' to /Yu",
                     cmdOpts->Yu_value->data );
            OPT_CLEAN_STRING( &(cmdOpts->Yu_value) );
        }
    } else if( cmdOpts->precomp_headers == OPT_precomp_headers_YX ) {
        if( cmdOpts->YX_value != NULL ) {
            Warning( "Ignoring unsupported parameter '%s' to /YX",
                     cmdOpts->YX_value->data );
            OPT_CLEAN_STRING( &(cmdOpts->YX_value) );
        }
    }
}


/*
 * Handle options which can be either /option- to disable or /option to
 * enable.  Option-specific information is passed as parameters.  Returns
 * non-zero if the option should be enabled or zero if it should be disabled.
 */
static int handle_on_off_option( bool *hasBeenCalled, char *optName, int isOn )
/*****************************************************************************/
{
    int     rc = isOn;
    char    ch;

    ch = GetCharContext();
    if( *hasBeenCalled ) {
        /*** Warn when one of /option and /option- overrides the other ***/
        if( isOn ) {
            if( ch == '-' ) {
                Warning( "Overriding /%s with /%s-", optName, optName );
                rc = 0;
            } else {
                UngetCharContext();
            }
        } else {
            if( ch != '-' ) {
                Warning( "Overriding /%s- with /%s", optName, optName );
                rc = 1;
            } else {
                UngetCharContext();
            }
        }
    } else {
        /*** Handle /option- for the first time ***/
        *hasBeenCalled = true;
        if( ch == '-' ) {
            rc = 0;
        } else {
            UngetCharContext();
        }
    }

    return( rc );
}


/*
 * Parse the /GX and /GX- options.
 */
static void handle_GX( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;

    x = x;
    if( handle_on_off_option( &hasBeenCalled, "GX", cmdOpts->GX ) ) {
        cmdOpts->GX = 1;
    } else {
        cmdOpts->GX = 0;
    }
}


/*
 * Parse the /Op and /Op- options.
 */
static void handle_Op( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;

    x = x;
    if( handle_on_off_option( &hasBeenCalled, "Op", cmdOpts->Op ) ) {
        cmdOpts->Op = 1;
    } else {
        cmdOpts->Op = 0;
    }
}


/*
 * Parse the /Oy and /Oy- options.
 */
static void handle_Oy( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    static bool         hasBeenCalled;

    x = x;
    if( handle_on_off_option( &hasBeenCalled, "Oy", cmdOpts->Oy ) ) {
        cmdOpts->Oy = 1;
    } else {
        cmdOpts->Oy = 0;
    }
}


#ifdef __TARGET_386__
/*
 * Parse the /QIfdiv and /QIfdiv- options.
 */
static void handle_QIfdiv( OPT_STORAGE *cmdOpts, int x )
/******************************************************/
{
    static bool         hasBeenCalled;

    x = x;
    if( handle_on_off_option( &hasBeenCalled, "QIfdiv", cmdOpts->QIfdiv ) ) {
        cmdOpts->QIfdiv = 1;
    } else {
        cmdOpts->QIfdiv = 0;
    }
}
#endif


/*
 * Parse the /TC option.
 */
static void handle_TC( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    cmdOpts = cmdOpts;
    x = x;
    ForceLanguage( FORCE_C_COMPILE );
}


/*
 * Parse the /TP option.
 */
static void handle_TP( OPT_STORAGE *cmdOpts, int x )
/**************************************************/
{
    cmdOpts = cmdOpts;
    x = x;
    ForceLanguage( FORCE_CPP_COMPILE );
}


/*
 * Return the next character and advance to the next one.
 */
int OPT_GET_LOWER( void )
/***********************/
{
    return( tolower( (unsigned char)GetCharContext() ) );
}


/*
 * If the next character is ch, it is consumed and a non-zero value
 * is returned; otherwise, it is not consumed and zero is returned.
 */
bool OPT_RECOG( int ch )
/**********************/
{
    return( CmdScanRecogChar( ch ) );
}

/*
 * If the next character is ch (in either uppercase or lowercase form), it
 * is consumed and a non-zero value is returned; otherwise, it is not
 * consumed and zero is returned.
 */
bool OPT_RECOG_LOWER( int ch )
/****************************/
{
    return( CmdScanRecogLowerChar( ch ) );
}


/*
 * Back up one character.
 */
void OPT_UNGET( void )
/********************/
{
    UngetCharContext();
}


/*
 * Scan an optional filename.  No leading whitespace is permitted.
 */
bool OPT_GET_FILE_OPT( OPT_STRING **p )
/*************************************/
{
    char *              filename;

    filename = CmdScanFileName();
    if( filename != NULL ) {
        add_string( p, filename );
    } else {
        OPT_CLEAN_STRING( p );
    }
    return( true );
}


/*
 * Scan a pathname.  No leading whitespace is permitted.
 */
bool OPT_GET_PATH( OPT_STRING **p )
/*********************************/
{
    char *              filename;

    filename = CmdScanFileName();
    if( filename != NULL ) {
        add_string( p, filename );
    } else {
        OPT_CLEAN_STRING( p );
    }
    return( true );
}


/*
 * Scan a number.  No leading whitespace is permitted.
 */
bool OPT_GET_NUMBER( unsigned *p )
/********************************/
{
    unsigned            value;

    if( CmdScanNumber( &value ) ) {
        *p = value;
        return( true );
    } else {
        return( false );
    }
}


/*
 * Scan an optional number.  No leading whitespace is permitted.
 */
bool OPT_GET_NUMBER_DEFAULT( unsigned *p, unsigned value )
/********************************************************/
{
    char ch;

    *p = value;
    ch = GetCharContext();
    UngetCharContext();
    if( isdigit( ch ) ) {
        if( CmdScanNumber( &value ) ) {
            *p = value;
            return( true );
        } else {
            return( false );
        }
    }
    return( true );
}


/*
 * Is this the end of an option chain?
 */
bool OPT_END( void )
/******************/
{
    char ch;

    ch = GetCharContext();
    if( ch == '\0' )
        return( true );
    UngetCharContext();
    if( isspace( ch ) )
        return( true );
    if( ch == '/' )
        return( true );
    if( ch == '-' )
        return( true );
    if( ch == '@' )
        return( true );
    if( ch == '"' )
        return( true );
    if( ch == '\'' )
        return( true );
    return( false );
}

/* Include after all static functions were declared */
#include "cmdlnprs.gc"
