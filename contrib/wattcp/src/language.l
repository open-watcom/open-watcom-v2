%{

/*
 *  lang_scan.l - Foreign language lexical scanner for Waterloo TCP/IP
 *
 *  Gisle Vanem 1997
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#include "wattcp.h"
#include "strings.h"
#include "sock_ini.h"
#include "misc.h"
#include "crc.h"

#if defined(USE_LANGUAGE)

#if defined(TEST_PROG)
  #include <conio.h>
  #define INCLINE()   fprintf (stderr, "<line %lu>\n", lang.line++)
  #define DEBUG(s,a)  fprintf (stderr, s, a)
#else
  #define INCLINE()   lang.line++
  #define DEBUG(s,a)  ((void)0)
#endif

#define BUF_SIZE    500

static int   yylex      (void);
static void  AddChar    (int ch);
static void  AddDecimal (const char *buf);
static void  AddHexcode (const char *buf);
static void  AddOctal   (const char *buf);
static void  AddEntry   (void);
static void  Abort      (const char *s1, const char *s2);
static void  error      (const char *err);

static char    buf [BUF_SIZE];
static char   *ptr       = NULL;
static char   *end       = buf+BUF_SIZE;
static int     line_cont = 0;
static DWORD   crc_ref   = 0;
static DWORD   do_ref    = 1;
static jmp_buf bail_out;

struct Node {
       DWORD        crc;
       char        *string;
       struct Node *next;
     };

struct Elem {
       DWORD  crc;
       char  *string;
     };

static struct Language {
       struct Node *list;
       struct Elem *array;
       char         prefix[4];     /* "en:"         */
       DWORD        entries;
       DWORD        line;
     } lang = {
       NULL, NULL, "en",
       0L, 0L
     };

/*---------------------------------------------------------------------*/

%}

/*
 * Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
 * as well as gratuitiously global symbol names, so we can have multiple
 * yacc generated parsers in the same program.  Note that these are only
 * the variables produced by yacc.  If other parser generators (bison,
 * byacc, etc) produce additional global names that conflict at link time,
 * then those parser generators need to be fixed instead of adding those
 * names to this list.
 */

%option prefix = "watt"

%x STRING EO_LINE PREFIX

NL                   \r?\n
WS                   [\t ]+
HEX                  (x[0-9a-fA-F]{1,2})
OCTAL                (0[0-7]{1,3})
DECIMAL              ([0-9]{1,3})
LANG                 "de"|"no"|"sv"

/*
 *  Possible LANG codes are according to ISO 3166
 *
 *  Ref. ftp://ftp.informatik.uni-erlangen.de/pub/doc/ISO/
 *
 *  aa Afar          ba Bashkir           ca Catalan
 *  ab Abkhazian     be Byelorussian      co Corsican
 *  af Afrikaans     bg Bulgarian         cs Czech
 *  am Amharic       bh Bihari            cy Welsh
 *  ar Arabic        bi Bislama
 *  as Assamese      bn Bengali; Bangla   da Danish
 *  ay Aymara        bo Tibetan           de German
 *  az Azerbaijani   br Breton            dz Bhutani
 *
 *  el Greek         fa Persian           ga Irish
 *  en English       fi Finnish           gd Scots Gaelic
 *  eo Esperanto     fj Fiji              gl Galician
 *  es Spanish       fo Faroese           gn Guarani
 *  et Estonian      fr French            gu Gujarati
 *  eu Basque        fy Frisian
 *
 *  ha Hausa         ia Interlingua       ja Japanese
 *  he Hebrew        id Indonesian        jw Javanese
 *  hi Hindi         ie Interlingue
 *  hr Croatian      ik Inupiak
 *  hu Hungarian     is Icelandic
 *  hy Armenian      it Italian
 *                   iu Inuktitut
 *  ka Georgian                           mg Malagasy
 *  kk Kazakh        la Latin             mi Maori
 *  kl Greenlandic   ln Lingala           mk Macedonian
 *  km Cambodian     lo Laothian          ml Malayalam
 *  kn Kannada       lt Lithuanian        mn Mongolian
 *  ko Korean        lv Latvian, Lettish  mo Moldavian
 *  ks Kashmiri                           mr Marathi
 *  ku Kurdish                            ms Malay
 *  ky Kirghiz                            mt Maltese
 *                                        my Burmese
 *  na Nauru         oc Occitan
 *  ne Nepali        om (Afan) Oromo      pa Punjabi
 *  nl Dutch         or Oriya             pl Polish
 *  no Norwegian                          ps Pashto, Pushto
 *                                        pt Portuguese
 *  qu Quechua
 *
 *  rm Rhaeto-Romance  sa Sanskrit        ta Tamil
 *  rn Kirundi         sd Sindhi          te Telugu
 *  ro Romanian        sg Sangro          tg Tajik
 *  ru Russian         sh Serbo-Croatian  th Thai
 *  rw Kinyarwanda     si Sinhalese       ti Tigrinya
 *                     sk Slovak          tk Turkmen
 *  ug Uighur          sl Slovenian       tl Tagalog
 *  uk Ukrainian ??    sm Samoan          tn Setswana
 *  ur Urdu            sn Shona           to Tonga
 *  uz Uzbek           so Somali          tr Turkish
 *                     sq Albanian        ts Tsonga
 *  vi Vietnamese      sr Serbian         tt Tatar
 *  vo Volapuk         ss Siswati         tw Twi
 *                     st Sesotho
 *  wo Wolof           su Sundanese
 *                     sv Swedish
 *  xh Xhosa           sw Swahili
 *
 *  yi Yiddish (formerly ji)
 *  yo Yoruba
 *
 *  za Zhuang
 *  zh Chinese
 *  zu Zulu
 */

