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


#include "skel.h"


/*
    Stuff dealing with symbol handles.
*/


walk_result     DIPENTRY DIPImpWalkSymList( imp_image_handle *ii,
                symbol_source ss, void *source, IMP_SYM_WKR *wk,
                imp_sym_handle *is, void *d )
{
    //TODO:
    /*
        Walk the list of symbols. There can be a number of sources
        of symbol lists. What kind you're walking is determined by the
        'ss' parameter. It can take on the following values:

        SS_MODULE:
                The 'source' is a pointer to an imp_mod_handle. If
                the *(imp_mod_handle *)source is NO_MOD, The list is
                all the module scope/global symbols in the image, otherwise
                it is the list of module scope/global symbols in the
                indicated module.

        SS_SCOPED:
                The 'source' is a pointer at an address. Considering the
                point of execution to be *(address *)source, the list
                is all the lexically scoped symbols visible from that
                point.

        SS_TYPE:
                The 'source' is a pointer to an imp_type_handle. If
                *(imp_type_handle *)source represents an enumerated
                type, the list is all the constant symbols of the
                enumeration. If the type handle represents a structure
                type, the list is all the field names in the structure.

                When walking structures with a inherited classes, there is
                a small trick. Just before starting to walk the fields
                of an inherited class, the DIP should do a:

                        wk( ii, SWI_INHERIT_START, NULL, d )

                This indicates to client that an inherited field list
                is about to be started. If the client wishes the DIP to
                actually walk the inherited list, it will return WR_CONTINUE.
                If it wants you to skip the inherited fields it will return
                WR_STOP. You should continue with the reminder of the fields
                in the current structure that you're walking.
                When you come to the end of the list of members for an
                inherited class do a:

                        wk( ii, SWI_INHERIT_END, NULL, d )

        SS_BLOCK:
                The 'source' is a pointer to a scope_block structure.
                Walk all the symbols in that lexical block _only_.

        SS_SCOPESYM:
                The 'source' is a pointer to an imp_sym_handle.
                This is a request to walk all the symbols in the
                scope identified by the imp_sym_handle. For example, if
                the imp_sym_handle is of type SK_NAMESPACE, walk all the
                symbols contained in that namespace.

        PSEUDO-CODE:

        for( each symbol in the list ) {
            if( starting new inherited base class ) {
                if( wk( ii, SWI_INHERIT_START, NULL, d ) != WR_CONTINUE ) {
                    skip it and continue with next field in current class
                }
            } else if( ending list of inherited base class ) {
                 wk( ii, SWI_INHERIT_END, NULL, d );
            } else {
                *is = fill in symbol handle information;
                wr = wk( ii, SWI_SYMBOL, is, d );
                if( wr != WR_CONTINUE ) return( wr );
            }
        }
    */
    return( WR_CONTINUE );
}


walk_result DIPENTRY DIPImpWalkSymListEx( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is,
                location_context *lc, void *d )
{
    /*
    Just like DIPImpWalkSymList but it gets an "lc", just in case
    we're debugging some wierd-ass language like javascript where
    the fields of a structure can change dynamically
    */
    lc=lc;
    return( WR_CONTINUE );
}

imp_mod_handle  DIPENTRY DIPImpSymMod( imp_image_handle *ii,
                        imp_sym_handle *is )
{
    //TODO:
    /*
        Return the module that the implementation symbol handle comes from.
    */
    return( NO_MOD );
}

unsigned        DIPENTRY DIPImpSymName( imp_image_handle *ii,
                        imp_sym_handle *is, location_context *lc,
                        symbol_name sn, char *buff, unsigned max )
{
    //TODO:
    /*
        Given the imp_sym_handle, copy the name of the symbol into 'buff'.
        Do not copy more than 'max' - 1 characters into the buffer and
        append a trailing '\0' character. Return the real length
        of the symbol name (not including the trailing '\0' character) even
        if you had to truncate it to fit it into the buffer. If something
        went wrong and you can't get the symbol name, call DCStatus and
        return zero. NOTE: the client might pass in zero for 'max'. In that
        case, just return the length of the symbol name and do not attempt
        to put anything into the buffer.
        The 'sn' parameter indicates what type of symbol name the client
        is interested in. It can have the following values:

        SN_SOURCE:
                The name of the symbol as it appears in the source code.

        SN_OBJECT:
                The name of the symbol as it appeared to the linker.

        SN_DEMANGLED:
                C++ names, with full typing (essentially it looks like
                a function prototype). If the symbol is not a C++ symbol
                (not mangled), return zero for the length.

        SN_EXPRESSION:
                Return whatever character string is necessary such that
                when scanned in an expression, the symbol handle can
                be reconstructed. Deprecated - never used.
    */
    return( 0 );
}

