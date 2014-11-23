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
        Stuff for source line cues
*/

walk_result     DIGENTRY DIPImpWalkFileList( imp_image_handle *ii,
                    imp_mod_handle im, IMP_CUE_WKR *wk, imp_cue_handle *ic,
                    void *d )
{
    //TODO:
    /*
        PSEUDO-CODE:

        for( each source file in the given module ) {
            *ic = fill in cue handle information for the first
                        source cue for the file;
            wr = wk( ii, ic, d );
            if( wr != WR_CONTINUE ) return( wr );
        }
    */
    return( WR_CONTINUE );
}

imp_mod_handle  DIGENTRY DIPImpCueMod( imp_image_handle *ii,
                                imp_cue_handle *ic )
{
    //TODO:
    /*
        Return the module the source cue comes from.
     */
     return( IMH_NOMOD );
}

unsigned        DIGENTRY DIPImpCueFile( imp_image_handle *ii,
                        imp_cue_handle *ic, char *buff, unsigned buff_size )
{
    /*
        Given the imp_cue_handle, copy the name of the source file into 'buff'.
        Do not copy more than 'buff_size' - 1 characters into the buffer and
        append a trailing '\0' character. Return the real length
        of the file name (not including the trailing '\0' character) even
        if you had to truncate it to fit it into the buffer. If something
        went wrong and you can't get the module name, call DCStatus and
        return zero. NOTE: the client might pass in zero for 'buff_size'. In that
        case, just return the length of the file name and do not attempt
        to put anything into the buffer.
    */
    return( 0 );
}

cue_fileid  DIGENTRY DIPImpCueFileId( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    //TODO:
    /*
        Given a imp_cue_handle, return a cue_fileid. The cue_fileid is
        an unsigned_32 magic cookie as far as the client is concerned,
        except that the value zero is reserved to indicate "no file".
        The client might pass the cue_fileid back to the LineCue
        function, or use it as a quick test to see if the source file
        has changed by comparing it against a previously returned value.
    */
    return( 0 );
}

dip_status      DIGENTRY DIPImpCueAdjust( imp_image_handle *ii,
                imp_cue_handle *src, int adj, imp_cue_handle *dst )
{
    //TODO:
    /*
        Adjust the 'src' cue by 'adj' amount and return the result in 'dst'.
        That is, If you get called with "DIPImpCueAdjust( ii, src, 1, dst )",
        the 'dst' handle should be filled in with implementation cue handle
        representing the source cue immediately following the 'src' cue.
        Passing in an 'adj' of -1 will get the immediately preceeding source
        cue. The list of source cues for each file are considered a ring,
        so if 'src' is the first cue in a file, an 'adj' of -1 will return
        the last source cue FOR THAT FILE. The cue adjust never crosses a
        file boundry. Also, if 'src' is the last cue in a file, and 'adj' of
        1 will get the first source cue FOR THAT FILE. If an adjustment
        causes a wrap from begining to end or vis-versa, you should return
        DS_WRAPPED status (NOTE: DS_ERR should *not* be or'd in, nor should
        DCStatus be called in this case). Otherwise DS_OK should be returned
        unless an error occurred.
    */
    return( DS_ERR|DS_FAIL );
}

unsigned long   DIGENTRY DIPImpCueLine( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    //TODO:
    /*
        Return the line number of source cue. Return zero if there
        is no line number associated with the cue, or an error occurs in
        getting the information.
    */
    return( 0 );
}

unsigned        DIGENTRY DIPImpCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    //TODO:
    /*
        Return the column number of source cue. Return zero if there
        is no column number associated with the cue, or an error occurs in
        getting the information.
    */
    return( 0 );
}

address         DIGENTRY DIPImpCueAddr( imp_image_handle *ii,
                        imp_cue_handle *ic )
{
    //TODO:
    /*
        Return the address of source cue. Return NilAddr if there
        is no address associated with the cue, or an error occurs in
        getting the information.
    */
    return( NilAddr );
}

search_result   DIGENTRY DIPImpLineCue( imp_image_handle *ii,
                imp_mod_handle im, cue_fileid file, unsigned long line,
                unsigned column, imp_cue_handle *ic )
{
    //TODO:
    /*
        Search for a source cue in the given module. If 'file' is zero,
        use the main source file of the module. If 'line' is zero, use the
        first line with a source cue in the given file. If 'column' is
        zero, use the first column with a source cue in the given line.
        Fill in the '*ic' handle with the result. If there was a cue at
        exactly the file/line/column specified return SR_EXACT. If there
        are cues with in the file with a line/column less than the given
        values, return the largest cue possible that is less then the
        passed in values and return SR_CLOSEST. If there are no cues with
        the proper characteristics, return SR_NONE
    */
    return( SR_NONE );
}

search_result   DIGENTRY DIPImpAddrCue( imp_image_handle *ii,
                imp_mod_handle im, address addr, imp_cue_handle *ic )
{
    //TODO:
    /*
        Search for the closest cue in the given module that has an address
        less then or equal to the given address. If there is no such cue
        return SR_NONE. If there is one exactly at the address return
        SR_EXACT. Otherwise, return SR_CLOSEST.
    */
    return( SR_NONE );
}

int DIGENTRY DIPImpCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                                imp_cue_handle *ic2 )
{
    //TODO:
    /*
        Compare two cue handles and return 0 if they refer to the same
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
