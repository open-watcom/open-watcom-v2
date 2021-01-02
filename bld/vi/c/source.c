/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "posix.h"
#include "parsecl.h"
#include "menu.h"
#include "ex.h"
#include "fts.h"
#include "pathgrp2.h"

#include "clibext.h"


static void     finiSource( labels *, vars_list *, sfile *, undo_stack * );
static vi_rc    initSource( vars_list *, const char * );
static vi_rc    barfScript( const char *, sfile *, vars_list *, srcline *, const char *);
static void     addResidentScript( const char *, sfile *, labels * );
static resident *residentScript( const char * );
static void     finiSourceErrFile( const char * );

static void freeSource( sfile *sf )
{
    sfile   *curr, *next;

    for( curr = sf; curr != NULL; curr = next ) {
        next = curr->next;
        MemFree( curr->data );
        MemFree( curr->arg1 );
        MemFree( curr->arg2 );
        MemFree( curr );
    }
}

/*
 * Source - main driver
 */
vi_rc Source( const char *fn, const char *data, srcline *sline )
{
    undo_stack  *atomic = NULL;
    labels      *lab, lb;
    vars_list   vl;
    files       fi;
    sfile       *sf, *curr;
    char        buff[MAX_SRC_LINE];
    char        sname[FILENAME_MAX];
    const char  *tmp;
    vi_rc       rc;
    bool        sicmp, wfb, ssa, exm;
    resident    *res;
    int         cTokenID;

    /*
     * startup
     */
    LastRC = LastRetCode;
    memset( &fi, 0, sizeof( fi ) );
    vl.head = vl.tail = NULL;
    res = residentScript( fn );
    if( res != NULL && EditFlags.LoadResidentScript ) {
        return( ERR_SCRIPT_ALREADY_RESIDENT );
    }
    if( EditFlags.CompileScript || res == NULL ) {
        lab = &lb;
        memset( lab, 0, sizeof( labels ) );
        sf = NULL;
    } else {
        lab = &res->lab;
        sf = res->sf;
    }
    if( EditFlags.CompileScript ) {
        data = GetNextWord1( data, sname );
    }

    /*
     * initialize variables
     */
    memset( &fi, 0, sizeof( fi ) );

    /*
     * initialize command parms variables (local)
     */
    rc = initSource( &vl, data );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    /*
     * pre-process
     */
    sicmp = EditFlags.ScriptIsCompiled;
    SourceErrCount = 0;
    if( EditFlags.CompileScript || res == NULL ) {
        EditFlags.ScriptIsCompiled = false;
        rc = PreProcess( fn, &sf, lab );
        finiSourceErrFile( fn );
        if( rc != ERR_NO_ERR || SourceErrCount > 0 ) {
            EditFlags.ScriptIsCompiled = sicmp;
            return( rc );
        }
    } else {
        EditFlags.ScriptIsCompiled = res->scriptcomp;
    }

    /*
     * if we were loading a resident script, then add it
     */
    if( EditFlags.LoadResidentScript ) {
        finiSource( NULL, &vl, NULL, NULL );
        if( SourceErrCount == 0 ) {
            addResidentScript( fn, sf, lab );
        }
        EditFlags.ScriptIsCompiled = sicmp;
        return( ERR_NO_ERR );
    }

    /*
     * if we were compiling, dump results and go back
     */
    if( EditFlags.CompileScript ) {
        rc = barfScript( fn, sf, &vl, sline, sname );
        finiSource( lab, &vl, sf, NULL );
        return( rc );
    }

    /*
     * process each source line
     */
    exm = EditFlags.ExMode;
    wfb = EditFlags.WatchForBreak;
    ssa = EditFlags.SourceScriptActive;
    EditFlags.SourceScriptActive = true;
    EditFlags.WatchForBreak = true;
    EditFlags.ExMode = true;
    rc = ERR_NO_ERR;
    for( curr = sf->next;  rc == ERR_NO_ERR && curr != NULL; curr = curr->next ) {

        cTokenID = curr->token - SRC_T_NULL - 1;

        if( !EditFlags.Starting ) {
            if( EditFlags.BreakPressed ) {
                ClearBreak();
                break;
            }
        }
        rc = LastError = ERR_NO_ERR;
        tmp = curr->data;
        if( tmp == NULL ) {
            tmp = "";
        }

        if( EditFlags.Appending ) {
            if( curr->hasvar ) {
                tmp = Expand( buff, tmp, &vl );
            }
            rc = AppendAnother( tmp );
        } else if( cTokenID == PCL_T_ENDFILETYPESOURCE ) {
            rc = FTSEnd();
        } else if( EditFlags.FileTypeSource ) {
            rc = FTSAddCmd( tmp, curr->token );
        } else if( curr->token == SRC_T_RETURN ) {
            if( curr->data != NULL ) {
                int     ret;

                rc = GetErrorTokenValue( &ret, curr->data );
                if( rc == ERR_NO_ERR ) {
                    rc = (vi_rc)ret;
                } else if( rc == NO_NUMBER ) {
                    rc = (vi_rc)atoi( curr->data );
                } else {
                    rc = ERR_NO_ERR;
                }
            } else {
                rc = ERR_NO_ERR;
            }
            break;
        } else if( curr->token > SRC_T_NULL ) {
            if( curr->hasvar ) {
                tmp = Expand( buff, tmp, &vl );
            }
            rc = TryCompileableToken( cTokenID, tmp, false );
            if( rc == NOT_COMPILEABLE_TOKEN ) {
                rc = ProcessWindow( cTokenID, tmp );
            }
            if( rc < ERR_NO_ERR ) {
                rc = ERR_NO_ERR;
            }
        } else {
            switch( curr->token ) {
            case SRC_T_ATOMIC:
                if( atomic == NULL ) {
                    atomic = UndoStack;
                    StartUndoGroup( atomic );
                }
                break;
            case SRC_T_IF:
                rc = SrcIf( &curr, &vl );
                break;
            case SRC_T_GOTO:
                rc = SrcGoTo( &curr, tmp, lab );
                break;
            case SRC_T_LABEL:
                break;
            case SRC_T_GET:
                SrcGet( tmp, &vl );
                rc = ERR_NO_ERR;
                break;
            case SRC_T_INPUT:
                LastRC = SrcInput( tmp, &vl );
                if( LastRC != NO_VALUE_ENTERED && LastRC != ERR_NO_ERR ) {
                    rc = LastRC;
                }
                break;
            case SRC_T_NEXTWORD:
                rc = SrcNextWord( tmp, &vl );
                break;
            case SRC_T_ASSIGN:
                rc = SrcAssign( tmp, &vl );
                break;
            case SRC_T_EXPR:
                rc = SrcExpr( curr, &vl );
                break;
            case SRC_T_OPEN:
                LastRC = SrcOpen( curr, &fi, tmp, &vl );
                if( LastRC != ERR_FILE_NOT_FOUND && LastRC != ERR_NO_ERR ) {
                    rc = LastRC;
                }
                break;
            case SRC_T_READ:
                LastRC = SrcRead( curr, &fi, tmp, &vl );
                if( LastRC != END_OF_FILE && LastRC != ERR_NO_ERR ) {
                    rc = LastRC;
                }
                break;
            case SRC_T_WRITE:
                rc = SrcWrite( curr, &fi, tmp, &vl );
                break;
            case SRC_T_CLOSE:
                rc = SrcClose( curr, &fi, tmp, &vl );
                break;
            default:
    #ifdef __WIN__
                {
                    if( RunWindowsCommand( tmp, &LastRC, &vl ) ) {
                        rc = LastRC;
                        break;
                    }
                }
    #endif
                if( curr->hasvar ) {
                    tmp = Expand( buff, tmp, &vl );
                }
                LastRC = RunCommandLine( tmp );
                if( LastRC == DO_NOT_CLEAR_MESSAGE_WINDOW ) {
                    LastRC = LastError;
                }
                break;
            }
        }
    }
    if( EditFlags.Appending ) {
        AppendAnother( "." );
    }
    if( curr != NULL ) {
        *sline = curr->sline;
    } else {
        *sline = CurrentSrcLine;
        rc = ERR_NO_ERR;
    }
    EditFlags.WatchForBreak = wfb;
    EditFlags.SourceScriptActive = ssa;
    EditFlags.ScriptIsCompiled = sicmp;
    EditFlags.ExMode = exm;
    if( res != NULL && !EditFlags.CompileScript ) {
        sf = NULL;
        lab = NULL;
    }
    finiSource( lab, &vl, sf, atomic );
    return( rc );

} /* Source */

