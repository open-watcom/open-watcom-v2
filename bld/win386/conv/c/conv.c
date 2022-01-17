/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Generate *.obj and *.asm thunks for interfacing from
*               WIN386 (32-bit flat) to 16-bit Windows API function
*               (16-bit segmented). It uses Pharlap Easy OMF (32-bit)
*               for *.obj thunks.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "clibext.h"

#define MAX_BUFF 256

/*
 * depth into stack from which to get parms: 3 dwords pushed
 */
#define STACK_FRAME  3*4+4

#define LINE  ";****************************************************************************\n"
#define BLANK ";***                                                                      ***\n"

#define STRIP_TRAIL_WS(i,s) for( i = strlen( s ); i-- > 0 && isspace( (s)[i] ); ) (s)[i] = '\0'
#define SKIP_LEAD_WS(s)     while( isspace( *(s) ) ) (s)++


int genstubs = 0;
int quiet = 0;
char *listfile = NULL;

typedef enum {
    PARM_PTR,
    PARM_WORD,
    PARM_DWORD,
    PARM_VOID,
    PARM_DOUBLE
} parm_types;

typedef enum {
    RETURN_PTR,
    RETURN_INT,
    RETURN_CHAR,
    RETURN_DWORD,
    RETURN_VOID
} return_types;

struct  subparm {
    struct subparm *nextparm;
    unsigned    offset;
    unsigned    parmnum;
};

/*
 * data structure containing all data about a prototype
 */
typedef struct fcn {
    struct fcn  *next;      /* link */
    char        *fn;            /* function name */
    char        *plist;         /* parameter list */
    struct subparm *subparms;   /* sub parms that need aliasing */
    int         class;          /* what class it belongs to */
    struct fcn  *next_class;/* connects to next function in same class */
    char        pcnt;           /* number of parms */
    char        returntype;     /* function return type */
    char        aliascnt;       /* number of 16-bit aliases to be created */
    unsigned    thunkindex:5;   /* ThunkStrs index */
    unsigned    thunk:1;        /* requires a thunking layer */
    unsigned    is_16:1;        /* is an _16 function */
    unsigned    noregfor_16:1;  /* _16 function has no regular function */
    unsigned    need_fpusave:1; /* function requires floating point save */
    unsigned    __special_func:1;/* special @func requires extra thunking */
} fcn;

struct parmlist {
    struct parmlist     *next;
    char                parm_count;
    char                alias_count;
    char                parm_types[1];
};

char    **ThunkStrs;
char    * ThunkGenerated;
int     ThunkIndex;
int     MaxAliasCount;
struct parmlist *ParmList;

fcn     *Class, *CurrClass;      /* class list */
fcn     *Head, *Curr;            /* list of all prototypes */
fcn     *VoidHead, *VoidCurr;    /* list of all prototypes */

static void *myalloc( size_t size )
{
    void        *tmp;

    tmp = calloc( 1, size );
    if( tmp == NULL ) {
        printf("Out of Memory!\n");
        exit( 1 );
    }
    return( tmp );

} /* myalloc */

static void *_fmyalloc( size_t size )
{
    void     *tmp;

    tmp = malloc( size );
    if( tmp == NULL ) {
        printf("Out of Memory!\n");
        exit( 1 );
    }
    memset( tmp, 0, size );
    return( tmp );

} /* _fmyalloc */

static void *_fmyrealloc( void *ptr, size_t size )
{
    void     *tmp;

    tmp = realloc( ptr, size );
    if( tmp == NULL ) {
        printf("Out of Memory!\n");
        exit( 1 );
    }
    return( tmp );
} /* _fmyrealloc */

static int IsWord( char *str )
{
    if( !stricmp( str,"int" ) ||
        !stricmp( str,"char" ) ||
        !stricmp( str,"unsigned char" ) ||
        !stricmp( str,"short" ) ||
        !stricmp( str,"unsigned" ) ||
        !stricmp( str,"unsigned short" ) ||
        !stricmp( str,"unsigned int" ) ) {
        return( 1 );
    }
    return( 0 );

} /* IsWord */

/*
 * ClassifyParm - decide if a parm is a pointer, word or dword
 */
static parm_types ClassifyParm( char *buff )
{
    size_t  i;

    STRIP_TRAIL_WS( i, buff );
    if( IsWord( buff ) ) {
        return( PARM_WORD );
    } else if( strchr( buff, '*' ) != NULL ) {
        return( PARM_PTR );
    } else if( stricmp( buff, "void" ) == 0 ) {
        return( PARM_VOID );
    } else {
        return( PARM_DWORD );
    }

} /* ClassifyParm */

/*
 * ClassifyReturnType - decide if type is a pointer, int, void, char, or dword
 */
static return_types ClassifyReturnType( char *buff )
{
    size_t  i;

    STRIP_TRAIL_WS( i, buff );
    if( stricmp( buff, "int" ) == 0 || stricmp( buff, "short" ) == 0 ) {
        return( RETURN_INT );
    } else if( stricmp( buff, "void" ) == 0 ) {
        return( RETURN_VOID );
    } else if( stricmp( buff, "char" ) == 0 ) {
        return( RETURN_CHAR );
    } else if( strchr( buff, '*' ) != NULL ) {
        return( RETURN_PTR );
    } else {
        return( RETURN_DWORD );
    }

} /* ClassifyReturnType */

static void ClassifyParmList( char *plist, fcn *tmpf )
{
    int             i;
    int             j;
    int             k;
    int             parmcnt;
    struct parmlist *p;
    struct subparm  *subparm;
    char            parm_list[32];
    char            c;
    char            parmtype;

    k = 0;
    i = 0;
    parmcnt = 0;
    for( ;; ) {
        c = plist[i];
        if( c == '\0' || c == ',' ) {
            char *t;

            plist[i] = '\0';
            t = &plist[k];
            SKIP_LEAD_WS( t );
            parmtype = ClassifyParm( t );
            if( parmtype == PARM_DOUBLE ) {
                parmtype = PARM_DWORD;
                parm_list[parmcnt++] = parmtype;
            }
            parm_list[parmcnt++] = parmtype;
            if( c == '\0' )
                break;
            k = i + 1;
        } else if( c == '[' ) {
            plist[i] = '\0';
            j = i + 1;
            for( ;; ) {
                i++;
                c = plist[i];
                if( c == ';' || c == ']' ) {
                    plist[i] = '\0';
                    subparm = myalloc( sizeof( struct subparm ) );
                    subparm->nextparm = tmpf->subparms;
                    tmpf->subparms = subparm;
                    subparm->parmnum = parmcnt;
                    subparm->offset = atoi( &plist[j] );
                    j = i + 1;
                    if( c == ']' ) {
                        break;
                    }
                }
            }
        }
        i++;
    }
    if( parmcnt == 1 && parm_list[0] == PARM_VOID ) {
        tmpf->pcnt = 0;
        tmpf->aliascnt = 0;
        tmpf->plist = NULL;
    } else {
        for( p = ParmList; p != NULL; p = p->next ) {
            if( p->parm_count == parmcnt ) {
                if( memcmp( p->parm_types, parm_list, parmcnt ) == 0 ) {
                    break;
                }
            }
        }
        if( p == NULL ) {
            p = myalloc( sizeof( struct parmlist ) + parmcnt );
            p->next = ParmList;
            ParmList = p;
            p->parm_count = (char)parmcnt;
            memcpy( p->parm_types, parm_list, parmcnt );
            p->alias_count = 0;
            for( i = 0; i < parmcnt; i++ ) {
                if( parm_list[i] == PARM_PTR ) {
                    p->alias_count++;
                }
            }
            if( p->alias_count > MaxAliasCount ) {
                MaxAliasCount = p->alias_count;
            }
        }
        tmpf->pcnt = (char)parmcnt;
        tmpf->aliascnt = p->alias_count;
        tmpf->plist = p->parm_types;
    }
} /* ClassifyParmList */


