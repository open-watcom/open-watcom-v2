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
* Description:  Translate MS options to Watcom options.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "cl.h"
#include "cmdline.h"
#include "error.h"
#include "macro.h"
#include "message.h"
#include "pathconv.h"
#include "translat.h"

#define UNSUPPORTED_STR_SIZE    512


/*
 * Various flags to keep in mind while translating options.
 */
static struct XlatStatus {
    int     debugLevel;                 /* value for -d<num> switch */
    int     warnLevel;                  /* value for -w<num> switch */
    int     charTypeUnsigned    : 1;    /* char = signed char by default */
    int     parmsInRegs         : 1;    /* use register calling convention */
    int     justCompile         : 1;    /* don't link */
    int     preprocessToFile    : 1;    /* preprocess file.c to file.i */
    int     disable_c           : 1;    /* compile */
    int     disable_FA          : 1;    /* generate listing file */
    int     disable_Fa          : 1;    /* set listing file name */
    int     disable_Fm          : 1;    /* generate map file */
    int     disable_Fo          : 1;    /* set object file name */
    int     opt_oa              : 1;    /* use -oa option */
    int     opt_od              : 1;    /* use -od option */
    int     opt_oi              : 1;    /* use -oi option */
    int     opt_ol              : 1;    /* use -ol option */
    int     opt_ol_plus         : 1;    /* use -ol+ option */
    int     opt_om              : 1;    /* use -om option */
    int     opt_on              : 1;    /* use -on option */
    int     opt_op              : 1;    /* use -op option */
    int     opt_os              : 1;    /* use -os option */
    int     opt_ot              : 1;    /* use -ot option */
    int     opt_ox              : 1;    /* use -ox option */
#ifdef __TARGET_386__
    int     opt_of              : 1;    /* use -of option */
    int     opt_or              : 1;    /* use -or option */
#endif
} status;


/*
 * Add one more unsupported option to optStr.
 */
static void append_unsupported( char *optStr, char *opt )
/*******************************************************/
{
    if( optStr[0] != '\0' ) {
        strcat( optStr, " /" );
    } else {
        strcat( optStr, "/" );
    }
    strcat( optStr, opt );
}


/*
 * Parse unsupported options.
 */
static void unsupported_opts( OPT_STORAGE *cmdOpts )
/**************************************************/
{
    char                opts[UNSUPPORTED_STR_SIZE];

    /*** Build a string listing all unsupported options that were used ***/
    opts[0] = '\0';
    if( cmdOpts->EHa  )  append_unsupported( opts, "EHa"  );
    if( cmdOpts->EHac )  append_unsupported( opts, "EHac" );
    if( cmdOpts->EHc  )  append_unsupported( opts, "EHc"  );
    if( cmdOpts->EHca )  append_unsupported( opts, "EHca" );
    if( cmdOpts->EHcs )  append_unsupported( opts, "EHcs" );
    if( cmdOpts->EHs  )  append_unsupported( opts, "EHs"  );
    if( cmdOpts->EHsc )  append_unsupported( opts, "EHsc" );
    if( cmdOpts->Fa   )  append_unsupported( opts, "Fa"   );
    if( cmdOpts->FA   )  append_unsupported( opts, "FA"   );
    if( cmdOpts->FAc  )  append_unsupported( opts, "FAc"  );
    if( cmdOpts->FAcs )  append_unsupported( opts, "FAcs" );
    if( cmdOpts->FAs  )  append_unsupported( opts, "FAs"  );
    if( cmdOpts->Fd   )  append_unsupported( opts, "Fd"   );
    if( cmdOpts->Fr   )  append_unsupported( opts, "Fr"   );
    switch( cmdOpts->calling_convention ) {
      case OPT_calling_convention_Gd:
        append_unsupported( opts, "Gd" );
        break;
      case OPT_calling_convention_Gr:
        append_unsupported( opts, "Gr" );
        break;
      case OPT_calling_convention_Gz:
        append_unsupported( opts, "Gz" );
        break;
    }
    if( cmdOpts->H    )  append_unsupported( opts, "H"    );
    if( cmdOpts->Ow   )  append_unsupported( opts, "Ow"   );
    if( cmdOpts->u    )  append_unsupported( opts, "u"    );
    if( cmdOpts->V    )  append_unsupported( opts, "V"    );
    if( cmdOpts->vd0  )  append_unsupported( opts, "vd0"  );
    if( cmdOpts->vd1  )  append_unsupported( opts, "vd1"  );
    if( cmdOpts->vmb  )  append_unsupported( opts, "vmb"  );
    if( cmdOpts->vmg  )  append_unsupported( opts, "vmg"  );
    if( cmdOpts->vmm  )  append_unsupported( opts, "vmm"  );
    if( cmdOpts->vms  )  append_unsupported( opts, "vms"  );
    if( cmdOpts->vmv  )  append_unsupported( opts, "vmv"  );
    if( cmdOpts->X    )  append_unsupported( opts, "X"    );
    if( cmdOpts->debug_info == OPT_debug_info_Zi ) {
        append_unsupported( opts, "Zi"  );
    }
    if( cmdOpts->Zn   )  append_unsupported( opts, "Zn"   );

    /*** If an unsupported option was used, give a warning ***/
    if( opts[0] != '\0' ) {
        UnsupportedOptsMessage( opts );
    }
}


