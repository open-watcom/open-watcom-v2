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


#include "myfile.h"
#include "compress.h"
#include "hcmem.h"

#define BLOCK_SIZE      128
#define PAGE_SIZE       1024

struct BlockFile : public File
{
    unsigned long size() { return size_; };
    void        reset() { fseek( fp, 0, SEEK_SET ); };
    int get_block( char * dest );
    BlockFile ( char const fname[] );
private:
    unsigned long size_;
};

BlockFile::BlockFile( char const fname[] ) : File( fname, File::READ )
{
    if( !bad_file ){
        fseek( fp, 0, SEEK_END );
        size_ = ftell( fp );
        fseek( fp, 0, SEEK_SET );
    }
}

int BlockFile::get_block( char * dest )
{
    int result = fread( dest, 1, BLOCK_SIZE, fp );
    return result;
}

static Memory bogus;

int main( int argc, char *argv[] )
{
    if( argc == 1 ){
        HCError( ARG_ERR );
    }

    BlockFile   input( argv[1] );
    if( !input ) {
        HCError( FILE_ERR );
    }

    Buffer<char>        block(BLOCK_SIZE);
    Buffer<int>         pagebreaks( input.size() / BLOCK_SIZE + 1 );

    int         amount_read = 0;
    int         amount_written = 0;
    CompWriter  trashcan;
    CompReader  compactor( &trashcan );
    int i=0;

    for( int num_pages=0 ;; num_pages++ ){
        amount_read = input.get_block( block );
        if( amount_read == 0 ) break;
        amount_written += compactor.compress( block, amount_read );
        if( amount_written > PAGE_SIZE ){
            pagebreaks[i++] = num_pages;
            compactor.flush();
            amount_written = compactor.compress( block, amount_read );
        }
    }

    input.reset();
    OutFile     output("pagecomp.out");
    if( !output ){
        HCError( FILE_ERR );
    }
    CompOutFile newwriter( &output );

    compactor.reset( &newwriter );
    i = 0;
    amount_written = 0;
    for( int j=0; j < num_pages; j++ ){
        if( j == pagebreaks[i] ){
            compactor.flush();
            while( amount_written < PAGE_SIZE ){
                output.writech( 0 );
                amount_written++;
            }
            amount_written = 0;
            i++;
        } else {
            amount_read = input.get_block( block );
            amount_written += compactor.compress( block, amount_read );
        }
    }
    return 1;
}