/*
 * initSource - initialize command parms variables
 */
static vi_rc initSource( vars_list *vl, const char *data )
{
    int         j;
    char        tmp[MAX_SRC_LINE];
    char        name[MAX_NUM_STR];
    char        all[MAX_SRC_LINE];

    /*
     * break up command line parms
     */
    all[0] = '\0';
    for( j = 1; GetNextWordOrString( &data, tmp ) == ERR_NO_ERR; ++j ) {
        sprintf( name, "%d", j );
        VarAddStr( name, tmp, vl );
        if( j == 1 ) {
            strcpy( all, tmp );
        } else {
            StrMerge( 2, all, SingleBlank, tmp );
        }
    }
    VarAddStr( "*", all, vl );

    return( ERR_NO_ERR );

} /* initSource */

/*
 * finiSource - release language variables
 */
static void finiSource( labels *lab, vars_list *vl, sfile *sf, undo_stack *atomic )
{
    info        *cinfo;

    if( lab != NULL ) {
        MemFreeList( lab->cnt, lab->name );
        MemFree( lab->pos );
    }

    VarListDelete( vl );

    freeSource( sf );

    /*
     * make sure this undo stack is still around
     */
    if( atomic != NULL ) {
        for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
            if( atomic == cinfo->UndoStack ) {
                break;
            }
        }
        if( cinfo != NULL ) {
            EndUndoGroup( atomic );
        }
    }

} /* finiSource */

