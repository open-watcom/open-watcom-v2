.chap *refid=cromabl Creating ROM-based Applications
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'ROMable code'
This chapter provides information for developers who wish to write
applications to be placed in read-only memory (ROM).
.*
.section ROMable Functions
.*
.np
The following functions in the &cmpname library are not dependent on
any operating system.
Therefore they can be used for embedded applications.
.ix 'math functions'
.ix 'ROM-based functions'
.ix 'functions' 'in ROM'
The math functions are listed here because they are ROMable, however
you must supply a different
.id _matherr
function if you are not running in the DOS, OS/2 or Windows NT
environment.
.contents
abs~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
acos~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
alloca~b~b~b~b~b~b~b~b~b~b~b~b~b
asctime~b~b~b~b~b~b~b~b~b~b~b~b
asin~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
atan~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
atan2~b~b~b~b~b~b~b~b~b~b~b~b~b~b
atexit~b~b~b~b~b~b~b~b~b~b~b~b~b
atof~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
atoi~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
atol~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
bsearch~b~b~b~b~b~b~b~b~b~b~b~b
cabs~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ceil~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_chain_intr~b~b~b~b~b~b~b~b
_clear87~b~b~b~b~b~b~b~b~b~b~b
_control87~b~b~b~b~b~b~b~b~b
cos~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
cosh~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
difftime~b~b~b~b~b~b~b~b~b~b~b
_disable~b~b~b~b~b~b~b~b~b~b~b
div~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_enable~b~b~b~b~b~b~b~b~b~b~b~b
exp~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fabs~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
floor~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_fmemccpy~b~b~b~b~b~b~b~b~b~b
_fmemchr~b~b~b~b~b~b~b~b~b~b~b
_fmemcmp~b~b~b~b~b~b~b~b~b~b~b
_fmemcpy~b~b~b~b~b~b~b~b~b~b~b
_fmemicmp~b~b~b~b~b~b~b~b~b~b
_fmemmove~b~b~b~b~b~b~b~b~b~b
_fmemset~b~b~b~b~b~b~b~b~b~b~b
fmod~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
FP_OFF~b~b~b~b~b~b~b~b~b~b~b~b~b
FP_SEG~b~b~b~b~b~b~b~b~b~b~b~b~b
_fpreset~b~b~b~b~b~b~b~b~b~b~b
frexp~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_fstrcat~b~b~b~b~b~b~b~b~b~b~b
_fstrchr~b~b~b~b~b~b~b~b~b~b~b
_fstrcmp~b~b~b~b~b~b~b~b~b~b~b
_fstrcpy~b~b~b~b~b~b~b~b~b~b~b
_fstrcspn~b~b~b~b~b~b~b~b~b~b
_fstricmp~b~b~b~b~b~b~b~b~b~b
_fstrlen~b~b~b~b~b~b~b~b~b~b~b
_fstrlwr~b~b~b~b~b~b~b~b~b~b~b
_fstrncat~b~b~b~b~b~b~b~b~b~b
_fstrncmp~b~b~b~b~b~b~b~b~b~b
_fstrncpy~b~b~b~b~b~b~b~b~b~b
_fstrnicmp~b~b~b~b~b~b~b~b~b
_fstrnset~b~b~b~b~b~b~b~b~b~b
_fstrpbrk~b~b~b~b~b~b~b~b~b~b
_fstrrchr~b~b~b~b~b~b~b~b~b~b
_fstrrev~b~b~b~b~b~b~b~b~b~b~b
_fstrset~b~b~b~b~b~b~b~b~b~b~b
_fstrspn~b~b~b~b~b~b~b~b~b~b~b
_fstrstr~b~b~b~b~b~b~b~b~b~b~b
_fstrtok~b~b~b~b~b~b~b~b~b~b~b
_fstrupr~b~b~b~b~b~b~b~b~b~b~b
gmtime~b~b~b~b~b~b~b~b~b~b~b~b~b
hypot~b~b~b~b~b~b~b~b~b~b~b~b~b~b
inp~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
inpw~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
int86~b(1)~b~b~b~b~b~b~b~b~b~b
int86x~b(1)~b~b~b~b~b~b~b~b~b
int386~b(2)~b~b~b~b~b~b~b~b~b
int386x~b(2)~b~b~b~b~b~b~b~b
intr~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
isalnum~b~b~b~b~b~b~b~b~b~b~b~b
isalpha~b~b~b~b~b~b~b~b~b~b~b~b
isascii~b~b~b~b~b~b~b~b~b~b~b~b
iscntrl~b~b~b~b~b~b~b~b~b~b~b~b
isdigit~b~b~b~b~b~b~b~b~b~b~b~b
isgraph~b~b~b~b~b~b~b~b~b~b~b~b
islower~b~b~b~b~b~b~b~b~b~b~b~b
isprint~b~b~b~b~b~b~b~b~b~b~b~b
ispunct~b~b~b~b~b~b~b~b~b~b~b~b
isspace~b~b~b~b~b~b~b~b~b~b~b~b
isupper~b~b~b~b~b~b~b~b~b~b~b~b
isxdigit~b~b~b~b~b~b~b~b~b~b~b
itoa~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
j0~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
j1~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
jn~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
labs~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ldexp~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ldiv~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
lfind~b~b~b~b~b~b~b~b~b~b~b~b~b~b
localeconv~b~b~b~b~b~b~b~b~b
log~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
log10~b~b~b~b~b~b~b~b~b~b~b~b~b~b
longjmp~b~b~b~b~b~b~b~b~b~b~b~b
_lrotl~b~b~b~b~b~b~b~b~b~b~b~b~b
_lrotr~b~b~b~b~b~b~b~b~b~b~b~b~b
lsearch~b~b~b~b~b~b~b~b~b~b~b~b
ltoa~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
matherr~b~b~b~b~b~b~b~b~b~b~b~b
mblen~b~b~b~b~b~b~b~b~b~b~b~b~b~b
mbstowcs~b~b~b~b~b~b~b~b~b~b~b
mbtowc~b~b~b~b~b~b~b~b~b~b~b~b~b
memccpy~b~b~b~b~b~b~b~b~b~b~b~b
memchr~b~b~b~b~b~b~b~b~b~b~b~b~b
memcmp~b~b~b~b~b~b~b~b~b~b~b~b~b
memcpy~b~b~b~b~b~b~b~b~b~b~b~b~b
memicmp~b~b~b~b~b~b~b~b~b~b~b~b
memmove~b~b~b~b~b~b~b~b~b~b~b~b
memset~b~b~b~b~b~b~b~b~b~b~b~b~b
MK_FP~b~b~b~b~b~b~b~b~b~b~b~b~b~b
modf~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
movedata~b~b~b~b~b~b~b~b~b~b~b
offsetof~b~b~b~b~b~b~b~b~b~b~b
outp~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
outpw~b~b~b~b~b~b~b~b~b~b~b~b~b~b
pow~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
qsort~b~b~b~b~b~b~b~b~b~b~b~b~b~b
rand~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_rotl~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_rotr~b~b~b~b~b~b~b~b~b~b~b~b~b~b
segread~b~b~b~b~b~b~b~b~b~b~b~b
setjmp~b~b~b~b~b~b~b~b~b~b~b~b~b
setlocale~b~b~b~b~b~b~b~b~b~b
sin~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sinh~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sprintf~b~b~b~b~b~b~b~b~b~b~b~b
sqrt~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
srand~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sscanf~b~b~b~b~b~b~b~b~b~b~b~b~b
stackavail~b~b~b~b~b~b~b~b~b
_status87~b~b~b~b~b~b~b~b~b~b
strcat~b~b~b~b~b~b~b~b~b~b~b~b~b
strchr~b~b~b~b~b~b~b~b~b~b~b~b~b
strcmp~b~b~b~b~b~b~b~b~b~b~b~b~b
strcmpi~b~b~b~b~b~b~b~b~b~b~b~b
strcoll~b~b~b~b~b~b~b~b~b~b~b~b
strcpy~b~b~b~b~b~b~b~b~b~b~b~b~b
strcspn~b~b~b~b~b~b~b~b~b~b~b~b
strdup~b~b~b~b~b~b~b~b~b~b~b~b~b
strerror~b~b~b~b~b~b~b~b~b~b~b
stricmp~b~b~b~b~b~b~b~b~b~b~b~b
strlen~b~b~b~b~b~b~b~b~b~b~b~b~b
strlwr~b~b~b~b~b~b~b~b~b~b~b~b~b
strncat~b~b~b~b~b~b~b~b~b~b~b~b
strncmp~b~b~b~b~b~b~b~b~b~b~b~b
strncpy~b~b~b~b~b~b~b~b~b~b~b~b
strnicmp~b~b~b~b~b~b~b~b~b~b~b
strnset~b~b~b~b~b~b~b~b~b~b~b~b
strpbrk~b~b~b~b~b~b~b~b~b~b~b~b
strrchr~b~b~b~b~b~b~b~b~b~b~b~b
strrev~b~b~b~b~b~b~b~b~b~b~b~b~b
strset~b~b~b~b~b~b~b~b~b~b~b~b~b
strspn~b~b~b~b~b~b~b~b~b~b~b~b~b
strstr~b~b~b~b~b~b~b~b~b~b~b~b~b
strtod~b~b~b~b~b~b~b~b~b~b~b~b~b
strtok~b~b~b~b~b~b~b~b~b~b~b~b~b
strtol~b~b~b~b~b~b~b~b~b~b~b~b~b
strtoul~b~b~b~b~b~b~b~b~b~b~b~b
strupr~b~b~b~b~b~b~b~b~b~b~b~b~b
strxfrm~b~b~b~b~b~b~b~b~b~b~b~b
swab~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
tan~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
tanh~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
tolower~b~b~b~b~b~b~b~b~b~b~b~b
toupper~b~b~b~b~b~b~b~b~b~b~b~b
ultoa~b~b~b~b~b~b~b~b~b~b~b~b~b~b
utoa~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
va_arg~b~b~b~b~b~b~b~b~b~b~b~b~b
va_end~b~b~b~b~b~b~b~b~b~b~b~b~b
va_start~b~b~b~b~b~b~b~b~b~b~b
vsprintf~b~b~b~b~b~b~b~b~b~b~b
vsscanf~b~b~b~b~b~b~b~b~b~b~b~b
wcstombs~b~b~b~b~b~b~b~b~b~b~b
wctomb~b~b~b~b~b~b~b~b~b~b~b~b~b
y0~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
y1~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
yn~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
.econtents
.illust begin
* (1) 16-bit libraries
* (2) 32-bit libraries
.illust end
.*
.section System-Dependent Functions
.*
.np
.ix 'DOS-dependent functions'
.ix 'functions' 'DOS-dependent'
.ix 'OS/2-dependent functions'
.ix 'functions' 'OS/2-dependent'
.ix 'Windows NT-dependent functions'
.ix 'functions' 'Windows NT-dependent'
The following functions in the C/C++ library directly or indirectly
make use of operating system functions.
They cannot be used on systems that are not running on one of the DOS,
OS/2 or Windows NT operating systems.
.contents
abort~b~b~b~b~b~b~b~b~b~b~b~b~b~b
access~b~b~b~b~b~b~b~b~b~b~b~b~b
assert~b~b~b~b~b~b~b~b~b~b~b~b~b
bdos~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_beginthread~b~b~b~b~b~b~b
_bios_disk~b~b~b~b~b~b~b~b~b
_bios_equiplist~b~b~b~b
_bios_keybrd~b~b~b~b~b~b~b
_bios_memsize~b~b~b~b~b~b
_bios_printer~b~b~b~b~b~b
_bios_serialcom~b~b~b~b
_bios_timeofday~b~b~b~b
calloc~b~b~b~b~b~b~b~b~b~b~b~b~b
cgets~b~b~b~b~b~b~b~b~b~b~b~b~b~b
chdir~b~b~b~b~b~b~b~b~b~b~b~b~b~b
chmod~b~b~b~b~b~b~b~b~b~b~b~b~b~b
chsize~b~b~b~b~b~b~b~b~b~b~b~b~b
clearerr~b~b~b~b~b~b~b~b~b~b~b
clock~b~b~b~b~b~b~b~b~b~b~b~b~b~b
close~b~b~b~b~b~b~b~b~b~b~b~b~b~b
closedir~b~b~b~b~b~b~b~b~b~b~b
cprintf~b~b~b~b~b~b~b~b~b~b~b~b
cputs~b~b~b~b~b~b~b~b~b~b~b~b~b~b
creat~b~b~b~b~b~b~b~b~b~b~b~b~b~b
cscanf~b~b~b~b~b~b~b~b~b~b~b~b~b
ctime~b~b~b~b~b~b~b~b~b~b~b~b~b~b
cwait~b~b~b~b~b~b~b~b~b~b~b~b~b~b
delay~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_dos_allocmem~b~b~b~b~b~b
_dos_close~b~b~b~b~b~b~b~b~b
_dos_creat~b~b~b~b~b~b~b~b~b
_dos_creatnew~b~b~b~b~b~b
_dos_findfirst~b~b~b~b~b
_dos_findnext~b~b~b~b~b~b
_dos_freemem~b~b~b~b~b~b~b
_dos_getdate~b~b~b~b~b~b~b
_dos_getdiskfree~b~b~b
_dos_getdrive~b~b~b~b~b~b
_dos_getfileattr~b~b~b
_dos_getftime~b~b~b~b~b~b
_dos_gettime~b~b~b~b~b~b~b
_dos_getvect~b~b~b~b~b~b~b
_dos_keep~b~b~b~b~b~b~b~b~b~b
_dos_open~b~b~b~b~b~b~b~b~b~b
_dos_read~b~b~b~b~b~b~b~b~b~b
_dos_setblock~b~b~b~b~b~b
_dos_setdate~b~b~b~b~b~b~b
_dos_setdrive~b~b~b~b~b~b
_dos_setfileattr~b~b~b
_dos_setftime~b~b~b~b~b~b
_dos_settime~b~b~b~b~b~b~b
_dos_setvect~b~b~b~b~b~b~b
_dos_write~b~b~b~b~b~b~b~b~b
dosexterr~b~b~b~b~b~b~b~b~b~b
dup~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
dup2~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_endthread~b~b~b~b~b~b~b~b~b
eof~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
execl~b(1)~b~b~b~b~b~b~b~b~b~b
execle~b(1)~b~b~b~b~b~b~b~b~b
execlp~b(1)~b~b~b~b~b~b~b~b~b
execlpe~b(1)~b~b~b~b~b~b~b~b
execv~b(1)~b~b~b~b~b~b~b~b~b~b
execve~b(1)~b~b~b~b~b~b~b~b~b
execvp~b(1)~b~b~b~b~b~b~b~b~b
execvpe~b(1)~b~b~b~b~b~b~b~b
exit~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_exit~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fclose~b~b~b~b~b~b~b~b~b~b~b~b~b
fcloseall~b~b~b~b~b~b~b~b~b~b
fdopen~b~b~b~b~b~b~b~b~b~b~b~b~b
feof~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ferror~b~b~b~b~b~b~b~b~b~b~b~b~b
fflush~b~b~b~b~b~b~b~b~b~b~b~b~b
_ffree~b~b~b~b~b~b~b~b~b~b~b~b~b
_fheapchk~b~b~b~b~b~b~b~b~b~b
_fheapgrow~b(1)~b~b~b~b~b
_fheapmin~b~b~b~b~b~b~b~b~b~b
_fheapset~b~b~b~b~b~b~b~b~b~b
_fheapshrink~b~b~b~b~b~b~b
_fheapwalk~b~b~b~b~b~b~b~b~b
fgetc~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fgetpos~b~b~b~b~b~b~b~b~b~b~b~b
fgets~b~b~b~b~b~b~b~b~b~b~b~b~b~b
filelength~b~b~b~b~b~b~b~b~b
fileno~b~b~b~b~b~b~b~b~b~b~b~b~b
flushall~b~b~b~b~b~b~b~b~b~b~b
_fmalloc~b~b~b~b~b~b~b~b~b~b~b
fopen~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fprintf~b~b~b~b~b~b~b~b~b~b~b~b
fputc~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fputs~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fread~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_frealloc~b~b~b~b~b~b~b~b~b~b
free~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
freopen~b~b~b~b~b~b~b~b~b~b~b~b
fscanf~b~b~b~b~b~b~b~b~b~b~b~b~b
fseek~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fsetpos~b~b~b~b~b~b~b~b~b~b~b~b
fstat~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ftell~b~b~b~b~b~b~b~b~b~b~b~b~b~b
fwrite~b~b~b~b~b~b~b~b~b~b~b~b~b
getc~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
getch~b~b~b~b~b~b~b~b~b~b~b~b~b~b
getchar~b~b~b~b~b~b~b~b~b~b~b~b
getche~b~b~b~b~b~b~b~b~b~b~b~b~b
getcmd~b~b~b~b~b~b~b~b~b~b~b~b~b
getcwd~b~b~b~b~b~b~b~b~b~b~b~b~b
getenv~b~b~b~b~b~b~b~b~b~b~b~b~b
getpid~b~b~b~b~b~b~b~b~b~b~b~b~b
gets~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
halloc~b~b~b~b~b~b~b~b~b~b~b~b~b
_heapchk~b~b~b~b~b~b~b~b~b~b~b
_heapgrow~b~b~b~b~b~b~b~b~b~b
_heapmin~b~b~b~b~b~b~b~b~b~b~b
_heapset~b~b~b~b~b~b~b~b~b~b~b
_heapshrink~b~b~b~b~b~b~b~b
_heapwalk~b~b~b~b~b~b~b~b~b~b
hfree~b~b~b~b~b~b~b~b~b~b~b~b~b~b
intdos~b~b~b~b~b~b~b~b~b~b~b~b~b
intdosx~b~b~b~b~b~b~b~b~b~b~b~b
isatty~b~b~b~b~b~b~b~b~b~b~b~b~b
kbhit~b~b~b~b~b~b~b~b~b~b~b~b~b~b
localtime~b~b~b~b~b~b~b~b~b~b
lock~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
locking~b~b~b~b~b~b~b~b~b~b~b~b
lseek~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_makepath~b~b~b~b~b~b~b~b~b~b
malloc~b~b~b~b~b~b~b~b~b~b~b~b~b
mkdir~b~b~b~b~b~b~b~b~b~b~b~b~b~b
mktime~b~b~b~b~b~b~b~b~b~b~b~b~b
_nfree~b~b~b~b~b~b~b~b~b~b~b~b~b
_nheapchk~b~b~b~b~b~b~b~b~b~b
_nheapgrow~b~b~b~b~b~b~b~b~b
_nheapmin~b~b~b~b~b~b~b~b~b~b
_nheapset~b~b~b~b~b~b~b~b~b~b
_nheapshrink~b~b~b~b~b~b~b
_nheapwalk~b~b~b~b~b~b~b~b~b
_nmalloc~b~b~b~b~b~b~b~b~b~b~b
_nrealloc~b~b~b~b~b~b~b~b~b~b
nosound~b~b~b~b~b~b~b~b~b~b~b~b
open~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
opendir~b~b~b~b~b~b~b~b~b~b~b~b
perror~b~b~b~b~b~b~b~b~b~b~b~b~b
printf~b~b~b~b~b~b~b~b~b~b~b~b~b
putc~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
putch~b~b~b~b~b~b~b~b~b~b~b~b~b~b
putchar~b~b~b~b~b~b~b~b~b~b~b~b
putenv~b~b~b~b~b~b~b~b~b~b~b~b~b
puts~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
raise~b~b~b~b~b~b~b~b~b~b~b~b~b~b
read~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
readdir~b~b~b~b~b~b~b~b~b~b~b~b
realloc~b~b~b~b~b~b~b~b~b~b~b~b
remove~b~b~b~b~b~b~b~b~b~b~b~b~b
rename~b~b~b~b~b~b~b~b~b~b~b~b~b
rewind~b~b~b~b~b~b~b~b~b~b~b~b~b
rmdir~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sbrk~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
scanf~b~b~b~b~b~b~b~b~b~b~b~b~b~b
_searchenv~b~b~b~b~b~b~b~b~b
setbuf~b~b~b~b~b~b~b~b~b~b~b~b~b
setmode~b~b~b~b~b~b~b~b~b~b~b~b
setvbuf~b~b~b~b~b~b~b~b~b~b~b~b
signal~b~b~b~b~b~b~b~b~b~b~b~b~b
sleep~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sopen~b~b~b~b~b~b~b~b~b~b~b~b~b~b
sound~b~b~b~b~b~b~b~b~b~b~b~b~b~b
spawnl~b~b~b~b~b~b~b~b~b~b~b~b~b
spawnle~b~b~b~b~b~b~b~b~b~b~b~b
spawnlp~b~b~b~b~b~b~b~b~b~b~b~b
spawnlpe~b~b~b~b~b~b~b~b~b~b~b
spawnv~b~b~b~b~b~b~b~b~b~b~b~b~b
spawnve~b~b~b~b~b~b~b~b~b~b~b~b
spawnvp~b~b~b~b~b~b~b~b~b~b~b~b
spawnvpe~b~b~b~b~b~b~b~b~b~b~b
_splitpath~b~b~b~b~b~b~b~b~b
stat~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
strftime~b~b~b~b~b~b~b~b~b~b~b
system~b~b~b~b~b~b~b~b~b~b~b~b~b
tell~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
time~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
tmpfile~b~b~b~b~b~b~b~b~b~b~b~b
tmpnam~b~b~b~b~b~b~b~b~b~b~b~b~b
tzset~b~b~b~b~b~b~b~b~b~b~b~b~b~b
umask~b~b~b~b~b~b~b~b~b~b~b~b~b~b
ungetc~b~b~b~b~b~b~b~b~b~b~b~b~b
ungetch~b~b~b~b~b~b~b~b~b~b~b~b
unlink~b~b~b~b~b~b~b~b~b~b~b~b~b
unlock~b~b~b~b~b~b~b~b~b~b~b~b~b
utime~b~b~b~b~b~b~b~b~b~b~b~b~b~b
vfprintf~b~b~b~b~b~b~b~b~b~b~b
vfscanf~b~b~b~b~b~b~b~b~b~b~b~b
vprintf~b~b~b~b~b~b~b~b~b~b~b~b
vscanf~b~b~b~b~b~b~b~b~b~b~b~b~b
wait~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
write~b~b~b~b~b~b~b~b~b~b~b~b~b~b
~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b~b
.econtents
.illust begin
* (1) 16-bit libraries
.illust end
.*
.section Modifying the Startup Code
.*
.np
.ix 'ROMable code' 'startup'
.ix 'startup code'
Source files are included in the package for the &cmpname application
start-up (or initialization) sequence.
These files are described in the section entitled :HDREF refid='cplibrt'..
The startup code will have to be modified if you are creating a
ROMable application or you are not running in a DOS, OS/2, QNX, or
Windows environment.
.*
.section Choosing the Correct Floating-Point Option
.*
.np
.ix 'floating-point in ROM'
.ix 'options' 'floating-point in ROM'
.ix 'options' 'fpi87'
.ix 'options' 'fpc'
.ix 'options' 'fpi'
.ix 'options' 'fp2'
.ix 'options' 'fp3'
.ix 'options' 'fp5'
If there will be a math coprocessor chip in your embedded system, then
you should compile your application with the "fpi87" option and
one of "fp2", "fp3" or "fp5" depending on which math coprocessor chip
will be in your embedded system.
If there will not be a math coprocessor chip in your embedded system,
then you should compile your application with the "fpc" option.
You should not use the "fpi" option since that will cause extra code
to be linked into your application to decode and emulate the 80x87
instructions contained in your application.