static void ProcessDefFile( FILE *f )
{
    char        buffer[MAX_BUFF];
    char        *buff;
    size_t      i,j,k;
    char        *fn;            // function name
    char        *type;          // return type
    char        *plist;         // parameter list
    fcn         *tmpf;

    while( (buff = fgets( buffer, sizeof( buffer ), f )) != NULL ) {
        SKIP_LEAD_WS( buff );
        if( buff[0] == '#' || buff[0] == '\0' ) {
            continue;
        }
        STRIP_TRAIL_WS( i, buff );
        if( buff[0] == '!' ) {
            ThunkStrs = _fmyrealloc( ThunkStrs, sizeof( char * ) * ( ThunkIndex + 1 ) );
            ThunkStrs[ThunkIndex] = _fmyalloc( i + 1 );
            strcpy( ThunkStrs[ThunkIndex], &buff[1] );
            ThunkIndex++;
            continue;
        }
        /*
         * flag a function that returns a pointer to a function
         */
        if( strncmp( buff, "int (", 5 ) == 0 ) {
            printf( "Oh, oh, something returns a function!!!\n" );
            exit( 1 );
        }

        /*
         * look for opening paren
         */
        i = 0;
        while( buff[i] != '(' ) {
            i++;
        }

        /*
         * look for closing ')'
         */
        j = i;
        k = 0;                  // keep track of nested ()
        for( ;; ) {
            if( buff[j] == ')' ) {
                --k;
                if( k == 0 ) {
                    break;
                }
            } else if( buff[j] == '(' ) {
                ++k;
            }
            ++j;
        }

        /*
         * back up away from opening paren to obtain function name
         */
        k = i;
        for( ;; ) {
            if( buff[k] == ' ' )
                break;
            if( buff[k] == '*' )
                break;
            if( k == 0 )
                break;
            k--;
        }
        if( k == 0 ) {          /* no function name and/or no type */
            fprintf( stderr, "ERR: %s", buff );
            continue;
        }
        //  int function(long,int);
        //  ^  ^        ^        ^
        //  |  |        |        |
        //  0  k        i        j

        type = &buff[0];        // return type
        if( buff[k] == '*' ) {
            buff[k - 1] = '*';
        }
        buff[k] = '\0';
        fn = &buff[k + 1];      // function name
        buff[i] = '\0';
        plist = &buff[i + 1];   // parameter list
        buff[j] = '\0';

        /*
         * save all data but parms
         */
        tmpf = _fmyalloc( sizeof( fcn ) );
        tmpf->returntype = ClassifyReturnType( type );

        tmpf->need_fpusave = 0;
        if( *fn == '^' ) {
            fn++;
            tmpf->need_fpusave = 1;
        }
        tmpf->__special_func = 0;
        if( *fn == '@' ) {
            *fn = '#';
            tmpf->__special_func = 1;
        }
        if( *fn == '#' ) {
            fn++;
            tmpf->thunk = 1;
            tmpf->thunkindex = ThunkIndex - 1;
        } else {
            tmpf->thunk = 0;
        }
        tmpf->fn = myalloc( strlen( fn ) + 1 );
        strcpy( tmpf->fn, fn );
        tmpf->noregfor_16 = 0;
        if( !strncmp( fn, "_16", 3 ) ) {
            tmpf->is_16 = 1;
        } else {
            tmpf->is_16 = 0;
        }
        ClassifyParmList( plist, tmpf );

        /*
         * chain this guy into our list
         */
        if( tmpf->returntype == RETURN_VOID ) {
            if( VoidHead == NULL ) {
                VoidCurr = VoidHead = tmpf;
            } else {
                VoidCurr->next = tmpf;
                VoidCurr = tmpf;
            }
        } else {
            if( Head == NULL ) {
                Curr = Head = tmpf;
            } else {
                Curr->next = tmpf;
                Curr = tmpf;
            }
        }
    }
}

static void BuildClasses( void )
{
    int         j;
    int         class_count;
    fcn         *tmpf;
    fcn         *cl;

    /*
     * attach void list to end: Having the void list on the end ensures
     * that void's will be put in a class with a non-void.  Otherwise,
     * if a void came before a non-void, then the non-void would be
     * put in the same class as the void.  In English, a function
     * with a return value would not get a return value at all.
     */
    if( Curr != NULL ) {
        Curr->next = VoidHead;
    } else {
        Head = VoidHead;
    }
    Curr = VoidCurr;

    /*
     * build classes : functions with the same parameter list and
     * the same return type (word/dword) are put together so that
     * we only have to generate the code for them once.  Void functions
     * are thrown in with whoever has the same parameter types as they
     * do (return value is ignored).
     */
    CurrClass = Class = Head;
    class_count = 0;
    Class->class = class_count++;
    for( tmpf = Head->next; tmpf != NULL; tmpf = tmpf->next ) {
        j = 0;
        for( cl = Class; cl != NULL; cl = cl->next_class ) {
            /*
             * same number of parameters, and need fpu save?
             */
            if( tmpf->pcnt == cl->pcnt ) {
                if( tmpf->plist == cl->plist ) {
                    tmpf->class = cl->class;
                    j = 1;
                    break;
                }
            }
        }
        if( j == 0 ) {
            tmpf->class = class_count++;
            CurrClass->next_class = tmpf;
            CurrClass = tmpf;
        }
    }
    if( !quiet ) {
        printf( "Total Classes: %d\n", class_count );
    }
} /* BuildClasses */

/*
 * ClosingComments
 */
static void ClosingComments( void )
{
    fcn         *tmpf;

    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->fn[0] == '_' && tmpf->fn[1] == '_' ) {
            if( !quiet ) {
                printf("stub for '%s' requires intervention\n", tmpf->fn );
            }
        }
    }

} /* ClosingComments */

/*
 * GenerateThunkC - generate dllthunk.c
 */
static void GenerateThunkC( void )
{
    FILE        *f;

    f = fopen( "dllthunk.c", "w" );
    fprintf( f, "/*\n * This file automatically generated by CONV.EXE\n */\n" );
    fprintf( f, "#include <stdio.h>\n" );
    fprintf( f, "#include <windows.h>\n" );
    fprintf( f, "#include \"dllthunk.h\"\n" );
    fprintf( f, "\n" );
    fprintf( f, "void DLLLoadFail( char *str )\n" );
    fprintf( f, "{\n" );
    fprintf( f, "    MessageBox( NULL, \"DLL Load Failed\", str, MB_OK );\n" );
    fprintf( f, "}\n" );
    fclose( f );

} /* GenerateThunkC */

