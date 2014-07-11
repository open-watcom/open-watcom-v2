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


/* stuff broken out of asmins.c
 * used only by WASM not the inline assembler
 */

#include "asmglob.h"

#include "directiv.h"
#include "condasm.h"
#include "asmexpnd.h"
#include "asmlabel.h"
#include "asmstruc.h"

/* prototypes */
extern bool             OrgDirective( token_idx );
extern bool             AlignDirective( asm_token, token_idx );
extern bool             ForDirective( token_idx, enum irp_type );

bool directive( token_idx i, asm_token direct )
/*********************************************/
/* Handle all directives */
{
    bool        ret;

    /* no expansion on the following */
    switch( direct ) {
    case T_MASM:
        Options.mode &= ~MODE_IDEAL;
        Options.locals_len = 0;
        return( RC_OK );
    case T_IDEAL:
        Options.mode |= MODE_IDEAL;
        Options.locals_len = 2;
        return( RC_OK );
    case T_DOT_286C:
        direct = T_DOT_286;
    case T_DOT_8086:
    case T_DOT_186:
    case T_DOT_286:
    case T_DOT_286P:
    case T_DOT_386:
    case T_DOT_386P:
    case T_DOT_486:
    case T_DOT_486P:
    case T_DOT_586:
    case T_DOT_586P:
    case T_DOT_686:
    case T_DOT_686P:
    case T_DOT_8087:
    case T_DOT_287:
    case T_DOT_387:
    case T_DOT_NO87:
    case T_DOT_K3D:
    case T_DOT_MMX:
    case T_DOT_XMM:
    case T_DOT_XMM2:
    case T_DOT_XMM3:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        } else {
            ret = cpu_directive( direct );
            if( Parse_Pass == PASS_1 )
                return( ret );
            return( RC_OK );
        }
    case T_P286N:
        direct = T_P286;
    case T_P8086:
    case T_P186:
    case T_P286:
    case T_P286P:
    case T_P386:
    case T_P386P:
    case T_P486:
    case T_P486P:
    case T_P586:
    case T_P586P:
    case T_P686:
    case T_P686P:
    case T_P8087:
    case T_P287:
    case T_P387:
    case T_PK3D:
    case T_PMMX:
    case T_PXMM:
    case T_PXMM2:
    case T_PXMM3:
        ret = cpu_directive( direct );
        if( Parse_Pass == PASS_1 )
            return( ret );
        return( RC_OK );
    case T_DOT_DOSSEG:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_DOSSEG:
        Globals.dosseg = TRUE;
        return( RC_OK );
    case T_PUBLIC:
        /* special case - expanded inside iff it is an EQU to a symbol */
        if( Parse_Pass == PASS_1 )
            return( PubDef( i + 1 ) );
        return( RC_OK );
    case T_ELSE:
    case T_ELSEIF:
    case T_ELSEIF1:
    case T_ELSEIF2:
    case T_ELSEIFB:
    case T_ELSEIFDEF:
    case T_ELSEIFE:
    case T_ELSEIFNB:
    case T_ELSEIFNDEF:
    case T_ELSEIFDIF:
    case T_ELSEIFDIFI:
    case T_ELSEIFIDN:
    case T_ELSEIFIDNI:
    case T_ENDIF:
    case T_IF:
    case T_IF1:
    case T_IF2:
    case T_IFB:
    case T_IFDEF:
    case T_IFE:
    case T_IFNB:
    case T_IFNDEF:
    case T_IFDIF:
    case T_IFDIFI:
    case T_IFIDN:
    case T_IFIDNI:
        return( conditional_assembly_directive( i ) );
    case T_DOT_ERR:
    case T_DOT_ERRB:
    case T_DOT_ERRDEF:
    case T_DOT_ERRDIF:
    case T_DOT_ERRDIFI:
    case T_DOT_ERRE:
    case T_DOT_ERRIDN:
    case T_DOT_ERRIDNI:
    case T_DOT_ERRNB:
    case T_DOT_ERRNDEF:
    case T_DOT_ERRNZ:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_ERR:
    case T_ERRIFB:
    case T_ERRIFDEF:
    case T_ERRIFDIF:
    case T_ERRIFDIFI:
    case T_ERRIFE:
    case T_ERRIFIDN:
    case T_ERRIFIDNI:
    case T_ERRIFNDEF:
        return( conditional_error_directive( i ) );
    case T_ENDS:
        if( Definition.struct_depth != 0 )
            return( StructDef( i ) );
        // else fall through to T_SEGMENT
    case T_SEGMENT:
        if( Parse_Pass == PASS_1 )
            return( SegDef(i) );
        return( SetCurrSeg(i) );
    case T_GROUP:
        if( Parse_Pass == PASS_1 )
            return( GrpDef(i) );
        return( RC_OK );
    case T_PROC:
        return( ProcDef( i, TRUE ) );
    case T_ENDP:
        return( ProcEnd(i) );
    case T_ENUM:
        return( EnumDef( i ) );
    case T_DOT_CODE:
    case T_DOT_STACK:
    case T_DOT_DATA:
    case T_DOT_DATA_UN:
    case T_DOT_FARDATA:
    case T_DOT_FARDATA_UN:
    case T_DOT_CONST:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_CODESEG:
    case T_STACK:
    case T_DATASEG:
    case T_UDATASEG:
    case T_FARDATA:
    case T_UFARDATA:
    case T_CONST:
        return( SimSeg(i) );
    case T_WARN:
    case T_NOWARN:
        return( RC_OK ); /* Not implemented yet */
    case T_DOT_ALPHA:
    case T_DOT_SEQ:
    case T_DOT_LIST:
    case T_DOT_LISTALL:
    case T_DOT_LISTIF:
    case T_DOT_LISTMACRO:
    case T_DOT_LISTMACROALL:
    case T_DOT_NOLIST:
    case T_DOT_XLIST:
    case T_DOT_TFCOND:
    case T_DOT_SFCOND:
    case T_DOT_LFCOND:
    case T_DOT_CREF:
    case T_DOT_XCREF:
    case T_DOT_NOCREF:
    case T_DOT_SALL:
    case T_PAGE:
    case T_TITLE:
    case T_SUBTITLE:
    case T_SUBTTL:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
        AsmWarn( 4, IGNORING_DIRECTIVE );
        return( RC_OK );
    case T_DOT_BREAK:
    case T_DOT_CONTINUE:
    case T_DOT_ELSE:
    case T_DOT_ENDIF:
    case T_DOT_ENDW:
    case T_DOT_IF:
    case T_DOT_RADIX:
    case T_DOT_REPEAT:
    case T_DOT_UNTIL:
    case T_DOT_WHILE:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_ECHO:
    case T_HIGH:
    case T_HIGHWORD:
    case T_LOW:
    case T_LOWWORD:
    case T_ADDR:
    case T_BOUND:
    case T_CASEMAP:
    case T_INVOKE:
    case T_LROFFSET:
    case T_OPATTR:
    case T_OPTION:
    case T_POPCONTEXT:
    case T_PUSHCONTEXT:
    case T_PROTO:
    case T_THIS:
    case T_WIDTH:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_CATSTR:
    case T_MASK:
    case T_PURGE:
    case T_RECORD:
    case T_TYPEDEF:
    case T_UNION:
        AsmError( NOT_SUPPORTED );
        return( RC_ERROR );
    case T_ORG:
        ExpandTheWorld( 0, FALSE, TRUE );
        break;
    case T_TEXTEQU:     /* TEXTEQU */
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_EQU2:        /* = */
    case T_EQU:         /* EQU */
        /* expand any constants and simplify any expressions */
        ExpandTheConstant( 0, FALSE, TRUE );
        break;
    case T_NAME:        /* no expand parameters */
        break;
    case T_DOT_STARTUP:
    case T_DOT_EXIT:
        if( Options.mode & MODE_IDEAL ) {
            AsmError( UNKNOWN_DIRECTIVE );
            return( RC_ERROR );
        }
    case T_STARTUPCODE:
    case T_EXITCODE:
    default:
        /* expand any constants in all other directives */
        ExpandAllConsts( 0, FALSE );
        break;
    }

    switch( direct ) {
    case T_ALIAS:
        if( Parse_Pass == PASS_1 )
            return( AddAlias( i ) );
        return( RC_OK );
    case T_EXTERN:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_EXTRN:
        if( Parse_Pass == PASS_1 )
            return( ExtDef( i + 1, FALSE ) );
        return( RC_OK );
    case T_COMM:
        if( Parse_Pass == PASS_1 )
            return( CommDef( i + 1 ) );
        return( RC_OK );
    case T_EXTERNDEF:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_GLOBAL:
        if( Parse_Pass == PASS_1 )
            return( ExtDef( i + 1, TRUE ) );
        return( RC_OK );
    case T_DOT_MODEL:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_MODEL:
        return( Model(i) );
    case T_INCLUDE:
        return( Include( i + 1 ) );
    case T_INCLUDELIB:
        if( Parse_Pass == PASS_1 )
            return( IncludeLib( i + 1 ) );
        return( RC_OK );
    case T_ASSUME:
        return( SetAssume(i) );
    case T_END:
        return( ModuleEnd(Token_Count) );
    case T_EQU:
        return( DefineConstant( i-1, FALSE, FALSE ) );
    case T_EQU2:
        return( DefineConstant( i-1, TRUE, FALSE ) );
    case T_TEXTEQU:
        return( DefineConstant( i-1, TRUE, TRUE ) );
    case T_MACRO:
        return( MacroDef(i, FALSE ) );
    case T_ENDM:
        return( MacroEnd( FALSE ) );
    case T_EXITM:
        return( MacroEnd( TRUE ) );
    case T_ARG:
        if( Parse_Pass == PASS_1 )
            return( ArgDef(i) );
        return( RC_OK );
    case T_USES:
        if( Parse_Pass == PASS_1 )
            return( UsesDef(i) );
        return( RC_OK );
    case T_LOCAL:
        if( Parse_Pass == PASS_1 )
            return( LocalDef(i) );
        return( RC_OK );
    case T_COMMENT:
        if( Options.mode & MODE_IDEAL )
            break;
        return( Comment( START_COMMENT, i, NULL ) );
    case T_STRUCT:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_STRUC:
        return( StructDef( i ) );
    case T_NAME:
        if( Parse_Pass == PASS_1 )
            return( NameDirective(i) );
        return( RC_OK );
    case T_LABEL:
        return( LabelDirective( i ) );
    case T_ORG:
        return( OrgDirective( i ) );
    case T_ALIGN:
    case T_EVEN:
        return( AlignDirective( direct, i ) );
    case T_FOR:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_IRP:
        return( ForDirective( i + 1, IRP_WORD ) );
    case T_FORC:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_IRPC:
        return( ForDirective( i + 1, IRP_CHAR ) );
    case T_REPEAT:
        if( Options.mode & MODE_IDEAL ) {
            break;
        }
    case T_REPT:
        return( ForDirective( i + 1, IRP_REPEAT ) );
    case T_DOT_STARTUP:
    case T_DOT_EXIT:
    case T_STARTUPCODE:
    case T_EXITCODE:
        return( Startup ( i ) );
    case T_LOCALS:
    case T_NOLOCALS:
        return( Locals( i ) );
    }
    AsmError( UNKNOWN_DIRECTIVE );
    return( RC_ERROR );
}
