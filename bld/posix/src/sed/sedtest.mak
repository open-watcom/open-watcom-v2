# testsed.mak
#
# Makefile to test sed. Portable among NT, OS/2 and UNIX

!include cproj.mif

!ifndef __UNIX__
blackhole=nul
q=" # quote for shell
E=  # quote for echo - can not use Q as wmake macros are case insensitive
here=.\
!else
blackhole=/dev/null
q='
E='
here=./
!endif

!ifdef %TESTSED
sed = $(%TESTSED)
!else
!ifndef __UNIX__
sed = sed # $(here)sed here is .\sed but in sed itself .\sed is .sed. ;(
!else
sed = $(here)sed
!endif
!endif

# Shorthands for "standard" operations
axdiff =afore     2>> stray | diff xpect -
a2xdiff=afore 2>&1 >> stray | diff xpect -
aadiff =afore     2>> stray | diff afore -

.before
    @rm -f stray
    @%create stray

.after
    # stray should be empty
    @diff stray $(blackhole) > $(blackhole) || echo Some stray output was written
    @diff stray $(blackhole) | $(sed) $(q)1d;s/..//$q
    @diff stray $(blackhole) > $(blackhole)
    @rm -f stray

all: .symbolic sedcomp sed susv3 bre dospencer

dospencer: .symbolic ./spencer.cmd
    $]@ 2>&1 | tee spencer.log
    # spencer.log should be empty
    @diff spencer.log $(blackhole) > $(blackhole)

sedcomp: .symbolic main
    @%null

sed: .symbolic execute
    @%null

clean: .symbolic
    rm -f afore after xpect scrip scr0p scr1p
    rm -f spencer.cmd spencer.log spen?.??? listto* a b c d e f g h i written

main: .symbolic badarg enarg fgarg noef badsquig labels resolve compile recomp
    @%null

badarg: .symbolic noargs
    > afore echo hello
    > xpect echo sed: error processing: -e
    $(sed) -e < $(a2xdiff)
    > xpect echo sed: error processing: -f
    $(sed) -f < $(a2xdiff)
    > xpect cat <<
sed: unknown flag ?
Usage: sed [-g] [-n] script file ...
       sed [-g] [-n] -e script ... -f script_file ... file ...

  -g: global substitute             -n: no default copy,
  script: append to commands,       script_file: append contents to commands
  file: a text file - lines of upto 8K bytes, \n not needed on last line
        default is to read from standard input

General command format:
    [addr1][,addr2] [!]function [arguments]
    [addr1][,addr2] [!]s/[RE]/replace/[p|P][g][w file]

Command list:
    a: append,      b: branch,    c: change,   d/D: delete,
    g/G: get,       h/H: hold,    i: insert,   l: list,
    n/N: next,      p/P: print,   q: quit,     r: read,
    s: substitute,  t/T: test,    w/W: write,  x: xchange,
    y: translate,   :label,       =: number,   {}: grouping

Regular expression elements:
    \n, and \t: newline, and tab
    ^: line begin,             $$: line end,
    [a-z$$0-9]: class,          [^a-z$$]: inverse-class,
    .: one character,          *: 0 or more repeats,
    +: one or more repeats,    \{n,m\} n to m repeats,
    \(..\): subexpression,     \1..\9: matched subexpression,

Substitution patterns:
    \n,\t: newline and tab
    &: match pattern,          \1..\9: matched subexpression
<<
    $(sed) -?    < $(a2xdiff)

# Response very similar to long "unknown flag" response above.
noargs: .symbolic
   -$(sed) 2> after

enarg: .symbolic
    > afore echo hello
    $(sed) -e p -n $(aadiff)

fgarg: .symbolic
    > afore echo hello
    > xpect echo heLlo
    >>xpect echo heLLo
    > scrip echo s/l/L/
    # -g only applies to subsequent arguments
    $(sed) -e "s/l/L/p;s/L/l/" -g -f scrip $(axdiff)

noef: .symbolic
    > afore echo hello
    $(sed) -n p  $(aadiff)
    $(sed) -e "" $(aadiff)

badsquig: .symbolic
    > afore echo hello
    %write xpect sed: too many }'s
    $(sed) -e "}"  $(a2xdiff)
    %write xpect sed: too many {'s
    $(sed) -e "{p" $(a2xdiff)

labels: .symbolic
    # labels can be unused or duplicated.
    # One can jump forward to a defined label, forward to the end of the script,
    # forward to a missing label or backward.
    # unused
    > afore echo hello
    $(sed) -e :label -e p -n                         $(aadiff)
    # duplicated
    > xpect echo $(e)sed: duplicate label ":label"$e
    $(sed) -e :label -e :label                       $(a2xdiff)
    # forward to a defined label
    $(sed) -neblabel "-ep;:label" -ep $(aadiff)
    # forward to the end of the script
    $(sed) -eb -ep                                   $(aadiff)
    # forward to a missing label
    > xpect echo $(e)sed: undefined label ":label"$e
    $(sed) -eblabel                                  $(a2xdiff)
    # backward
    $(sed) -eblabel1 -e:label "-eq;:label1" -eblabel $(aadiff)

compile: .symbolic semicolon AGMSG emptyre TRAIL cmdcomp cmdline address
    @%null

semicolon: .symbolic
    > afore echo hello
    $(sed) -e "$# This is a comment" -e "; p" -n $(aadiff)

AGMSG: .symbolic
    > afore echo hello
    # "$Esed:" does not mean "$(E)sed:" in wmake. Probably means "$(ESED):".
    > xpect echo $(E)sed: garbled address /\)/p$E
    $(sed) -e "/\)/p" $(a2xdiff)

dummy: .symbolic
    $(sed) -e //p                               $(a2xdiff)

emptyre: .symbolic
    > afore echo hello
    > xpect echo sed: first RE must be non-null
    $(sed) -e //p                               $(a2xdiff)
    $(sed) -e /hello/!d -e //p -n               $(aadiff)
    $(sed) -e 1!d -e //p -n                     $(a2xdiff)
    # last regular expression is dynamic rather than analysable at compile time
    $(sed) -n -e b1 -e:0 -e "//p;q;:1" -e /h/b0 $(aadiff)
    $(sed) -n -e $(q)$$p$q                      $(aadiff)
    >>afore echo world
    $(sed) -n -e "1,2 p"                        $(aadiff)
    # Curiously, this scenario should not give a diagnostic WFB 20040815
    > xpect echo world
    $(sed) -n -e "2,1 p"                        $(axdiff)
    $(sed) -n -e "1;2p"                         $(aadiff)
    > xpect echo $(E)sed: garbled address 1;p$E
    $(sed) -n -e "1;p"                          $(a2xdiff)
    > xpect echo hello
    $(sed) -n -e " $$ ! p"                      $(axdiff)

