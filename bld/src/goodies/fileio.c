/* Time buffered file I/O using a buffer of arbitrary size in low or
        extended memory.
*/

#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>

int iobufsize = 32;                                                     /* in K */

void fatal (char *msg)
        {
        printf ("fileio: %s.\n", msg);
        exit (1);
        }

void main (int argc, char *argv[])
        {
        union REGS r;
        FILE *fp;
        register int i;
        clock_t start, end;
        char *outputbuf;
        int do_low = 0;

        /* Allocate an output buffer, in either low memory or extended memory,
                as directed.  Associate the buffer with the output file.
        */
        while (argc-- > 1)
                {
                if ((argv[argc][0] == '-') || (argv[argc][0] == '/'))
                        {
                        if ((argv[argc][1] == 'l') || (argv[argc][1] == 'L'))
                                do_low = 1;
                        }
                else
                        iobufsize = atoi(argv[argc]);
                }

        if (do_low)
                {
                r.x.eax = 0x100;                                                /* DPMI allocate low memory */
                r.x.ebx = iobufsize * (1024 / 16);
                int386 (0x31, &r, &r);

                if (r.w.cflag)
                        fatal ("couldn't allocate low memory");
                outputbuf = (char *) (r.x.eax << 4);
                }
        else
                {
                if (! (outputbuf = malloc (iobufsize * 1024)))
                        fatal ("couldn't allocate extended memory");
                }
        printf ("Using %dK buffer in %s memory\n", iobufsize, do_low ? "low" : "extended");

        if (! (fp = fopen ("bigfile", "w")))
                fatal ("couldn't open output file");
        if (setvbuf (fp, outputbuf, _IOFBF, iobufsize * 1024))
                fatal ("couldn't buffer output file");

        start = clock ();
        for (i = 256 * 1024; i > 0; i--)
                fputc ('x', fp);                                                /* Write 256K 'x' characters */
        fflush (fp);                                                            /* Commit to disk */
        end = clock ();

        if (ferror (fp))
                fatal ("error writing output file");

        fclose (fp);
        remove ("bigfile");
        printf ("elapsed time: %d clocks\n", (int) (end - start));
        }