/*
 * FileSPVAR - build file special variables
 */
void FileSPVAR( void )
{
    char        path[_MAX_PATH];
    pgroup2     pg;
    int         i;

    /*
     * build path
     */
    if( CurrentFile == NULL ) {
        GlobVarAddStr( GLOBVAR_FILEFULLNAME, "" );
        GlobVarAddStr( GLOBVAR_FILEHOME, "" );
        pg.drive = pg.dir = pg.fname = pg.ext = "";
    } else {
        GlobVarAddStr( GLOBVAR_FILEFULLNAME, CurrentFile->name );
        GlobVarAddStr( GLOBVAR_FILEHOME, CurrentFile->home );
        ConditionalChangeDirectory( CurrentFile->home );
        _splitpath2( CurrentFile->name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    }
    GlobVarAddStr( GLOBVAR_USER_FILEPATH, pg.dir );
    GlobVarAddStr( GLOBVAR_USER_FILEDRIVE, pg.drive );
    _makepath( path, pg.drive, pg.dir, NULL, NULL );
    i = strlen( path );
    if( i-- > 1 ) {
#ifdef __UNIX__
        if( path[i] == FILE_SEP ) {
#else
        if( path[i] == FILE_SEP && ( i != 2 || path[1] != DRV_SEP ) ) {
#endif
            path[i] = '\0';
        }
    }
    if( CurrentFile != NULL ) {
        PushDirectory( path );
        GetCWD2( path, sizeof( path ) );
        PopDirectory();
    } else {
        path[0] = '\0';
    }
    if( path[strlen( path ) - 1] == FILE_SEP ) {
        StrMerge( 2, path, pg.fname, pg.ext );
    } else {
        StrMerge( 3, path, FILE_SEP_STR, pg.fname, pg.ext );
    }
    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    GlobVarAddStr( GLOBVAR_FILEDRIVE, pg.drive );
    GlobVarAddStr( GLOBVAR_FILEPATH, pg.dir );
    GlobVarAddStr( GLOBVAR_FILENAME, pg.fname );
    GlobVarAddStr( GLOBVAR_FILEEXT, pg.ext );

} /* FileSPVAR */

static char srcErrFileName[] = "__err__.vi_";
static FILE *srcErrFile;

/*
 * SourceError - dump a source error
 */
void SourceError( char *msg )
{
    if( EditFlags.CompileScript ) {
        if( srcErrFile == NULL ) {
            srcErrFile = fopen( srcErrFileName, "w" );
            if( srcErrFile == NULL ) {
                return;
            }
        }
        MyFprintf( srcErrFile, "Error on line %u: \"%s\"\n", CurrentSrcLine, msg );
    }
    SourceErrCount++;

} /* SourceError */


/*
 * finiSourceErrFile - close up error file
 */
static void finiSourceErrFile( const char *fn )
{
    pgroup2     pg;
    char        path[FILENAME_MAX];
    char        tmp[MAX_SRC_LINE];

    if( !EditFlags.CompileScript ) {
        return;
    }
    _splitpath2( fn, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    _makepath( path, pg.drive, pg.dir, pg.fname, "err" );
    remove( path );
    if( srcErrFile != NULL ) {
        GetDateTimeString( tmp );
        MyFprintf( srcErrFile, "\nCompile of %s finished on %s\n", fn, tmp );
        MyFprintf( srcErrFile, "%d errors encountered\n", SourceErrCount );
        fclose( srcErrFile );
        srcErrFile = NULL;
        rename( srcErrFileName, path );
    }

} /* finiSourceErrFile */

/*
 * barfScript - write a compiled script
 */
static vi_rc barfScript( const char *fn, sfile *sf, vars_list *vl, srcline *sline, const char *vn )
{
    sfile       *curr;
    FILE        *foo;
    pgroup2     pg;
    char        path[FILENAME_MAX];
    char        buff[MAX_SRC_LINE];
    const char  *tmp;
    size_t      i, k;
    vi_rc       rc;

    /*
     * get compiled file name, and make error file
     */
    if( vn[0] == '\0' ) {
        _splitpath2( fn, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
        _makepath( path, pg.drive, pg.dir, pg.fname, "_vi" );
    } else {
        strcpy( path, vn );
    }
    foo = fopen( path, "w" );
    if( foo == NULL ) {
        return( ERR_FILE_OPEN );
    }
    MyFprintf( foo, "VBJ__\n" );
    *sline = 1;

    /*
     * process all lines
     */
    for( curr = sf; curr != NULL; curr = curr->next ) {
        if( curr->token == SRC_T_NULL )
            continue;
        tmp = curr->data;
        if( tmp == NULL ) {
            tmp = "";
        }

        /*
         * expand variables, if requested
         */
        if( EditFlags.CompileAssignments ) {
            /*
             * process the assign command
             */
            if( curr->token == SRC_T_ASSIGN ) {
                rc = SrcAssign( tmp, vl );
                if( rc != ERR_NO_ERR ) {
                    fclose( foo );
                    return( rc );
                }
                if( !EditFlags.CompileAssignments ) {
                    tmp = curr->data;
                    EditFlags.CompileAssignments = true;
                } else {
                    continue;
                }
            } else {
                if( curr->token != SRC_T_IF ) {
                    if( curr->hasvar ) {
                        tmp = Expand( buff, tmp, vl );
                        curr->hasvar = false;
                        k = strlen( curr->data );
                        for( i = 0; i < k; i++ ) {
                            if( curr->data[i] == '%' ) {
                                curr->hasvar = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        /*
         * process any additional commands
         */
        switch( curr->token ) {
        /*
         * process the map command
         */
        case PCL_T_MAP + SRC_T_NULL + 1:
        case PCL_T_MAP_DMT + SRC_T_NULL + 1:
            if( curr->token == PCL_T_MAP_DMT + SRC_T_NULL + 1 ) {
                rc = MapKey( MAPFLAG_DAMMIT, tmp );
            } else {
                rc = MapKey( 0, tmp );
            }
            if( rc != ERR_NO_ERR ) {
                fclose( foo );
                return( rc );
            }
            tmp = WorkLine->data;
            break;
        }

        /*
         * spew out line
         */
        MyFprintf( foo, "%c%d %s", ( curr->hasvar ) ? '1' : '0', curr->token, tmp );
        if( curr->token == SRC_T_GOTO ) {
            MyFprintf( foo, " %d", curr->branchcond );
        }
        MyFprintf( foo, "\n" );
        *sline += 1;

    }
    fclose( foo );
    return( ERR_NO_ERR );

} /* barfScript */


static resident *resHead = NULL;

/*
 * addResidentScript - add a script to the resident list
 */
static void addResidentScript( const char *fn, sfile *sf, labels *lab )
{
    resident    *tmp;

    tmp = MemAlloc( sizeof( resident ) );
    tmp->fn = DupString( fn );
    tmp->sf = sf;
    memcpy( &tmp->lab, lab, sizeof( labels ) );
    tmp->scriptcomp = EditFlags.ScriptIsCompiled;
    tmp->next = resHead;
    resHead = tmp;

} /* addResidentScript */


/*
 * DeleteResidentScripts - delete the resident list
 */
void DeleteResidentScripts( void )
{
    resident    *tmp, *tmp_next;

    for( tmp = resHead; tmp != NULL; tmp = tmp_next ) {
        tmp_next = tmp->next;
        freeSource( tmp->sf );
        MemFreeList( tmp->lab.cnt, tmp->lab.name );
        MemFree( tmp->lab.pos );
        MemFree( tmp->fn );
        MemFree( tmp );
    }

} /* DeleteResidentScripts */


/*
 * residentScript - check for a resident script
 */
static resident *residentScript( const char *fn )
{
    resident    *tmp;

    for( tmp = resHead; tmp != NULL; tmp = tmp->next ) {
        if( stricmp( fn, tmp->fn ) == 0 ) {
            break;
        }
    }
    return( tmp );

} /* residentScript */