TRAIL: .symbolic
    > afore echo hello
    > xpect echo $(e)sed: command "pq" has trailing garbage$e
    $(sed) -e pq                                $(a2xdiff)
    $(sed) -n -e "/foo/d;p"                     $(aadiff)
    $(sed) -n -e "/foo/d;p$# This is a comment" $(aadiff)

cmdcomp: .symbolic brace equals q label branch airc D s l wW y
    > afore echo hello # Quick test of all commands
    $(sed) -e "q;{p;};=;:label" -e b -e t -e T -e "a;i;r;c;D;s/a/a/l;w;W;y/a/b/" $(aadiff)

brace: .symbolic
    > afore echo hello
    >>afore echo world
    $(sed) -n "{p;}"             $(aadiff)
    $(sed) -n -e "{" -e p -e "}" $(aadiff)
    > xpect echo hello
    $(sed) -n "1{p;}"            $(axdiff)
    $(sed) -n "1,2!{p;};p"       $(aadiff)
    > xpect echo world
    $(sed) -n "1!{p;}"           $(axdiff)
    > xpect echo sed: no addresses allowed for 1}
    $(sed) -n "1}"               $(a2xdiff)
    %write xpect sed: too many }'s
    $(sed) -n "}"                $(a2xdiff)

equals: .symbolic
    > afore echo hello
    >>afore echo world
    > xpect echo 1
    >>xpect echo 2
    $(sed) -n /h/,/l/= $(axdiff)

q: .symbolic
    > afore echo hello
    >>afore echo world
    > xpect echo sed: only one address allowed for /h/,/l/q
    $(sed) /h/,/l/q          $(a2xdiff)
    > xpect echo hello
    $(sed) /h/q              $(axdiff)

label: .symbolic gettext
    > afore echo hello
    > xpect echo sed: no addresses allowed for 1:label
    $(sed) 1:label             $(a2xdiff)
    > xpect echo $(E)sed: duplicate label ":label"$E
    $(sed) -e :label -e :label $(a2xdiff)
    $(sed) :label              $(aadiff)

branch: .symbolic
    > afore echo hello
    $(sed) -n -e "/d/ blabel" -e "p;:label" $(aadiff)
    $(sed) -e "/d/ ! blabel"  -e "p;:label" $(aadiff)

airc: .symbolic
    > afore echo hello
    > xpect echo sed: only one address allowed for 1,2a
    # 1addr on air seems restrictive but complies with SUSV3 WFB 20040731
    $(sed) 1,2a     $(a2xdiff)
    > xpect echo sed: only one address allowed for 1,2i
    $(sed) 1,2i     $(a2xdiff)
    > xpect echo sed: only one address allowed for 1,2r
    $(sed) 1,2r     $(a2xdiff)
    # Impossibly? difficult to do multi-line -e command portably WFB 20040731
    > scrip echo $(e)c\$e
    >>scrip echo world
    > xpect echo world
    $(sed) -f scrip $(axdiff)

D: .symbolic
    > afore echo hello
    >>afore echo world
    > xpect echo world
    $(sed) "N;D" $(axdiff)

s: .symbolic rhscomp badshare
    > afore echo hello
    > xpect echo $(E)sed: garbled command s/\)//$E
    $(sed) $(q)s/\)//$q    $(a2xdiff)
    > xpect echo $(E)sed: garbled command s/a/\1/$E
    $(sed) $(q)s/a/\1/$q   $(a2xdiff)
    > xpect echo heLLo
    $(sed) -g s/l/L/       $(axdiff)
    $(sed) s/l/L/g         $(axdiff)
    >>afore echo world
    > xpect echo heLlo
    $(sed) -n "N;s/l/L/P"  $(axdiff)
    >>xpect echo world
    $(sed) -n "N;s/l/L/p"  $(axdiff)
    > xpect echo heLlo
    >>xpect echo world
    >>xpect echo heLlo
    >>xpect echo world
    $(sed) -n "N;s/l/L/pw" $(axdiff)
    # Seems strange that s supports w flag but not W WFB 20040803
    %write xpect sed: command "N;s/l/L/W" has trailing garbage
    $(sed) -n "N;s/l/L/W"  $(a2xdiff)
    > xpect echo heLlo
    >>xpect echo heLlo
    >>xpect echo world
    $(sed) -n "N;s/l/L/Pw" $(axdiff)
    > xpect echo helLo
    $(sed) -n s/l/L/2p     $(axdiff)
    > xpect echo sed: bad value for match count on s command s/l/L/0
    $(sed) s/l/L/0         $(a2xdiff)
    > xpect echo sed: bad value for match count on s command s/l/L/2048
    $(sed) s/l/L/2047      $(aadiff)
    $(sed) s/l/L/2048      $(a2xdiff)

badshare: .symbolic
!ifndef __UNIX__
    # fails with share conflict between stdout redirection and file WFB 20040803
    > xpect echo sed: cannot create after
   -$(sed) -n "N;s/l/L/pwafter" afore 2> after
    diff after xpect
!else
    @%null
!endif

l: .symbolic
    > afore echo hello
    >>afore echo world
    > xpect echo hello
    >>xpect echo world$$
    $(sed) -n "N;l"       $(axdiff)
    $(sed) -n "N;lwafter" afore
    diff after xpect

wW: .symbolic
    > afore echo hello
    >>afore echo world
    > xpect echo sed: too many w files
    rm -f a b c d e f g h i
    $(sed) -n -e w -e wa -e wb -e wc -e wd -e we -e wf -e wg -e wh -e wi -e Wj $(a2xdiff)
    > xpect echo hello
    $(sed) -n "N;Wafter" afore && diff after xpect
    >>xpect echo world
    $(sed) -n "N;wafter" afore && diff after xpect

y: .symbolic ycomp
    > afore echo hello
    > xpect echo sed: garbled command y/abcdefghijklmnopqrstuvwxyz/A-Z/
    $(sed) y/abcdefghijklmnopqrstuvwxyz/A-Z/                        $(a2xdiff)
    > xpect echo HELLO
    $(sed) y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/ $(axdiff)

rhscomp: .symbolic
    > afore echo hello
    > xpect echo $(E)sed: garbled command s/l/\1/$E
    $(sed) $(q)s/l/\1/$q after $(a2xdiff)
    > xpect echo helo
    $(sed) $(q)s/\(l\)\1/\1/$q $(axdiff)
    > xpect echo sed: garbled command s/l/l
    $(sed) s/l/l               $(a2xdiff)