#define OBJBUF_SIZE     512
static FILE *objFile = NULL;
static int objBufIndex;
static char objBuf[OBJBUF_SIZE];

static void writeObjBuf( void )
{
    if( objBufIndex != 0 ) {
        fwrite( objBuf, 1, objBufIndex, objFile );
        objBufIndex = 0;
    }
}

static void writeObj( void *p, int len )
{
    int         n;

    for( ;; ) {
        n = ( OBJBUF_SIZE - objBufIndex < len ) ? OBJBUF_SIZE - objBufIndex : len;
        memcpy( &objBuf[objBufIndex], p, n );
        objBufIndex += n;
        if( objBufIndex == OBJBUF_SIZE ) {
            writeObjBuf();
        }
        len -= n;
        if( len == 0 )
            break;
        p = (char *)p + n;
    }
}

static void emitBYTE( int byte )
{
    char        b;

    b = (char)byte;
    writeObj( &b, 1 );
}

static void emitWORD( int word )
{
    unsigned short  w;

    w = (unsigned short)word;
    writeObj( &w, 2 );
}

static void emitDWORD( unsigned long dword )
{
#if defined( _M_I86 )
    unsigned long   dw;
#else
    unsigned        dw;
#endif

    dw = dword;
    writeObj( &dw, 4 );
}

static void emitSTRING( char *data )
{
    char        len;

    len = (char)strlen( data );
    writeObj( &len, 1 );
    writeObj( data, len );
}

static void emitTHEADR( int modindex )
{
    char        buff[20];

    sprintf( buff, "win%d", modindex );
    emitBYTE( 0x80 );
    emitWORD( (int)( 2 + strlen( buff ) ) );
    emitSTRING( buff );
    emitBYTE( 0 );
}

static void emitCOMMENT( void )
{
    emitBYTE( 0x88 );
    emitWORD( 8 );
    emitBYTE( 0x80 );
    emitBYTE( 0xAA );
    emitBYTE( 0x38 );
    emitBYTE( 0x30 );
    emitBYTE( 0x33 );
    emitBYTE( 0x38 );
    emitBYTE( 0x36 );
    emitBYTE( 0x0 );
}

static void emitLNAMES( void )
{
    emitBYTE( 0x96 );
    emitWORD( (int)( 1 + 1 + strlen( "" ) + 1 + strlen( "_TEXT" ) + 1 + strlen( "CODE" ) ) );
    emitSTRING( "" );
    emitSTRING( "_TEXT" );
    emitSTRING( "CODE" );
    emitBYTE( 0 );
}

static void emitSEGDEF( unsigned long segment_length )
{
    emitBYTE( 0x98 );
    emitWORD( 9 );              /* len */
    emitBYTE( 0x48 );           /* attributes */
    emitDWORD( segment_length );/* length of segment (# of bytes of code) */
    emitBYTE( 2 );              /* seg name index */
    emitBYTE( 3 );              /* class name index */
    emitBYTE( 1 );              /* ovl name index */
    emitBYTE( 0 );
}


static void emitPUBDEF( char *name )
{
    emitBYTE( 0x90 );
    emitWORD( (int)( 2 + 1 + strlen( name ) + 6 ) );
    emitBYTE( 0 );              /* group index */
    emitBYTE( 1 );              /* seg index */
    emitSTRING( name );
    emitWORD( 0 );              /* frame num? */
    emitWORD( 0 );              /* offset */
    emitBYTE( 0 );              /* type */
    emitBYTE( 0 );
}

static void emitEXTDEF( char *func )
{
    emitBYTE( 0x8c );
    emitWORD( (int)( strlen( func ) + 1 + 1 + 1 ) );
    emitSTRING( func );
    emitBYTE( 0 );                      /* type */
    emitBYTE( 0 );
}

static char *getThunkName( fcn *tmpf )
{
    char        *name;

    if( tmpf->thunk == 0 )
        return( NULL );
    if( tmpf->is_16 ) {
        name = &tmpf->fn[3];
    } else {
        name = tmpf->fn;
    }
    return( name );
}

static size_t sizeofThunkName( fcn *tmpf )
{
    char        *name;

    name = getThunkName( tmpf );
    if( name == NULL )
        return( 0 );
    return( strlen( name ) + 1 + 1 );
}

static void emitThunkName( fcn *tmpf )
{
    char        *name;

    emitBYTE( tmpf->thunkindex );
    for( name = getThunkName( tmpf ); *name != '\0'; name++ ) {
        emitBYTE( *name );
    }
    emitBYTE( 0 );
}

static void emitSaveFPU( fcn *tmpf )
{
    if( tmpf->need_fpusave ) {
        emitBYTE( 0xFF );       /* call dword ptr [__addr] */
        emitBYTE( 0x15 );
        emitDWORD( 0 );
    }
}

static int sizeofSubParms( struct subparm *p )
{
    int         size;

    size = 0;
    for( ; p != NULL; p = p->nextparm ) {
        size += 5;
    }
    if( size )
        size += 2 + 3;      // for "push count" and "add esp,nnn"
    return( size );
}

static void emitSubParms( struct subparm *p )
{
    int         count;

    count = 0;
    for( ; p != NULL; p = p->nextparm ) {
        count++;
        emitBYTE( 0x6a );               /* push offset */
        emitBYTE( p->offset );
        emitBYTE( 0xff );
        emitBYTE( 0x77 );
        emitBYTE( 0x10 + p->parmnum * 4 );
    }
    if( count != 0 ) {
        emitBYTE( 0x6a );               /* push count */
        emitBYTE( count );
    }
}

static void cleanupSubParms( struct subparm *p )
{
    int         count;

    count = 4;
    for( ; p != NULL; p = p->nextparm ) {
        count += 8;
    }
    emitBYTE( 0x83 );   /* add esp,nnn */
    emitBYTE( 0xC4 );
    emitBYTE( count );
}

/*************************** NOTE **********************************/
/***  If you change the instructions generated by these thunks  ****/
/***  make sure DLLPATCH.ASM is modified accordingly, since it  ****/
/***  tries to locate the DLL index and DLL name at the end of  ****/
/***  the function.                                             ****/
/*******************************************************************/