/*
 * Initialize a struct XlatStatus.
 */
static void init_status( struct XlatStatus *status )
/**************************************************/
{
    memset( status, 0, sizeof(struct XlatStatus) );
    status->warnLevel = 1;              /* default is /W1 */
    status->parmsInRegs = 1;            /* it's faster this way */
    status->opt_od = 1;                 /* default is no optimization */
    #ifdef __TARGET_386__
        status->opt_of = 1;             /* default is to make stack frames */
        status->opt_or = 1;             /* default is pipeline optimizing */
    #endif
}


/*
 * Translate options related to C++ and not to C.
 */
static void c_plus_plus_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                              CmdLine *compCmdLine )
/**************************************************************************/
{
    status = status;
    AppendCmdLine( compCmdLine, CL_C_CPP_OPTS_SECTION, "-xs" );
    if( cmdOpts->_10x ) {
        AppendCmdLine( compCmdLine, CL_C_CPP_OPTS_SECTION, "-zo" );
    }
}


/*
 * Translate options related to the preprocessor.
 */
static void preprocessor_opts( struct XlatStatus *status,
                               OPT_STORAGE *cmdOpts, CmdLine *compCmdLine )
/*************************************************************************/
{
    int                 preserveComments = 0;
    int                 includeLines = 1;       /* use #line directives */
    int                 preprocess = 0;
    OPT_STRING *        optStr;
    char *              newpath;

    optStr = cmdOpts->I_value;
    while( optStr != NULL ) {
        newpath = PathConvert( optStr->data, '"' );
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-i=%s", newpath );
        optStr = optStr->next;
    }

    optStr = cmdOpts->FI_value;
    while( optStr != NULL ) {
        newpath = PathConvert( optStr->data, '"' );
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-fi=%s", newpath );
        optStr = optStr->next;
    }

    if( cmdOpts->C ) {
        if( !cmdOpts->E && !cmdOpts->P && !cmdOpts->EP ) {
            Warning( "/C requires /E, /P, or /EP -- option ignored" );
        } else {
            preserveComments = 1;
        }
    }

    if( cmdOpts->E ) {
        status->disable_c = 1;
        status->disable_FA = 1;
        status->disable_Fa = 1;
        status->disable_Fm = 1;
        status->disable_Fo = 1;
        preprocess = 1;
    }

    if( cmdOpts->P ) {
        status->disable_c = 1;
        status->disable_FA = 1;
        status->disable_Fa = 1;
        status->disable_Fm = 1;
        status->disable_Fo = 1;
        preprocess = 1;
        status->preprocessToFile = 1;
    }

    if( cmdOpts->EP ) {
        status->disable_c = 1;
        status->disable_FA = 1;
        status->disable_Fa = 1;
        status->disable_Fm = 1;
        status->disable_Fo = 1;
        preprocess = 1;
        includeLines = 0;
    }

    if( preprocess ) {
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-p%s%s",
                          includeLines ? "l" : "",
                          preserveComments ? "c" : "" );
    }
}


