/******************************************************************************
*
* File output functions
*
******************************************************************************/

/* Include files */
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "mp3dec.h"

/* External functions and variables (defined elsewhere, and used here) */
extern char filename[256];

//static char audio_name[20] = AUDIO;

/* Start of code */


#ifdef OUTPUT_RAW

void audio_write( uint32_t *samples, uint32_t nsamples, int sample_rate )
{
    static int      init = 0;
    static FILE     *fd;
    char            fname[1024];

    if( init == 0 ) {
        init = 1;

        sprintf (fname, "%s.raw", filename);

        fd = fopen(fname, "wb");
        if( fd == NULL ) {
            perror (fname);
            exit (-1);
        }
    }

    fwrite( samples, nsamples, 4, fd );
    return;
} /* audio_write() */

#endif

#ifdef OUTPUT_WAV

void audio_write( uint32_t *samples, uint32_t nsamples, int sample_rate )
{
    static int      init = 0;
    static FILE     *fd;
    char            fname[1024];
    int             i, l/*, nch*/;

    if( init == 0 ) {
        char    sig[5];

        init = 1;

        strcpy( fname, filename );
        if( strrchr(fname, '.') )
            fname[strrchr(fname, '.') - fname] = 0;
        strcat(fname, ".wav");

        fd = fopen(fname, "wb");

        if (fd == NULL) {
            perror (fname);
            exit (-1);
        }
        strcpy(sig, "RIFF");
        fwrite(sig, 4, 1, fd);
        fwrite(&i, 4, 1, fd);

        strcpy(sig, "WAVE");
        fwrite(sig, 4, 1, fd);
        strcpy(sig, "fmt ");
        fwrite(sig, 4, 1, fd);
        i = 16; fwrite(&i, 4, 1, fd);
        i = 1; fwrite(&i, 2, 1, fd);
        i = 2; fwrite(&i, 2, 1, fd);
        fwrite(&sample_rate, 4, 1, fd);
        i = sample_rate*4; fwrite(&i, 4, 1, fd);
        i = 4; fwrite(&i, 2, 1, fd);
        i = 16; fwrite(&i, 2, 1, fd);

        strcpy(sig, "data");
        fwrite(sig, 4, 1, fd);
        fwrite(&i, 4, 1, fd);
    }

    if (nsamples) {
        fwrite(samples, nsamples, 4, fd);
    } else {
        l = ftell(fd);
        fseek(fd, 4, SEEK_SET);
        i = l - 8;
        fwrite(&i, 4, 1, fd);
        fseek(fd, 40, SEEK_SET);
        i = l - 44;
        fwrite(&i, 4, 1, fd);
    }
    return;
}

#endif