static void emitnormalThunk( char *proc, fcn *tmpf, int index )
{
    size_t      size;
    size_t      segsize;
    size_t      subparmsize;

    size = sizeofThunkName( tmpf );
    segsize = size + 29;
    if( tmpf->need_fpusave )
        segsize += 2 * 6;
    if( tmpf->returntype == RETURN_INT )
        segsize += 3;
    subparmsize = sizeofSubParms( tmpf->subparms );
    segsize += subparmsize;
    if( subparmsize != 0 ) {
        subparmsize -= 3;       // adjust for relocation offsets
    }
    emitSEGDEF( (unsigned long)segsize );
    emitPUBDEF( proc );
    emitEXTDEF( "_LocalPtr" );
    if( tmpf->returntype == RETURN_PTR ) {
        if( subparmsize == 0 ) {
            emitEXTDEF( "__WIN16THUNK3ADDR" );
        } else {
            emitEXTDEF( "__WIN16THUNK6ADDR" );
        }
    } else {
        if( subparmsize == 0 ) {
            emitEXTDEF( "__WIN16THUNK1ADDR" );
        } else {
            emitEXTDEF( "__WIN16THUNK5ADDR" );
        }
    }
    if( tmpf->need_fpusave ) {
        emitEXTDEF( "__FSTENV" );
        emitEXTDEF( "__FLDENV" );
    }

    emitBYTE( 0xa0 );           /* LEDATA */
    emitWORD( (int)( segsize + 6 ) ); /* len */
    emitBYTE( 1 );              /* segment index */
    emitDWORD( 0 );             /* enumerated data offset */

    emitBYTE( 0x55 );           /* push ebp */
    emitBYTE( 0x57 );           /* push edi */
    emitBYTE( 0x56 );           /* push esi */
    emitBYTE( 0x8b );           /* mov edi,esp */
    emitBYTE( 0xfc );           /* ... */
    emitSaveFPU( tmpf );
    emitSubParms( tmpf->subparms );
    emitBYTE( 0x8e );           /* mov ds,_LocalPtr */
    emitBYTE( 0x1d );
    emitDWORD( 0 );
    emitBYTE( 0xBB );           /* mov ebx, (index<<16)|class */
    emitWORD( tmpf->class * 2 );
    emitWORD( index );
    emitBYTE( 0x2E );           /* call fword ptr cs:[WINFUNCTION0ADDR] */
    emitBYTE( 0xFF );
    emitBYTE( 0x1D );
    emitDWORD( 0 );
    emitSaveFPU( tmpf );
    if( tmpf->returntype == RETURN_INT ) {
        emitBYTE( 0x0F );       /* movsx eax,ax */
        emitBYTE( 0xBF );
        emitBYTE( 0xC0 );
    }
    if( tmpf->subparms != NULL ) {
        cleanupSubParms( tmpf->subparms );
    }
    emitBYTE( 0x5E );           /* pop esi */
    emitBYTE( 0x5F );           /* pop edi */
    emitBYTE( 0x5D );           /* pop ebp */
    emitBYTE( 0xC2 );           /* ret  nnnn */
    emitWORD( 4 * tmpf->pcnt );
    if( size != 0 ) {
        emitThunkName( tmpf );
    }

    emitBYTE( 0 );

    /* emit fixups for external references */
    emitBYTE( 0x9c );   /* FIXUPP */
    if( tmpf->need_fpusave ) {
        emitWORD( 18 );         /* len */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( 0x07 );       /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 3 );          /* __FSTENV */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( (int)( 0x0d + subparmsize ) ); /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 1 );          /* _LocalPtr */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( (int)( 0x19 + subparmsize ) ); /* offset into LEDATA record */
        emitBYTE( 0x06 );       /* data */
        emitBYTE( 1 );          /* data */
        emitBYTE( 2 );          /* WINFUNCTION0ADDR */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( (int)( 0x1f + subparmsize ) ); /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 4 );          /* __FLDENV */
    } else {
        emitWORD( 10 );         /* len */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( (int)( 0x07 + subparmsize ) ); /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 1 );          /* _LocalPtr */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( (int)( 0x13 + subparmsize ) ); /* offset into LEDATA record */
        emitBYTE( 0x06 );       /* data */
        emitBYTE( 1 );          /* data */
        emitBYTE( 2 );          /* WINFUNCTION0ADDR */
    }
    emitBYTE( 0 );
}

static void emitspecialThunk( char *proc, fcn *tmpf, int index )
{
    size_t      size;
    size_t      segsize;
    int         offset;

    switch( tmpf->pcnt ) {
    case 0:     segsize = 18 + 7;   break;
    case 1:     segsize = 24 + 7;   break;
    case 2:     segsize = 28 + 7;   break;
    case 3:     segsize = 32 + 7;   break;
    case 4:     segsize = 36 + 7;   break;
    case 5:     segsize = 40 + 7;   break;
    default:    segsize = 0;        break;
    }
    size = sizeofThunkName( tmpf );
    segsize += size;
    if( tmpf->need_fpusave )
        segsize += 2 * 6;
    if( tmpf->returntype == RETURN_INT )
        segsize += 3;
    emitSEGDEF( (unsigned long)segsize );
    emitPUBDEF( proc );
    emitEXTDEF( "_LocalPtr" );
    if( tmpf->returntype == RETURN_PTR ) {
        emitEXTDEF( "__WIN16THUNK4ADDR" );
    } else {
        emitEXTDEF( "__WIN16THUNK2ADDR" );
    }
    if( tmpf->need_fpusave ) {
        emitEXTDEF( "__FSTENV" );
        emitEXTDEF( "__FLDENV" );
    }

    emitBYTE( 0xa0 );           /* LEDATA */
    emitWORD( (int)( segsize + 6 ) );    /* len */
    emitBYTE( 1 );              /* segment index */
    emitDWORD( 0 );             /* enumerated data offset */

    emitBYTE( 0x55 );           /* push ebp */
    emitBYTE( 0x57 );           /* push edi */
    emitBYTE( 0x56 );           /* push esi */
    switch( tmpf->pcnt ) {
    case 5:                     /* mov edi,20h[esp] */
        emitBYTE( 0x8B ); emitBYTE( 0x7c ); emitBYTE( 0x24 ); emitBYTE( 0x20 );
    case 4:                     /* mov esi,1Ch[esp] */
        emitBYTE( 0x8B ); emitBYTE( 0x74 ); emitBYTE( 0x24 ); emitBYTE( 0x1C );
    case 3:                     /* mov ecx,18h[esp] */
        emitBYTE( 0x8B ); emitBYTE( 0x4c ); emitBYTE( 0x24 ); emitBYTE( 0x18 );
    case 2:                     /* mov edx,14h[esp] */
        emitBYTE( 0x8B ); emitBYTE( 0x54 ); emitBYTE( 0x24 ); emitBYTE( 0x14 );
    case 1:                     /* mov eax,10h[esp] */
        emitBYTE( 0x8B ); emitBYTE( 0x44 ); emitBYTE( 0x24 ); emitBYTE( 0x10 );
    }
    emitSaveFPU( tmpf );
    emitBYTE( 0x8e );           /* mov ds,_LocalPtr */
    emitBYTE( 0x1d );
    emitDWORD( 0 );
    emitBYTE( 0xBB );           /* mov ebx, (index<<16)|class */
    emitWORD( tmpf->class * 2 );
    emitWORD( index );
    emitBYTE( 0x2E );           /* call fword ptr cs:[WINFUNCTION1ADDR] */
    emitBYTE( 0xFF );
    emitBYTE( 0x1D );
    emitDWORD( 0 );
    emitSaveFPU( tmpf );
    if( tmpf->returntype == RETURN_INT ) {
        emitBYTE( 0x0F );       /* movsx eax,ax */
        emitBYTE( 0xBF );
        emitBYTE( 0xC0 );
    }
    emitBYTE( 0x5E );           /* pop esi */
    emitBYTE( 0x5F );           /* pop edi */
    emitBYTE( 0x5D );           /* pop ebp */
    if( tmpf->pcnt != 0 ) {
        emitBYTE( 0xC2 );       /* ret  nnnn */
        emitWORD( 4 * tmpf->pcnt );
    } else {
        emitBYTE( 0xC3 );       /* ret */
    }
    if( size != 0 ) {
        emitThunkName( tmpf );
    }

    emitBYTE( 0 );

    /* emit fixups for external references */
    emitBYTE( 0x9c );   /* FIXUPP */
    if( tmpf->need_fpusave ) {
        emitWORD( 18 );         /* len */
        emitBYTE( 0xD4 );       /* data */
        offset = 3+2+(4*tmpf->pcnt); /* offset into LEDATA record */
        emitBYTE( offset );     /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 3 );          /* __FSTENV */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( offset+4+2 ); /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 1 );          /* _LocalPtr */
        emitBYTE( 0xD4 );       /* data */
        offset += 4+2+4+5+3;    /* advance to fixup */
        emitBYTE( offset );     /* offset into LEDATA record */
        emitBYTE( 0x06 );       /* data */
        emitBYTE( 1 );          /* data */
        emitBYTE( 2 );          /* WINFUNCTION1ADDR */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( offset+6 );   /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 4 );          /* __FLDENV */
    } else {
        emitWORD( 10 );         /* len */
        emitBYTE( 0xD4 );       /* data */
        offset = 3+2+(4*tmpf->pcnt); /* offset into LEDATA record */
        emitBYTE( offset );     /* offset into LEDATA record */
        emitBYTE( 0x56 );       /* data */
        emitBYTE( 1 );          /* _LocalPtr */
        emitBYTE( 0xD4 );       /* data */
        emitBYTE( offset+4+5+3 ); /* offset into LEDATA record */
        emitBYTE( 0x06 );       /* data */
        emitBYTE( 1 );          /* data */
        emitBYTE( 2 );          /* WINFUNCTION1ADDR */
    }
    emitBYTE( 0 );
}

