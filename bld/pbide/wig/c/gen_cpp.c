/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include "global.h"
#include "types.h"
#include "sruinter.h"
#include "keywords.h"
#include "pbmem.h"
#include "error.h"
#include "options.h"
#include "ytab.h"
#include "list.h"
#include "filelist.h"
#include "gen_cpp.h"


static BOOL generateHeaderFile( sru_file *sru );
static void generateCoverFile( sru_file *sru );
static void generateCodeFile( sru_file *sru );

#define WIG_BEG_HEADER  \
"// $PB$ -- begin generated code for object <%s>.  Do not modify this code\n"

#define WIG_END_HEADER  \
"// $PB$ -- end generated code for object <%s>.\n"


#define COVER_HEADER    \
"/* This file is generated by PowerBuilder.\n"\
" * Do not modify this file.\n"\
" * This file contains interface code called by PowerBuilder.\n"\
" */ \n"

#define HPP_HEADER      \
"/* This file contains code generated by PowerBuilder.\n"\
" * Do not modify code delimited by comments of the form:\n"\
" * "WIG_BEG_HEADER\
" * "WIG_END_HEADER\
" * This file contains the the C++ class definition for your user object.\n"\
" */ \n"\
"\n"\
"#include <string.hpp>\n"\
"#include <windows.h>\n"\
"\n"

#define CPP_HEADER      \
"/* This file contains code generated by PowerBuilder.\n"\
" * Do not modify code delimited by comments of the form:\n"\
" * "WIG_BEG_HEADER\
" * "WIG_END_HEADER\
" * This file contains the bodies the functions for your user object.\n"\
" */ \n\n"

#define LIBMAIN_HEADER  \
"/* This file is generated by PowerBuilder.\n"\
" * You may modify it in any way you wish but do not remove\n"\
" * Libmain and WEP.  Without them you will be unable to link your DLL.\n"\
" */ \n"\
"\n"\
"#include <windows.h>\n"\
"#include \"pbdll.h\"\n\n"

#define DLLMAIN_FUNC    \
"\n"\
"extern \"C\" {\n"\
"\n"\
"int __stdcall DLLMain( DWORD, DWORD reason, DWORD )\n"\
"{\n"\
"    if( reason == DLL_PROCESS_ATTACH ) {\n"\
"        extern char __WD_Present;\n"\
"        if( __WD_Present ) { // this is a hook for the Watcom debugger.\n"\
"            extern void Int3WithSignature( char __far * );\n"\
"            #pragma aux Int3WithSignature parm caller [] = \\\n"\
"                    \"int 3\" \\\n"\
"                    \"jmp short L1\" \\\n"\
"                    'W' 'V' 'I' 'D' 'E' 'O' \\\n"\
"                    \"L1:\"\n"\
"            Int3WithSignature( \"DLL Loaded\" );\n"\
"        }\n"\
"    }\n"\
"    return( 1 );\n"\
"}\n"\
"\n"\
"};\n\n"


#define LIBMAIN_FUNC    \
"\n"\
"int PB_EXPORT LibMain( HANDLE, WORD, WORD, LPSTR )\n"\
"{\n"\
"    return( 1 );\n"\
"}\n\n"

#define WEP_FUNC        \
"int PB_EXPORT WEP( int )\n"\
"{\n"\
"    return( 1 );\n"\
"}\n\n"

#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif

#define SYSTEM_INCLUDE  "\n#include <%s>\n"
#define PBDLL_H         "pbdll.h"
#define THIS_HDL        "this_hdl"
#define CLOSE_END       " );\n"
#define TERM_START_FUNC " ) {\n"
#define CLOSE_SCOPE     "}\n"
#define CLOSE_CLASS     "};\n"
#define START_RETURN    "return( "
#define START_CALL      "this_hdl->%s( "
#define CLOSE_PAREN     " )"
#define END_STATEMENT   ";\n"
#define PARM_DECL       "%s %s"
#define COMMA_DELIM     ", "
#define SCOPE_EXTERN_C  "\nextern \"C\" {\n"
#define INDENT          "    "
#define RETURN_ZERO     "    return( 0 );\n"
#define RETURN_NULL     "    return( (void*)0 );\n"
#define IF_TRUE         "#if 1\n"
#define IF_FALSE        "#if 0\n"
#define IF_NT           "#ifdef __NT__\n"
#define ELSE            "#else\n"
#define END_IF          "#endif // PowerBuilder code, do not remove \n"
#define CONS_DECL_TMPLT "%s *PB_EXPORT %s();\n"
#define DES_DECL_TMPLT  "void PB_EXPORT %s( %s *this_hdl );\n"
#define PBEXPORT_SRCH   " PB_EXPORT "


