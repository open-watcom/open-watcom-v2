/******************************************************************************
*
* Mainline for mp3 decoder
*
******************************************************************************/

/* Include files */
#include <string.h>
#include <stdlib.h>
#include "mp3dec.h"
#include "internal.h"

extern void audio_write_wav( uint32_t *samples, uint32_t nsamples, int sample_rate );
extern void audio_write( uint32_t *samples, uint32_t nsamples, int sample_rate );

char                filename[256];
static uint32_t     mp3_outdata[2*576];


int main( int argc, char *argv[] )
{
    int decode = FALSE;
    int stream_pos;


    if( argc != 2 ) {
        printf( "Usage: %s <filename>\n", argv[0] );
        return( EXIT_FAILURE );
    }

    if( MPG_Stream_Open( argv[argc - 1] ) ) {
        printf( "Failed to open: %s\n", argv[0] );
        return( EXIT_FAILURE );
    }
    strcpy( filename, argv[argc - 1] );

    while( (stream_pos = MPG_Get_Filepos()) != C_MPG_EOF ) {

        if( MPG_Read_Frame() == OK ) {
            decode = TRUE;
        } else {
            if( MPG_Get_Filepos() == C_MPG_EOF ) {
                break;
            } else {
                decode = FALSE;
                ERR( "Not enough data to decode frame\n" );
            }
        }

        if( decode ) {
            /* Decode the compressed frame into 1152 mono/stereo PCM audio samples */
            MPG_Decode_L3( mp3_outdata );
            audio_write( (uint32_t *)&mp3_outdata, 2 * 576,
                     g_sampling_frequency[g_frame_header.id][g_frame_header.sampling_frequency] );
        }

        /* Remote control handling would be here */
    }

#ifdef OUTPUT_WAV
    audio_write( 0, 0, 0 );
#endif

    /* Done */
    return( EXIT_SUCCESS );
}