static void emitMODEND( void )
{
    emitBYTE( 0x8a );
    emitWORD( 2 );
    emitBYTE( 0 ); /* attributes */
    emitBYTE( 0 );
}

static void emitOBJECT( int modindex, char *proc, fcn *tmpf, int index )
{
    emitTHEADR( modindex );
    emitCOMMENT();
    emitLNAMES();
    if( tmpf->aliascnt == 0 && tmpf->pcnt < 6 ) {
        emitspecialThunk( proc, tmpf, index );
    } else {
        emitnormalThunk( proc, tmpf, index );
    }
    emitMODEND();
}

static void startOBJECT( void )
{
    objBufIndex = 0;
}

static void endOBJECT( void )
{
    writeObjBuf();
}


/*
 * GenerateCStubs
 */
static void GenerateCStubs( void )
{
    fcn         *tmpf, *tmpf2;
    short       i=0,index,ii=0;
    char        fn2[128];
    FILE        *fp;
    char        fname[20];

    if( !genstubs ) {
        return;
    }
    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->__special_func ) {
            fn2[0] = '_';
            fn2[1] = '_';
            strcpy( &fn2[2], tmpf->fn );
        } else if( tmpf->fn[0] == '_' && tmpf->fn[1] == '_' ) {
            strcpy( fn2, &tmpf->fn[2] );
        } else {
            strcpy( fn2, tmpf->fn );
        }
        if( !quiet ) {
            printf( "Generating stub %s\n", fn2 );
        }

        if( tmpf->is_16 ) {
            if( tmpf->noregfor_16 ) {
                index = i++;
            } else {
                index = 0;
                for( tmpf2 = Head; tmpf2 != NULL; tmpf2 = tmpf2->next ) {
                    if( strcmp( &fn2[3], tmpf2->fn ) == 0 ) {
                        break;
                    }
                    /*
                     * don't count _16 functions!
                     */
                    if( tmpf2->is_16 ) {
                        if( tmpf2->noregfor_16 ) {
                            index++;
                        }
                    } else {
                        index++;
                    }
                }
            }
        } else {
            index = i++;
        }

        sprintf( fname, "win%d.obj", ii );
        objFile = fopen( fname, "wb" );
        if( objFile == NULL ) {
            fprintf( stderr, "error opening %s\n", fname );
            exit( 1 );
        }
        startOBJECT();
        strupr( fn2 );
        emitOBJECT( ii, fn2, tmpf, 4 * index );
        endOBJECT();
        fclose( objFile );
        ii++;
    }
    if( listfile != NULL ) {
        fp = fopen( listfile, "w" );
        for( i = 0; i < ii; ++i ) {
            fprintf( fp, "win%d.obj\n", i );
        }
        fclose( fp );
    }

} /* GenerateCStubs */


/*
 * OpenHeader - open on header comments
 */
static void OpenHeader( FILE *f )
{
    fprintf( f, LINE );
    fprintf( f, BLANK );

} /* OpenHeader */


/*
 * CloseHeader - close off header comments
 */
static void CloseHeader( FILE *f )
{
    fprintf( f, ";***                                                                      ***\n" );
    fprintf( f, ";*** By:  Craig Eisler                                                    ***\n" );
    fprintf( f, ";***      December 1990-November 1992                                     ***\n" );
    fprintf( f, ";*** By:  F.W.Crigger May 1993                                            ***\n" );
    fprintf( f, BLANK );
    fprintf( f, LINE );

} /* CloseHeader */


static void SegmentsDecl( FILE *f )
{
    fprintf( f, "DGROUP group _DATA\n" );
    fprintf( f, "\n" );
    fprintf( f, "_TEXT segment word public 'CODE' USE16\n" );
    fprintf( f, "_TEXT ends\n" );
    fprintf( f, "\n" );
    fprintf( f, "_DATA segment word public 'DATA' USE16\n" );
    fprintf( f, "_DATA ends\n" );
}


/*
 * DLLThunkHeader - generate header for DLL thunking stuff
 */
static void DLLThunkHeader( FILE *f )
{
    OpenHeader( f );
    fprintf( f, ";*** DLLTHK.ASM - thunking layer to Windows 3.1 DLLs                      ***\n" );
    fprintf( f, ";***              This set of functions makes sure that the proper dll    ***\n" );
    fprintf( f, ";***              is loaded, and gets the real address of the function    ***\n" );
    fprintf( f, ";***              to invoke, which is back-patched into the table.        ***\n" );
    CloseHeader( f );
    fprintf( f, "\n" );
    fprintf( f, "extrn PASCAL LoadLibrary : far\n" );
    fprintf( f, "extrn PASCAL FreeLibrary : far\n" );
    fprintf( f, "extrn PASCAL GetProcAddress : far\n" );
    fprintf( f, "extrn   \"C\", DLLLoadFail : near\n" );
//    fprintf( f, "extrn        _FunctionTable : word\n" );
    fprintf( f, "\n" );
    SegmentsDecl( f );

} /* DLLThunkHeader */


