.func localeconv
#include <locale.h>
struct lconv *localeconv( void );
.ixfunc2 '&Locale' &func
.funcend
.desc begin
The &func function sets the components of an object of type
.kw struct lconv
with values appropriate for the formatting of numeric quantities
according to the current locale.
The components of the
.kw struct lconv
and their meanings are as follows:
.begnote $setptnt 12
.termhd1 Component
.termhd2 Meaning
.note char *decimal_point
The decimal-point character used to format non-monetary quantities.
.note char *thousands_sep
The character used to separate groups of digits to the left of the
decimal-point character in formatted non-monetary quantities.
.if '&machsys' ne 'QNX' .do begin
.* QNX version is described below
.note char *grouping
A string whose elements indicate the size of each group of digits
in formatted non-monetary quantities.
.do end
.note char *int_curr_symbol
The international currency symbol applicable to the current locale.
The first three characters contain the alphabetic international currency
symbol in accordance with those specified in
.us ISO 4217 Codes for the Representation of Currency and Funds.
The fourth character (immediately preceding the null character) is the
character used to separate the international currency symbol from the
monetary quantity.
.note char *currency_symbol
The local currency symbol applicable to the current locale.
.note char *mon_decimal_point
The decimal-point character used to format monetary quantities.
.note char *mon_thousands_sep
The character used to separate groups of digits to the left of the
decimal-point character in formatted monetary quantities.
.note char *mon_grouping
A string whose elements indicate the size of each group of digits
in formatted monetary quantities.
.if '&machsys' eq 'QNX' .do begin
.* DOS version is described above
.note char *grouping
A string whose elements indicate the size of each group of digits
in formatted non-monetary quantities.
.do end
.note char *positive_sign
The string used to indicate a nonnegative-valued monetary quantity.
.note char *negative_sign
The string used to indicate a negative-valued monetary quantity.
.note char int_frac_digits
The number of fractional digits (those to the right of the decimal-point)
to be displayed in an internationally formatted monetary quantity.
.note char frac_digits
The number of fractional digits (those to the right of the decimal-point)
to be displayed in a formatted monetary quantity.
.note char p_cs_precedes
Set to 1 or 0 if the
.kw currency_symbol
respectively precedes or follows the value for a nonnegative
formatted monetary quantity.
.note char p_sep_by_space
Set to 1 or 0 if the
.kw currency_symbol
respectively is or is not separated by a space from the value for a
nonnegative formatted monetary quantity.
.note char n_cs_precedes
Set to 1 or 0 if the
.kw currency_symbol
respectively precedes or follows the value for a negative
formatted monetary quantity.
.note char n_sep_by_space
Set to 1 or 0 if the
.kw currency_symbol
respectively is or is not separated by a space from the value for a
negative formatted monetary quantity.
.note char p_sign_posn
The position of the
.kw positive_sign
for a nonnegative formatted monetary quantity.
.note char n_sign_posn
The position of the
.kw positive_sign
for a negative formatted monetary quantity.
.endnote
.pp
The elements of
.kw grouping
and
.kw mon_grouping
are interpreted according to the following:
.begnote $setptnt 12
.termhd1 Value
.termhd2 Meaning
.note CHAR_MAX
.ix 'CHAR_MAX'
No further grouping is to be performed.
.note 0
The previous element is to be repeatedly used for the remainder of
the digits.
.note other
The value is the number of digits that comprise the current group.
The next element is examined to determine the size of the next group
of digits to the left of the current group.
.endnote
.pp
The value of
.kw p_sign_posn
and
.kw n_sign_posn
is interpreted as follows:
.begnote
.termhd1 Value
.termhd2 Meaning
.note 0
Parentheses surround the quantity and
.kw currency_symbol
.ct .li .
.note 1
The sign string precedes the quantity and
.kw currency_symbol
.ct .li .
.note 2
The sign string follows the quantity and
.kw currency_symbol
.ct .li .
.note 3
The sign string immediately precedes the quantity and
.kw currency_symbol
.ct .li .
.note 4
The sign string immediately follows the quantity and
.kw currency_symbol
.ct .li .
.endnote
.desc end
.return begin
The &func function returns a pointer to the filled-in object.
.return end
.see begin
.seelist localeconv setlocale
.see end
.exmp begin
#include <stdio.h>
#include <locale.h>

void main()
  {
    struct lconv *lc;
.exmp break
    lc = localeconv();
    printf( "*decimal_point (%s)\n",
        lc->decimal_point );
.exmp break
    printf( "*thousands_sep (%s)\n",
        lc->thousands_sep );
.exmp break
    printf( "*int_curr_symbol (%s)\n",
        lc->int_curr_symbol );
.exmp break
    printf( "*currency_symbol (%s)\n",
        lc->currency_symbol );
.exmp break
    printf( "*mon_decimal_point (%s)\n",
        lc->mon_decimal_point );
.exmp break
    printf( "*mon_thousands_sep (%s)\n",
        lc->mon_thousands_sep );
.exmp break
    printf( "*mon_grouping (%s)\n",
        lc->mon_grouping );
.exmp break
    printf( "*grouping (%s)\n",
        lc->grouping );
.exmp break
    printf( "*positive_sign (%s)\n",
        lc->positive_sign );
.exmp break
    printf( "*negative_sign (%s)\n",
        lc->negative_sign );
.exmp break
    printf( "int_frac_digits (%d)\n",
        lc->int_frac_digits );
.exmp break
    printf( "frac_digits (%d)\n",
        lc->frac_digits );
.exmp break
    printf( "p_cs_precedes (%d)\n",
        lc->p_cs_precedes );
.exmp break
    printf( "p_sep_by_space (%d)\n",
        lc->p_sep_by_space );
.exmp break
    printf( "n_cs_precedes (%d)\n",
        lc->n_cs_precedes );
.exmp break
    printf( "n_sep_by_space (%d)\n",
        lc->n_sep_by_space );
.exmp break
    printf( "p_sign_posn (%d)\n",
        lc->p_sign_posn );
.exmp break
    printf( "n_sign_posn (%d)\n",
        lc->n_sign_posn );
  }
.exmp end
.class ANSI
.system