recomp: .symbolic starplusdol set handle_cket listto.exe
    > afore echo hello
    > xpect echo heLo
    $(sed) /ll*/s//L/        $(axdiff)
    $(sed) /l+/s//L/         $(axdiff)
    > xpect echo ello
    $(sed) s/^h//            $(axdiff)
    > xpect echo $(e)sed: garbled command s/\(h//$e
    $(sed) "s/\(h//"         $(a2xdiff)
    > afore echo hello world
    > xpect echo row ollehld
    $(sed) "s/\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)/\9\8\7\6\5\4\3\2\1/" $(axdiff)
    > xpect echo row ollehd
    $(sed) "s/\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)\(.\)/\9\8\7\6\5\4\3\2\1/" $(axdiff)
    > xpect echo $(e)sed: garbled command s/\(.\)\)//$e
    $(sed) "s/\(.\)\)//"     $(a2xdiff)
    > xpect echo $(e)sed: garbled command s/\(.\)/\2/$e
    $(sed) "s/\(.\)/\2/"     $(a2xdiff)
    > xpect echo $(e)sed: garbled address /\{1,2\}/p$e
    $(sed) "/\{1,2\}/p"      $(a2xdiff)
    > xpect echo $(e)sed: garbled address /.\{,2\}/p$e
    $(sed) "/.\{,2\}/p"      $(a2xdiff)
    > xpect echo $(e)sed: garbled address /.\{256\}/p$e
    $(sed) "/.\{256\}/p"     $(a2xdiff)
    > xpect echo $(e)sed: garbled address /.\{1,256\}/p$e
    $(sed) "/.\{1,256\}/p"   $(a2xdiff)
    > xpect echo hello world
    $(sed) "/.\{255\}!/p"    $(aadiff)
    $(sed) "/.\{255,\}!/p"   $(aadiff)
    > xpect echo $(e)sed: garbled address /.\{255,2a\}!/p$e
    $(sed) "/.\{255,2a\}!/p" $(a2xdiff)
    > xpect echo $(e)sed: garbled address /.\{2,1\}/p$e
    $(sed) "/.\{2,1\}/p"     $(a2xdiff)
    > xpect echo $(e)sed: garbled address /.\{2a\}/p$e
    $(sed) "/.\{2a\}/p"      $(a2xdiff)
    > xpect $(here)listto.exe  $(q)sed: garbled command s/h\\\nello//\n$q
    > scrip echo $(e)s/h\$e
    >>scrip echo ello//
    $(sed) -f scrip               $(a2xdiff)
    # Some editors turn tab into a sequence of spaces
    > afore $(here)listto.exe $(q)hello\nwor\tld$q
    > xpect echo hello world
    $(sed) "N;s/o\nw/o w/;s/\t//" $(axdiff)
    > xpect echo $(e)sed: garbled address /h$e
    > scrip echo /h
    $(sed) -f scrip               $(a2xdiff)
    > afore echo hello
    >>afore echo world
    > xpect echo hello world
    $(sed) "N;s/o.w/o w/"         $(axdiff)
    > afore echo $(e)\hello$e
    > xpect echo $(e)sed: garbled address /\h/p$e
    $(sed) -n "/\h/p"             $(aadiff) # \<non-special> is erroneous in IEEE, <non-special> here
    $(sed) -n "/\\h/p"            $(aadiff)

starplusdol: .symbolic
    > afore echo ++aa
    > xpect echo hello world
    $(sed) -f <<    $(axdiff)