/*
 * GenerateDLLData - generate data for a DLL thunk
 */
static void GenerateDLLData( FILE *f )
{
    int     i;
    char    *thunkstr;

    fprintf( f, "_DATA segment\n" );
    fprintf( f, "\n" );
    fprintf( f, "public DLLHandles\n" );
    fprintf( f, "DLLHandles LABEL WORD\n" );
    for( i = 0; i < ThunkIndex; i++ ) {
        fprintf( f, "\tdw    0\n" );
    }
    fprintf( f, "\n" );
    fprintf( f, "public DLLNames\n" );
    fprintf( f, "DLLNames LABEL WORD\n" );
    for( i = 0; i < ThunkIndex; i++ ) {
        thunkstr = ThunkStrs[i];
        fprintf( f, "\tdw    DGROUP:%s\n", thunkstr );
    }
    fprintf( f, "\n" );
    for( i = 0; i < ThunkIndex; i++ ) {
        thunkstr = ThunkStrs[i];
        fprintf( f, "%s\tdb    '%s.dll',0\n", thunkstr, thunkstr );
    }
    fprintf( f, "\n" );
    fprintf( f, "_DATA ends\n" );

} /* GenerateDLLData */


/*
 * GenerateDLLCode - generate code for a DLL thunk
 */
static void GenerateDLLCode( FILE *f )
{
    int      i;

    fprintf( f, "_TEXT segment\n" );
    fprintf( f, "\tassume cs:_TEXT\n" );
    fprintf( f, "\tassume ds:DGROUP\n" );
    fprintf( f, "\n" );
#if 0
    fprintf( f, "public __BackPatch\n" );
    fprintf( f, "__BackPatch proc near\n" );
    fprintf( f, "\tpop\tsi\n" );
    fprintf( f, "\tmov\tdi,cs:[si]\n" );
    fprintf( f, "\tmov\tax,[di]\n" );
    fprintf( f, "\tor\tax,ax\n" );
    fprintf( f, "\tjne\tloaded\n" );
    fprintf( f, "\t  mov\tax,di\n" );
    fprintf( f, "\t  add\tax,2\n" );
    fprintf( f, "\t  push\tds\n" );
    fprintf( f, "\t  push\tax\n" );
    fprintf( f, "\t  call\tLoadLibrary\n" );
    fprintf( f, "\t  mov\t[di],ax\n" );
    fprintf( f, "loaded:\tadd    si,2\n" );
    fprintf( f, "\tcmp\tax,32\n" );
    fprintf( f, "\tjb\tloadfail\n" );
    fprintf( f, "\tpush\tax\n" );
    fprintf( f, "\tpush\tcs\n" );
    fprintf( f, "\tpush\tsi\n" );
    fprintf( f, "\tcall\tGetProcAddress\n" );
    fprintf( f, "\tmov\t_FunctionTable[bx],ax\n" );
    fprintf( f, "\tmov\t_FunctionTable+2[bx],dx\n" );
//        mov    word ptr [bx + offset DGROUP:_FunctionTable],ax
//        mov    word ptr [bx + offset DGROUP:_FunctionTable+2],dx
    fprintf( f, "\tpush\tdx\n" );
    fprintf( f, "\tpush\tax\n" );
    fprintf( f, "\tretf\n" );
    fprintf( f, "loadfail:lea   ax,2[di]\n" );
    fprintf( f, "\tcall\tDLLLoadFail\n" );
    fprintf( f, "\tretf\n" );
    fprintf( f, "__BackPatch endp\n" );
    fprintf( f, "\n" );
#endif
    fprintf( f, "public __BackPatch_xxx\n" );
    fprintf( f, "__BackPatch_xxx proc near\n" );
    fprintf( f, "\tpush\tdi\n" );
    fprintf( f, "\txor\tdh,dh\n" );
    fprintf( f, "\tmov\tdi,dx\n" );
    fprintf( f, "\tadd\tdi,di\n" );
    fprintf( f, "\tmov\tdx,ax\n" );
    fprintf( f, "\tmov\tax,DLLHandles[di]\n" );
    fprintf( f, "\tor\tax,ax\n" );
    fprintf( f, "\tjne\tload1\n" );
    fprintf( f, "\t  push\tdx\n" );
    fprintf( f, "\t  mov\tax,DLLNames[di]\n" );
    fprintf( f, "\t  push\tds\n" );
    fprintf( f, "\t  push\tax\n" );
    fprintf( f, "\t  call\tLoadLibrary\n" );
    fprintf( f, "\t  mov\tDLLHandles[di],ax\n" );
    fprintf( f, "\t  pop\tdx\n" );
    fprintf( f, "load1:\tcmp\tax,32\n" );
    fprintf( f, "\tjb\tloadf1\n" );
    fprintf( f, "\tpush\tax\n" );
    fprintf( f, "\tpush\tds\n" );
    fprintf( f, "\tpush\tdx\n" );
    fprintf( f, "\tcall\tGetProcAddress\n" );
    fprintf( f, "\tpop\tdi\n" );
    fprintf( f, "\tretf\n" );
    fprintf( f, "loadf1:\tmov\tax,DLLNames[di]\n" );
    fprintf( f, "\tcall\tDLLLoadFail\n" );
    fprintf( f, "\tsub\tax,ax\n" );
    fprintf( f, "\tsub\tdx,dx\n" );
    fprintf( f, "\tpop\tdi\n" );
    fprintf( f, "\tretf\n" );
    fprintf( f, "__BackPatch_xxx endp\n" );
    fprintf( f, "\n" );
    fprintf( f, "public FiniDLLs_\n" );
    fprintf( f, "FiniDLLs_ proc far\n" );
    fprintf( f, "\tpush\tsi\n" );
    fprintf( f, "\tsub\tsi,si\n" );
    fprintf( f, "next:\tmov\tax,DLLHandles[si]\n" );
    fprintf( f, "\tcmp\tax,32\n" );
    fprintf( f, "\tjb\tnextdll\n" );
    fprintf( f, "\t  push\tax\n" );
    fprintf( f, "\t  call\tFreeLibrary\n" );
    fprintf( f, "nextdll:\tadd\tsi,2\n" );
    fprintf( f, "\tcmp\tsi,%d\n", ThunkIndex * 2 );
    fprintf( f, "\tjne\tnext\n" );
    fprintf( f, "\tpop\tsi\n" );
    fprintf( f, "\tret\n" );
    fprintf( f, "FiniDLLs_ endp\n" );
    fprintf( f, "\n" );
    for( i = 0; i < ThunkIndex; i++ ) {
        fprintf( f, "public BackPatch_%s_\n", ThunkStrs[i] );
        fprintf( f, "BackPatch_%s_ proc far\n", ThunkStrs[i] );
        fprintf( f, "\tmov\tdl,%d\n", i );
        fprintf( f, "\tjmp\t__BackPatch_xxx\n" );
        fprintf( f, "BackPatch_%s_ endp\n", ThunkStrs[i] );
        fprintf( f, "\n" );
    }
#if 0
    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->thunk ) {
            GenerateDLLThunk( f, tmpf );
            fprintf( f, "\n" );
        }
    }