/*
 * Translate options related to precompiled headers.
 */
static void precomp_header_opts( struct XlatStatus *status,
                                 OPT_STORAGE *cmdOpts, CmdLine *compCmdLine )
/***************************************************************************/
{
    char *              newpath;

    status = status;
    if( cmdOpts->Fp ) {
        newpath = PathConvert( cmdOpts->Fp_value->data, '"' );
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-fhq=%s", newpath );
    } else {
        switch( cmdOpts->precomp_headers ) {
          case OPT_precomp_headers_Yc:
            /* fall through */
          case OPT_precomp_headers_Yu:
            /* fall through */
          case OPT_precomp_headers_YX:
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-fhq" );
            break;
          case OPT_precomp_headers_default:
            break;
          default:
            Zoinks();
        }
    }

    if( cmdOpts->Yd ) {
        /* done by default */
    }
}


/*
 * Parse various options which defy categorization.
 */
static void misc_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                       CmdLine *compCmdLine )
/*********************************************************************/
{
    if( cmdOpts->J ) {
        status->charTypeUnsigned = 1;
    }

    switch( cmdOpts->warn_level ) {
      case OPT_warn_level_w:
        status->warnLevel = 0;
        break;
      case OPT_warn_level_W:
        status->warnLevel = cmdOpts->W_value;
        break;
      case OPT_warn_level_default:
        /* use default value */
        break;
      default:
        Zoinks();
    }

    switch( cmdOpts->iso ) {
      case OPT_iso_Za:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-za" );
        if( cmdOpts->iso_timestamp > cmdOpts->Op_timestamp ) {
            status->opt_op = 1;
        }
        break;
      case OPT_iso_Ze:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-ze" );
        break;
      case OPT_iso_default:
        /* use default value */
        break;
      default:
        Zoinks();
    }

    if( cmdOpts->Zs ) {
        status->disable_c = 1;
        status->disable_FA = 1;
        status->disable_Fa = 1;
        status->disable_Fm = 1;
        status->disable_Fo = 1;
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zs" );
    }

    if( !status->disable_c ) {
        if( cmdOpts->FR ) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-db" );
        }

        #ifdef __TARGET_386__
            switch( cmdOpts->arch_i86 ) {   /* what is the CPU */
              case OPT_arch_i86_G3:
                if( status->parmsInRegs ) {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-3r" );
                } else {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-3s" );
                }
                break;
              case OPT_arch_i86_G4:
              case OPT_arch_i86_GB:
              default:
                if( status->parmsInRegs ) {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-4r" );
                } else {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-4s" );
                }
                break;
              case OPT_arch_i86_G5:
                if( status->parmsInRegs ) {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-5r" );
                } else {
                    AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-5s" );
                }
                break;
            }
        #endif

        switch( cmdOpts->stack_probes ) {
          case OPT_stack_probes_Ge:
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-st" );
            break;
          case OPT_stack_probes_Gs:
        #ifndef __TARGET_AXP__
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-sg" );
        #else
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-s" );
        #endif
            break;
          case OPT_stack_probes_default:
            break;
          default:
            Zoinks();
        }

        if( cmdOpts->WX ) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-we" );
        }

        if( cmdOpts->Zg ) {
            status->disable_c = 1;
            status->disable_FA = 1;
            status->disable_Fa = 1;
            status->disable_Fm = 1;
            status->disable_Fo = 1;
            Warning( "Prototypes will be output to .def file(s), not standard output" );
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-v" );
        }
    }
}


/*
 * Translate options related to object files.
 */