%%

<INITIAL>{                               /* get reference string */
        {WS}         /* discard */
        [#;].*{NL}   lang.line++;
        {NL}         INCLINE();
        \"           {
                       if (!line_cont)
                          ptr = buf;
                       BEGIN (STRING);
                     }
        .            {
                       unput (yytext[0]);
                       BEGIN (PREFIX);
                     }
}

<STRING>{
        \"           line_cont = 0; BEGIN (EO_LINE);
        \\a          AddChar ('\7');
        \\b          AddChar ('\b');
        \\t          AddChar ('\t');
        \\f          AddChar ('\f');
        \\e          AddChar (27);
        \\r          AddChar ('\r');
        \\n          AddChar ('\n');
        \\\\         AddChar ('\\');
        \\\"         AddChar ('\"');
        \\{HEX}      AddHexcode (yytext+2);
        \\{OCTAL}    AddOctal   (yytext+1);
        \\{DECIMAL}  AddDecimal (yytext+1);
        \\.          Abort ("Unknown ESC code: ", yytext);
        {NL}         Abort ("unterminated line", NULL);
        .            AddChar (yytext[0]);
        <<EOF>>      Abort ("Unexpected end-of-file", NULL);
}

<EO_LINE>{
        \\\\         {
                       line_cont = 1;
                       BEGIN (INITIAL);
                     }
        {NL}         {
                       *ptr = 0;
                       ptr  = NULL;
                       if (do_ref)
                            crc_ref = watt_crc_bytes (buf, strlen(buf));
                       else AddEntry();
                       INCLINE();
                       BEGIN (INITIAL);
                     }
        .            /* discard */
        <<EOF>>      Abort ("Unexpected end-of-string", NULL);
}

