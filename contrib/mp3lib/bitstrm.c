/******************************************************************************
*
*  This file contains an abstraction of the bitstream I/O used
*  by the MPEG decoder. It will map high-level calls to the
*  appropiate file I/O provided by the underlying OS.
*
******************************************************************************/

/* Include files */
#include "mp3dec.h"
#include "internal.h"

/* Local functions and variables */
static FILE *fp = NULL;


/******************************************************************************
*
* Description: Opens a mp3 file with the given name.
* Parameters: Name of the file.
* Return value: OK if file open succeeded, ERROR otherwise.
*
******************************************************************************/
int MPG_Stream_Open( const char *filename )
{
    if( (fp = fopen( filename, "rb")) == NULL ) {
        return( ERROR );
    }
    return( OK );
}


/******************************************************************************
*
* Description: This function returns the next byte from the bitstream, or EOF.
*              If we're not on an byte-boundary the bits remaining until
*              the next boundary are discarded before getting that byte.
* Parameters: None
* Return value: The next byte in bitstream in the lowest 8 bits, or C_MPG_EOF.
*
******************************************************************************/
uint32_t MPG_Get_Byte( void )
{
    uint32_t    val;

    /* Get byte */
    val = fgetc( fp ) & 0xff;

    /* EOF? */
    if( feof( fp ) ) {
        val = C_MPG_EOF;
    }

    /* Done */
    return( val );
}


/******************************************************************************
*
* Description: This file reads 'no_of_bytes' bytes of data from the input
*              stream into 'data_vec[]'.
* Parameters: Number of bytes to read, vector pointer where to store them.
* Return value: OK or ERROR if the operation couldn't be performed.
*
******************************************************************************/
int MPG_Get_Bytes( uint32_t no_of_bytes, uint32_t data_vec[] )
{
    int         i;
    uint32_t    val;

    for( i = 0; i < no_of_bytes; i++ ) {
        val = MPG_Get_Byte();

        if( val == C_MPG_EOF ) {
            return( C_MPG_EOF );
        } else {
            data_vec[i] = val;
        }
    }

    return( OK );
}


/******************************************************************************
*
* Description: This function returns the current file position in bytes.
* Parameters: None
* Return value: File pos in bytes, or 0 if no file open.
*
******************************************************************************/
uint32_t MPG_Get_Filepos( void )
{
    /* File open? */
    if( fp == NULL ) {
        return( 0 );
    }

    if( feof( fp ) ) {
        return( C_MPG_EOF );
    } else {
        return( (uint32_t)ftell( fp ) );
    }
}
