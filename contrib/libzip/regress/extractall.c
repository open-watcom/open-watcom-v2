/*
  extractall.c -- test full archive enumeration and extraction
  Copyright (C) 2026 The Open Watcom Contributors.

  Simulates what the OW installer does: open a ZIP archive,
  enumerate all entries, stat each one, read all data.
  Uses test.zip (stored files) and broken.zip (mixed methods).
*/



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zip.h"
#include "mkname.h"

#define BUFSIZE 8192

int extract_all(const char *archive, int expected_nent);
int enumerate_all(const char *archive, int expected_nent);
int test_create_add_extract(void);



int
main(int argc, char *argv[])
{
    int fail;

    fail = 0;

    /* test.zip: 3 entries -- full extract like installer does */
    fail += extract_all("test.zip", 3);

    /* broken.zip: 5 entries with intentional CRC/zlib errors --
       verify we can open, enumerate, and stat all entries
       (reading will fail on corrupt entries by design) */
    fail += enumerate_all("broken.zip", 5);

    /* create archive, add file, close, reopen, extract, verify */
    fail += test_create_add_extract();

    if (fail)
        printf("%d test(s) FAILED\n", fail);
    else
        printf("all tests passed\n");

    exit(fail ? 1 : 0);
}



/* Open archive, enumerate all entries, stat and read each one.
   This is the core installer flow. */

int
extract_all(const char *archive, int expected_nent)
{
    struct zip *z;
    struct zip_file *zf;
    struct zip_stat st;
    int ze, nent, i, n;
    char buf[BUFSIZE];
    const char *name;
    unsigned long total;

    if ((z = zip_open(mkname(archive), ZIP_CHECKCONS, &ze)) == NULL) {
        printf("fail: %s: can't open (%d)\n", archive, ze);
        return 1;
    }

    nent = zip_get_num_files(z);
    if (nent != expected_nent) {
        printf("fail: %s: expected %d entries, got %d\n",
               archive, expected_nent, nent);
        zip_close(z);
        return 1;
    }

    for (i = 0; i < nent; i++) {
        name = zip_get_name(z, i, 0);
        if (name == NULL) {
            printf("fail: %s: can't get name for entry %d: %s\n",
                   archive, i, zip_strerror(z));
            zip_close(z);
            return 1;
        }

        if (zip_stat_index(z, i, 0, &st) < 0) {
            printf("fail: %s: can't stat '%s': %s\n",
                   archive, name, zip_strerror(z));
            zip_close(z);
            return 1;
        }

        /* skip directories */
        if (st.size == 0)
            continue;

        if ((zf = zip_fopen_index(z, i, 0)) == NULL) {
            printf("fail: %s: can't fopen '%s': %s\n",
                   archive, name, zip_strerror(z));
            zip_close(z);
            return 1;
        }

        total = 0;
        while ((n = zip_fread(zf, buf, sizeof(buf))) > 0)
            total += n;

        if (n < 0) {
            printf("fail: %s: read error on '%s'\n", archive, name);
            zip_fclose(zf);
            zip_close(z);
            return 1;
        }

        if (total != st.size) {
            printf("fail: %s: '%s' size mismatch: read %lu, expected %lu\n",
                   archive, name, total, (unsigned long)st.size);
            zip_fclose(zf);
            zip_close(z);
            return 1;
        }

        zip_fclose(zf);
    }

    zip_close(z);
    return 0;
}



/* Open archive, enumerate and stat all entries without reading.
   Used for archives with intentionally corrupt data. */

int
enumerate_all(const char *archive, int expected_nent)
{
    struct zip *z;
    struct zip_stat st;
    int ze, nent, i;
    const char *name;

    if ((z = zip_open(mkname(archive), ZIP_CHECKCONS, &ze)) == NULL) {
        printf("fail: %s: can't open (%d)\n", archive, ze);
        return 1;
    }

    nent = zip_get_num_files(z);
    if (nent != expected_nent) {
        printf("fail: %s: expected %d entries, got %d\n",
               archive, expected_nent, nent);
        zip_close(z);
        return 1;
    }

    for (i = 0; i < nent; i++) {
        name = zip_get_name(z, i, 0);
        if (name == NULL) {
            printf("fail: %s: can't get name for entry %d: %s\n",
                   archive, i, zip_strerror(z));
            zip_close(z);
            return 1;
        }

        if (zip_stat_index(z, i, 0, &st) < 0) {
            printf("fail: %s: can't stat '%s': %s\n",
                   archive, name, zip_strerror(z));
            zip_close(z);
            return 1;
        }
    }

    zip_close(z);
    return 0;
}



/* Create archive, add a file from buffer, close, reopen,
   extract and verify contents match. Round-trip test. */

int
test_create_add_extract(void)
{
    struct zip *z;
    struct zip_file *zf;
    struct zip_source *zs;
    int ze, n;
    char buf[256];
    const char *testdata = "Open Watcom installer test data.\n";
    const char *testname = "testfile.txt";
    const char *archive = "roundtrip_test.zip";
    int testlen;

    testlen = strlen(testdata);

    /* create and add */
    if ((z = zip_open(archive, ZIP_CREATE, &ze)) == NULL) {
        printf("fail: roundtrip: can't create archive (%d)\n", ze);
        return 1;
    }

    if ((zs = zip_source_buffer(z, testdata, testlen, 0)) == NULL
        || zip_add(z, testname, zs) == -1) {
        zip_source_free(zs);
        printf("fail: roundtrip: can't add file: %s\n", zip_strerror(z));
        zip_close(z);
        remove(archive);
        return 1;
    }

    if (zip_close(z) != 0) {
        printf("fail: roundtrip: can't close after add\n");
        remove(archive);
        return 1;
    }

    /* reopen with consistency check and extract */
    if ((z = zip_open(archive, ZIP_CHECKCONS, &ze)) == NULL) {
        printf("fail: roundtrip: can't reopen (%d)\n", ze);
        remove(archive);
        return 1;
    }

    if (zip_get_num_files(z) != 1) {
        printf("fail: roundtrip: expected 1 entry, got %d\n",
               zip_get_num_files(z));
        zip_close(z);
        remove(archive);
        return 1;
    }

    if ((zf = zip_fopen(z, testname, 0)) == NULL) {
        printf("fail: roundtrip: can't fopen '%s': %s\n",
               testname, zip_strerror(z));
        zip_close(z);
        remove(archive);
        return 1;
    }

    n = zip_fread(zf, buf, sizeof(buf));
    zip_fclose(zf);
    zip_close(z);
    remove(archive);

    if (n != testlen || memcmp(buf, testdata, testlen) != 0) {
        printf("fail: roundtrip: data mismatch (read %d bytes, expected %d)\n",
               n, testlen);
        return 1;
    }

    return 0;
}