#endif
    fprintf( f, "_TEXT ends\n" );

} /* GenerateDLLCode */


/*
 * DLLThunkTrailer
 */
static void DLLThunkTrailer( FILE *f )
{
    fprintf( f, "end\n" );

} /* DLLThunkTrailer */


/*
 * GenerateStartupCode - init. code for each glue function
 */
static void GenerateStartupCode( FILE *f, fcn *tmpf )
{
    fprintf( f, "public  __Thunk%d\n", tmpf->class );
    fprintf( f, "__Thunk%d proc near\n", tmpf->class );

} /* GenerateStartupCode */


/*
 * GenerateStackLocals - generate equates to access local variables (where
 *                       we store the 16-bit aliases for 32-bit ptrs)
 */
static int GenerateStackLocals( FILE *f, fcn *tmpf )
{
    int         i;
    int         offset;

    /* unused parameters */ (void)f;

    offset = 0;
    for( i = tmpf->pcnt; i-- > 0; ) {
        if( tmpf->plist[i] == PARM_PTR ) {
            offset += 8;
//          fprintf( f, "Parm%dAlias = dword ptr [bp-%d]\n", i + 1, offset );
        }
    }
    return( offset );

} /* GenerateStackLocals */


/*
 * Generate16BitAliases - code to generate a 16-bit alias for each
 *                        of the applications 32-bit pointers
 */
static void Generate16BitAliases( FILE *f, fcn *tmpf )
{
    int         i;
    int         offset;
    char        *first;

    first = "First";
    offset = STACK_FRAME;
    for( i = tmpf->pcnt; i-- > 0; ) {
        if( tmpf->plist[i] == PARM_PTR ) {
            fprintf( f, "\tmov\teax,es:[edi+%d]\t\t; Parm%d\n", offset, i + 1 );
            fprintf( f, "\tcall\tGet%s16Alias\n", first );
            first = "";
        }
        offset += 4;
    }

} /* Generate16BitAliases */


static int Parm1Offset( fcn *tmpf )
{
    return( tmpf->pcnt * 4 + 12 );
}

/*
 * GenerateAPICall - code to invoke the actual api call: pushes
 *                   variables onto the extenders stack, and does
 *                   the call.
 */
static void GenerateAPICall( FILE *f, fcn *tmpf )
{
    int         i, j;
    int         offset;
    int         alias_offset;
    static char *RegParms[] = { "eax", "edx", "ecx", "esi", "edi" };
    const char  *r;

    GenerateStartupCode( f, tmpf );
//    GenerateStackAccessEquates( f, tmpf );
    alias_offset = GenerateStackLocals( f, tmpf );
    Generate16BitAliases( f, tmpf );

    if( tmpf->aliascnt == 0 && tmpf->pcnt < 6 ) {
        j = tmpf->pcnt;
        for( i = 0; i < tmpf->pcnt; i++ ) {
            j--;
            r = RegParms[j];
            if( tmpf->plist[i] == PARM_WORD )
                r++;
            fprintf( f, "\tpush\t%s\n", r );
        }
    } else {
        offset = Parm1Offset( tmpf );
        for( i = 0; i < tmpf->pcnt; i++ ) {
            if( tmpf->plist[i] == PARM_PTR ) {
                fprintf( f, "\tpush\tdword ptr [bp-%d]\t; Parm%dAlias\n", alias_offset, i + 1 );
                alias_offset -= 8;
            } else if( tmpf->plist[i] == PARM_WORD ) {
                fprintf( f, "\tpush\tword ptr es:[edi+%d]\t; Parm%d\n", offset, i + 1 );
            } else {
                fprintf( f, "\tpush\tdword ptr es:[edi+%d]\t; Parm%d\n", offset, i + 1 );
            }
            offset -= 4;
        }
    }
    if( tmpf->aliascnt != 0 ) {
        fprintf( f, "\tjmp\tFree16Alias\n" );
    } else {
        fprintf( f, "\tcall\tdword ptr ds:[bx]\n" );
        if( tmpf->pcnt >= 6 ) {
            fprintf( f, "\tpush\tdx\n" );
            fprintf( f, "\tpush\tax\n" );
            fprintf( f, "\tpop\teax\n" );
        }
        fprintf( f, "\tret\n" );
    }
    fprintf( f, "__Thunk%d endp\n", tmpf->class );

} /* GenerateAPICall */


#if 0
/*
 * GenerateStackAccessEquates - generate equates to access the variables
 *                              passed in by the 32 bit application
 */
void GenerateStackAccessEquates( FILE *f, fcn *tmpf )
{
    int         i;
    int         offset;
    const char  *p;

    offset = STACK_FRAME;
    for( i = tmpf->pcnt; i-- > 0; ) {
        p = "dword";
        if( tmpf->plist[i] == PARM_WORD )
            p++;
        fprintf( f, "Parm%d   = %s ptr es:[edi+%d]\n", i + 1, p, offset );
        offset += 4;
    }

} /* GenerateStackAccessEquates */
#endif

/*
 * FunctionHeader - build glue functions header area
 */
static void FunctionHeader( FILE *f )
{
    fcn         *tmpf;
    const char  *fn_name;

    OpenHeader( f );
    fprintf( f, ";*** WINGLUE.ASM - windows glue functions                                 ***\n" );
    fprintf( f, ";***               This set of functions encompasses all possible types   ***\n" );
    fprintf( f, ";***               of calls.  Each API call has a little                  ***\n" );
    fprintf( f, ";***               stub which generates the appropriate call into these   ***\n" );
    fprintf( f, ";***               functions.                                             ***\n" );
    CloseHeader( f );
    fprintf( f, "\n" );
    fprintf( f, ".386p\n" );
    fprintf( f, "\n" );
    fprintf( f, "extrn        __DLLPatch:far\n" );
    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->is_16 && tmpf->noregfor_16 ) {
            fn_name = &tmpf->fn[3];
        } else {
            fn_name = tmpf->fn;
        }
        if( tmpf->thunk ) {
            if( !tmpf->is_16 || tmpf->noregfor_16 ) {
                fprintf( f, ";extrn PASCAL %s:FAR ; t=%d (%s)\n", fn_name, tmpf->class, ThunkStrs[tmpf->thunkindex] );
            } else {
                fprintf( f, ";      PASCAL %s ; t=%d (%s)\n", fn_name, tmpf->class, ThunkStrs[tmpf->thunkindex] );
            }
        } else {
            if( !tmpf->is_16 || tmpf->noregfor_16 ) {
                fprintf( f, "extrn PASCAL %s:FAR ; t=%d\n", fn_name, tmpf->class );
            } else {
                fprintf( f, ";      PASCAL %s ; t=%d\n", fn_name, tmpf->class );
            }
        }
    }
    fprintf( f, "extrn        GetFirst16Alias:near\n" );
    fprintf( f, "extrn        Get16Alias:near\n" );
    fprintf( f, "extrn        Free16Alias:near\n" );
    fprintf( f, "\n" );
    SegmentsDecl( f );

} /* FunctionHeader */


