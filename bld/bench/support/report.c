#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define REPORT_OUTPUT_FILE	"BENCH_REPORT_FILE"

void Report( char *bench_name, double elapsed_time )
{
    char	*out_file;
    FILE	*fp;

    out_file = getenv( REPORT_OUTPUT_FILE );
    if( out_file == NULL ) {
	out_file = "bench.out";
    }
    fp = fopen( out_file, "a" );
    if( fp == NULL ) {
	fprintf( stderr, "unable to open output file '%s' - %s\n",
	    out_file, strerror( errno ) );
	return;
    }
    fprintf( fp, "%s %.14lf\n", bench_name, elapsed_time );
    fclose( fp );
}