static void object_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                         CmdLine *compCmdLine )
/***********************************************************************/
{
    char *              newpath;

    if( cmdOpts->Fo && !status->disable_Fo ) {
        newpath = PathConvert( cmdOpts->Fo_value->data, '"' );
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-fo=%s", newpath );
    }

    if( cmdOpts->Gh ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-ep" );
    }

    if( cmdOpts->Gy ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zm" );
    }

    if( cmdOpts->LD ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-bd" );
        if( cmdOpts->threads_linking==OPT_threads_linking_default ) {
            cmdOpts->threads_linking = OPT_threads_linking_MT;
        }
    }

    switch( cmdOpts->threads_linking ) {
      case OPT_threads_linking_MD:
      case OPT_threads_linking_MDd:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-bm" );
        if( !cmdOpts->_10x ) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-br" );
        }
        break;
      case OPT_threads_linking_ML:
      case OPT_threads_linking_MLd:
        break;
      case OPT_threads_linking_MT:
      case OPT_threads_linking_MTd:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-bm" );
        break;
      case OPT_threads_linking_default:
        /* let the compiler use its default */
        break;
      default:
        Zoinks();
    }

    switch( cmdOpts->debug_info ) {
      case OPT_debug_info_Z7:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-hd" );
        status->debugLevel = 2;
        break;
      case OPT_debug_info_Zd:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-hd" );
        status->debugLevel = 1;
        break;
      case OPT_debug_info_Zi:
        /* unsupported */
      case OPT_debug_info_default:
        /* do nothing */
        break;
      default:
        Zoinks();
    }

    if( cmdOpts->Zl ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zl" );
    }

    if( cmdOpts->Zp ) {
        AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zp%d",
                          cmdOpts->Zp_value );
    }
}


/*
 * Translate options related to optimization.
 */
static void optimization_opts( struct XlatStatus *status,
                               OPT_STORAGE *cmdOpts, CmdLine *compCmdLine )
/*************************************************************************/
{
    if( cmdOpts->opt_level != OPT_opt_level_Od ) {
        status->opt_od = 0;

        switch( cmdOpts->opt_level ) {
          case OPT_opt_level_O1:                /* minimize size */
            cmdOpts->Og = 1;
            cmdOpts->opt_size_time = OPT_opt_size_time_Os;
            cmdOpts->Oy = 1;
            cmdOpts->Ob_value = 1;
            cmdOpts->stack_probes = OPT_stack_probes_Gs;
            cmdOpts->Gf = 1;
            cmdOpts->GF = 1;
            cmdOpts->Gy = 1;
            break;
          case OPT_opt_level_O2:                /* maximize speed */
            cmdOpts->Og = 1;
            cmdOpts->Oi = 1;
            cmdOpts->opt_size_time = OPT_opt_size_time_Ot;
            cmdOpts->Oy = 1;
            cmdOpts->Ob_value = 1;
            cmdOpts->stack_probes = OPT_stack_probes_Gs;
            cmdOpts->Gf = 1;
            cmdOpts->GF = 1;
            cmdOpts->Gy = 1;
            break;
          case OPT_opt_level_Ox:
            cmdOpts->Ob_value = 1;
            cmdOpts->Og = 1;
            cmdOpts->Oi = 1;
            cmdOpts->opt_size_time = OPT_opt_size_time_Ot;
            cmdOpts->Oy = 1;
            cmdOpts->stack_probes = OPT_stack_probes_Gs;
            status->opt_ol_plus = 1;
            status->opt_om = 1;
            status->opt_on = 1;
            status->opt_ox = 1;
          case OPT_opt_level_default:
            /* let the compiler use its default */
            break;
          default:
            Zoinks();
        }


        if( cmdOpts->Oa ) {
            status->opt_oa = 1;
        }

        if( cmdOpts->Ob_value == 0 ) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-oe=0" );
        }

        if( cmdOpts->Og ) {
            status->opt_ol = 1;
            status->opt_ot = 1;
        }

        if( cmdOpts->Oi ) {
            status->opt_oi = 1;
        }

        if( cmdOpts->Op ) {
            status->opt_op = 1;
        }

        #ifdef __TARGET_386__
            if( cmdOpts->Oy ) {
                status->opt_of = 0;
            }
        #endif
    }

    switch( cmdOpts->opt_size_time ) {
      case OPT_opt_size_time_Os:
        status->opt_os = 1;
        break;
      case OPT_opt_size_time_Ot:
        status->opt_ot = 1;
        break;
      case OPT_opt_size_time_default:
        break;
      default:
        Zoinks();
    }
}