s/++a+/hello world/
<<
    > xpect echo $(e)sed: garbled command s/\(+a+/hello world/$e
    $(sed) $(q)s/\(+a+/hello world/$q $(a2xdiff)
    > afore echo $(e)**$e
    > xpect echo hello world
    $(sed) "s/**a*/hello world/"      $(axdiff)
    > xpect echo $(e)sed: garbled command s/\(*a+/hello world/$e
    $(sed) "s/\(*a+/hello world/"     $(a2xdiff)
    > xpect echo *hello world
    $(sed) "s/*$$/hello world/"       $(axdiff)

set: .symbolic listto.exe
    > afore echo hello
    > xpect echo haaaa
    $(sed) "s/[^h]/a/g"       $(axdiff)
    > xpect echo sed: garbled command /[h
    $(sed) /[h                $(a2xdiff)
    > afore echo h-llo
    > xpect echo haaao
    $(sed) s/[-l]/a/g         $(axdiff)
    > afore echo hello
    > xpect echo aaaao
    $(sed) s/[e-l]/a/g        $(axdiff)
    > xpect echo sed: garbled command s/[l-e]/a/g
    $(sed) s/[l-e]/a/g        $(a2xdiff)
    > xpect echo sed: garbled command /[e-l-o]/p
    $(sed) -n /[e-l-o]/p      $(a2xdiff)
    # fgets() quietly swallows \r on NT.
    > afore $(here)listto.exe $(q)hello\nwor\tld$q
    > xpect echo halloaworald
    $(sed) "N;s/[\e\n\t]/a/g" $(axdiff)
    > afore echo h]llo
    > xpect echo a]aaa
    $(sed) "s/[^]]/a/g"       $(axdiff)
    $(sed) -n /[[:alpha:]]/p  $(aadiff)
    > xpect echo sed: invalid character class name /[[:foo:]]/p
    $(sed) /[[:foo:]]/p       $(a2xdiff)

handle_cket: .symbolic
    > afore echo hello
    $(sed) -n /a*/p                 $(aadiff) # CCHR
    $(sed) -n /.*/p                 $(aadiff) # CDOT
    $(sed) -n /[aeiou]*/p           $(aadiff) # CCL
    $(sed) -n /^*/!p                $(aadiff) # * is literal after special ^
    $(sed) -n $q/$$*/p$q            $(aadiff) # $ is literal if not last
    > xpect echo $(e)sed: garbled address /\(*\)/p$e
    $(sed) -n $q/\(*\)/p$q          $(a2xdiff)       # CBRA
    $(sed) -n $q/\([aeiou]\)*/p$q   $(aadiff) # CKET
    $(sed) -n $q/\([aeiou]\)\1*/p$q $(aadiff) # CBACK
    > xpect echo sed: no such command as 1*p
    $(sed) -n 1*p                   $(a2xdiff) # * after line number is not special
    # By definition, CEND* can't happen CEND is always followed by CEOF
    # By definition, CEOF* can't happen

cmdline: .symbolic
    > afore echo hello
    > xpect echo sed: error processing: -e
    $(sed) -e      <    $(a2xdiff)
    > xpect $(here)listto.exe $(q)sed: garbled command s/h/b\\\n\n$q
    # This used to seize. WFB 20040803
    $(sed) -f <<        $(a2xdiff)
s/h/b\
<<
    # This used to seize. WFB 20040813
    > xpect echo bello
    $(sed) $(q)s/h/b/$q $(axdiff)

address: .symbolic
    > afore echo hello
    $(sed) -n $(q)$$p$q       $(aadiff)
    $(sed) -n $(q)$$p$q afore $(aadiff)
    $(sed) -n $(q)\hehp$q     $(aadiff)
    $(sed) -n /e/p            $(aadiff)
    $(sed) -n 1p              $(aadiff)
    $(sed) -n p               $(aadiff)

gettext: .symbolic listto.exe
    > afore echo hello
    > scrip $(here)listto.exe $(q)a\\\n    world\n$q
    > xpect echo hello
    >>xpect echo     world
    $(sed) -f scrip $(axdiff)
    > scrip $(here)listto.exe $(q)a\\\n     w\\or \\\\ ld\\\nuniverse\n$q
    > xpect $(here)listto.exe $(q)hello\n     wor \\ ld\nuniverse\n$q
    $(sed) -f scrip $(axdiff)

resolve: .symbolic
    > afore echo hello
    > xpect echo $(e)sed: undefined label ":label"$e
    $(sed) blabel              $(a2xdiff)
    $(sed) -e blabel -e :label $(aadiff)

ycomp: .symbolic
    > afore echo $(e)he\lo$e
    > scrip echo y/h
    > xpect echo sed: garbled command y/h
    $(sed) -f scrip                 $(a2xdiff)
    > xpect echo abclo
    $(sed) $(q)y/he\\/abc/$q        $(axdiff)
    > afore echo hello
    >>afore echo world
    > xpect echo hello world
    # "If a backslash followed by an 'n' appear in string1 or string2,
    # the two characters shall be handled as a single <newline>."
    $(sed) "N;y/\n/ /"              $(axdiff)
    > afore echo hello
    > xpect echo h
    >>xpect echo llo
    $(sed) $(q)y/e/\n/$q            $(axdiff)
    # "If the number of characters in string1 and string2 are not equal,
    # or if any of the characters in string1 appear more than once,
    # the results are undefined."
    > afore echo hello
    > xpect echo sed: garbled command y/a//
    $(sed) y/a//                    $(a2xdiff)
    > xpect echo sed: garbled command y//a/
    $(sed) y//a/                    $(a2xdiff)
    > xpect echo sed: garbled command y/hh/el/
    $(sed) y/hh/el/                 $(a2xdiff)
    # Any character other than backslash or <newline>
    # can be used instead of slash to delimit the strings.
    > xpect echo HELLO
    $(sed) yaheloaHELOa             $(axdiff)
    > xpect $(here)listto.exe $(q)sed: garbled command y\\helo\\HELO\\\n\n$q
    $(sed) -f <<                    $(a2xdiff)
y\helo\HELO\
<<
    > xpect $(here)listto.exe $(q)sed: garbled command y\\\\helo\\\\HELO\\\\\n$q
    # >>xpect echo afore
    $(sed) -f <<                    $(a2xdiff)
y\\helo\\HELO\\
<<
    > xpect echo sed: garbled command y
    # "If the delimiter is not 'n' , within string1 and string2,
    # the delimiter itself can be used as a literal character
    # if it is preceded by a backslash."
    # [ "If the delimiter is not 'n'" because \n has a special meaning.]
    > xpect echo HELLh
    $(sed) $(q)yh\helohHEL\hh$q     $(axdiff)
    # "If a backslash character is immediately followed by a backslash
    # character in string1 or string2, the two backslash characters shall be
    # counted as a single literal backslash character. The meaning of a
    # backslash followed by any character that is not 'n' , a backslash, or the
    # delimiter character is undefined."
    >>afore echo $(e)w\rld$e
    > xpect echo h
    >>xpect echo $(e)llo wo\ld$e
    $(sed) $(q)N;y/\\r\ne/o\\ \n/$q $(axdiff)
    > xpect echo $(e)sed: garbled command yh\h\?elohHEL\hh$e
    $(sed) $(q)yh\h\?elohHEL\hh$q   $(a2xdiff)
    > xpect echo $(e)sed: garbled command yh\helohH\?L\hh$e
    $(sed) $(q)yh\helohH\?L\hh$q    $(a2xdiff)
    > xpect echo sed: garbled command y
    > scrip echo y
    >>scrip echo helo
    >>scrip echo HELO
    $(sed) -f scrip                 $(a2xdiff)
    > xpect echo sed: garbled command y/a/
    $(sed) y/a/                     $(a2xdiff)
    > xpect echo sed: garbled command y/a/b
    $(sed) y/a/b                    $(a2xdiff)
    > xpect echo $(e)sed: garbled command y/\h/\h/$e
    $(sed) $(q)y/\h/\h/$q           $(a2xdiff)

execute: .symbolic selected command match
    > afore echo hello
    %write xpect sed: can't open nosuch
    $(sed) -n p nosuch                 afore 2>&1 > after | diff xpect -
    diff afore after
    > xpect echo hello hello
    $(sed) -n $(q)N;$$s/\n/ /p$q afore $(axdiff)
    >>afore echo world
    > xpect echo world
    $(sed) "N;D"                       $(axdiff)
    $(sed) -e b -e p                   $(aadiff)
    $(sed) -e blab -e "p;:lab"         $(aadiff)
    > afore echo hello
    > scrip echo $(e)a\$e
    >>scrip echo world
    > xpect echo hello
    >>xpect echo world
    $(sed) -f scrip                    $(axdiff)

selected: .symbolic
    > afore echo hello
    $(sed) -n $(q)1{p;}$q  $(aadiff)
    >>afore echo world
    > xpect echo world
    $(sed) -n $(q)1!{p;}$q $(axdiff)
    $(sed) -n $(q)$$p$q    $(axdiff)
    >>afore echo universe
    $(sed) -n 2,2p         $(axdiff)
    $(sed) -n /world/,2p   $(axdiff)

match: .symbolic advance
    > afore echo hello
    > xpect echo heLLo
    $(sed) -g s/l/L/ $(axdiff)
    > xpect echo heLlo
    $(sed) s/l/L/    $(axdiff)
    > xpect echo hellO
    $(sed) s/./O/5   $(axdiff)

advance: .symbolic mtype memeql
    > afore echo hello
    > xpect echo Hello
    $(sed) s/h/H/                $(axdiff) # CCHR
    $(sed) s/./H/                $(axdiff) # CDOT
    >>afore echo world
    > xpect echo HHHHHHHHHHH
    $(sed) "N;s/./H/g"           $(axdiff) # CDOT, CDOT | STAR # . should match \n as well as other characters
    > afore echo heho
    >>afore echo horld
    > xpect echo Hehonhorld
    $(sed) "N;s/^h/H/g;s/\n/n/"  $(axdiff) # CNL
    > afore echo delld
    >>afore echo hodld
    > xpect echo delldnhodlD
    $(sed) "N;s/d$$/D/g;s/\n/n/" $(axdiff) # CDOL
    > afore echo hello
    > xpect echo hellO
    $(sed) s/o/O/                $(axdiff) # CEOF
    $(sed) "s/[o][o]*/O/"        $(axdiff) # CCL, CCL | STAR, star
    $(sed) "s/\(o\)/O/"          $(axdiff) # CBRA, CKET
    > xpect echo helo
    $(sed) "s/\(l\)\1/\1/"       $(axdiff) # CBACK
    $(sed) "s/\(o\)\1/\1/"       $(aadiff) # CBACK
    $(sed) "s/\(l\)\1/\1/"       $(axdiff) # CBACK
    > afore echo abababg
    > xpect echo abg
    $(sed) "s/\(..\)\1*/\1/"     $(axdiff) # CBACK | STAR
    $(sed) -n "/\(c*\)\1*/p"     $(aadiff) # CBACK | STAR - zero width \1
    # \(...\)* sets \n to last match
    > afore echo abcdefg
    > xpect echo efg
    $(sed) "s/\(..\)*/\1/"       $(axdiff) # CBRA | STAR, CKET | STAR
    $(sed) -n "/\(foo\)*/p"      $(aadiff) # CBRA | STAR, CKET | STAR
    $(sed) -n "/\(foo\)*\1*/p"   $(aadiff) # CBRA | STAR, CKET | STAR

mtype: .symbolic
    > afore echo aaaaaaa
    > xpect echo b
    $(sed) -n "s/a\{0,\}/b/p"        $(axdiff) # CCHR | MTYPE
    $(sed) -n "s/a\{1,\}/b/p"        $(axdiff)
    > xpect echo baaaaaa
    $(sed) -n "s/a\{1,1\}/b/p"       $(axdiff)
    > xpect echo baa
    $(sed) -n "s/a\{3,5\}/b/p"       $(axdiff)
    > xpect echo b
    $(sed) -n "s/a\{3,8\}/b/p"       $(axdiff)
    $(sed) -n "/a\{8,\}/!p"          $(aadiff)
    > xpect echo baaaaaaa
    $(sed) -n "s/b\{0,\}/b/p"        $(axdiff)
    > afore echo abcdefg
    > xpect echo b
    $(sed) -n "s/.\{0,\}/b/p"        $(axdiff) # CDOT | MTYPE
    $(sed) -n "s/.\{1,\}/b/p"        $(axdiff)
    > xpect echo bbcdefg
    $(sed) -n "s/.\{1,1\}/b/p"       $(axdiff)
    > xpect echo bfg
    $(sed) -n "s/.\{3,5\}/b/p"       $(axdiff)
    > xpect echo b
    $(sed) -n "s/.\{3,8\}/b/p"       $(axdiff)
    $(sed) -n "/.\{8,\}/!p"          $(aadiff)
    > xpect echo b
    $(sed) -n "s/.\{0,\}/b/p"        $(axdiff)
    > afore echo abcdefg
    > xpect echo b
    $(sed) -n "s/[a-g]\{0,\}/b/p"    $(axdiff) # CCL | MTYPE
    $(sed) -n "s/[a-g]\{1,\}/b/p"    $(axdiff)
    > xpect echo bbcdefg
    $(sed) -n "s/[a-g]\{1,1\}/b/p"   $(axdiff)
    > xpect echo bfg
    $(sed) -n "s/[a-g]\{3,5\}/b/p"   $(axdiff)
    > xpect echo b
    $(sed) -n "s/[a-g]\{3,8\}/b/p"   $(axdiff)
    $(sed) -n "/[a-g]\{8,\}/!p"      $(aadiff)
    > xpect echo b
    $(sed) -n "s/[a-g]\{0,\}/b/p"    $(axdiff)
    > afore echo aaaaaaa
    > xpect echo b
    $(sed) -n "s/\(.\)\1\{0,\}/b/p"  $(axdiff) # CBACK | MTYPE
    $(sed) -n "s/\(.\)\1\{1,\}/b/p"  $(axdiff)
    > xpect echo baaaaa
    $(sed) -n "s/\(.\)\1\{1,1\}/b/p" $(axdiff)
    > xpect echo ba
    $(sed) -n "s/\(.\)\1\{3,5\}/b/p" $(axdiff)
    > xpect echo b
    $(sed) -n "s/\(.\)\1\{3,8\}/b/p" $(axdiff)
    $(sed) -n "/\(.\)\1\{8,\}/!p"    $(aadiff)
    > xpect echo b
    $(sed) -n "s/\(.\)\1\{0,\}/b/p"  $(axdiff)
    > afore echo aaaaaa
    > xpect echo b
    $(sed) -n "s/\(.\)\{0,\}/b/p"    $(axdiff) # CKET | MTYPE
    $(sed) -n "s/\(.\)\{1,\}/b/p"    $(axdiff)
    > xpect echo baaaaa
    $(sed) -n "s/\(.\)\{1,1\}/b/p"   $(axdiff)
    > xpect echo ba
    $(sed) -n "s/\(.\)\{3,5\}/b/p"   $(axdiff)
    > xpect echo b
    $(sed) -n "s/\(.\)\{3,8\}/b/p"   $(axdiff)
    $(sed) -n "/\(.\)\{8,\}/!p"      $(aadiff)
    > xpect echo b
    $(sed) -n "s/\(.\)\{0,\}/b/p"    $(axdiff)

substitute: .symbolic dosub
    > afore echo hello
    $(sed) s/a/b/    $(aadiff)
    > xpect echo heLLo
    $(sed) -g s/l/L/ $(axdiff)

dosub: .symbolic place
    > afore echo hello
    > xpect echo hellooa
    $(sed) "s/.*\(.\)/&\1a/" $(axdiff)

place: .symbolic # already tested by dosub
    @%null

listto: .symbolic listto.exe
    # BS gets "Illegal character value 08H in file" from wmake
    # ESC gets "Illegal character value 1bH in file" from wmake
    # CR completes the command list
    > afore $(here)listto.exe $(q)hello TAB\tNL\nline end$q
    > xpect echo $(e)hello TAB\tNL$e
    >>xpect echo $(e)line end$$$e
    $(sed) -n "N;l" $(axdiff)
!ifndef __UNIX__
    > xpect cat <<
\00\00This sed silently ignores data between NUL and NL$$
This sed quietly terminates a line with a decimal 26 character$$
\01\02\03\04\05\06\a\b\t$$
\v\f\0e\0f\10\11\12\13\14\15\16\17\18\19$$
\1b\1c\1d\1e\1f !"$#$$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNO\
PQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\7f\80\81\82\83\
\84\85\86\87\88\89\8a\8b\8c\8d\8e\8f\90\91\92\93\94\95\96\97\98\
\99\9a\9b\9c\9d\9e\9f\a0\a1\a2\a3\a4\a5\a6\a7\a8\a9\aa\ab\ac\ad\
\ae\af\b0\b1\b2\b3\b4\b5\b6\b7\b8\b9\ba\bb\bc\bd\be\bf\c0\c1\c2\
\c3\c4\c5\c6\c7\c8\c9\ca\cb\cc\cd\ce\cf\d0\d1\d2\d3\d4\d5\d6\d7\
\d8\d9\da\db\dc\dd\de\df\e0\e1\e2\e3\e4\e5\e6\e7\e8\e9\ea\eb\ec\
\ed\ee\ef\f0\f1\f2\f3\f4\f5\f6\f7\f8\f9\fa\fb\fc\fd\fe\ff$$
<<
!else
    > xpect cat <<
$$
This sed does not treat a decimal 26 character as special\1a\01\
\02\03\04\05\06\a\b\t$$
\v\f\r\0e\0f\10\11\12\13\14\15\16\17\18\19\1a\1b\1c\1d\1e\1f !"\
$#$$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`\
abcdefghijklmnopqrstuvwxyz{|}~\7f\80\81\82\83\84\85\86\87\88\89\
\8a\8b\8c\8d\8e\8f\90\91\92\93\94\95\96\97\98\99\9a\9b\9c\9d\9e\
\9f\a0\a1\a2\a3\a4\a5\a6\a7\a8\a9\aa\ab\ac\ad\ae\af\b0\b1\b2\b3\
\b4\b5\b6\b7\b8\b9\ba\bb\bc\bd\be\bf\c0\c1\c2\c3\c4\c5\c6\c7\c8\
\c9\ca\cb\cc\cd\ce\cf\d0\d1\d2\d3\d4\d5\d6\d7\d8\d9\da\db\dc\dd\
\de\df\e0\e1\e2\e3\e4\e5\e6\e7\e8\e9\ea\eb\ec\ed\ee\ef\f0\f1\f2\
\f3\f4\f5\f6\f7\f8\f9\fa\fb\fc\fd\fe\ff$$
<<
!endif
    $(here)listto.exe | $(sed) -n l 2>> stray | diff xpect -

listto.exe: $(__MAKEFILES__)
    > listto.c cat <<
$#include <stdio.h>
int main( int argc, char *argv[] ) {
    int i = 0;

    if( argc <= 1 ) {
        putc( i, stdout );
        fprintf( stdout, "%c%s\n", 0, "This sed silently ignores data between NUL and NL" );
        fprintf( stdout, "%s%c", "This sed quietly terminates a line with a decimal 26 character", 26 );
        while( ++i != 256 ) {
            putc( i, stdout );
        }
    } else {
        char const *cp = argv[1];

        while( ( i = *cp++ ) != 0 ) {
            if( i == '\\' ) {
                i = *cp++;
                switch( i ) {
                case 'n': i = '\n'; break;
                case 'r': i = '\r'; break;
                case 't': i = '\t'; break;
                }
            }
            fprintf( stdout, "%c", i );
        }
    }
    return( 0 );
}
<<
!ifdef __UNIX__
    cp listto.c afore
    sed "s/quietly.*character/does not treat a decimal 26 character as special/" afore > listto.c
!endif
    $(bld_cl) listto.c $(bld_clflags) $(bld_ldflags)

command: .symbolic substitute listto readout getline
    > afore echo hello
    > scrip echo $(e)a\$e
    >>scrip echo world
    > xpect echo hello
    >>xpect echo world
    $(sed) -f scrip              $(axdiff)
    > scrip echo $(e)c\$e
    >>scrip echo world
    > xpect echo world
    $(sed) -f scrip              $(axdiff)
    >   afore echo hello
    >>  afore echo world
    > xpect echo world
    $(sed) 1d                    $(axdiff)
    $(sed) "N;D"                 $(axdiff)
    > xpect echo 2
    $(sed) -n $$=                $(axdiff)
    > xpect echo hello
    $(sed) -n "1h;2{g;p;}"       $(axdiff)
    > xpect echo world
    >>xpect echo hello
    $(sed) -n "1h;2{G;p;}"       $(axdiff)
    > xpect echo\
    >>xpect echo hello
    $(sed) -n "1H;2{g;p;}"       $(axdiff)
    >   afore echo hello
    > scrip echo $(e)i\$e
    >>scrip echo world
    > xpect echo world
    >>xpect echo hello
    $(sed) -f scrip              $(axdiff)
    $(sed) b                     $(aadiff)
    > xpect echo $(e)hello$$$e
    $(sed) -n l                  $(axdiff)
    >>  afore echo world
    > scrip echo $(e)a\$e
    >>scrip echo universe
    > xpect echo hello
    >>xpect echo universe
    >>xpect echo world
    $(sed) -f scrip -e n         $(axdiff)
    > xpect echo universe
    >>xpect echo hello
    >>xpect echo world
    $(sed) -f scrip -e N         $(axdiff)
    > xpect echo hello
    $(sed) -n 1p                 $(axdiff)
    $(sed) -n "N;P"              $(axdiff)
    $(sed) q                     $(axdiff)
    > xpect echo hello
    >>xpect echo hello
    >>xpect echo world
    >>xpect echo world
    $(sed) 1rafore               $(axdiff)
    > xpect echo Hello
    >>xpect echo world
    $(sed) s/h/H/                $(axdiff)
    $(sed) -n "N;s/h/H/p"        $(axdiff)
    > xpect echo Hello
    $(sed) -n "N;s/h/H/P"        $(axdiff)
    $(sed) -n s/h/H/wafter       afore
    diff after xpect
    $(sed) -n -e "s/h/H/;T" -e p $(axdiff)
    > xpect echo world
    $(sed) -n -e "s/h/H/;t" -e p $(axdiff)
    > xpect echo hello
    $(sed) -n "N;W"              $(axdiff)
    >>xpect echo world
    $(sed) -n "N;w"              $(axdiff)
    > xpect echo hello
    $(sed) -n $(e)1h;2x;$$p$e    $(axdiff)
    > xpect echo heLLo
    >>xpect echo worLd
    $(sed) y/l/L/                $(axdiff)

getline: .symbolic # already deemed tested WFB 20040802
    @%null

memeql: .symbolic # already deemed tested WFB 20040802
    @%null

readout: .symbolic
    > afore echo hello
    # Test call from main loop
    > scrip echo rwritten
    >>scrip echo $(e)a\$e
    >>scrip echo $(e)append0\$e
    >>scrip echo append1
    >>scrip echo wwritten
    > xpect echo hello
    >>xpect echo hello
    >>xpect echo append0
    >>xpect echo append1
    $(sed) -f scrip      $(axdiff)
    # Call from n command
    >>afore echo world
    > xpect echo hello
    >>xpect echo hello
    >>xpect echo append0
    >>xpect echo append1
    >>xpect echo world
    $(sed) -f scrip -e n $(axdiff)
    # Call from N command
    > xpect echo hello
    >>xpect echo append0
    >>xpect echo append1
    >>xpect echo hello
    >>xpect echo world
    $(sed) -f scrip -e N $(axdiff)
    # Call from q command
    > xpect echo hello
    >>xpect echo hello
    >>xpect echo append0
    >>xpect echo append1
    $(sed) -f scrip -e q $(axdiff)

spen0.sed: $(__MAKEFILES__)
    > spen0.sed cat <<
$# Scrap lines which start $#
/^$#/d
$# ( -> \( and ) -> \)
s/[()]/\\&/g
$# ^0 introduces a matching pattern
s,^0:\(.*\):\(.*\),echo $E\2$E| $(sed) $q/\1/d$q  2>\&1,
$# ^1 introduces a non-matching pattern
s,^1:\(.*\):\(.*\),echo $E\2$E| $(sed) $q/\1/!d$q 2>\&1,
$# ^2 introduces an unacceptable pattern
s,^2:\(.*\):\(.*\),echo $E\2$E| $(sed) $q/\1/h$q  2>$(blackhole),
$# Output the line and the line number and go for the next line
p;=;d
<<

spen0.dat: $(__MAKEFILES__)
    > spen0.dat cat <<
0:abc:abc
1:abc:xbc
1:abc:axc
1:abc:abx
0:abc:xabcy
0:abc:ababc
0:ab*c:abc
0:ab*bc:abc
0:ab*bc:abbc
0:ab*bc:abbbbc
0:ab+bc:abbc
1:ab+bc:abc
1:ab+bc:abq
0:ab+bc:abbbbc
$#0:ab?bc:abbc                          ? is not special in THIS sed.
$#0:ab?bc:abc
1:ab?bc:abbbbc
$#0:ab?c:abc
0:^abc$$:abc
1:^abc$$:abcc
0:^abc:abcc
1:^abc$$:aabc
0:abc$$:aabc
0:^:abc
0:$$:abc
0:a.c:abc
0:a.c:axc
0:a.*c:axyzc
1:a.*c:axyzd
1:a[bc]d:abc
0:a[bc]d:abd
1:a[b-d]e:abd
0:a[b-d]e:ace
0:a[b-d]:aac
0:a[-b]:a-
0:a[b-]:a-
2:a[b-a]:-
2:a[]b:-
2:a[:-
0:a]:a]
0:a[]]b:a]b
0:a[^bc]d:aed
1:a[^bc]d:abd
0:a[^-b]c:adc
1:a[^-b]c:a-c
1:a[^]b]c:a]c
0:a[^]b]c:adc
$#0:ab|cd:abc
$#0:ab|cd:abcd
0:()ef:def
0:()*:-
1:*a:-
$#0:^*:-                                Test wrongly says * special after ^
0:$$*:-
$#1:(*)b:-                              Spurious test; substituted test below
1:$$b:b
2:a\:-
0:a\(b:a(b
$#0:a\(*b:ab
$#0:a\(*b:a((b
1:a\x:a\x
$#1:abc):-                              Spurious test; substituted test below
2:(abc:-
0:((a)):abc
0:(a)b(c):abc
0:a+b+c:aabbabc
0:a**:-
$#0:a*?:-
0:(a*)*:-
0:(a*)+:-
$#0:(a|)*:-                             ^| is not special in THIS sed.
$#0:(a*|b)*:-
$#0:(a+|b)*:ab
$#0:(a+|b)+:ab
$#0:(a+|b)?:ab
0:[^ab]*:cde
0:(^)*:-
$#0:(ab|)*:-
2:)(:-
1:abc:
1:abc:
0:a*:
0:([abc])*d:abbbcd
0:([abc])*bcd:abcd
$#0:a|b|c|d|e:e
$#0:(a|b|c|d|e)f:ef
$#0:((a*|b))*:-
0:abcd*efg:abcdefg
0:ab*:xabyabbbz
0:ab*:xayabbbz
$#0:(ab|cd)e:abcde
0:[abhgefdc]ij:hij
$#1:^(ab|cd)e:abcde
$#0:(abc|)ef:abcdef
$#0:(a|b)c*d:abcd
$#0:(ab|ab*)bc:abc
0:a([bc]*)c*:abc
0:a([bc]*)(c*d):abcd
0:a([bc]+)(c*d):abcd
0:a([bc]*)(c+d):abcd
0:a[bcd]*dcdcde:adcdcde
1:a[bcd]+dcdcde:adcdcde
$#0:(ab|a)b*c:abc
0:((a)(b)c)(d):abcd
0:[A-Za-z_][A-Za-z0-9_]*:alpha
$#0:^a(bc+|b[eh])g|.h$$:abh
$##0:(bc+d$$|ef*g.|h?i(j|k)):effgz
$#0:(bc+d$$|ef*g.|h?i(j|k)):ij
$#1:(bc+d$$|ef*g.|h?i(j|k)):effg
$#1:(bc+d$$|ef*g.|h?i(j|k)):bcdd
$#0:(bc+d$$|ef*g.|h?i(j|k)):reffgz
1:((((((((((a)))))))))):-
0:(((((((((a))))))))):a
1:multiple words of text:uh-uh
0:multiple words:multiple words, yeah
0:(.*)c(.*):abcde
1:\((.*),:(.*)\)
1:[k]:ab
0:abcd:abcd
0:a(bc)d:abcd
$#0:a[^A-^C]?c:a^Bc wmake can't handle SOH, STX or ETX in an inline file
0:(....).*\1:beriberi
2:(*)b:-
2:abc):-
<<

spen1.sed: $(__MAKEFILES__)
!ifndef __UNIX__
    > spen1.sed echo $(e)1i\$e
    >>spen1.sed echo @echo %debug% off
!else
    rm -f spen1.sed
!endif
    >>spen1.sed cat <<
$# The file contains line pairs: a sed command on the 1st; a number on the 2nd.
$# Transform each pair into one line with a conditional failure diagnostic.
N;s,\n\(.*\), | $(sed) -n \"1s/.*/Test $#\1 failed!/p\",
<<

./spencer.cmd: spen0.sed spen0.dat spen1.sed
    $(sed) -f spen0.sed spen0.dat | $(sed) -f spen1.sed > $@

# This rule tests things, not otherwise tested, specified in IEEE Std 1003.1,
# 2004 Edition http://www.opengroup.org/onlinepubs/000095399/utilities/sed.html
susv3: .symbolic
    # "the order of presentation of the -e and -f options is significant."
    > afore echo hello
    > xpect echo help
    > scrip echo p
    $(sed) -n -e s/lo/p/ -f scrip                                   $(axdiff)
    # "Multiple -e and -f options may be specified."
    > xpect echo Help
    > scr0p echo s/o/p/
    > scr1p echo p
    $(sed) -n -e s/l// -f scr0p -e s/h/H/ -f scr1p                  $(axdiff)
    # "If multiple file operands are specified, the named files shall be read
    # in the order specified"
    > xpect echo hello world
    > scr0p echo hello
    > scr1p echo world
    $(sed) -n "N;s/ //g;s/\n/ /p" scr0p scr1p | diff xpect -

    # "INPUT FILES The input files shall be text files. The
    # script_files named by the -f option shall consist of editing commands."
    # This implementation allows \n absent from all input files.

    # "ENVIRONMENT VARIABLES The following environment variables
    # shall affect the execution of sed: ...".
    # All the variables are setlocale() oriented. None are supported as Open
    # Watcom C library documentation says: "Watcom C/C++ supports only the "C"
    # locale and so invoking this function will have no effect upon the
    # behavior of a program at present.#

    # "The command can be preceded by <blank>s and/or semicolons. The function
    # can be preceded by <blank>s."
    $(sed) -n "    ;  1  p  "                                       $(aadiff)

    # "The pattern and hold spaces shall each be able to hold
    # at least 8192 bytes."
    > afore echo 12345678 1 2345678 2 2345678 3 2345678 4 2345678 5 2345678 6 23
    #          128  256  512 1024  2048 4096 8192
    $(sed) -n "h;G; h;G; h;G; h;G; h;G; h;G; h;G; h;P"              $(aadiff)
    > xpect echo sed: can only fit 8192 bytes at line 1
    #          128  256  512 1024  2048 4096 8192 16384
    $(sed) -n "h;G; h;G; h;G; h;G; h;G; h;G; h;G; h;G;P"            afore 2>&1 > after | diff xpect -
    diff afore after
    > xpect echo sed: can only fit 8192 bytes at line 1
    #          128  256  512 1024  2048 4096 8192 8193
    $(sed) -n "h;G; h;G; h;G; h;G; h;G; h;G; h;G; s/^/?/P"          afore 2>&1 > after | diff xpect -
    echo ?12345678 1 2345678 2 2345678 3 2345678 4 2345678 5 2345678 6 23| diff after -

    # "In a context address, the construction "\cBREc" ,
    # where c is any character other than backslash or <newline>,
    # shall be identical to "/BRE/" ."
    > afore echo hello
    $(sed) -n $q\chcp$q                                             $(aadiff)

    # "The r and w command verbs, and the w flag to the s command, take an
    # optional rfile (or wfile) parameter, separated from the command verb
    # letter or flag by one or more <blank>s; implementations may allow
    # zero separation as an extension."
    # This implementation does so allow.

    # "The argument rfile or the argument wfile shall terminate the editing
    # command. Each wfile shall be created before processing begins.
    # Implementations shall support at least ten wfile arguments in the
    # script; the actual number (greater than or equal to 10) that is
    # supported by the implementation is unspecified. The use of the wfile
    # parameter shall cause that file to be initially created, if it does not
    # exist, or shall replace the contents of an existing file."
    # This implementation supports 10 such files.

    # "The contents of the file specified for the r command shall be as of
    # the time the output is written, not the time the r command is applied."
    > xpect echo Hello
    >>xpect echo hello
    $(sed) -e rscrip -e wscrip -e s/h/H/                            $(axdiff)
    diff afore scrip

    # "It is unspecified whether <blank>s can follow a '!' character, and
    # conforming applications shall not follow a '!' character with <blank>s."
    # This implementation allows that unspecified behavior.
    $(sed) -e "/h/ ! d"                                             $(aadiff)

    # "[2addr]b [label] ... The implementation shall support labels recognized
    # as unique up to at least 8 characters; the actual length (greater than
    # or equal to 8) that shall be supported by the implementation is
    # unspecified. It is unspecified whether exceeding a label length causes
    # an error or a silent truncation."
    # This implementation imposes no length restrictions.
    $(sed) -n -e "bvery long label" -e b -e ":very long label" -e p $(aadiff)

    # "[2addr]l ... Non-printable characters not in that table shall be written
    # as one three-digit octal number (with a preceding backslash) for
    # each byte in the character (most significant byte first)."
    # This implementation uses hex instead.

    # "[2addr]s/BRE/replacement/flags ... The value of flags shall be zero or
    # more of: ... g Globally substitute for all non-overlapping instances of
    # the BRE rather than just the first one.
    # If both g and n are specified, the results are unspecified."
    # If both g and n are specified, this implementation does n and subsequent.
    > xpect echo helaa
    $(sed) -e s/./a/4g                                              $(axdiff)

    # In [2addr]s/BRE/replacement/flags, an extra P flag is supported.
    > afore echo hello
    >>afore echo world
    > xpect echo hello
    $(sed) -n -e "N;s/d/D/P"                                        $(axdiff)

    # [0addr] Ignore this empty command.
    > afore echo hello
    $(sed) -e ""                                                    $(aadiff)

    # "[0addr]# Ignore the '#' and the remainder of the line
    # (treat them as a comment), with the single exception that if the first
    # two characters in the script are "#n" , the default output shall be
    # suppressed; this shall be the equivalent of specifying -n on the command
    # line."
    > afore echo hello
    $(sed) -e "$#n" -e p                                            $(aadiff)
    > afore echo hello
    $(sed) -e "$#N"                                                 $(aadiff)

bre: .symbolic
    # Note compliance with
    # http://www.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap09.html
    #tag_09_03
    # " 9.3.1 BREs Matching a Single Character or Collating Element
    # A BRE ordinary character, a special character preceded by a backslash, or
    # a period shall match a single character. A bracket expression shall match
    # a single character or a single collating element."
    # This implementation does not support collating elements.

    # "9.3.3 BRE Special Characters
    # This implementation does not recognise .[\ as special
    > xpect echo sed: garbled command /.[\/p
    $(sed) -n /.[\/p $(a2xdiff)

    # "9.3.5 RE Bracket Expression
    #   collating elements, collating symbols and equivalence classes are
    #   unsupported;
    #   character sequences "[.", "[=", and "[:" are not special.

    # " 9.3.7 BRE Precedence
    # The order of precedence shall be as shown in the following table:
    # BRE Precedence (from high to low)
    # Collation-related bracket symbols [==] [::] [..]
    # Escaped characters                \<special character>
    # Bracket expression                []
    # Subexpressions/back-references    \(\) \n
    # Single-character-BRE duplication  * \{m,n\}
    # Concatenation
    # Anchoring                         ^ $"
    # This implementation does not recognise Collation-related bracket symbols.

    # "9.3.8 BRE Expression Anchoring
    # This does not treat a circumflex as an anchor when used as the first
    # character of a subexpression.
    # This does not treat a dollar sign as an anchor when used as the last
    # character of a subexpression.