#define PLACE_USER_CODE_HERE    \
"//==================================\n"\
"\n"\
"    /*\n"\
"     * PUT YOUR CODE HERE\n"\
"     */\n"\
"\n"

#define PLACE_DECLARATIONS_HERE         \
"\n"\
"    /*\n"\
"     * PUT YOUR DECLARATIONS HERE\n"\
"     */\n"\
"\n"


enum { /* type of function to create */
    O_PROTOTYPE,
    O_CLASS_PROTO,
    O_FULL_FUNC,
    O_CLASS_FUNC
};

enum { /* type of variable definition to create */
    DATA_INSTANCE,
    DATA_STATIC
};

typedef struct {
    FILE        *fp;
    char        *name;
} FileInfo;

#define LINE_SIZE       512
static char     lineBuffer[LINE_SIZE];
static char     lineBuffer2[LINE_SIZE];

static void genTmpFName( char *file, char *buf ) {
/*************************************************/

    char        fname[ _MAX_FNAME ];
    char        dir[ _MAX_DIR ];
    char        drive[ _MAX_DRIVE ];
    unsigned    i;

    _splitpath( file, drive, dir, NULL, NULL );
    for( i=0 ; i < 0x1000; i++ ) {
        sprintf( fname, "TMP%03X", i );
        _makepath( buf, drive, dir, fname, ".tmp" );
        if( access( buf, F_OK ) ) break;
    }
}