static void FunctionData( FILE *f )
{
    fcn         *tmpf;
    const char  *fn_name;

    fprintf( f, "_DATA segment\n" );
    fprintf( f, "\n" );
    fprintf( f, "public \"C\", FunctionTable\n" );
    fprintf( f, "FunctionTable LABEL DWORD\n" );
    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->is_16 && tmpf->noregfor_16 ) {
            fn_name = &tmpf->fn[3];
        } else {
            fn_name = tmpf->fn;
        }
        if( tmpf->thunk ) {
            if( !tmpf->is_16 || tmpf->noregfor_16 ) {
                fprintf( f, "\tdd\t__DLLPatch ; (%s) %s\n", ThunkStrs[tmpf->thunkindex], fn_name );
            }
        } else {
            if( !tmpf->is_16 || tmpf->noregfor_16 ) {
                fprintf( f, "\tdd\t%s\n", fn_name );
            }
        }
    }
    fprintf( f, "\n" );
    fprintf( f, "_DATA ends\n" );

}

static void FunctionCode( FILE *f )
{
    fcn         *tmpf;

    fprintf( f, "_TEXT segment\n" );
    fprintf( f, "\tassume cs:_TEXT\n" );
    fprintf( f, "\tassume ds:dgroup\n" );
    fprintf( f, "\n" );
    fprintf( f, "public __ThunkTable\n" );
    fprintf( f, "__ThunkTable LABEL WORD\n" );
    for( tmpf = Class; tmpf != NULL; tmpf = tmpf->next_class ) {
        fprintf( f, "\tdw\t__Thunk%d\n", tmpf->class );
    }
    fprintf( f, "\n" );
    for( tmpf = Class; tmpf != NULL; tmpf = tmpf->next_class ) {
        if( !quiet ) {
            printf( "Code for class %d\n", tmpf->class );
        }
        GenerateAPICall( f, tmpf );
        fprintf( f, "\n" );
    }
    fprintf( f, "_TEXT ends\n" );

}

/*
 * FunctionTrailer - very last crap at end of glue functions
 */
static void FunctionTrailer( FILE *f )
{
    fprintf( f, "end\n" );

} /* FunctionTrailer */


/*
 * GenerateCode - generate code for all glue functions
 */
static void GenerateCode( void )
{
    fcn     *tmpf;
    fcn     *tmpf2;
    FILE    *f;

    /*
     * find out which _16 functions have a regular type
     */
    for( tmpf = Head; tmpf != NULL; tmpf = tmpf->next ) {
        if( tmpf->is_16 ) {
            for( tmpf2 = Head; tmpf2 != NULL; tmpf2 = tmpf2->next ) {
                if( strcmp( &tmpf->fn[3], tmpf2->fn ) == 0 ) {
                    break;
                }
            }
            if( tmpf2 == NULL ) {
                tmpf->noregfor_16 = 1;
            }
        }
    }

    f = fopen( "winglue.asm", "w" );
    if( f == NULL ) {
        fprintf( stderr, "Error opening glue file\n" );
        exit( 1 );
    }
    FunctionHeader( f );
    fprintf( f, "\n" );
    fprintf( f, "\n" );
    FunctionData( f );
    fprintf( f, "\n" );
    fprintf( f, "\n" );
    FunctionCode( f );
    fprintf( f, "\n" );
    FunctionTrailer( f );
    fclose( f );

    f = fopen( "dllthk.asm", "w" );
    if( f == NULL ) {
        fprintf( stderr, "Error opening glue file\n" );
        exit( 1 );
    }
    DLLThunkHeader( f );
    fprintf( f, "\n" );
    fprintf( f, "\n" );
    GenerateDLLData( f );
    fprintf( f, "\n" );
    fprintf( f, "\n" );
    GenerateDLLCode( f );
    fprintf( f, "\n" );
    DLLThunkTrailer( f );
    fclose( f );

} /* GenerateCode */

#if 0
/*
 * AddCommentTrailer -  add trailing '***' to a comment line
 */
static void AddCommentTrailer( FILE *f, char *tmp )
{
    size_t      i, j;
    char        tmp2[81];

    tmp2[0] = '\0';
    i = strlen( tmp );
    if( i < 75 )  {
        for( j = i; j <= 75; j++ ) {
            tmp2[j - i] = ' ';
        }
        tmp2[j - i] = '\0';
    }
    strcat( tmp, tmp2 );
    fprintf( f, "%s***\n", tmp );

} /* AddCommentTrailer */
#endif

static void normalize( char *dir )
{
    for( ; *dir != '\0'; dir++ ) {
#ifdef __UNIX__
        if( *dir == '\\' ) {
            *dir = '/';
        }
#else
        if( *dir == '/' ) {
            *dir = '\\';
        }
#endif
    }
}

int main( int argc, char *argv[] )
{
    char        *name;
    char        *dir;
    FILE        *f;
    FILE        *pf;
    char        defname[80];
    char        buffer[50];
    char        *fname;
    size_t      i;
    int         j;
    int         k;

    for( j = argc; --j > 0; ) {
        if( argv[j][0] == '-' ) {
            for( i = 1; i < strlen( argv[j] ); i++ ) {
                switch( argv[j][i] ) {
                case 'l':
                    if( argv[j][i + 1] == '=' ) {
                        listfile = argv[j] + i + 2;
                        normalize( listfile );
                        i += strlen( argv[j] + i );
                    }
                    break;
                case 'q':
                    quiet = 1;
                    break;
                case 's':
                    genstubs = 1;
                    break;
                case '?':
                    printf( "conv -s (gen stubs)\n" );
                    exit( 1 );
                default:
                    fprintf( stderr, "Unrecognized switch %c", argv[j][i] );
                    break;
                }
            }
            for( k = j; k < argc; k++ ) {
                argv[k] = argv[k + 1];
            }
            argc--;
        }
    }

    if( argc < 2 ) {
        name = "win386";
    } else {
        name = argv[1];
    }
    if( argc < 3 ) {
        dir = "def";
    } else {
        dir = argv[2];
        normalize( dir );
    }

    pf = fopen( name, "r" );
    if( pf == NULL ) {
        printf( "error opening file %s\n", name );
        exit( 1 );
    }

    while( (fname = fgets( buffer, sizeof( buffer ), pf )) != NULL ) {
        SKIP_LEAD_WS( fname );
        if( fname[0] == '#' || fname[0] == '\0' ) {
            continue;
        }
        STRIP_TRAIL_WS( i, fname );
#ifdef __UNIX__
        sprintf( defname, "%s/%s", dir, fname );
#else
        sprintf( defname, "%s\\%s", dir, fname );
#endif
        f = fopen( defname, "r" );
        if( f == NULL ) {
            printf( "error opening file %s\n", defname );
            exit( 1 );
        }
        ProcessDefFile( f );
        fclose( f );
    }
    fclose( pf );
    BuildClasses();
    GenerateCode();
    GenerateThunkC();
    GenerateCStubs();
    ClosingComments();
    return( 0 );

} /* main */