/*
 * Parse compiler options.
 */
static void compiler_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                           CmdLine *compCmdLine )
/*************************************************************************/
{
    preprocessor_opts( status, cmdOpts, compCmdLine );  /* should be first */
    precomp_header_opts( status, cmdOpts, compCmdLine );
    if( !status->disable_c ) {
        optimization_opts( status, cmdOpts, compCmdLine );  /* before object_opts */
        object_opts( status, cmdOpts, compCmdLine );
        c_plus_plus_opts( status, cmdOpts, compCmdLine );
    }
    misc_opts( status, cmdOpts, compCmdLine );

    if( status->disable_c ) {
        cmdOpts->c = 1;         /* tell mainline to skip link phase */
    }
}


/*
 * Parse linker options.
 */
static void linker_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                         CmdLine *linkCmdLine )
/***********************************************************************/
{
    OPT_STRING *        optStr;
    char *              newpath;

    if( cmdOpts->F ) {
        AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/STACK:%s",
                          cmdOpts->F_value->data );
    }

    if( cmdOpts->Fe ) {
        AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/OUT:%s",
                          cmdOpts->Fe_value->data );
    }

    optStr = cmdOpts->o_value;
    if( cmdOpts->o_value != NULL ) {
        newpath = PathConvert( optStr->data, '"' );
        AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/OUT:%s",
        newpath );
    }

    if( cmdOpts->Fm ) {
        if( cmdOpts->Fm_value != NULL ) {
            AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/MAP:%s",
                              cmdOpts->Fm_value->data );
        } else {
            AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/MAP" );
        }
    }

    if( cmdOpts->LD ) {
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/DLL" );
    }

    if( cmdOpts->link ) {
        optStr = cmdOpts->link_value;
        while( optStr != NULL ) {
            AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "%s",
                              optStr->data );
            optStr = optStr->next;
        }
    }

    switch( cmdOpts->debug_info ) {
      case OPT_debug_info_Zd:
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/DEBUG" );
        status->debugLevel = 1;
        break;
      case OPT_debug_info_Z7:
        /* fall through */
      case OPT_debug_info_Zi:
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/DEBUG" );
        status->debugLevel = 2;
        break;
      case OPT_debug_info_default:
        /* do nothing */
        break;
      default:
        Zoinks();
    }
}


/*
 * Activate default options.
 */
static void default_opts(struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                         CmdLine *compCmdLine, CmdLine *linkCmdLine )
{
    OPT_STRING *curr;

    status = status;
    /*** Emit default options if so desired ***/
    if (!cmdOpts->nowopts)
    {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-bt=nt" );
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zlf" );

        #ifdef __TARGET_386__
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-ei" );
        #endif

        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-zq" );
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/nologo" );
    } /* if */

    /*** Add any options meant for the Watcom tools ***/
    if (cmdOpts->passwopts)
    {
        for (curr = cmdOpts->passwopts_value; curr; curr = curr->next)
        {
            AppendCmdLine(compCmdLine, CL_C_OPTS_SECTION, curr->data);
        }
    }
} /* default_opts() */


/*
 * Activate options which have been parsed but not yet turned on.
 */
static void merge_opts( struct XlatStatus *status, OPT_STORAGE *cmdOpts,
                        CmdLine *compCmdLine, CmdLine *linkCmdLine )
