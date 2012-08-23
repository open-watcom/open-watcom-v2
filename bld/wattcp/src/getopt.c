/* getopt.c (emx+gcc) -- Copyright (c) 1990-1993 by Eberhard Mattes */
/* Adapted for Waterloo TCP/IP by G. Vanem Nov-96                   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "language.h"
#include "getopt.h"

char *optarg          = NULL;
int   optopt          = 0;
int   optind          = 0;              /* Default: first call             */
int   opterr          = 1;              /* Default: error messages enabled */
char *optswchar       = "-/";           /* '-' or '/' starts options       */
enum _optmode optmode = GETOPT_UNIX;

static char *next_opt;             /* Next character in cluster of options */
static char *empty = "";           /* Empty string                         */

static int  done;
static char sw_char;

static char **options;            /* List of entries which are options     */
static char **non_options;        /* List of entries which are not options */
static int    options_count;
static int    non_options_count;

#define BEGIN do {
#define END   } while (0)

#define PUT(dst) BEGIN \
                  if (optmode == GETOPT_ANY) \
                     dst[dst##_count++] = argv[optind]; \
                 END

int getopt (int argc, char **argv, const char *opt_str)
{
  char c, *q;
  int  i, j;

  if (optind == 0)
  {
    optind   = 1;
    done     = 0;
    next_opt = empty;
    if (optmode == GETOPT_ANY)
    {
      options     = malloc (argc * sizeof(char*));
      non_options = malloc (argc * sizeof(char*));
      if (!options || !non_options)
      {
        printf (_LANG("out of memory (getopt)\n"));
        exit (255);
      }
      options_count     = 0;
      non_options_count = 0;
    }
  }
  if (done)
     return (EOF);

restart:
  optarg = NULL;
  if (*next_opt == 0)
  {
    if (optind >= argc)
    {
      if (optmode == GETOPT_ANY)
      {
        j = 1;
        for (i = 0; i < options_count; ++i)
            argv[j++] = options[i];
        for (i = 0; i < non_options_count; ++i)
            argv[j++] = non_options[i];
        optind = options_count+1;
        free (options);
        free (non_options);
      }
      done = 1;
      return (EOF);
    }
    else if (!strchr (optswchar, argv[optind][0]) || argv[optind][1] == 0)
    {
      if (optmode == GETOPT_UNIX)
      {
        done = 1;
        return (EOF);
      }
      PUT (non_options);
      optarg = argv[optind++];
      if (optmode == GETOPT_ANY)
         goto restart;
      /* optmode==GETOPT_KEEP */
      return (0);
    }
    else if (argv[optind][0] == argv[optind][1] && argv[optind][2] == 0)
    {
      if (optmode == GETOPT_ANY)
      {
        j = 1;
        for (i = 0; i < options_count; ++i)
            argv[j++] = options[i];
        argv[j++] = argv[optind];
        for (i = 0; i < non_options_count; ++i)
            argv[j++] = non_options[i];
        for (i = optind+1; i < argc; ++i)
            argv[j++] = argv[i];
        optind = options_count+2;
        free (options);
        free (non_options);
      }
      ++optind;
      done = 1;
      return (EOF);
    }
    else
    {
      PUT (options);
      sw_char  = argv[optind][0];
      next_opt = argv[optind]+1;
    }
  }
  c = *next_opt++;
  if (*next_opt == 0)  /* Move to next argument if end of argument reached */
     optind++;
  if (c == ':' || (q = strchr (opt_str, c)) == NULL)
  {
    if (opterr)
    {
      if (c < ' ' || c >= 127)
           printf (_LANG("Invalid option; character code=0x%.2x\n"), c);
      else printf (_LANG("Invalid option `%c%c'\n"), sw_char, c);
    }
    optopt = '?';
    return ('?');
  }
  if (q[1] == ':')
  {
    if (*next_opt != 0)         /* Argument given */
    {
      optarg = next_opt;
      next_opt = empty;
      ++optind;
    }
    else if (q[2] == ':')
      optarg = NULL;            /* Optional argument missing */
    else if (optind >= argc)
    {                         /* Required argument missing */
      if (opterr)
         printf (_LANG("No argument for `%c%c' option\n"), sw_char, c);
      c = '?';
    }
    else
    {
      PUT (options);
      optarg = argv[optind++];
    }
  }
  optopt = c;
  return (c);
}