dip_status      DIPENTRY DIPImpSymType( imp_image_handle *ii,
                imp_sym_handle *is, imp_type_handle *it )
{
    //TODO:
    /*
        Get the implementation type handle for the type of the
        given symbol.
    */
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymLocation( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, location_list *ll )
{
    /*
        Get the location of the given symbol.
    */
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymValue( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, void *buff )
{
    //TODO:
    /*
        Copy the value of a constant symbol into 'buff'. You can get the
        size required by doing a SymType followed by a TypeInfo.
    */
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymInfo( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, sym_info *si )
{
    //TODO:
    /*
        Get some generic information about a symbol.
    */
    memset( si, 0, sizeof( *si ) );
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymParmLocation( imp_image_handle *ii,
                    imp_sym_handle *is, location_context *lc,
                    location_list *ll, unsigned n )
{
    //TODO:
    /*
        Get information about where a routine's parameters/return value
        are located.
        If the 'n' parameter is zero, fill in the location list structure
        pointed at by 'll' with the information on the location of the
        function's return value. Otherwise fill it in with the location
        of the n'th parameter.
    */
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymObjType( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
{
    //TODO:
    /*
        Fill in the imp_type_handle with the type of the 'this' object
        for a C++ member function.
        If 'ti' is not NULL, fill in the dip_type_info with the kind of 'this'
        pointer that the routine is expecting (near/far, 16/32). If the
        routine is a static member, set ti->kind to TK_NONE.
    */
    return( DS_FAIL );
}

dip_status      DIPENTRY DIPImpSymObjLocation( imp_image_handle *ii,
                                imp_sym_handle *is, location_context *lc,
                                 location_list *ll )
{
    //TODO:
    /*
        Fill in the location list with the location of the '*this' object
        for a C++ member function. Return DS_FAIL if it's a static member
        function.
     */
     return( DS_FAIL );
}

search_result   DIPENTRY DIPImpAddrSym( imp_image_handle *ii,
                            imp_mod_handle im, address a, imp_sym_handle *is )
{
    //TODO:
    /*
        Search the given module for a symbol who's address is less than
        or equal to 'addr'. If none is found return SR_NONE. If you find
        a symbol at that address exactly, fill in '*is' and return SR_EXACT.
        Otherwise, fill in '*is' and return SR_CLOSEST.
    */
    return( SR_NONE );
}


search_result   DIPENTRY DIPImpLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    //TODO:
    /*
        Lookup a symbol by name.

        The 'ss' and 'source' indicate where to search for the name. 'ss'
        can take on the following values:

        SS_MODULE:
                The 'source' is a pointer to an imp_mod_handle.
                Search all the file scope symbols in that module.
                If *(imp_mod_handle *)source is NO_MOD, Search all file
                scope symbols in the image.

        SS_SCOPED:
                The 'source' is a pointer at an address. Considering the
                point of execution to be *(address *)source, search all
                the lexically scoped symbols visible from that point.

        SS_TYPE:
                The 'source' is a pointer to an imp_type_handle. If
                *(imp_type_handle *)source represents an enumerated
                type, search all the constant symbols of the
                enumeration. If the type handle represents a structure
                type, search all the field names in the structure (including
                any inherited fields).

        SS_BLOCK:
                Will never happen.

        SS_SCOPESYM:
                The 'source' is a pointer to an imp_sym_handle. Find symbols
                only in the scope identified by the 'source' pointer.

        The 'li' identifies the symbol name being looked up. It contains the
        following fields:

        mod_handle              mod;
                Inside the implmentation, this field is actually an
                'imp_mod_handle'. If 'mod' is not NO_MOD, then the symbol
                must be in the indicated module. If it is NO_MOD, the
                symbol can be from any module in the image.

        lookup_token    name;
                The name of the symbol being looked up. A lookup_token
                contains a pointer to the start of the name and a length.

        lookup_token    scope;
                The C++ scope of the name. E.g., in the symbol "foo::bar",
                this field would indicate "foo". Yeah, I know that you
                can have more than one scope name, but I don't want to think
                about it until someone complains. If the start pointer in
                the lookup_token is NULL, then there is no scope name.
                Deprecated - new clients will use SS_SCOPESYM instead.
                Eventually support for this will be removed.

        lookup_token    source;
                This is the source name of the symbol being looked up. I.e.
                what the user typed in. It differs from the name field in
                that the client might try munging the symbol name to deal
                with the compiler adding characters to the front or end of
                the symbol name before writing it to the object file. E.g.
                The 'source' might be "foo", while the 'name' field is
                "foo_".

        unsigned                file_scope      : 1;
                If true, the symbol must be defined at file scope. E.g., in
                C++ terms, the user typed "::bar".

        unsigned                case_sensitive  : 1;
                If true, string comparisons are case sensitive. Otherwise,
                case insensitive.

        unsigned                type            : 4;
                This field can have the following values:
                    ST_NONE
                        Normal everyday joe-blow symbol.
                    ST_OPERATOR
                        C++ operator symbol. E.g. "operator +".
                        Note: In this case the name field would be "+".
                    ST_DESTRUCTOR
                        C++ destructor. E.g. "~foo".
                        Note: In this case the name field would be "foo".
                    ST_TYPE
                        A typedef name.
                    ST_STRUCT_TAG
                        A structure tag. E.g. "struct mas".
                        Note: The name field would just be "mas".
                    ST_CLASS_TAG
                        A class tag. E.g. "class mas".
                        Note: The name field would just be "mas".
                    ST_UNION_TAG
                        A union tag. E.g. "union mas".
                        Note: The name field would just be "mas".
                    ST_ENUM_TAG
                        A enum tag. E.g. "enum mas".
                        Note: The name field would just be "mas".

        The DIP should search in the indicated locations for the
        indicated symbol. Since more than one symbol may match the
        criteria due to C++ function overloading, a pointer to an
        imp_sym_handle is not passed as a parameter to be filled in.
        Instead, when the DIP finds a symbol that matches the lookup
        parameters it should do the following call:

                is = DCSymCreate( ii, d );

        Where 'ii' and 'd' are the parameters that were passed into
        the DIPImpLookupSym functions. If the client returns NULL, the
        sym handle could not be created for some reason and the DIP
        should terminate the symbol lookup and return SR_FAIL. If the
        return is non-NULL then it is a pointer to a freshly minted
        imp_sym_handle which the implementation should fill in with
        the appropriate values for the symbol that it had just found.
        The DIP should then continue looking for the next symbol that
        matches the search criteria (unless it can tell that there
        won't be anymore). After the search has completed, the routine
        should return SR_EXACT if any symbols were found and SR_NONE
        otherwise.
    */
    return( SR_NONE );
}

search_result   DIPENTRY DIPImpLookupSymEx( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    // see above
    return( SR_NONE );
}

search_result   DIPENTRY DIPImpAddrScope( imp_image_handle *ii,
                imp_mod_handle im, address addr, scope_block *scope )
{
    //TODO:
    /*
        Find the range of the lexical scope block enclosing 'addr' in
        module 'im'. If there is no such scope, return SR_NONE. Otherwise
        fill in scope->start with the address of the start of the lexical
        block and scope->len with the size of the block. Fill in
        scope->unique with something that uniquely identifies the lexical
        block in question. This is used to disamibiguate between blocks
        that start at the same address and have the same length. The value
        should be chosen so that
                1. It remains valid and consistant across a DIPUnloadInfo
                   and DIPLoadInfo of the same information.
                2. It remains the same whether the scope_block was obtained
                   by DIPImpAddrScope or DIPImpScopeOuter.
        Then return SR_EXACT/SR_CLOSEST as appropriate.
    */
    return( SR_NONE );
}

search_result   DIPENTRY DIPImpScopeOuter( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
{
    //TODO:
    /*
        Given the scope_block pointed to by 'in' in the module 'im', find
        the parent lexical block of it and fill in the scope_block pointed
        to by 'out' with the information. Return SR_EXACT/SR_CLOSEST as
        appropriate. Return SR_NONE if there is no parent block.
        Make sure that the case where 'in' and 'out' point to the same
        address is handled.
    */
    return( SR_NONE );
}

int DIPENTRY DIPImpSymCmp( imp_image_handle *ii, imp_sym_handle *is1,
                                imp_sym_handle *is2 )
{
    //TODO:
    /*
        Compare two sym handles and return 0 if they refer to the same
        information. If they refer to differnt things return either a
        positive or negative value to impose an 'order' on the information.
        The value should obey the following constraints.
        Given three handles H1, H2, H3:
                - if H1 < H2 then H1 is always < H2
                - if H1 < H2 and H2 < H3 then H1 is < H3
        The reason for the constraints is so that a client can sort a
        list of handles and binary search them.
    */
    return( 0 );
}

dip_status DIPENTRY DIPImpSymAddRef( imp_image_handle *ii, imp_sym_handle *is )
{
    /*
    see DIPImpTypeAddRef
    */
    return(DS_OK);
}

dip_status DIPENTRY DIPImpSymRelease( imp_image_handle *ii, imp_sym_handle *is )
{
    /*
    see DIPImpTypeRelease
    */
    return(DS_OK);
}

dip_status DIPENTRY DIPImpSymFreeAll( imp_image_handle *ii )
{
    /*
    see DIPImpTypeFreeAll
    */
    return(DS_OK);
}