/**********************************************************************/
{
    char                buf[128];
    char *              macro;

    /*** Handle /D and /U ***/
    for( ;; ) {                                 /* defines */
        macro = GetNextDefineMacro();
        if( macro == NULL )  break;
        AppendFmtCmdLine( compCmdLine, CL_C_MACROS_SECTION, "-d%s", macro );
    }
    for( ;; ) {                                 /* undefines */
        macro = GetNextUndefineMacro();
        if( macro == NULL )  break;
        AppendFmtCmdLine( compCmdLine, CL_C_MACROS_SECTION, "-u%s", macro );
    }

    /*** Merge optimization options ***/
    if( status->opt_od ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-od" );
    } else {
        strcpy( buf, "-o" );
        #ifdef __TARGET_386__
            if( status->opt_of )   strcat( buf, "f" );
            if( status->opt_or )   strcat( buf, "r" );
        #endif
        if( status->opt_oa )       strcat( buf, "a" );
        if( status->opt_oi )       strcat( buf, "i" );
        if( status->opt_ol )       strcat( buf, "l" );
        if( status->opt_ol_plus )  strcat( buf, "l+" );
        if( status->opt_om )       strcat( buf, "m" );
        if( status->opt_on )       strcat( buf, "n" );
        if( status->opt_op )       strcat( buf, "p" );
        if( status->opt_os )       strcat( buf, "s" );
        if( status->opt_ot )       strcat( buf, "t" );
        if( status->opt_ox )       strcat( buf, "x" );
        if( strcmp( buf, "-o" ) != 0 ) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, buf );
        }
    }

    /*** Handle debugging options ***/
    switch( status->debugLevel ) {
      case 0:
        /* no debugging info */
        break;
      case 1:
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-d1" );
        break;
      case 2:
        if (!cmdOpts->lesswd) {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-d2" );
        } else {
            AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-d1" );
        }
        break;
      default:
        Zoinks();
    }

    if( !status->charTypeUnsigned ) {
        AppendCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-j" );
    }
    AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-w%d", status->warnLevel );

    if( cmdOpts->showwopts ) {
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/showwopts" );
    }

    if( cmdOpts->noinvoke ) {
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/noinvoke" );
    }

    if( cmdOpts->nowopts ) {
        AppendCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/nowopts" );
    }
}


/*
 * This function is for options which affect all source files differently.
 * Given the filename, it updates the given command line appropriately.  Any
 * CmdLine pointer which is NULL is ignored.
 */
void HandleFileTranslate( const char *filename, CmdLine *compCmdLine,
                          CmdLine *linkCmdLine )
/*******************************************************************/
{
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                fname[_MAX_FNAME];
    char                fullPath[_MAX_PATH];
    char *              newpath;

    linkCmdLine = linkCmdLine;

    /*** Handle the /P switch ***/
    if( status.preprocessToFile ) {
        if( compCmdLine != NULL ) {
            _splitpath( filename, drive, dir, fname, NULL );
            _makepath( fullPath, NULL, NULL, fname, ".i" );
            newpath = PathConvert( fullPath, '"' );
            AppendFmtCmdLine( compCmdLine, CL_C_OPTS_SECTION, "-fo=%s",
                              newpath );
        }
    }
}


/*
 * Translate scanned MS options to Watcom options.
 */
void OptionsTranslate( OPT_STORAGE *cmdOpts, CmdLine *compCmdLine,
                       CmdLine *linkCmdLine )
/****************************************************************/
{
    /*** Parse the /nologo switch now so we can print the banner ***/
    init_status( &status );
    if( cmdOpts->nologo ) {
        QuietModeMessage();
    } else {
        BannerMessage();
    }

    /*** Parse everything ***/
    unsupported_opts( cmdOpts );
    default_opts( &status, cmdOpts, compCmdLine, linkCmdLine );
    compiler_opts( &status, cmdOpts, compCmdLine );
    linker_opts( &status, cmdOpts, linkCmdLine );
    merge_opts( &status, cmdOpts, compCmdLine, linkCmdLine );
}
