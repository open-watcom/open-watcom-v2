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


#ifndef _MFILE_INCLUDED
#define _MFILE_INCLUDED

// System includes --------------------------------------------------------

extern "C" {
    #include <sys\types.h>
    #include <sys\stat.h>
    #include <fcntl.h>
    #include <wstd.h>
};
// Project includes -------------------------------------------------------

// Defines ----------------------------------------------------------------

#define MFileORead      ( O_RDONLY )
#define MFileOReadB     ( O_RDONLY | O_BINARY )
#define MFileOWrite     ( O_WRONLY | O_CREAT | O_TRUNC )
#define MFileOWriteB    ( O_WRONLY | O_CREAT | O_TRUNC | O_BINARY )

/*
 * Access rights which can be specified when the O_CREAT option is
 * used.  These are passed as the second parameter to safeOpen.
 */
#define MFileCreateDefault      ( S_IRUSR | S_IWUSR )

class MFile
{
public:
                                MFile( char * fname );
                                ~MFile();
    bool                        open( int access = MFileOReadB,
                                      int createmode = MFileCreateDefault );
    void                        close();
    int                         read( void * buffer, int length );
    int                         write( void * buffer, int length );
    int                         putch( unsigned char c );
    long                        seek( long pos );
    long                        tell();
    unsigned char               getch();
    void                        gets( char * str, int & length );
    char *                      name();
    long                        fileSize();
    uint_32                     readULEB128();
    int                         writeULEB128( uint_32 value );
    void                        append( char * path );

    static long                 fileSize( char * path );

    char *                      _fileName;

private:
    int         _handle;
    bool        _isOpen;
};

enum {
    MFileErrGeneral
};
#endif

