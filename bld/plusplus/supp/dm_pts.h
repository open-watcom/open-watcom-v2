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


#error obsolete
#define DM_DEFS \
    DM_DEF( BASIC_TYPE ) \
    DM_DEF( CHAR_ENCODING ) \
    DM_DEF( CHAR_ENCODING_SIGNED ) \
    DM_DEF( SET_INDEX ) \
    DM_DEF( POINTER ) \
    DM_DEF( ZAP_SPACE ) \
    DM_DEF( EMIT_SPACE ) \
    DM_DEF( RESET_INDEX ) \
    DM_DEF( ARRAY_PREFIX ) \
    DM_DEF( ARRAY_SIZE ) \
    DM_DEF( ARRAY_SUFFIX ) \
    DM_DEF( ARRAY ) \
    DM_DEF( OPEN_PAREN ) \
    DM_DEF( CLOSE_PAREN ) \
    DM_DEF( FUNCTION ) \
    DM_DEF( FUNCTION_PREFIX ) \
    DM_DEF( FUNCTION_SUFFIX ) \
    DM_DEF( FUNCTION_ARG_SEPARATOR ) \
    DM_DEF( THIS_FUNCTION ) \
    DM_DEF( UNMODIFIED_TYPE ) \
    DM_DEF( BASED_ENCODING ) \
    DM_DEF( BASED_SUFFIX ) \
    DM_DEF( BASED_SELF ) \
    DM_DEF( BASED_VOID ) \
    DM_DEF( BASED_STRING_PREFIX ) \
    DM_DEF( IDENTIFIER ) \
    DM_DEF( COPY_STRING ) \
    DM_DEF( BASED_STRING_SUFFIX ) \
    DM_DEF( MODIFIER_LIST ) \
    DM_DEF( TYPE_ENCODING ) \
    DM_DEF( TEMPLATE_ARG ) \
    DM_DEF( INTEGER ) \
    DM_DEF( RECURSE_BEGIN ) \
    DM_DEF( RECURSE_END ) \
    DM_DEF( TEMPLATE_NAME ) \
    DM_DEF( TEMPLATE_PREFIX ) \
    DM_DEF( TEMPLATE_SUFFIX ) \
    DM_DEF( TEMPLATE_ARG_SEPARATOR ) \
    DM_DEF( WATCOM_OBJECT ) \
    DM_DEF( ANONYMOUS_ENUM ) \
    DM_DEF( OPERATOR_PREFIX ) \
    DM_DEF( OPERATOR_FUNCTION ) \
    DM_DEF( RELATIONAL_FUNCTION ) \
    DM_DEF( ASSIGNMENT_FUNCTION ) \
    DM_DEF( OPERATOR_NEW ) \
    DM_DEF( OPERATOR_DELETE ) \
    DM_DEF( DESTRUCTOR_CHAR ) \
    DM_DEF( DESTRUCTOR ) \
    DM_DEF( CONSTRUCTOR ) \
    DM_DEF( OPERATOR_CONVERT ) \
    DM_DEF( CTOR_DTOR_NAME ) \
    DM_DEF( SCOPED_NAME ) \
    DM_DEF( NAME ) \
    DM_DEF( SCOPE ) \
    DM_DEF( SCOPE_SEPARATOR ) \
    DM_DEF( MANGLED_NAME ) \
    DM_DEF( TRUNCATED_NAME ) \
    DM_DEF( OPERATOR_DELETE_ARRAY ) \
    DM_DEF( OPERATOR_NEW_ARRAY ) \
    DM_DEF( DECLSPEC_IMPORT ) \
    DM_DEF( INVALID )

typedef enum {
    #define DM_DEF( id )        DM_##id,
    DM_DEFS
    #undef DM_DEF
} dm_pts;