static void generateLibMain( void ) {
/*******************************/

    FILE        *fp;
    char        *fname;

    fname = GetLmainName();
    if( !access( fname, F_OK ) ) {
        /* never overwrite this file if it exists */
        return;
    }
    fp = WigOpenFile( fname, "wt" );
    if( !fp ) {
        Error( FILE_OPEN_ERR, fname );
    }

    if( fputs( LIBMAIN_HEADER, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( IF_NT, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( DLLMAIN_FUNC, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( ELSE, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( LIBMAIN_FUNC, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( WEP_FUNC, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    if( fputs( END_IF, fp ) == EOF ) Error( FILE_WRITE_ERR, fname );
    WigCloseFile( fp );
}


void GenerateCPPFiles( sru_file *sru ) {
/*************************************/

    assert( sru );

    generateLibMain();
    if( !(Options & OPT_GEN_C_CODE) ) {
        if( !generateHeaderFile( sru ) ) return;
        generateCoverFile( sru );
    }
    generateCodeFile( sru );
}


static void outInclude( FileInfo *fp ) {
/**************************************/

    char fname[ _MAX_FNAME ];
    char ext[ _MAX_EXT ];

    assert( fp );

    if( fprintf( fp->fp, SYSTEM_INCLUDE, PBDLL_H ) < 0 ) {
        Error( FILE_WRITE_ERR, fp->name );
    }
    if( !(Options & OPT_GEN_C_CODE ) ) {
        _splitpath( GetHeaderFile(), NULL, NULL, fname, ext );
        if( fprintf( fp->fp, "\n#include \"%s%s\"\n", fname, ext ) < 0 ) {
            Error( FILE_WRITE_ERR, fp->name );
        }
    }
}

#define MANGLE_LEN      4
#define NON_MANGLE_LEN  ( PB_NAME_LEN - MANGLE_LEN )

void GenerateCoverFnName( char *uoname, char *fnname, char *buf ) {
/******************************************************************/

    unsigned    len1;
    unsigned    len2;
    char        mangle[MANGLE_LEN + 1];
    char        *tmpbuf;

    len1 = strlen( uoname );
    len2 = strlen( fnname );

    if( len1 + len2 < NON_MANGLE_LEN ) {
        strcpy( buf, uoname );
        strcat( buf, fnname );
    } else {
        tmpbuf = alloca( len1 + len2 + 1 );
        strcpy( tmpbuf, uoname );
        strcpy( tmpbuf + len1, fnname );
        GetHash( tmpbuf, mangle, MANGLE_LEN );
        strncpy( buf, tmpbuf, NON_MANGLE_LEN );
        strcpy( buf + NON_MANGLE_LEN, mangle );
    }
}

static void outPutFunc( const sp_header *sp, FileInfo *fp, char *class, int typ, char *line )
/*******************************************************************************************/
/* Construct and spit out an appropriate function header */
{
    var_rec     *parm;
    int         len;
    char        buffer[100];    /* function proto buffer */
    char        fnname[ PB_NAME_LEN + 1 ];

    assert( sp );
    assert( fp );
    assert( class );

    /* if we could not determine any of its types, dont process it */
    if( sp->fake ) {
        return;
    }

    /* process beginning of function header */
    parm = sp->parm_list;
    switch( typ ) {
    case( O_CLASS_PROTO ):
        sprintf( buffer, "virtual %s %s( ", ConvertRetType( sp->typ_id ),
                 sp->name );
        break;
    case( O_PROTOTYPE ):
    case( O_FULL_FUNC ):
        GenerateCoverFnName( class, sp->name, fnname );
        if( Options & OPT_GEN_C_CODE ) {
            sprintf( buffer, "%s PB_EXPORT %s( ",
                        ConvertRetType( sp->typ_id ), fnname );
        } else {
            len = sprintf( buffer, "%s PB_EXPORT %s( %s *this_hdl",
                           ConvertRetType( sp->typ_id ), fnname, class );
            if( parm && ( strcmp( parm->name, THIS_HDL ) || parm->next ) ) {
                sprintf( buffer + len, COMMA_DELIM );
            }
        }
        break;
    case( O_CLASS_FUNC ):
        sprintf( buffer, "%s %s::%s( ", ConvertRetType( sp->typ_id ), class,
                 sp->name);
        break;
    default:
        assert( FALSE );
    }
    if( line ) {
        strcpy( line, buffer );
    }
    if( fprintf( fp->fp, buffer ) < 0 ) {
        Error( FILE_WRITE_ERR, fp->name );
    }

    /* remove the this_hdl parm from the list if necessary */
    if( parm && !strcmp( parm->name, THIS_HDL ) ) {
        parm = parm->next;
    }

    /* process the parameters */
    while( parm ) {
        ConvertParmType( buffer, parm->name, parm->typ_id, parm->array );
        len = strlen( buffer );
        parm = parm->next;
        if( parm ) {
            sprintf( buffer + len, COMMA_DELIM );
        }
        if( line ) {
            strcat( line, buffer );
        }
        if( fprintf( fp->fp, buffer ) < 0 ) {
            Error( FILE_WRITE_ERR, fp->name );
        }
    }

    /* terminate function header */
    switch( typ ) {
    case( O_CLASS_PROTO ):
    case( O_PROTOTYPE ):
        sprintf( buffer, CLOSE_END );
        break;
    case( O_FULL_FUNC ):
    case( O_CLASS_FUNC ):
        sprintf( buffer, TERM_START_FUNC );
        break;
    default:
        assert( FALSE );
    }
    if( line ) {
        strcat( line, buffer );
    }
    if( fprintf( fp->fp, buffer ) < 0 ) {
        Error( FILE_WRITE_ERR, fp->name );
    }
}


static void outHeader( FileInfo *fp ) {
/**************************************/

    if( fputs( WIG_HEADER, fp->fp ) == EOF ) {
        Error( FILE_WRITE_ERR, fp->name );
    }
}


static void outCoverCall( sp_header *sp, FileInfo *fp ) {
/*******************************************************/

/* generate body of a cover function */

    var_rec     *parm;

    assert( sp );
    assert( fp );

    /* if we could not determine any of its types, dont process it */
    if( sp->fake ) {
        return;
    }

    /* indent code and begin start of call */
    if( fputs( INDENT, fp->fp ) == EOF )  Error( FILE_WRITE_ERR, fp->name );
    if( _BaseType( sp->typ_id ) ) {
        if( fputs( START_RETURN, fp->fp ) == EOF ) {
            Error( FILE_WRITE_ERR, fp->name );
        }
    }
    if( fprintf( fp->fp, START_CALL, sp->name ) < 0 ) {
        Error( FILE_WRITE_ERR, fp->name );
    }

    /* dump parameters */
    parm = sp->parm_list;

    /* remove the this_hdl parm from the list if necessary */
    if( parm && !strcmp( parm->name, THIS_HDL ) ) {
        parm = parm->next;
    }

    /* dump the parms */
    while( parm ) {
        if( fprintf( fp->fp, "%s", parm->name ) < 0 ) {
            Error( FILE_WRITE_ERR, fp->name );
        }
        parm = parm->next;
        if( parm ) {
            if( fprintf( fp->fp, COMMA_DELIM ) < 0 ) {
                Error( FILE_WRITE_ERR, fp->name );
            }
        }
    }

    /* finish of call */
    if( _BaseType( sp->typ_id ) ) {
        if( fprintf( fp->fp, CLOSE_PAREN ) < 0 ) {
            Error( FILE_WRITE_ERR, fp->name );
        }
    }
    if( fprintf( fp->fp, "%s%s\n", CLOSE_END, CLOSE_SCOPE ) < 0 ) {
        Error( FILE_WRITE_ERR, fp->name );
    }
}


static void generateCoverFile( sru_file *sru ) {
/***********************************************/

/* generate the cover file which calls the methods of the object */

    FileInfo    fpd;
    statement   *curr;
    char        *classname;
    int         rc;

    assert( sru );

    /* Open file */
    fpd.name = GetCoverFile();
    fpd.fp = WigOpenFile( fpd.name, "wt" );
    if( !fpd.fp ) {
        Error( FILE_OPEN_ERR, fpd.name );
    }
    curr = sru->cpp_prots;

    /* dump preliminary data */
    outHeader( &fpd );
    if( fprintf( fpd.fp, COVER_HEADER ) < 0 ) {
        Error( FILE_WRITE_ERR, fpd.name );
    }
    outInclude( &fpd );
    if( fprintf( fpd.fp, SCOPE_EXTERN_C ) < 0 ) {
        Error( FILE_WRITE_ERR, fpd.name );
    }
    classname = GetClass();

    /* dump prototypes */
    while( curr ) {
        if( !curr->data.sp.fake ) {
            outPutFunc( &(curr->data.sp), &fpd, classname, O_PROTOTYPE, NULL );
        }
        curr = curr->link;
    }
    if( !(Options & OPT_GEN_C_CODE) ) {
        if( fprintf( fpd.fp, CONS_DECL_TMPLT, classname, sru->con_name ) < 0 ) {
            Error( FILE_WRITE_ERR, fpd.name );
        }
        if( fprintf( fpd.fp, DES_DECL_TMPLT, sru->des_name, classname ) < 0 ) {
            Error( FILE_WRITE_ERR, fpd.name );
        }
    }
    if( fputs( CLOSE_SCOPE, fpd.fp ) == EOF
        || fputs( "\n", fpd.fp ) == EOF ) {
        Error( FILE_WRITE_ERR, fpd.name );
    }

    /* dump function calls */
    curr = sru->cpp_prots;
    while( curr ) {
        if( !curr->data.sp.fake ) {
            outPutFunc( &(curr->data.sp), &fpd, classname, O_FULL_FUNC, NULL );
            outCoverCall( &(curr->data.sp), &fpd );
        }
        curr = curr->link;
    }

    /* generate constructors if necessary */
    if( !(Options & OPT_GEN_C_CODE) ) {
        rc = fprintf( fpd.fp, "%s *PB_EXPORT %s() {\n"
                              "    return( new %s );\n"
                              "}\n\n", classname, sru->con_name, classname );
        if( rc < 0 ) {
            Error( FILE_WRITE_ERR, fpd.name );
        }
        rc = fprintf( fpd.fp, "void PB_EXPORT %s( %s *this_hdl ) {\n"
                              "    delete this_hdl;\n"
                              "}\n\n", sru->des_name, classname );
        if( rc < 0 ) {
            Error( FILE_WRITE_ERR, fpd.name );
        }
    }

    /* close file */
    WigCloseFile( fpd.fp );
}


static void transferUserData( FileInfo *fpi, FileInfo *fpo, BOOL look,
                              char *name ) {
/***************************************************************************/

/* transfers user code from input file to output file and stops if necessary
   at WIG code begin labels
*/

    char        *cmp;

    assert( fpi );
    assert( fpo );
    assert( name );

    if( look ) {
        cmp = alloca( sizeof( WIG_BEG_HEADER ) + strlen( name ) );
        sprintf( cmp, WIG_BEG_HEADER, name );
    }

    for( ;; ) {
        errno = 0;
        if( !fgets( lineBuffer, sizeof( lineBuffer ), fpi->fp ) ) {
            if( errno ) {
                Error( FILE_READ_ERR, fpi->name );
            }
            break;
        }
        if( look && !strcmp( lineBuffer, cmp ) ) {
                break;
        }
        if( fputs( lineBuffer, fpo->fp ) == EOF ) {
            Error( FILE_WRITE_ERR, fpo->name );
        }
    }
}

static BOOL doOutputDataMembers( FileInfo *fpo, statement *curr, char *prefix,
                                id_type access, char *header ) {
/**********************************************************************/

    int         type_fmt_len;
    VarInfo     *info;
    unsigned    var_cnt;
    unsigned    i;
    BOOL        need_header;
    char        arraydesc[30];
    int         rc;

    need_header = TRUE;
    type_fmt_len = 30;
    type_fmt_len -= strlen( prefix );
    if( type_fmt_len < 0 ) type_fmt_len = 0;
    while( curr != NULL ) {
        if( !curr->data.vars.fake && curr->data.vars.access_id == access ) {
            if( need_header && header != NULL ) {
                if( fputs( header, fpo->fp ) == EOF ) {
                    Error( FILE_WRITE_ERR, fpo->name );
                }
                need_header = FALSE;
            }
            var_cnt = GetListCount( curr->data.vars.varlist );
            for( i=0; i < var_cnt; i++ ) {
                info = GetListItem( curr->data.vars.varlist, i );
                if( !info->fake ) {
                    if( info->flags & VAR_ARRAY ) {
                        ConvertVarType( lineBuffer, arraydesc,
                                        curr->data.vars.typ_id,
                                        &info->array );
                    } else {
                        ConvertVarType( lineBuffer, arraydesc,
                                        curr->data.vars.typ_id, NULL );
                    }
                    rc = fprintf( fpo->fp, "%s%-*s %s%s;\n", prefix, type_fmt_len,
                             lineBuffer, info->name, arraydesc );
                    if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
                }
            }
        }
        curr = curr->link;
    }
    return( !need_header );
}

typedef struct {
    id_type     type;
    char        *header;
} AccessInfo;

static AccessInfo accessNames[] = {     ST_PROTECTED,   "protected:\n",
                                        ST_PRIVATE,     "private:\n",
                                        0,              NULL
};


static void outPutDataMembers( FileInfo *fpo, sru_file *sru ) {
/******************************************************/

    unsigned            i;
    char                *header;
    BOOL                rc;

    for( i=0; accessNames[i].header != NULL; i++ ) {
        header = accessNames[i].header;
        rc = doOutputDataMembers( fpo, sru->obj_vars, INDENT,
                                  accessNames[i].type, header );
        if( rc ) header = NULL;
        doOutputDataMembers( fpo, sru->shared_vars, INDENT"static ",
                           accessNames[i].type, header );
    }
}

static void dumpNewClassDef( sru_file *sru, FileInfo *fpo ) {
/*******************************************************/

/* declares a class */

    statement   *curr;
    char        *classname;
    char        *parent;
    int         rc;

    assert( fpo );
    assert( sru );

    /* create header */
    classname = GetClass();
    parent = GetParentClass();
    rc = fprintf( fpo->fp, WIG_BEG_HEADER, classname );
    if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
    if( parent != NULL ) {
        rc = fprintf( fpo->fp, "class %s : public %s {\n", classname, parent );
    } else {
        rc = fprintf( fpo->fp, "class %s {\n", classname );
    }
    if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );

    /* dump methods */
    curr = sru->cpp_prots;
    if( curr != NULL ) {
        rc = fprintf( fpo->fp, "public:\n" );
        if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
    }
    while( curr != NULL ) {
        if( !curr->data.sp.fake ) {
            rc = fprintf( fpo->fp, INDENT );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            outPutFunc( &(curr->data.sp), fpo, classname, O_CLASS_PROTO, NULL );
        }
        curr = curr->link;
    }

    outPutDataMembers( fpo, sru );

    rc = fprintf( fpo->fp, WIG_END_HEADER, classname );
    if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
}


static void skipToEndToken( FileInfo *fpi, char *name ) {
/***************************************************/

/* eats up file until it runs into a WIG END token */

    char        *cmp;

    assert( fpi );
    assert( name );

    cmp = alloca( sizeof( WIG_END_HEADER ) + strlen( name ) );
    sprintf( cmp, WIG_END_HEADER, name );

    for( ;; ) {
        errno = 0;
        if( !fgets( lineBuffer, sizeof( lineBuffer ), fpi->fp ) ) {
            if( errno ) {
                Error( FILE_READ_ERR, fpi->name );
            }
            break;
        }
        if( !strcmp( lineBuffer, cmp ) ) {
                break;
        }
    }
}

static BOOL isInclude( FileInfo *fpi ) {
/***************************************/

    if( !fgets( lineBuffer, sizeof( lineBuffer ), fpi->fp ) ) {
        Error( FILE_READ_ERR, fpi->name );
    }
    return( !strncmp( lineBuffer, "#include", 8 ) );
}

static void putParentInclude( FileInfo *fpo ) {
/**********************************************/
    char        *parent;

    parent = GetParentClass();
    if( parent != NULL ) {
        if( fprintf( fpo->fp, WIG_BEG_HEADER, GetClass() ) < 0 ||
            fprintf( fpo->fp, "#include \"%s\"\n", GetParentHeader() ) < 0 ||
            fprintf( fpo->fp, WIG_END_HEADER, GetClass() ) < 0 ||
            fprintf( fpo->fp, "\n" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo->name );
        }
    }
}

static BOOL filesAreTheSame( char *fname, FileInfo *fp1 ) {
/**********************************************************/

    FILE        *fp2;
    size_t      ret1;
    size_t      ret2;
    BOOL        ret;

    fp2 = WigOpenFile( fname, "rt" );
    if( fp2 == NULL ) {
        Error( FILE_OPEN_ERR, fname );
    }
    fseek( fp1->fp, 0, SEEK_SET );
    ret = FALSE;
    for( ;; ) {
        ret1 = fread( lineBuffer, 1, LINE_SIZE, fp1->fp );
        ret2 = fread( lineBuffer2, 1, LINE_SIZE, fp2 );
        if( ret1 == ret2 ) {
            if( memcmp( lineBuffer, lineBuffer2, ret1 ) ) break;
            if( ret1 < LINE_SIZE ) {
                ret = TRUE;
                break;
            }
        } else {
            break;
        }
    }
    WigCloseFile( fp2 );
    return( ret );
}

static BOOL generateHeaderFile( sru_file *sru ) {
/***********************************************/

/* generate the HPP file which has the class declaration */

    FileInfo    fpi;
    FileInfo    fpo;
    char        *infile;
    char        outfilebuf[MAX_PATH+1];
    char        *classname;
    char        *parent;
    BOOL        same;

    assert( sru );

    /* open appropriate files */
    classname = GetClass();
    infile = GetHeaderFile();
    parent = GetParentClass();
    fpi.fp = WigOpenFile( infile, "rt" );
    fpi.name = infile;
    errno = 0;
    genTmpFName( infile, outfilebuf );
    fpo.fp = WigOpenFile( outfilebuf, "wt" );
    if( fpo.fp == NULL ) {
        Error( FILE_OPEN_ERR, outfilebuf );
    }
    fpo.name = outfilebuf;

    /* dump class, and any user code if necessary */
    if( fpi.fp == NULL ) {
        outHeader( &fpo );
        if( fprintf( fpo.fp, HPP_HEADER, "", "" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
        if( fprintf( fpo.fp, "\n\n" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
        putParentInclude( &fpo );
        dumpNewClassDef( sru, &fpo );
        if( fprintf( fpo.fp, "public:\n" ) < 0
            || fprintf( fpo.fp, "%svirtual ~%s() {}", INDENT, classname ) < 0
            || fprintf( fpo.fp, PLACE_DECLARATIONS_HERE ) < 0
            || fprintf( fpo.fp, CLOSE_CLASS ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
    } else {
        transferUserData( &fpi, &fpo, TRUE, classname );
        if( isInclude( &fpi ) ) {
            putParentInclude( &fpo );
            skipToEndToken( &fpi, classname );
            transferUserData( &fpi, &fpo, TRUE, classname );
        } else {
            putParentInclude( &fpo );
        }
        dumpNewClassDef( sru, &fpo );
        skipToEndToken( &fpi, classname );
        transferUserData( &fpi, &fpo, FALSE, classname );
    }

    /* compare the generated header to the current one.  If they are
     * the same then don't update any files in the project.  This prevents
     * unneccisary makes */

    WigCloseFile( fpo.fp );
    if( fpi.fp != NULL ) {
        same = filesAreTheSame( outfilebuf, &fpi );
        WigCloseFile( fpi.fp );
        if( !same ) {
            if( remove( infile ) ) Error( FILE_RM_ERR, infile );
        } else {
            if( remove( outfilebuf ) ) {
                Error( FILE_RM_ERR, infile );
            }
            return( FALSE );
        }
    }
    if( rename( outfilebuf, infile ) ) {
        Error( FILE_REN_ERR, outfilebuf, infile );
    }
    return( TRUE );
}


static void addAdditionalMethods( statement *curr, FileInfo *fpo ) {
/***************************************************************/

/* adds more methods to the CPP file ( mehtods which have not been defined */

    char        *classname;
    int         rc;

    assert( fpo );

    classname = GetClass();

    /* loop through a linked list of methods until all are dumped */
    while( curr ) {
        if( !curr->data.sp.fake ) {
            /* dump stub function */
            rc = fprintf( fpo->fp, WIG_BEG_HEADER, classname );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            rc = fprintf( fpo->fp, IF_TRUE );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            if( Options & OPT_GEN_C_CODE ) {
                outPutFunc( &(curr->data.sp), fpo, classname, O_FULL_FUNC,
                            NULL);
            } else {
                outPutFunc( &(curr->data.sp), fpo, classname,
                            O_CLASS_FUNC, NULL);
            }
            rc = fprintf( fpo->fp, WIG_END_HEADER, classname );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            rc = fprintf( fpo->fp, PLACE_USER_CODE_HERE );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            if( (Options & OPT_GEN_RETURN) && !curr->data.sp.subroutine ) {
                if( IsPtrType( curr->data.sp.typ_id ) ) {
                     rc = fprintf( fpo->fp, RETURN_NULL );
                } else {
                     rc = fprintf( fpo->fp, RETURN_ZERO );
                }
                if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
            }
            rc = fprintf( fpo->fp, "%s%s\n\n", CLOSE_SCOPE, END_IF );
            if( rc < 0 ) Error( FILE_WRITE_ERR, fpo->name );
        }
        curr = curr->link; /* next one in the additional chain */
    }
}


static void removeFromMethodChain( statement **curr, statement *func )
/********************************************************************/
{
    statement   **finger;
    assert( curr );
    assert( func );

    finger = curr;

    while( *finger ) {
        if( *finger == func ) {
            *finger = func->link;
            func->link = NULL;
            return;
        }
        finger = &((*finger)->link);
    }
}


static void traverseCodeFile( statement **curr, sru_file *sru, FileInfo *fpi,
                              FileInfo *fpo ) {
/*****************************************/

/* moves through a WIG/user main CPP file avoiding user code, and modifying
   its own
*/

    statement   *func;
    char        *cmp_str;
    char        *ptr;
    char        *qtr;
    char        *classname;
    int         cmp_len;
    int         len;

    assert( curr );
    assert( sru );
    assert( fpi );
    assert( fpo );

    /* define searching parameters for line determination */
    classname = GetClass();
    if( Options & OPT_GEN_C_CODE ) {
        cmp_str = PBEXPORT_SRCH;
    } else {
        cmp_str = alloca( strlen( classname ) + 4 );
        sprintf( cmp_str, " %s::", classname );
    }
    cmp_len = strlen( cmp_str );

    /* loop through the file */
    for( ;; ) {
        /* pass through user code */
        transferUserData( fpi, fpo, TRUE, classname );
        if( feof( fpi->fp ) ) {
            break;
        }

        /* dump new WIG header and skip over unneeded code */
        if( fprintf( fpo->fp, WIG_BEG_HEADER, classname ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo->name );
        }
        if( !fgets( lineBuffer, sizeof( lineBuffer ), fpi->fp )
            || !fgets( lineBuffer, sizeof( lineBuffer ), fpi->fp ) ) {
            Error( FILE_READ_ERR, fpi->name );
        }

        /* Get function name */
        ptr = strstr( lineBuffer, cmp_str );
        if( !ptr ) {
            Error( FILE_FORMAT_ERR, NULL );
        }
        ptr += cmp_len;
        qtr = strchr( ptr, '(' );
        if( !qtr ) {
            Error( FILE_FORMAT_ERR, NULL );
        }
        *qtr = 0;
        len = strlen( ptr );

        /* determine whether we must ifdef it out and do so */
        func = FindHashEntry( sru->type_prots, HashString(ptr, len), ptr, len );
        if( func != NULL && !func->data.sp.fake ) {
            if( fprintf( fpo->fp, IF_TRUE ) < 0 ) {
                Error( FILE_WRITE_ERR, fpo->name );
            }
            if( Options & OPT_GEN_C_CODE ) {
                outPutFunc( &(func->data.sp), fpo, classname,
                            O_FULL_FUNC, lineBuffer2 );
            } else {
                outPutFunc( &(func->data.sp), fpo, classname,
                            O_CLASS_FUNC, lineBuffer2 );
            }
            removeFromMethodChain( curr, func );
            *qtr = '(';
            if( strcmp( lineBuffer2, lineBuffer ) ) {
                *qtr = 0;
                Warning( CHANGED_PROTOTYPE, ptr );
            }
        } else {
            if( fprintf( fpo->fp, "%s%s", IF_FALSE, lineBuffer ) < 0 ) {
                Error( FILE_WRITE_ERR, fpo->name );
            }
            *qtr = '(';
        }

        /* close WIG code and loop */
        skipToEndToken( fpi, classname );
        if( fprintf( fpo->fp, WIG_END_HEADER, classname ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo->name );
        }
    }
}


static void generateCodeFile( sru_file *sru ) {
/*********************************************/

/* generates the stub methods/functions for the interface class */

    FileInfo    fpi;
    FileInfo    fpo;
    char        *infile;
    char        outfilebuf[MAX_PATH+1];
    statement   *curr;

    assert( sru );

    /* open files */
    infile = GetCodeFile();
    fpi.fp = WigOpenFile( infile, "rt" );
    fpi.name = infile;
    errno = 0;
    genTmpFName( infile, outfilebuf );
    fpo.fp = WigOpenFile( outfilebuf, "wt" );
    if( !fpo.fp ) {
        Error( FILE_OPEN_ERR, outfilebuf );
    }
    fpo.name = outfilebuf;

    curr = sru->cpp_prots;
    /* go through file */
    if( fpi.fp ) {
        traverseCodeFile( &curr, sru, &fpi, &fpo );
        WigCloseFile( fpi.fp );
        if( remove( infile ) ) {
            Error( FILE_RM_ERR, infile );
        }
    } else {
        outHeader( &fpo );
        if( fprintf( fpo.fp, CPP_HEADER, "", "" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
        if( fprintf( fpo.fp, "\n" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
        outInclude( &fpo );
        if( fprintf( fpo.fp, "\n\n" ) < 0 ) {
            Error( FILE_WRITE_ERR, fpo.name );
        }
    }

    /* add new methods if any */
    addAdditionalMethods( curr, &fpo );

    /* close of files and rename to proper name */
    WigCloseFile( fpo.fp );
    if( rename( outfilebuf, infile ) ) {
        Error( FILE_REN_ERR, outfilebuf, infile );
    }
}
