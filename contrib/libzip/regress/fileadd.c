#include <stdio.h>
#include <stdlib.h>

#include "zip.h"

int
main(int argc, char *argv[])
{
    struct zip *za;
    int i;

    if (argc < 3) {
	fprintf(stderr, "%s: need at least two arguments: zipfile"
		" and files to add\n", argv[0]);
	exit(1);
    }

    if ((za=zip_open(argv[1], ZIP_CHECKCONS|ZIP_CREATE)) == NULL) {
	fprintf(stderr, "%s: can't open zipfile %s: %s\n", argv[0],
		argv[1], zip_err_str[zip_err]);
	exit(1);
    }

    for (i=0; i<argc-2; i++)
	if (zip_add_file(za, NULL, NULL, argv[i+2], 0, -1)==-1) {
	    fprintf(stderr, "%s: can't add file %s: %s\n", argv[0],
		    argv[i+2], zip_err_str[zip_err]);
	    exit(1);
	}

    if (zip_close(za) == -1) {
	fprintf(stderr, "%s: can't close zipfile %s: %s\n", argv[0],
		argv[1], zip_err_str[zip_err]);
	exit(1);
    }

    return 0;
}