<PREFIX>{
        {LANG}":"    {
                       DEBUG ("lang `%s',\n", yytext);
                       if (!strncmp(yytext,lang.prefix,3))
                       {
                         ptr    = buf;
                         do_ref = 0;
                       }
                       else
                       {
                         ptr    = NULL;
                         do_ref = 1;
                       }
                       BEGIN (INITIAL);
                     }
        [#;].*{NL}   lang.line++;
        {NL}         INCLINE();
        .            Abort ("unknown language prefix: ", yytext);
}

%%

/*---------------------------------------------------------------------*/

/*
 *  terminate yylex() when EOF reached
 */
int yywrap (void)
{
  return (1);
}

/*
 *  Add a character to output buffer
 */
static void AddChar (int ch)
{
  if (ptr)
  {
    DEBUG ("%c", ch);
    *ptr++ = ch;
    if (ptr >= end)
       error ("string too large");
  }
}

/*
 *  Add a decimal code to output buffer
 */
static void AddDecimal (const char *buf)
{
  if (ptr)
  {
    int dec;
    sscanf (buf, "%d", &dec);
    DEBUG ("dec <%d>", dec);
    *ptr++ = dec;
    if (dec > 255)
       Abort ("Illegal decimal", NULL);
    if (ptr >= end)
       error ("string too large");
  }
}

/*
 *  Add a hex code to output buffer
 */
static void AddHexcode (const char *buf)
{
  if (ptr)
  {
    WORD hex;
    sscanf (buf, "%hx", &hex);
    DEBUG ("hex <%X>", hex);
    *ptr++ = hex;
    if (ptr >= end)
       error ("string too large");
  }
}

/*
 *  Add an octal code to output buffer
 */
static void AddOctal (const char *buf)
{
  if (ptr)
  {
    unsigned oct;
    sscanf (buf, "%o", &oct);
    DEBUG ("oct <%o>", oct);
    *ptr++ = oct;
    if (ptr >= end)
       error ("string too large");
  }
}

/*
 *  Add an language entry for translation
 */
static void AddEntry (void)
{
  char        *str;
  struct Node *list = NULL;

  if ((str = strdup(buf)) == NULL ||
     (list = malloc(sizeof(*list))) == NULL)
  {
    outsnl ("Language file too big!");
    longjmp (bail_out, 1);
  }

  list->next   = lang.list;
  list->crc    = crc_ref;
  list->string = str;
  lang.list    = list;
  lang.entries++;
  do_ref = 1;
}

/*
 * Comparision routine needed by quick-sort and bsearch routines.
 * Don't use unsigned arithmetic
 */
static int Compare (const struct Elem *a, const struct Elem *b)
{
  if (a->crc > b->crc) return (1);
  if (a->crc < b->crc) return (-1);
  return (0);
}

/*
 * Sort the linked list in accending order of CRC value
 */
static int SortLanguageEntries (DWORD num)
{ 
  struct Node *lng, *next;
  struct Elem *array = malloc ((size_t)(num * sizeof(*array)));

  lang.array = array;
  if (!array)
     return (0);

  for (lng = lang.list; lng; lng = next, array++)
  {
    array->crc    = lng->crc;
    array->string = lng->string;
    next = lng->next;
    free (lng);
  }
  lang.list = NULL;
  qsort (lang.array, (size_t)num, (size_t)sizeof(*array), (int(*)())Compare);
  return (1);
}

/*
 *  Initialize the linked list for language.
 *
 *  The wattcp.cfg entry has format LANGUAGE = lang,language-file
 *  e.g. "LANGUAGE = de,$(ETC)\language.txt"
 *
 *  Country-code MUST be two characters (see ISO639 list above)
 *
 *  May be called multiple times to extend language entries from
 *  more than 1 file.
 */
void lang_init (const char *value)
{
  FILE *langf;
  char *s, *fname;

  if (!value)
     return;

  s = strchr (value, ',');
  if (!s || (s - value) != 2)
     Abort ("Illegal LANGUAGE syntax", NULL);

  lang.line = 1;
  lang.prefix[0] = value[0];
  lang.prefix[1] = value[1];
  lang.prefix[2] = ':';
  lang.prefix[3] = 0;
  strlwr (lang.prefix);

  if (!strnicmp(lang.prefix,"en",2))  /* english is default language */
     return;

  if (!watt_crc_init())
  {
    outsnl ("CRC init failed");
    return;
  }

  fname = ++s;
  langf = fopen (fname, "rb");
  if (!langf)
  {
    outs ("Cannot open language file ");
    outsnl (fname);
    return;
  }

  if (!setjmp(bail_out))
  {
    yyin = langf;
    yylex();
    fclose (langf);

    if (!SortLanguageEntries(lang.entries))
    {
      outsnl ("Could not sort LANGUAGE entries");
      lang.entries = 0;
    }
#if defined(TEST_PROG)
    printf ("file-name    = `%s'\n",   fname);
    printf ("lang.prefix  = `%s'\n",   lang.prefix);
    printf ("lang.entries = %lu\n",    lang.entries);
    printf ("lang.lines   = %lu\n",    lang.line);
    printf ("lang.list    = %08lX\n",  (DWORD)lang.list);
    printf ("lang.array   = %08lX\n\n",(DWORD)lang.array);
    {
      struct Elem *elem;
      int    i;
      for (i = 0, elem = lang.array; i < lang.entries; i++, elem++)
          printf ("CRC = %08lX, <%s>\n", elem->crc, elem->string);
    }
#endif
  }
}

/*
 * return foreign translation of string str
 */
const char *_LANG (const char *str)
{
  struct Elem *elem;
  DWORD  hash;
  int    len;

  if (!lang.entries || !str)
     return (str);

  len = strlen (str);
  if (len == 0)
     return (str);

  hash = watt_crc_bytes (str, len);

  elem = (struct Elem*) bsearch (&hash, lang.array, (size_t)lang.entries,
                                 (size_t)sizeof(*elem), (int(*)())Compare);

  if (!elem || !elem->string || !elem->string[0])
     return (str);

  return (const char*)elem->string;
}

/*
 * Print message and abort program
 */
static void Abort (const char *s1, const char *s2)
{
  if (lang.line > 0)
  {
#ifdef __DJGPP__
    (*_printf) ("\r\nline %lu: ", lang.line);
#else
    char buf [15];
    ltoa (lang.line, buf, 10);
    outs ("\r\nline ");
    outs (buf);
    outs (": ");
#endif
  }
  else
    outs ("\r\n");

  outs (s1);
  if (s2)
     outsnl (s2);
  outs ("\r\n");
  exit (1);
}

/*
 * Print error message for current line
 */
static void error (const char *err)
{
#ifdef __DJGPP__
  (*_printf) ("%s at line %lu\r\n", err, lang.line);
#else
  char buf [15];
  ltoa (lang.line, buf, 10);
  outs (err);
  outs (" at line ");
  outs (buf);
  outs ("\r\n");
#endif
}

#if defined(TEST_PROG)
int main (void)
{
  puts ("You must define a non-english language to parse language-file.\n"
        "Use e.g: \"language = no,$(ETC)\\watlang.txt\" in WATTCP.CFG\n"
        "Press any key to initialise Watt-32.");
  getch();
  sock_init();
  return (0);
}
#endif  /* TEST_PROG    */
#endif  /* USE_LANGUAGE */
