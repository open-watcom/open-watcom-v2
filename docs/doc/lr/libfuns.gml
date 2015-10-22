.*
.* .ty Starting build pass for &machsys system
.*
.* Note:
.*      "QNX W" functions are those included in the QNX book whose
.*      descriptions were written by WATCOM.
.*
.*      "QXO" are functions for which QSSL provides documentation.
.*
.*      "QX40" are obsolete functions for which QSLL provided
.*      documentation at one time.
.*
.*      "DOS" are functions which are included in the C/C++ and F77 packages.
.*
.*      "WIN32" are functions which are included in the Power++ package.
.*
.*      "NEC" are functions which are included in the Japanese C/C++ and
.*      F77 packages.
.*
.*      "TEST" are functions that are included in a "test" version of
.*      the documentation.
.*
.* All functions described in one gml file must be in adjacent lines!
.*
.*
.fnc abort                      abort.gml       DOS QNX W WIN32
.fnc abort_handler_s            abrt_hnd.gml    DOS QNX W WIN32
.fnc abs                        abs.gml         DOS QNX W WIN32
.fnc access                     access.gml      DOS QXO   WIN32
.fnc _access                    access.gml      DOS       WIN32
.fnw _waccess                   access.gml      DOS       WIN32
.fnc acos                       acos.gml        DOS QNX W WIN32
.fnc acosh                      acosh.gml       DOS QNX W WIN32
.fnc alarm                      alarm.gml           QXO
.fnc alloca                     alloca.gml      DOS QNX W WIN32
.fnc _arc                       gr_arc.gml      DOS QNX W GRAPH
.fnc _arc_w                     gr_arc.gml      DOS QNX W GRAPH
.fnc _arc_wxy                   gr_arc.gml      DOS QNX W GRAPH
.fnc asctime                    asctime.gml     DOS QNX W WIN32
.fnc _asctime                   asctime.gml     DOS QNX W WIN32
.fnw _wasctime                  asctime.gml     DOS QNX W WIN32
.fnw __wasctime                 asctime.gml     DOS QNX W WIN32
.fnc asctime_s                  asctim_s.gml    DOS       WIN32
.fnc _wasctime_s                asctim_s.gml    DOS       WIN32
.fnc asin                       asin.gml        DOS QNX W WIN32
.fnc asinh                      asinh.gml       DOS QNX W WIN32
.fnc assert                     assert.gml      DOS QNX W WIN32
.fnc atan                       atan.gml        DOS QNX W WIN32
.fnc atan2                      atan2.gml       DOS QNX W WIN32
.fnc atanh                      atanh.gml       DOS QNX W WIN32
.fnc atexit                     atexit.gml      DOS QNX W WIN32
.fnc atof                       atof.gml        DOS QNX W WIN32
.fnw _wtof                      atof.gml        DOS QNX W WIN32
.fnc atoh                       atoh.gml            QXO
.fnc atoi                       atoi.gml        DOS QNX W WIN32
.fnw _wtoi                      atoi.gml        DOS QNX W WIN32
.fnc atol                       atol.gml        DOS QNX W WIN32
.fnw _wtol                      atol.gml        DOS QNX W WIN32
.fnc atoll                      atoll.gml       DOS QNX W WIN32
.fnw _wtoll                     atoll.gml       DOS QNX W WIN32
.fnc _atouni                    _atouni.gml     DOS QNX W WIN32
.fnc basename                   basename.gml    DOS QNX W WIN32
.fnc bdos                       bdos.gml        DOS
.fnc _beginthread               _bthread.gml    DOS QXO   WIN32
.fnc _beginthreadex             _bthread.gml    DOS       WIN32
.fnc bessel                     bessel.gml      DOS QNX W WIN32
.fnc j0                         bessel.gml      DOS QNX W WIN32
.fnc j1                         bessel.gml      DOS QNX W WIN32
.fnc jn                         bessel.gml      DOS QNX W WIN32
.fnc y0                         bessel.gml      DOS QNX W WIN32
.fnc y1                         bessel.gml      DOS QNX W WIN32
.fnc yn                         bessel.gml      DOS QNX W WIN32
.fn8 bcmp                       bcmp.gml        DOS QNX W WIN32
.fn8 bcopy                      bcopy.gml       DOS QNX W WIN32
.fnc _bfreeseg                  _bfreese.gml    DOS QNX W
.fnc _bgetcmd                   _bgetcmd.gml    DOS QNX W WIN32
.fnc _bheapseg                  _bheapse.gml    DOS QNX W
.fnc _bios_disk                 b_disk.gml      DOS TEST
.fnc _bios_equiplist            b_equip.gml     DOS TEST
.fnc _bios_keybrd               b_keybrd.gml    DOS TEST
.fnc _bios_memsize              b_memsiz.gml    DOS
.fnc _bios_printer              b_print.gml     DOS
.fnc _bios_serialcom            b_serial.gml    DOS
.fnc _bios_timeofday            b_time.gml      DOS
.fnc block_read                 block_re.gml        QXO
.fnc block_write                block_wr.gml        QXO
.fnc _bprintf                   _bprintf.gml    DOS QNX W WIN32
.fnw _bwprintf                  _bprintf.gml    DOS QNX W WIN32
.fnc break_off                  breakoff.gml    DOS
.fnc break_on                   breakoff.gml    DOS
.fnc bsearch                    bsearch.gml     DOS QNX W WIN32
.fnc bsearch_s                  bsrch_s.gml     DOS QNX W WIN32
.fnc btom                       btom.gml        NEC
.fnw btowc                      btowc.gml       DOS       WIN32
.fnf _fbtom                     btom.gml        NEC
.fn8 bzero                      bzero.gml       DOS QNX W WIN32
.fnc cabs                       cabs.gml        DOS QNX W WIN32
.fnc calloc                     calloc.gml      DOS QNX W WIN32
.fnc _bcalloc                   calloc.gml      DOS QNX W
.fnf _fcalloc                   calloc.gml      DOS QNX W
.fnc _ncalloc                   calloc.gml      DOS QNX W WIN32
.fnc cbrt                       cbrt.gml        DOS QNX W WIN32
.fnc ceil                       ceil.gml        DOS QNX W WIN32
.fnc cfgetispeed                cfgetisp.gml        QXO
.fnc cfgetospeed                cfgetosp.gml        QXO
.fnc cfsetispeed                cfsetisp.gml        QXO
.fnc cfsetospeed                cfsetosp.gml        QXO
.fnc cgets                      cgets.gml       DOS QNX W WIN32
.fnc _chain_intr                chainint.gml    DOS
.fnc chdir                      chdir.gml       DOS QNX W WIN32
.fnc _chdir                     chdir.gml       DOS QNX W WIN32
.fnw _wchdir                    chdir.gml       DOS       WIN32
.fnc _chdrive                   chdrive.gml     DOS       WIN32
.fnc chkctype                   chkctype.gml    NEC
.fnc chmod                      chmod.gml       DOS QXO   WIN32
.fnc _chmod                     chmod.gml       DOS       WIN32
.fnw _wchmod                    chmod.gml       DOS       WIN32
.fnc chown                      chown.gml           QXO
.fnc chsize                     chsize.gml      DOS QNX W WIN32
.fnc _chsize                    chsize.gml      DOS       WIN32
.fnc _clear87                   _clear87.gml    DOS QNX W WIN32
.fnc clearenv                   clearenv.gml    DOS QNX W WIN32
.fnc clearerr                   clearerr.gml    DOS QNX W WIN32
.fnc _clearscreen               gr_clrsc.gml    DOS QNX W GRAPH
.fnc clock                      clock.gml       DOS QNX W WIN32
.fnc clock_getres               clockres.gml        QXO
.fnc clock_gettime              clockget.gml        QXO
.fnc clock_setres               clocsres.gml        QXO
.fnc clock_settime              clockset.gml        QXO
.fnc clock_getres               clkres.gml      DOS     W WIN32
.fnc clock_gettime              clkget.gml      DOS     W WIN32
.fnc clock_nanosleep            clkns.gml       DOS     W WIN32
.fnc clock_settime              clkset.gml      DOS     W WIN32
.fnc close                      close.gml       DOS QNX W WIN32
.fn8 _close                     close.gml       DOS       WIN32
.fnc closedir                   closedir.gml    DOS QNX W WIN32
.fnw _wclosedir                 closedir.gml    DOS       WIN32
.fnc _cmdname                   _cmdname.gml    DOS QNX W WIN32
.fnc _commit                    _commit.gml     DOS     W WIN32
.fnc confstr                    confstr.gml         QXO
.fnc console_active             con_actv.gml        QXO
.fnc console_arm                con_arm.gml         QXO
.fnc console_close              con_clo.gml         QXO
.fnc console_ctrl               con_ctrl.gml        QXO
.fnc console_font               con_font.gml        QXO
.fnc console_info               con_info.gml        QXO
.fnc console_open               con_open.gml        QXO
.fnc console_protocol           con_ptcl.gml        QXO
.fnc console_read               con_rd.gml          QXO
.fnc console_size               con_siz.gml         QXO
.fnc console_state              con_stat.gml        QXO
.fnc console_write              con_wr.gml          QXO
.fnc _control87                 control8.gml    DOS QNX W WIN32
.fn7 _controlfp                 controlf.gml    DOS QNX W WIN32
.fnc copysign                   copysign.gml    DOS QNX W WIN32
.fnc cos                        cos.gml         DOS QNX W WIN32
.fnc cosh                       cosh.gml        DOS QNX W WIN32
.fnc cprintf                    cprintf.gml     DOS QNX W WIN32
.fnc cputs                      cputs.gml       DOS QNX W WIN32
.fnc creat                      creat.gml       DOS QNX W WIN32
.fnc _creat                     creat.gml       DOS       WIN32
.fnw _wcreat                    creat.gml       DOS       WIN32
.fnc Creceive                   creceive.gml        QXO
.fnc Creceivemx                 crecvmx.gml         QXO
.fnc cscanf                     cscanf.gml      DOS QNX W WIN32
.fnc ctermid                    ctermid.gml         QXO
.fnc ctime                      ctime.gml       DOS QNX W WIN32
.fnc _ctime                     ctime.gml       DOS QNX W WIN32
.fnw _wctime                    ctime.gml       DOS QNX W WIN32
.fnw __wctime                   ctime.gml       DOS QNX W WIN32
.fnc ctime_s                    ctime_s.gml     DOS       WIN32
.fnw _wctime_s                  ctime_s.gml     DOS       WIN32
.fnc cuserid                    cuserid.gml         QXO
.fnc cwait                      cwait.gml       DOS       WIN32
.fnc _cwait                     cwait.gml       DOS       WIN32
.fnc delay                      delay.gml       DOS QNX W WIN32
.fnc dev_arm                    dev_arm.gml         QXO
.fnc dev_fdinfo                 dev_fdin.gml        QXO
.fnc dev_info                   dev_info.gml        QXO
.fnc dev_insert_chars           dev_ins.gml         QXO
.fnc dev_ischars                dev_isch.gml        QXO
.fnc dev_mode                   dev_mode.gml        QXO
.fnc dev_read                   dev_read.gml        QXO
.fnc dev_state                  dev_stat.gml        QXO
.fn8 _dieeetomsbin              dieee2ms.gml    DOS QNX W WIN32
.fnc difftime                   difftime.gml    DOS QNX W WIN32
.fnc dirname                    dirname.gml     DOS QNX W WIN32
.fnc _disable                   _disable.gml    DOS QNX W WIN32
.fnc disk_get_entry             disk_get.gml        QXO
.fnc disk_space                 disk_spa.gml        QXO
.fnc _displaycursor             gr_dispc.gml    DOS QNX W GRAPH
.fnc div                        div.gml         DOS QNX W WIN32
.fn8 _dmsbintoieee              dms2ieee.gml    DOS QNX W WIN32
.fnc _dos_allocmem              allocmem.gml    DOS       WIN32
.fnc _dos_close                 d_close.gml     DOS       WIN32
.fnc _dos_commit                doscommi.gml    DOS       WIN32
.fnc _dos_creat                 d_creat.gml     DOS       WIN32
.fnc _dos_creatnew              creatnew.gml    DOS       WIN32
.fnc dosexterr                  dosexter.gml    DOS
.fnc _dos_find&grpsfx           dosfind.gml     DOS       WIN32
.fnc _dos_findclose             dosfind.gml     DOS       WIN32
.fnc _dos_findfirst             dosfind.gml     DOS       WIN32
.fnc _dos_findnext              dosfind.gml     DOS       WIN32
.fnw _wdos_findclose            dosfind.gml     DOS       WIN32
.fnw _wdos_findfirst            dosfind.gml     DOS       WIN32
.fnw _wdos_findnext             dosfind.gml     DOS       WIN32
.fnc _dos_freemem               freemem.gml     DOS       WIN32
.fnc _dos_getdate               getdate.gml     DOS       WIN32
.fnc _dos_getdiskfree           dgetdskf.gml    DOS       WIN32
.fnc _dos_getdrive              dgetdriv.gml    DOS       WIN32
.fnc _dos_getfileattr           getfattr.gml    DOS       WIN32
.fnc _dos_getftime              getftime.gml    DOS       WIN32
.fnc _dos_gettime               gettime.gml     DOS       WIN32
.fnc _dos_getvect               getvect.gml     DOS
.fnc _dos_keep                  keep.gml        DOS
.fnc _dos_open                  d_open.gml      DOS       WIN32
.fnc _dos_read                  d_read.gml      DOS       WIN32
.fnc _dos_setblock              setblock.gml    DOS
.fnc _dos_setdate               setdate.gml     DOS       WIN32
.fnc _dos_setdrive              setdrive.gml    DOS       WIN32
.fnc _dos_setfileattr           setfattr.gml    DOS       WIN32
.fnc _dos_setftime              setftime.gml    DOS       WIN32
.fnc _dos_settime               settime.gml     DOS       WIN32
.fnc _dos_setvect               setvect.gml     DOS
.fnc _dos_write                 d_write.gml     DOS       WIN32
.fnc dup                        dup.gml         DOS QNX W WIN32
.fnc _dup                       dup.gml         DOS       WIN32
.fnc dup2                       dup2.gml        DOS QNX W WIN32
.fnc _dup2                      dup2.gml        DOS       WIN32
.fnc _dwDeleteOnClose           _dwdelcl.gml    DOS       WIN32
.fnc _dwSetAboutDlg             _dwstabo.gml    DOS       WIN32
.fnc _dwSetAppTitle             _dwstapt.gml    DOS       WIN32
.fnc _dwSetConTitle             _dwstcnt.gml    DOS       WIN32
.fnc _dwShutDown                _dwshutd.gml    DOS       WIN32
.fnc _dwYield                   _dwyield.gml    DOS       WIN32
.fnc ecvt                       ecvt.gml        DOS QNX W WIN32
.fnc _ecvt                      ecvt.gml        DOS QNX W WIN32
.fnw _wecvt                     ecvt.gml        DOS QNX W WIN32
.fnc _ellipse                   gr_ellip.gml    DOS QNX W GRAPH
.fnc _ellipse_w                 gr_ellip.gml    DOS QNX W GRAPH
.fnc _ellipse_wxy               gr_ellip.gml    DOS QNX W GRAPH
.fnc _enable                    _enable.gml     DOS QNX W WIN32
.fnc endpwent                   endpwent.gml    DOS QNX W WIN32
.fnc _endthread                 _ethread.gml    DOS QXO   WIN32
.fnc _endthreadex               _ethread.gml    DOS       WIN32
.fnc eof                        eof.gml         DOS QNX W WIN32
.fnc _eof                       eof.gml         DOS       WIN32
.fnc erf                        erf.gml         DOS QNX W WIN32
.fnc erfc                       erfc.gml        DOS QNX W WIN32
.fnc errno                      errno.gml           QXO
.fnc exec&grpsfx                exec.gml        DOS QNX W WIN32
.fnc execl                      exec.gml        DOS QNX W WIN32
.fnc _execl                     exec.gml        DOS QNX W WIN32
.fnw _wexecl                    exec.gml        DOS       WIN32
.fnc execle                     exec.gml        DOS QNX W WIN32
.fnc _execle                    exec.gml        DOS QNX W WIN32
.fnw _wexecle                   exec.gml        DOS       WIN32
.fnc execlp                     exec.gml        DOS QNX W WIN32
.fnc _execlp                    exec.gml        DOS QNX W WIN32
.fnw _wexeclp                   exec.gml        DOS       WIN32
.fnc execlpe                    exec.gml        DOS QNX W WIN32
.fnc _execlpe                   exec.gml        DOS QNX W WIN32
.fnw _wexeclpe                  exec.gml        DOS       WIN32
.fnc execv                      exec.gml        DOS QNX W WIN32
.fnc _execv                     exec.gml        DOS QNX W WIN32
.fnw _wexecv                    exec.gml        DOS       WIN32
.fnc execve                     exec.gml        DOS QNX W WIN32
.fnc _execve                    exec.gml        DOS QNX W WIN32
.fnw _wexecve                   exec.gml        DOS       WIN32
.fnc execvp                     exec.gml        DOS QNX W WIN32
.fnc _execvp                    exec.gml        DOS QNX W WIN32
.fnw _wexecvp                   exec.gml        DOS       WIN32
.fnc execvpe                    exec.gml        DOS QNX W WIN32
.fnc _execvpe                   exec.gml        DOS QNX W WIN32
.fnw _wexecvpe                  exec.gml        DOS       WIN32
.fnc _Exit                      _exit.gml       DOS QNX W WIN32
.fnc _exit                      _exit.gml       DOS QNX W WIN32
.fnc exit                       exit.gml        DOS QNX W WIN32
.fnc exp                        exp.gml         DOS QNX W WIN32
.fnc exp2                       exp2.gml        DOS QNX W WIN32
.fnc expm1                      expm1.gml       DOS QNX W WIN32
.fnc _expand                    _expand.gml     DOS QNX W WIN32
.fnc _bexpand                   _expand.gml     DOS QNX W
.fnf _fexpand                   _expand.gml     DOS QNX W
.fnc _nexpand                   _expand.gml     DOS QNX W WIN32
.fnc fabs                       fabs.gml        DOS QNX W WIN32
.fnc fchmod                     fchmod.gml          QXO
.fnc fchown                     fchown.gml          QXO
.fnc fclose                     fclose.gml      DOS QNX W WIN32
.fnc fcloseall                  fcloseal.gml    DOS QNX W WIN32
.fnc fcntl                      fcntl.gml           QXO
.fnc fcvt                       fcvt.gml        DOS QNX W WIN32
.fnc _fcvt                      fcvt.gml        DOS QNX W WIN32
.fnw _wfcvt                     fcvt.gml        DOS QNX W WIN32
.fnc fdatasync                  fdsync.gml          QXO
.fnc fdim                       fdim.gml        DOS QNX W WIN32
.fnc fdopen                     fdopen.gml      DOS QNX W WIN32
.fnc _fdopen                    fdopen.gml      DOS QNX W WIN32
.fnw _wfdopen                   fdopen.gml      DOS QNX W WIN32
.fnc feclearexcept              fecleare.gml    DOS QNX W WIN32
.fnc fedisableexcept            fedisabl.gml    DOS QNX W WIN32
.fnc __fedisableexcept          fedisabl.gml    DOS QNX W WIN32
.fnc feenableexcept             feenable.gml    DOS QNX W WIN32
.fnc __feenableexcept           feenable.gml    DOS QNX W WIN32
.fnc fegetenv                   fegetenv.gml    DOS QNX W WIN32
.fnc fegetexceptflag            fegetexc.gml    DOS QNX W WIN32
.fnc fegetround                 fegetrnd.gml    DOS QNX W WIN32
.fnc feholdexcept               feholdex.gml    DOS QNX W WIN32
.fnc feof                       feof.gml        DOS QNX W WIN32
.fnc feraiseexcept              feraise.gml     DOS QNX W WIN32
.fnc ferror                     ferror.gml      DOS QNX W WIN32
.fnc fesetenv                   fesetenv.gml    DOS QNX W WIN32
.fnc fesetexceptflag            fesetexc.gml    DOS QNX W WIN32
.fnc fesetround                 fesetrnd.gml    DOS QNX W WIN32
.fnc fetestexcept               fetestex.gml    DOS QNX W WIN32
.fnc feupdateenv                feupdenv.gml    DOS QNX W WIN32
.fnc fflush                     fflush.gml      DOS QNX W WIN32
.fnc ffs                        ffs.gml         DOS QNX W WIN32
.fnc fgetc                      fgetc.gml       DOS QNX W WIN32
.fnw fgetwc                     fgetc.gml       DOS QNX W WIN32
.fnc fgetchar                   fgetchar.gml    DOS QNX W WIN32
.fn7 _fgetchar                  fgetchar.gml    DOS QNX W WIN32
.fnw _fgetwchar                 fgetchar.gml    DOS QNX W WIN32
.fnc fgetpos                    fgetpos.gml     DOS QNX W WIN32
.fnc fgets                      fgets.gml       DOS QNX W WIN32
.fnw fgetws                     fgets.gml       DOS QNX W WIN32
.fn8 _fieeetomsbin              fieee2ms.gml    DOS QNX W WIN32
.fnc filelength                 fileleng.gml    DOS QNX W WIN32
.fnc _filelength                fileleng.gml    DOS       WIN32
.fn8 _filelengthi64             fileleng.gml    DOS       WIN32
.fnc FILENAME_MAX               filenmax.gml    DOS QNX W WIN32
.fnc fileno                     fileno.gml      DOS QNX W WIN32
.fn7 _findclose                 findclos.gml    DOS       WIN32
.fn7 _findfirst                 findfirs.gml    DOS       WIN32
.fn8 _findfirsti64              findfirs.gml    DOS       WIN32
.fnw _wfindfirst                findfirs.gml    DOS       WIN32
.fnw _wfindfirsti64             findfirs.gml    DOS       WIN32
.fn7 _findnext                  findnext.gml    DOS       WIN32
.fn7 _findnexti64               findnext.gml    DOS       WIN32
.fnw _wfindnext                 findnext.gml    DOS       WIN32
.fnw _wfindnexti64              findnext.gml    DOS       WIN32
.fn7 _finite                    _finite.gml     DOS QNX W WIN32
.fnc _floodfill                 gr_flood.gml    DOS QNX W GRAPH
.fnc _floodfill_w               gr_flood.gml    DOS QNX W GRAPH
.fnc floor                      floor.gml       DOS QNX W WIN32
.fnc flushall                   flushall.gml    DOS QNX W WIN32
.fnc fma                        fma.gml         DOS QNX W WIN32
.fnc fmax                       fmax.gml        DOS QNX W WIN32
.fnc fmin                       fmin.gml        DOS QNX W WIN32
.fnc fmod                       fmod.gml        DOS QNX W WIN32
.fn8 _fmsbintoieee              fms2ieee.gml    DOS QNX W WIN32
.fnc fnmatch                    fnmatch.gml     DOS QXO W WIN32
.fnc fopen                      fopen.gml       DOS QNX W WIN32
.fnw _wfopen                    fopen.gml       DOS QNX W WIN32
.fnc fopen_s                    fopen_s.gml     DOS QNX W WIN32
.fnc _wfopen_s                  fopen_s.gml     DOS QNX W WIN32
.fnc fork                       fork.gml            QXO
.fnc FP_OFF                     fp_off.gml      DOS QNX W WIN32
.fnc FP_SEG                     fp_seg.gml      DOS QNX W WIN32
.fnc fpathconf                  fpathcnf.gml        QXO
.fnc fpclassify                 fpclassf.gml    DOS QNX W WIN32
.fnc _fpreset                   _fpreset.gml    DOS QNX W WIN32
.fnc fprintf                    fprintf.gml     DOS QNX W WIN32
.fnw fwprintf                   fprintf.gml     DOS QNX W WIN32
.fnc fprintf_s                  fprntf_s.gml    DOS QNX W WIN32
.fnw fwprintf_s                 fprntf_s.gml    DOS QNX W WIN32
.fnc fputc                      fputc.gml       DOS QNX W WIN32
.fnw fputwc                     fputc.gml       DOS QNX W WIN32
.fnc fputchar                   fputchar.gml    DOS QNX W WIN32
.fnc _fputchar                  fputchar.gml    DOS QNX W WIN32
.fnw _fputwchar                 fputchar.gml    DOS QNX W WIN32
.fnc fputs                      fputs.gml       DOS QNX W WIN32
.fnw fputws                     fputs.gml       DOS QNX W WIN32
.fnc fread                      fread.gml       DOS QNX W WIN32
.fnc free                       free.gml        DOS QNX W WIN32
.fnc _bfree                     free.gml        DOS QNX W
.fnf _ffree                     free.gml        DOS QNX W
.fnc _nfree                     free.gml        DOS QNX W WIN32
.fnc _freect                    _freect.gml     DOS QNX W WIN32
.fnc freopen                    freopen.gml     DOS QNX W WIN32
.fnw _wfreopen                  freopen.gml     DOS QNX W WIN32
.fnc freopen_s                  freop_s.gml     DOS QNX W WIN32
.fnc _wfreopen_s                freop_s.gml     DOS QNX W WIN32
.fnc frexp                      frexp.gml       DOS QNX W WIN32
.fnc fscanf                     fscanf.gml      DOS QNX W WIN32
.fnw fwscanf                    fscanf.gml      DOS QNX W WIN32
.fnc fscanf_s                   fscanf_s.gml    DOS QNX W WIN32
.fnw fwscanf_s                  fscanf_s.gml    DOS QNX W WIN32
.fnc fseek                      fseek.gml       DOS QNX W WIN32
.fnc fsetpos                    fsetpos.gml     DOS QNX W WIN32
.fnc _fsopen                    _fsopen.gml     DOS QNX W WIN32
.fnw _wfsopen                   _fsopen.gml     DOS QNX W WIN32
.fnc fstat                      fstat.gml       DOS QNX W WIN32
.fnc _fstat                     fstat.gml       DOS       WIN32
.fn8 _fstati64                  fstat.gml       DOS       WIN32
.fnw _wfstat                    fstat.gml       DOS       WIN32
.fnw _wfstati64                 fstat.gml       DOS       WIN32
.fn8 fsync                      fsync.gml       DOS QNX W WIN32
.fnc fsys_fdinfo                fs_fdin.gml         QXO
.fnc fsys_fstat                 fs_fstat.gml        QXO
.fnc fsys_get_mount_dev         fsys_gmd.gml        QXO
.fnc fsys_get_mount_pt          fsys_gmp.gml        QXO
.fnc fsys_stat                  fs_stat.gml         QXO
.fnc ftell                      ftell.gml       DOS QNX W WIN32
.fnc ftime                      ftime.gml       DOS QNX W WIN32
.fnc _fullpath                  _fullpat.gml    DOS QNX W WIN32
.fnw _wfullpath                 _fullpat.gml    DOS       WIN32
.fnw fwide                      fwide.gml       DOS QNX W WIN32
.fnc fwrite                     fwrite.gml      DOS QNX W WIN32
.fnc gcvt                       gcvt.gml        DOS QNX W WIN32
.fnc _gcvt                      gcvt.gml        DOS QNX W WIN32
.fnw _wgcvt                     gcvt.gml        DOS QNX W WIN32
.fnc _getactivepage             gr_getap.gml    DOS QNX W GRAPH
.fnc _getarcinfo                gr_getai.gml    DOS QNX W GRAPH
.fnc _getbkcolor                gr_getbc.gml    DOS QNX W GRAPH
.fnc getc                       getc.gml        DOS QNX W WIN32
.fnw getwc                      getc.gml        DOS QNX W WIN32
.fnc getch                      getch.gml       DOS QNX W WIN32
.fnc getchar                    getchar.gml     DOS QNX W WIN32
.fnw getwchar                   getchar.gml     DOS QNX W WIN32
.fnc getche                     getche.gml      DOS QNX W WIN32
.fnc _getcliprgn                gr_getcr.gml    DOS QNX W GRAPH
.fnc getcmd                     getcmd.gml      DOS QNX W WIN32
.fnc _getcolor                  gr_getc.gml     DOS QNX W GRAPH
.fnc _getcurrentposition        gr_getcp.gml    DOS QNX W GRAPH
.fnc _getcurrentposition_w      gr_getcp.gml    DOS QNX W GRAPH
.fnc getcwd                     getcwd.gml      DOS QNX W WIN32
.fnw _wgetcwd                   getcwd.gml      DOS       WIN32
.fn7 _getdcwd                   getdcwd.gml     DOS       WIN32
.fnw _wgetdcwd                  getdcwd.gml     DOS       WIN32
.fnc getdelim                   getdelim.gml    DOS QNX W WIN32
.fnc _getdiskfree               getdiskf.gml    DOS       WIN32
.fnc _getdrive                  getdrive.gml    DOS       WIN32
.fnc getegid                    getegid.gml         QXO
.fnc getenv                     getenv.gml      DOS QNX W WIN32
.fnw _wgetenv                   getenv.gml      DOS QNX W WIN32
.fnc getenv_s                   getenv_s.gml    DOS QNX W WIN32
.fnc geteuid                    geteuid.gml         QXO
.fnc _getfillmask               gr_getfm.gml    DOS QNX W GRAPH
.fnc _getfontinfo               gr_getfi.gml    DOS QNX W GRAPH
.fnc getgid                     getgid.gml          QXO
.fnc getgrent                   getgrent.gml        QXO
.fnc endgrent                   getgrent.gml        QXO
.fnc setgrent                   getgrent.gml        QXO
.fnc getgrgid                   getgrgid.gml        QXO
.fnc getgrnam                   getgrnam.gml        QXO
.fnc getgroups                  getgrps.gml         QXO
.fnc _getgtextextent            gr_getge.gml    DOS QNX W GRAPH
.fnc _getgtextvector            gr_gettv.gml    DOS QNX W GRAPH
.fnc _getimage                  gr_getim.gml    DOS QNX W GRAPH
.fnc _getimage_w                gr_getim.gml    DOS QNX W GRAPH
.fnc _getimage_wxy              gr_getim.gml    DOS QNX W GRAPH
.fnc getline                    getline.gml     DOS QNX W WIN32
.fnc _getlinestyle              gr_getls.gml    DOS QNX W GRAPH
.fnc getlogin                   getlogin.gml        QXO
.fnm _getmbcp                   getmbcp.gml     DOS       WIN32
.fnc getnid                     getnid.gml          QXO
.fnc getopt                     getopt.gml      DOS QXO   WIN32
.fn7 _get_osfhandle             getosfha.gml    DOS       WIN32
.fnc getpgrp                    getpgrp.gml         QXO
.fnc _getphyscoord              gr_getpc.gml    DOS QNX W GRAPH
.fnc getpid                     getpid.gml      DOS QXO   WIN32
.fnc _getpid                    getpid.gml      DOS QXO   WIN32
.fnc _getpixel                  gr_getpx.gml    DOS QNX W GRAPH
.fnc _getpixel_w                gr_getpx.gml    DOS QNX W GRAPH
.fnc _getplotaction             gr_getpa.gml    DOS QNX W GRAPH
.fnc getppid                    getppid.gml         QXO
.fnc getprio                    getprio.gml         QXO
.fnc getpwent                   getpwent.gml    DOS QNX W WIN32
.fnc getpwnam                   getpwnam.gml    DOS QNX W WIN32
.fnc getpwuid                   getpwuid.gml    DOS QNX W WIN32
.fnc gets                       gets.gml        DOS QNX W WIN32
.fnw _getws                     gets.gml        DOS QNX W WIN32
.fnc gets_s                     gets_s.gml      DOS QNX W WIN32
.fnc _gettextcolor              gr_gettc.gml    DOS QNX W GRAPH
.fnc _gettextcursor             gr_gettu.gml    DOS QNX W GRAPH
.fnc _gettextextent             gr_gette.gml    DOS QNX W GRAPH
.fnc _gettextposition           gr_gettp.gml    DOS QNX W GRAPH
.fnc _gettextsettings           gr_getts.gml    DOS QNX W GRAPH
.fnc _gettextwindow             gr_gettw.gml    DOS QNX W GRAPH
.fnc getuid                     getuid.gml          QXO
.fnc _getvideoconfig            gr_getvc.gml    DOS QNX W GRAPH
.fnc _getviewcoord              gr_getlc.gml    DOS QNX W GRAPH
.fnc _getviewcoord_w            gr_getlc.gml    DOS QNX W GRAPH
.fnc _getviewcoord_wxy          gr_getlc.gml    DOS QNX W GRAPH
.fnc _getvisualpage             gr_getvp.gml    DOS QNX W GRAPH
.fnc _getw                      getw.gml        DOS QXO   WIN32
.fnc _getwindowcoord            gr_getwc.gml    DOS QNX W GRAPH
.fnc gmtime                     gmtime.gml      DOS QNX W WIN32
.fnc _gmtime                    gmtime.gml      DOS QNX W WIN32
.fnc gmtime_s                   gmtime_s.gml    DOS       WIN32
.fnc _grow_handles              _grow_ha.gml    DOS QXO   WIN32
.fnc _grstatus                  gr_stat.gml     DOS QNX W GRAPH
.fnc _grtext                    gr_grtxt.gml    DOS QNX W GRAPH
.fnc _grtext_w                  gr_grtxt.gml    DOS QNX W GRAPH
.fnc halloc                     halloc.gml      DOS QNX W
.fnc hantozen                   hantozen.gml    NEC
.fnc _harderr                   _harderr.gml    DOS
.fnc _hardresume                _harderr.gml    DOS
.fnc _hardretn                  _harderr.gml    DOS
.fnc _hdopen                    _hdopen.gml     DOS QXO   WIN32
.fnc _heapchk                   _heapchk.gml    DOS QNX W WIN32
.fnc _bheapchk                  _heapchk.gml    DOS QNX W
.fnf _fheapchk                  _heapchk.gml    DOS QNX W
.fnc _nheapchk                  _heapchk.gml    DOS QNX W WIN32
.fnc _heapenable                _heapena.gml    DOS QNX W WIN32
.fnc _heapgrow                  _heapgrw.gml    DOS QNX W WIN32
.fnf _fheapgrow                 _heapgrw.gml    DOS QNX W
.fnc _nheapgrow                 _heapgrw.gml    DOS QNX W WIN32
.fnc _heapmin                   _heapmin.gml    DOS QNX W WIN32
.fnc _bheapmin                  _heapmin.gml    DOS QNX W
.fnf _fheapmin                  _heapmin.gml    DOS QNX W
.fnc _nheapmin                  _heapmin.gml    DOS QNX W WIN32
.fnc _heapset                   _heapset.gml    DOS QNX W WIN32
.fnc _bheapset                  _heapset.gml    DOS QNX W
.fnf _fheapset                  _heapset.gml    DOS QNX W
.fnc _nheapset                  _heapset.gml    DOS QNX W WIN32
.fnc _heapshrink                _heapshr.gml    DOS QNX W WIN32
.fnc _bheapshrink               _heapshr.gml    DOS QNX W
.fnf _fheapshrink               _heapshr.gml    DOS QNX W
.fnc _nheapshrink               _heapshr.gml    DOS QNX W WIN32
.fnc _heapwalk                  _heapwal.gml    DOS QNX W WIN32
.fnc _bheapwalk                 _heapwal.gml    DOS QNX W
.fnf _fheapwalk                 _heapwal.gml    DOS QNX W
.fnc _nheapwalk                 _heapwal.gml    DOS QNX W WIN32
.fnc hfree                      hfree.gml       DOS QNX W
.fnc hypot                      hypot.gml       DOS QNX W WIN32
.fnc ignore_handler_s           ign_hnd.gml     DOS QNX W WIN32
.fnc ilogb                      ilogb.gml       DOS QNX W WIN32
.fnc _imagesize                 gr_imsiz.gml    DOS QNX W GRAPH
.fnc _imagesize_w               gr_imsiz.gml    DOS QNX W GRAPH
.fnc _imagesize_wxy             gr_imsiz.gml    DOS QNX W GRAPH
.fnc imaxabs                    imaxabs.gml     DOS QNX W WIN32
.fnc imaxdiv                    imaxdiv.gml     DOS QNX W WIN32
.fnc inp                        inp.gml         DOS QNX W WIN32
.fnc inpd                       inpd.gml        DOS QNX W WIN32
.fnc input_line                 input_ln.gml        QXO
.fnc inpw                       inpw.gml        DOS QNX W WIN32
.fnc int386                     int386.gml      DOS QNX W
.fnc int386x                    int386x.gml     DOS QNX W
.fnc int86                      int86.gml       DOS QNX W
.fnc int86x                     int86x.gml      DOS QNX W
.fnc intdos                     intdos.gml      DOS
.fnc intdosx                    intdosx.gml     DOS
.fnc intr                       intr.gml        DOS QNX W
.fnc isalkana                   isalkana.gml    NEC
.fnc isalnmkana                 isalnmkana.gml  NEC
.fnc isalnum                    isalnum.gml     DOS QNX W WIN32
.fnw iswalnum                   isalnum.gml     DOS QNX W WIN32
.fnc isalpha                    isalpha.gml     DOS QNX W WIN32
.fnw iswalpha                   isalpha.gml     DOS QNX W WIN32
.fnc isascii                    isascii.gml     DOS QNX W WIN32
.fnc __isascii                  isascii.gml     DOS QNX W WIN32
.fnw iswascii                   isascii.gml     DOS QNX W WIN32
.fnc isatty                     isatty.gml      DOS QXO   WIN32
.fnc _isatty                    isatty.gml      DOS       WIN32
.fnc isblank                    isblank.gml     DOS QNX W WIN32
.fnw iswblank                   isblank.gml     DOS QNX W WIN32
.fnc iscntrl                    iscntrl.gml     DOS QNX W WIN32
.fnw iswcntrl                   iscntrl.gml     DOS QNX W WIN32
.fnc iscsym                     iscsym.gml      DOS QNX W WIN32
.fnc __iscsym                   iscsym.gml      DOS QNX W WIN32
.fnw __iswcsym                  iscsym.gml      DOS QNX W WIN32
.fnc iscsymf                    iscsymf.gml     DOS QNX W WIN32
.fnc __iscsymf                  iscsymf.gml     DOS QNX W WIN32
.fnw __iswcsymf                 iscsymf.gml     DOS QNX W WIN32
.fnc isdigit                    isdigit.gml     DOS QNX W WIN32
.fnw iswdigit                   isdigit.gml     DOS QNX W WIN32
.fnc isfinite                   isfinite.gml    DOS QNX W WIN32
.fnc isgraph                    isgraph.gml     DOS QNX W WIN32
.fnw iswgraph                   isgraph.gml     DOS QNX W WIN32
.fnc isgrkana                   isgrkana.gml    NEC
.fnc isinf                      isinf.gml       DOS QNX W WIN32
.fnc iskana                     iskana.gml      NEC
.fnc iskanji                    iskanji.gml     NEC
.fnc iskanji2                   iskanji2.gml    NEC
.fnc iskmoji                    iskmoji.gml     NEC
.fnc iskpun                     iskpun.gml      NEC
.fnm isleadbyte                 isleadb.gml     DOS       WIN32
.fnc islower                    islower.gml     DOS QNX W WIN32
.fnw iswlower                   islower.gml     DOS QNX W WIN32
.fnm _ismbbalnum                ismbbaln.gml    DOS       WIN32
.fnm _ismbbalpha                ismbbalp.gml    DOS       WIN32
.fnm _ismbbgraph                ismbbgra.gml    DOS       WIN32
.fnm _ismbbkalnum               ismbbkal.gml    DOS       WIN32
.fnm _ismbbkana                 ismbbkan.gml    DOS       WIN32
.fnm _ismbbkalpha               ismbbkap.gml    DOS       WIN32
.fnm _ismbbkprint               ismbbkpr.gml    DOS       WIN32
.fnm _ismbbkpunct               ismbbkpu.gml    DOS       WIN32
.fnm _ismbblead                 ismbblea.gml    DOS       WIN32
.fnm _ismbbprint                ismbbpri.gml    DOS       WIN32
.fnm _ismbbpunct                ismbbpun.gml    DOS       WIN32
.fnm _ismbbtrail                ismbbtra.gml    DOS       WIN32
.fnm _ismbcalnum                ismbcaln.gml    DOS       WIN32
.fnm _ismbcalpha                ismbcalp.gml    DOS       WIN32
.fnm _ismbccntrl                ismbccnt.gml    DOS       WIN32
.fnm _ismbcdigit                ismbcdig.gml    DOS       WIN32
.fnm _ismbcgraph                ismbcgra.gml    DOS       WIN32
.fnm _ismbchira                 ismbchir.gml    DOS       WIN32
.fnm _ismbckata                 ismbckat.gml    DOS       WIN32
.fnm _ismbcl0                   ismbcl0.gml     DOS       WIN32
.fnm _ismbcl1                   ismbcl1.gml     DOS       WIN32
.fnm _ismbcl2                   ismbcl2.gml     DOS       WIN32
.fnm _ismbclegal                ismbcleg.gml    DOS       WIN32
.fnm _ismbclower                ismbclow.gml    DOS       WIN32
.fnm _ismbcprint                ismbcpri.gml    DOS       WIN32
.fnm _ismbcpunct                ismbcpun.gml    DOS       WIN32
.fnm _ismbcspace                ismbcspa.gml    DOS       WIN32
.fnm _ismbcsymbol               ismbcsym.gml    DOS       WIN32
.fnm _ismbcupper                ismbcupp.gml    DOS       WIN32
.fnm _ismbcxdigit               ismbcxdi.gml    DOS       WIN32
.fnc isnan                      isnan.gml       DOS QNX W WIN32
.fnc isnormal                   isnormal.gml    DOS QNX W WIN32
.fnc ispnkana                   ispnkana.gml    NEC
.fnc isprint                    isprint.gml     DOS QNX W WIN32
.fnw iswprint                   isprint.gml     DOS QNX W WIN32
.fnc isprkana                   isprkana.gml    NEC
.fnc ispunct                    ispunct.gml     DOS QNX W WIN32
.fnw iswpunct                   ispunct.gml     DOS QNX W WIN32
.fnc isspace                    isspace.gml     DOS QNX W WIN32
.fnw iswspace                   isspace.gml     DOS QNX W WIN32
.fnc isupper                    isupper.gml     DOS QNX W WIN32
.fnw iswupper                   isupper.gml     DOS QNX W WIN32
.fnw iswctype                   iswctype.gml    DOS QNX W WIN32
.fnc isxdigit                   isxdigit.gml    DOS QNX W WIN32
.fnw iswxdigit                  isxdigit.gml    DOS QNX W WIN32
.fnc itoa                       itoa.gml        DOS QNX W WIN32
.fnc _itoa                      itoa.gml        DOS QNX W WIN32
.fnw _itow                      itoa.gml        DOS QNX W WIN32
.fnc jasctime                   jasctime.gml    NEC
.fnc jctime                     jctime.gml      NEC
.fnc jgetmoji                   jgetmoji.gml    NEC
.fnf _fjgetmoji                 jgetmoji.gml    NEC
.fnc jisalpha                   jisalpha.gml    NEC
.fnc jisdigit                   jisdigit.gml    NEC
.fnc jishira                    jishira.gml     NEC
.fnc jiskana                    jiskana.gml     NEC
.fnc jiskigou                   jiskigou.gml    NEC
.fnc jisl0                      jisl0.gml       NEC
.fnc jisl1                      jisl1.gml       NEC
.fnc jisl2                      jisl2.gml       NEC
.fnc jislower                   jislower.gml    NEC
.fnc jisprint                   jisprint.gml    NEC
.fnc jisspace                   jisspace.gml    NEC
.fnc jistojms                   jistojms.gml    NEC
.fnc jisupper                   jisupper.gml    NEC
.fnc jiszen                     jiszen.gml      NEC
.fnc jmstojis                   jmstojis.gml    NEC
.fnc jperror                    jperror.gml     NEC
.fnc jputmoji                   jputmoji.gml    NEC
.fnf _fjputmoji                 jputmoji.gml    NEC
.fnc jstradv                    jstradv.gml     NEC
.fnf _fjstradv                  jstradv.gml     NEC
.fnc jstrcat                    jstrcat.gml     NEC
.fnf _fjstrcat                  jstrcat.gml     NEC
.fnc jstrchr                    jstrchr.gml     NEC
.fnf _fjstrchr                  jstrchr.gml     NEC
.fnc jstrcmp                    jstrcmp.gml     NEC
.fnf _fjstrcmp                  jstrcmp.gml     NEC
.fnc jstrcspn                   jstrcspn.gml    NEC
.fnf _fjstrcspn                 jstrcspn.gml    NEC
.fnc jstrerror                  jstrerror.gml   NEC
.fnc jstricmp                   jstricmp.gml    NEC
.fnf _fjstricmp                 jstricmp.gml    NEC
.fnc jstrlen                    jstrlen.gml     NEC
.fnf _fjstrlen                  jstrlen.gml     NEC
.fnc jstrlwr                    jstrlwr.gml     NEC
.fnf _fjstrlwr                  jstrlwr.gml     NEC
.fnc jstrmatch                  jstrmatc.gml    NEC
.fnf _fjstrmatch                jstrmatc.gml    NEC
.fnc jstrncat                   jstrncat.gml    NEC
.fnf _fjstrncat                 jstrncat.gml    NEC
.fnc jstrncmp                   jstrncmp.gml    NEC
.fnf _fjstrncmp                 jstrncmp.gml    NEC
.fnc jstrncpy                   jstrncpy.gml    NEC
.fnf _fjstrncpy                 jstrncpy.gml    NEC
.fnc jstrnicmp                  jstrnicm.gml    NEC
.fnf _fjstrnicmp                jstrnicm.gml    NEC
.fnc jstrnset                   jstrnset.gml    NEC
.fnf _fjstrnset                 jstrnset.gml    NEC
.fnc jstrrchr                   jstrrchr.gml    NEC
.fnf _fjstrrchr                 jstrrchr.gml    NEC
.fnc jstrrev                    jstrrev.gml     NEC
.fnf _fjstrrev                  jstrrev.gml     NEC
.fnc jstrset                    jstrset.gml     NEC
.fnf _fjstrset                  jstrset.gml     NEC
.fnc jstrskip                   jstrskip.gml    NEC
.fnf _fjstrskip                 jstrskip.gml    NEC
.fnc jstrspn                    jstrspn.gml     NEC
.fnf _fjstrspn                  jstrspn.gml     NEC
.fnc jstrstr                    jstrstr.gml     NEC
.fnf _fjstrstr                  jstrstr.gml     NEC
.fnc jstrtok                    jstrtok.gml     NEC
.fnf _fjstrtok                  jstrtok.gml     NEC
.fnc jstrupr                    jstrupr.gml     NEC
.fnf _fjstrupr                  jstrupr.gml     NEC
.fnc jtohira                    jtohira.gml     NEC
.fnc jtokata                    jtokata.gml     NEC
.fnc jtolower                   jtolower.gml    NEC
.fnc jtoupper                   jtoupper.gml    NEC
.fnc kbhit                      kbhit.gml       DOS QNX W WIN32
.fnc _kbhit                     kbhit.gml       DOS QNX W WIN32
.fnc kill                       kill.gml            QXO
.fnc labs                       labs.gml        DOS QNX W WIN32
.fnc ldexp                      ldexp.gml       DOS QNX W WIN32
.fnc ldiv                       ldiv.gml        DOS QNX W WIN32
.fnc lfind                      lfind.gml       DOS QNX W WIN32
.fnc lgamma                     lgamma.gml      DOS QNX W WIN32
.fnc lgamma_r                   lgamma_r.gml    DOS QNX W WIN32
.fnc _lineto                    gr_linto.gml    DOS QNX W GRAPH
.fnc _lineto_w                  gr_linto.gml    DOS QNX W GRAPH
.fnc link                       link.gml            QXO
.fnc llabs                      llabs.gml       DOS QNX W WIN32
.fnc lldiv                      lldiv.gml       DOS QNX W WIN32
.fnc localeconv                 localeco.gml    DOS QNX W WIN32
.fnc localtime                  localtim.gml    DOS QNX W WIN32
.fnc _localtime                 localtim.gml    DOS QNX W WIN32
.fnc localtime_s                lcltim_s.gml    DOS       WIN32
.fnc lock                       lock.gml        DOS QNX W WIN32
.fnc locking                    locking.gml     DOS QNX W WIN32
.fnc _locking                   locking.gml     DOS QNX W WIN32
.fnc log                        log.gml         DOS QNX W WIN32
.fnc log10                      log10.gml       DOS QNX W WIN32
.fnc log1p                      log1p.gml       DOS QNX W WIN32
.fnc log2                       log2.gml        DOS QNX W WIN32
.fnc logb                       logb.gml        DOS QNX W WIN32
.fnc longjmp                    longjmp.gml     DOS QNX W WIN32
.fnc _lrotl                     _lrotl.gml      DOS QNX W WIN32
.fnc _lrotr                     _lrotr.gml      DOS QNX W WIN32
.fnc lsearch                    lsearch.gml     DOS QNX W WIN32
.fnc lseek                      lseek.gml       DOS QNX W WIN32
.fnc _lseek                     lseek.gml       DOS       WIN32
.fn8 _lseeki64                  lseek.gml       DOS       WIN32
.fnc lstat                      lstat.gml           QXO
.fnc lltoa                      lltoa.gml       DOS QNX W WIN32
.fnc _lltoa                     lltoa.gml       DOS QNX W WIN32
.fnw _lltow                     lltoa.gml       DOS QNX W WIN32
.fnc ltoa                       ltoa.gml        DOS QNX W WIN32
.fnc _ltoa                      ltoa.gml        DOS QNX W WIN32
.fnw _ltow                      ltoa.gml        DOS QNX W WIN32
.fnc ltrunc                     ltrunc.gml          QXO
.fnc main                       main.gml        DOS QNX W WIN32
.fnw wmain                      main.gml        DOS QNX W WIN32
.fnc WinMain                    main.gml        DOS       WIN32
.fnc wWinMain                   main.gml        DOS       WIN32
.fnc _makepath                  _makepat.gml    DOS QNX W WIN32
.fnw _wmakepath                 _makepat.gml    DOS QNX W WIN32
.fnc malloc                     malloc.gml      DOS QNX W WIN32
.fnc _bmalloc                   malloc.gml      DOS QNX W
.fnf _fmalloc                   malloc.gml      DOS QNX W
.fnc _nmalloc                   malloc.gml      DOS QNX W WIN32
.fnc matherr                    matherr.gml     DOS QNX W WIN32
.fnc max                        max.gml         DOS QNX W WIN32
.fnm _mbbtombc                  mbbtombc.gml    DOS       WIN32
.fnm _mbbtype                   mbbtype.gml     DOS       WIN32
.fnm _mbccmp                    mbccmp.gml      DOS       WIN32
.fnn _fmbccmp                   mbccmp.gml      DOS       WIN32
.fnm _mbccpy                    mbccpy.gml      DOS       WIN32
.fnn _fmbccpy                   mbccpy.gml      DOS       WIN32
.fnm _mbcicmp                   mbcicmp.gml     DOS       WIN32
.fnn _fmbcicmp                  mbcicmp.gml     DOS       WIN32
.fnm _mbcjistojms               mbcjisto.gml    DOS QNX W WIN32
.fnm _mbcjmstojis               mbcjmsto.gml    DOS QNX W WIN32
.fnm _mbclen                    mbclen.gml      DOS       WIN32
.fnn _fmbclen                   mbclen.gml      DOS       WIN32
.fnm _mbctolower                mbctolow.gml    DOS       WIN32
.fnm _mbctoupper                mbctoupp.gml    DOS       WIN32
.fnm _mbctohira                 mbctohir.gml    DOS QNX W WIN32
.fnm _mbctokata                 mbctokat.gml    DOS QNX W WIN32
.fnm _mbctombb                  mbctombb.gml    DOS       WIN32
.fnm _mbgetcode                 mbgetcod.gml    DOS       WIN32
.fnn _fmbgetcode                mbgetcod.gml    DOS       WIN32
.fnc mblen                      mblen.gml       DOS QNX W WIN32
.fnn _fmblen                    mblen.gml       DOS       WIN32
.fnm _mbputchar                 mbputcha.gml    DOS       WIN32
.fnn _fmbputchar                mbputcha.gml    DOS       WIN32
.fnm mbrlen                     mbrlen.gml      DOS       WIN32
.fnn _fmbrlen                   mbrlen.gml      DOS       WIN32
.fnm mbrtowc                    mbrtowc.gml     DOS       WIN32
.fnn _fmbrtowc                  mbrtowc.gml     DOS       WIN32
.fnm _mbsbtype                  mbsbtype.gml    DOS       WIN32
.fnn _fmbsbtype                 mbsbtype.gml    DOS       WIN32
.fnm _mbsnbcat                  mbsnbcat.gml    DOS       WIN32
.fnn _fmbsnbcat                 mbsnbcat.gml    DOS       WIN32
.fnm _mbsnbcmp                  mbsnbcmp.gml    DOS       WIN32
.fnn _fmbsnbcmp                 mbsnbcmp.gml    DOS       WIN32
.fnm _mbsnbcnt                  mbsnbcnt.gml    DOS       WIN32
.fnn _fmbsnbcnt                 mbsnbcnt.gml    DOS       WIN32
.fnm _strncnt                   mbsnbcnt.gml    DOS QNX W WIN32
.fnw _wcsncnt                   mbsnbcnt.gml    DOS QNX W WIN32
.fnm _mbsnbcpy                  mbsnbcpy.gml    DOS       WIN32
.fnn _fmbsnbcpy                 mbsnbcpy.gml    DOS       WIN32
.fnm _mbsnbicmp                 mbsnbicm.gml    DOS       WIN32
.fnn _fmbsnbicmp                mbsnbicm.gml    DOS       WIN32
.fnm _mbsnbset                  mbsnbset.gml    DOS       WIN32
.fnn _fmbsnbset                 mbsnbset.gml    DOS       WIN32
.fnm _mbsnccnt                  mbsnccnt.gml    DOS       WIN32
.fnn _fmbsnccnt                 mbsnccnt.gml    DOS       WIN32
.fnm _strncnt                   mbsnccnt.gml    DOS QNX W WIN32
.fnw _wcsncnt                   mbsnccnt.gml    DOS QNX W WIN32
.fnm _mbsnextc                  mbsnextc.gml    DOS       WIN32
.fnn _fmbsnextc                 mbsnextc.gml    DOS       WIN32
.fnm _strnextc                  mbsnextc.gml    DOS QNX W WIN32
.fnw _wcsnextc                  mbsnextc.gml    DOS QNX W WIN32
.fnm mbsrtowcs                  mbsrtowc.gml    DOS       WIN32
.fnn _fmbsrtowcs                mbsrtowc.gml    DOS       WIN32
.fnc mbsrtowcs_s                mbsrto_s.gml    DOS       WIN32
.fnm _fmbsrtowcs_s              mbsrto_s.gml    DOS       WIN32
.fnc mbstowcs                   mbstowcs.gml    DOS QNX W WIN32 TEST
.fnn _fmbstowcs                 mbstowcs.gml    DOS       WIN32 TEST
.fnc mbstowcs_s                 mbstow_s.gml    DOS QNX W WIN32 TEST
.fnm _fmbstowcs_s               mbstow_s.gml    DOS       WIN32 TEST
.fnm _mbterm                    mbterm.gml      DOS       WIN32
.fnn _fmbterm                   mbterm.gml      DOS       WIN32
.fnc mbtowc                     mbtowc.gml      DOS QNX W WIN32
.fnn _fmbtowc                   mbtowc.gml      DOS       WIN32
.fnm _mbvtop                    mbvtop.gml      DOS       WIN32
.fnn _fmbvtop                   mbvtop.gml      DOS       WIN32
.fnc _memavl                    _memavl.gml     DOS QNX W WIN32
.fnc memccpy                    memccpy.gml     DOS QNX W WIN32
.fnf _fmemccpy                  memccpy.gml     DOS QNX W WIN32
.fnc memchr                     memchr.gml      DOS QNX W WIN32
.fnf _fmemchr                   memchr.gml      DOS QNX W WIN32
.fnw wmemchr                    memchr.gml      DOS QNX W WIN32
.fnc memcmp                     memcmp.gml      DOS QNX W WIN32
.fnf _fmemcmp                   memcmp.gml      DOS QNX W WIN32
.fnw wmemcmp                    memcmp.gml      DOS QNX W WIN32
.fnc memcpy                     memcpy.gml      DOS QNX W WIN32
.fnf _fmemcpy                   memcpy.gml      DOS QNX W WIN32
.fnw wmemcpy                    memcpy.gml      DOS QNX W WIN32
.fnc memcpy_s                   memcpy_s.gml    DOS       WIN32
.fnw wmemcpy_s                  memcpy_s.gml    DOS       WIN32
.fnc memicmp                    memicmp.gml     DOS QNX W WIN32
.fnc _memicmp                   memicmp.gml     DOS       WIN32
.fnf _fmemicmp                  memicmp.gml     DOS QNX W WIN32
.fnc _memmax                    _memmax.gml     DOS QNX W WIN32
.fnc memmove                    memmove.gml     DOS QNX W WIN32
.fnf _fmemmove                  memmove.gml     DOS QNX W WIN32
.fnw wmemmove                   memmove.gml     DOS QNX W WIN32
.fnc memmove_s                  memmov_s.gml    DOS       WIN32
.fnw wmemmove_s                 memmov_s.gml    DOS       WIN32
.fn7 _m_empty                   mempty.gml      DOS QXO   WIN32
.fnc memset                     memset.gml      DOS QNX W WIN32
.fnf _fmemset                   memset.gml      DOS QNX W WIN32
.fnw wmemset                    memset.gml      DOS QNX W WIN32
.fn7 _m_from_int                mfromint.gml    DOS QXO   WIN32
.fnc min                        min.gml         DOS QNX W WIN32
.fnc mkdir                      mkdir.gml       DOS QNX W WIN32
.fn8 _mkdir                     mkdir.gml       DOS       WIN32
.fnw _wmkdir                    mkdir.gml       DOS       WIN32
.fnc mkfifo                     mkfifo.gml          QXO
.fnc MK_FP                      mk_fp.gml       DOS QNX W WIN32
.fnc mknod                      mknod.gml           QXO
.fnc mkstemp                    mkstemp.gml     DOS QNX W WIN32
.fn7 _mktemp                    mktemp.gml      DOS       WIN32
.fnw _wmktemp                   mktemp.gml      DOS       WIN32
.fnc mktime                     mktime.gml      DOS QNX W WIN32
.fnc mlock                      mlock.gml       DOS QNX W WIN32
.fnc mlockall                   mlockall.gml    DOS QNX W WIN32
.fnc mmap                       mmap.gml        DOS QNX W WIN32
.fnc modf                       modf.gml        DOS QNX W WIN32
.fnc mount                      mount.gml           QXO
.fnc mouse_close                ms_close.gml        QXO
.fnc mouse_flush                ms_flush.gml        QXO
.fnc mouse_open                 ms_open.gml         QXO
.fnc mouse_param                ms_param.gml        QXO
.fnc mouse_read                 ms_read.gml         QXO
.fnc movedata                   movedata.gml    DOS QNX W WIN32
.fnc _moveto                    gr_movto.gml    DOS QNX W GRAPH
.fnc _moveto_w                  gr_movto.gml    DOS QNX W GRAPH
.fn7 _m_packssdw                mpckssdw.gml    DOS QNX W WIN32
.fn7 _m_packsswb                mpcksswb.gml    DOS QNX W WIN32
.fn7 _m_packuswb                mpckuswb.gml    DOS QNX W WIN32
.fn7 _m_paddb                   mpaddb.gml      DOS QNX W WIN32
.fn7 _m_paddd                   mpaddd.gml      DOS QNX W WIN32
.fn7 _m_paddsb                  mpaddsb.gml     DOS QNX W WIN32
.fn7 _m_paddsw                  mpaddsw.gml     DOS QNX W WIN32
.fn7 _m_paddusb                 mpaddusb.gml    DOS QNX W WIN32
.fn7 _m_paddusw                 mpaddusw.gml    DOS QNX W WIN32
.fn7 _m_paddw                   mpaddw.gml      DOS QNX W WIN32
.fn7 _m_pand                    mpand.gml       DOS QNX W WIN32
.fn7 _m_pandn                   mpandn.gml      DOS QNX W WIN32
.fn7 _m_pcmpeqb                 mpcmpeqb.gml    DOS QNX W WIN32
.fn7 _m_pcmpeqd                 mpcmpeqd.gml    DOS QNX W WIN32
.fn7 _m_pcmpeqw                 mpcmpeqw.gml    DOS QNX W WIN32
.fn7 _m_pcmpgtb                 mpcmpgtb.gml    DOS QNX W WIN32
.fn7 _m_pcmpgtd                 mpcmpgtd.gml    DOS QNX W WIN32
.fn7 _m_pcmpgtw                 mpcmpgtw.gml    DOS QNX W WIN32
.fn7 _m_pmaddwd                 mpmaddwd.gml    DOS QNX W WIN32
.fn7 _m_pmulhw                  mpmulhw.gml     DOS QNX W WIN32
.fn7 _m_pmullw                  mpmullw.gml     DOS QNX W WIN32
.fn7 _m_por                     mpor.gml        DOS QNX W WIN32
.fnc mprotect                   mprotect.gml    DOS QNX W WIN32
.fn7 _m_pslld                   mpslld.gml      DOS QNX W WIN32
.fn7 _m_pslldi                  mpslldi.gml     DOS QNX W WIN32
.fn7 _m_psllq                   mpsllq.gml      DOS QNX W WIN32
.fn7 _m_psllqi                  mpsllqi.gml     DOS QNX W WIN32
.fn7 _m_psllw                   mpsllw.gml      DOS QNX W WIN32
.fn7 _m_psllwi                  mpsllwi.gml     DOS QNX W WIN32
.fn7 _m_psrad                   mpsrad.gml      DOS QNX W WIN32
.fn7 _m_psradi                  mpsradi.gml     DOS QNX W WIN32
.fn7 _m_psraw                   mpsraw.gml      DOS QNX W WIN32
.fn7 _m_psrawi                  mpsrawi.gml     DOS QNX W WIN32
.fn7 _m_psrld                   mpsrld.gml      DOS QNX W WIN32
.fn7 _m_psrldi                  mpsrldi.gml     DOS QNX W WIN32
.fn7 _m_psrlq                   mpsrlq.gml      DOS QNX W WIN32
.fn7 _m_psrlqi                  mpsrlqi.gml     DOS QNX W WIN32
.fn7 _m_psrlw                   mpsrlw.gml      DOS QNX W WIN32
.fn7 _m_psrlwi                  mpsrlwi.gml     DOS QNX W WIN32
.fn7 _m_psubb                   mpsubb.gml      DOS QNX W WIN32
.fn7 _m_psubd                   mpsubd.gml      DOS QNX W WIN32
.fn7 _m_psubsb                  mpsubsb.gml     DOS QNX W WIN32
.fn7 _m_psubsw                  mpsubsw.gml     DOS QNX W WIN32
.fn7 _m_psubusb                 mpsubusb.gml    DOS QNX W WIN32
.fn7 _m_psubusw                 mpsubusw.gml    DOS QNX W WIN32
.fn7 _m_psubw                   mpsubw.gml      DOS QNX W WIN32
.fn7 _m_punpckhbw               mupckhbw.gml    DOS QNX W WIN32
.fn7 _m_punpckhdq               mupckhdq.gml    DOS QNX W WIN32
.fn7 _m_punpckhwd               mupckhwd.gml    DOS QNX W WIN32
.fn7 _m_punpcklbw               mupcklbw.gml    DOS QNX W WIN32
.fn7 _m_punpckldq               mupckldq.gml    DOS QNX W WIN32
.fn7 _m_punpcklwd               mupcklwd.gml    DOS QNX W WIN32
.fn7 _m_pxor                    mpxor.gml       DOS QNX W WIN32
.fnc _msize                     _msize.gml      DOS QNX W WIN32
.fnc _bmsize                    _msize.gml      DOS QNX W
.fnf _fmsize                    _msize.gml      DOS QNX W
.fnc _nmsize                    _msize.gml      DOS QNX W WIN32
.fnc msync                      msync.gml       DOS QNX W WIN32
.fnc mtob                       mtob.gml        NEC
.fnf _fmtob                     mtob.gml        NEC
.fn7 _m_to_int                  mtoint.gml      DOS QNX W WIN32
.fnc munlock                    munlock.gml     DOS QNX W WIN32
.fnc munlockall                 munlall.gml     DOS QNX W WIN32
.fnc munmap                     munmap.gml      DOS QNX W WIN32
.fnc nan                        nanf.gml        DOS QNX W WIN32
.fnc nanf                       nanf.gml        DOS QNX W WIN32
.fnc nanl                       nanf.gml        DOS QNX W WIN32
.fnc nanosleep                  nanoslep.gml        QXO
.fnc nearbyint                  nrbyint.gml     DOS QNX W WIN32
.fnc nextafter                  nxtafter.gml    DOS QNX W WIN32
.fnc nl_langinfo                langinfo.gml    DOS     W WIN32
.fnc nosound                    nosound.gml     DOS QNX W
.fnc nthctype                   nthctype.gml    NEC
.fnf _fnthctype                 nthctype.gml    NEC
.fnc offsetof                   offsetof.gml    DOS QNX W WIN32
.fnc onexit                     onexit.gml      DOS QNX W WIN32
.fnc open                       open.gml        DOS QNX W WIN32
.fn8 _open                      open.gml        DOS       WIN32
.fnw _wopen                     open.gml        DOS       WIN32
.fnc opendir                    opendir.gml     DOS QNX W WIN32
.fnw _wopendir                  opendir.gml     DOS       WIN32
.fn7 _open_osfhandle            openosfh.gml    DOS QXO   WIN32
.fnc _os_handle                 oshandle.gml    DOS       WIN32
.fnc _outgtext                  gr_outgt.gml    DOS QNX W GRAPH
.fnc _outmem                    gr_outm.gml     DOS QNX W GRAPH
.fnc outp                       outp.gml        DOS QNX W WIN32
.fnc outpd                      outpd.gml       DOS QNX W WIN32
.fnc outpw                      outpw.gml       DOS QNX W WIN32
.fnc _outtext                   gr_outtx.gml    DOS QNX W GRAPH
.fnc pathconf                   pathconf.gml        QXO
.fnc pause                      pause.gml           QXO
.fnc pclose                     pclose.gml      DOS QXO
.fnc _pclose                    _pclose.gml     DOS       WIN32
.fnc perror                     perror.gml      DOS QNX W WIN32
.fnw _wperror                   perror.gml      DOS QNX W WIN32
.fnc _pg_analyzechart           gr_pgac.gml     DOS QNX W GRAPH
.fnc _pg_analyzechartms         gr_pgac.gml     DOS QNX W GRAPH
.fnc _pg_analyzepie             gr_pgap.gml     DOS QNX W GRAPH
.fnc _pg_analyzescatter         gr_pgas.gml     DOS QNX W GRAPH
.fnc _pg_analyzescatterms       gr_pgas.gml     DOS QNX W GRAPH
.fnc _pg_chart                  gr_pgc.gml      DOS QNX W GRAPH
.fnc _pg_chartms                gr_pgc.gml      DOS QNX W GRAPH
.fnc _pg_chartpie               gr_pgcp.gml     DOS QNX W GRAPH
.fnc _pg_chartscatter           gr_pgcs.gml     DOS QNX W GRAPH
.fnc _pg_chartscatterms         gr_pgcs.gml     DOS QNX W GRAPH
.fnc _pg_defaultchart           gr_pgdc.gml     DOS QNX W GRAPH
.fnc _pg_getchardef             gr_pggc.gml     DOS QNX W GRAPH
.fnc _pg_getpalette             gr_pggp.gml     DOS QNX W GRAPH
.fnc _pg_getstyleset            gr_pggy.gml     DOS QNX W GRAPH
.fnc _pg_hlabelchart            gr_pghl.gml     DOS QNX W GRAPH
.fnc _pg_initchart              gr_pgic.gml     DOS QNX W GRAPH
.fnc _pg_resetpalette           gr_pgrp.gml     DOS QNX W GRAPH
.fnc _pg_resetstyleset          gr_pgry.gml     DOS QNX W GRAPH
.fnc _pg_setchardef             gr_pgsc.gml     DOS QNX W GRAPH
.fnc _pg_setpalette             gr_pgsp.gml     DOS QNX W GRAPH
.fnc _pg_setstyleset            gr_pgsy.gml     DOS QNX W GRAPH
.fnc _pg_vlabelchart            gr_pgvl.gml     DOS QNX W GRAPH
.fnc _pie                       gr_pie.gml      DOS QNX W GRAPH
.fnc _pie_w                     gr_pie.gml      DOS QNX W GRAPH
.fnc _pie_wxy                   gr_pie.gml      DOS QNX W GRAPH
.fnc pipe                       pipe.gml            QXO
.fn7 _pipe                      _pipe.gml       DOS       WIN32
.fnc _polygon                   gr_polyg.gml    DOS QNX W GRAPH
.fnc _polygon_w                 gr_polyg.gml    DOS QNX W GRAPH
.fnc _polygon_wxy               gr_polyg.gml    DOS QNX W GRAPH
.fnc popen                      popen.gml       DOS QXO
.fn7 _popen                     _popen.gml      DOS       WIN32
.fnw _wpopen                    _popen.gml      DOS       WIN32
.fnc pow                        pow.gml         DOS QNX W WIN32
.fnc printf                     printf.gml      DOS QNX W WIN32
.fnw wprintf                    printf.gml      DOS QNX W WIN32
.fnc printf_s                   printf_s.gml    DOS QNX W WIN32
.fnw wprintf_s                  printf_s.gml    DOS QNX W WIN32
.fnc print_usage                p_usage.gml         QXO
.fnc putc                       putc.gml        DOS QNX W WIN32
.fnw putwc                      putc.gml        DOS QNX W WIN32
.fnc putch                      putch.gml       DOS QNX W WIN32
.fnc putchar                    putchar.gml     DOS QNX W WIN32
.fnw putwchar                   putchar.gml     DOS QNX W WIN32
.fnc putenv                     putenv.gml      DOS QNX W WIN32
.fn7 _putenv                    putenv.gml      DOS QNX W WIN32
.fnw _wputenv                   putenv.gml      DOS QNX W WIN32
.fnc _putimage                  gr_putim.gml    DOS QNX W GRAPH
.fnc _putimage_w                gr_putim.gml    DOS QNX W GRAPH
.fnc puts                       puts.gml        DOS QNX W WIN32
.fnw _putws                     puts.gml        DOS QNX W WIN32
.fnc _putw                      putw.gml        DOS QNX W WIN32
.fnc qnx_device_attach          qdev_at.gml         QXO
.fnc qnx_device_detach          qdev_de.gml         QXO
.fnc qnx_display_hex            qdisphex.gml        QXO
.fnc qnx_display_msg            qdisp.gml           QXO
.fnc qnx_fd_attach              qfd_at.gml          QXO
.fnc qnx_fd_detach              qfd_de.gml          QXO
.fnc qnx_fd_query               qfd_qu.gml          QXO
.fnc qnx_fullpath               qfullpth.gml        QXO
.fnc qnx_getclock               qgetcloc.gml        QXO
.fnc qnx_getids                 qgetids.gml         QXO
.fnc qnx_hint_attach            qhint_at.gml        QXO
.fnc qnx_hint_detach            qhint_de.gml        QXO
.fnc qnx_hint_mask              qhint_ma.gml        QXO
.fnc qnx_hint_query             qhint_qu.gml        QXO
.fnc qnx_ioctl                  qioctl.gml          QXO
.fnc qnx_name_attach            qname_at.gml        QXO
.fnc qnx_name_detach            qname_de.gml        QXO
.fnc qnx_name_locate            qname_lo.gml        QXO
.fnc qnx_name_locators          qname_ls.gml        QXO
.fnc qnx_name_query             qname_qu.gml        QXO
.fnc qnx_nidtostr               qntostr.gml         QXO
.fnc qnx_osinfo                 qosinfo.gml         QXO
.fnc qnx_osstat                 qosstat.gml         QXO
.fnc qnx_pflags                 qpflags.gml         QXO
.fnc qnx_prefix_attach          qprfx_at.gml        QXO
.fnc qnx_prefix_detach          qprfx_de.gml        QXO
.fnc qnx_prefix_getroot         qprefixg.gml        QXO
.fnc qnx_prefix_query           qprfx_qu.gml        QXO
.fnc qnx_prefix_setroot         qprefixs.gml        QXO
.fnc qnx_proxy_attach           qprox_at.gml        QXO
.fnc qnx_proxy_detach           qprox_de.gml        QXO
.fnc qnx_proxy_rem_attach       qprxr_at.gml        QXO
.fnc qnx_proxy_rem_detach       qprxr_de.gml        QXO
.fnc qnx_psinfo                 qpsinfo.gml         QXO
.fnc qnx_scheduler              qsched.gml          QXO
.fnc qnx_segment_alloc          qseg_alc.gml        QXO
.fnc qnx_segment_alloc_flags    qseg_alf.gml        QXO
.fnc qnx_segment_arm            qseg_arm.gml        QXO
.fnc qnx_segment_flags          qseg_flg.gml        QXO
.fnc qnx_segment_free           qseg_fre.gml        QXO
.fnc qnx_segment_get            qseg_get.gml        QXO
.fnc qnx_segment_huge           qseg_hge.gml        QXO
.fnc qnx_segment_index          qseg_idx.gml        QXO
.fnc qnx_segment_info           qseg_inf.gml        QXO
.fnc qnx_segment_overlay        qseg_ovl.gml        QXO
.fnc qnx_segment_overlay_flags  qseg_ofl.gml        QXO
.fnc qnx_segment_put            qseg_put.gml        QXO
.fnc qnx_segment_raw_alloc      qralloc.gml         QXO
.fnc qnx_segment_raw_free       qrfree.gml          QXO
.fnc qnx_segment_realloc        qseg_ral.gml        QXO
.fnc qnx_setclock               qsetcloc.gml        QXO
.fnc qnx_setids                 qsetids.gml         QXO
.fnc qnx_sflags                 qsflags.gml         QXO
.fnc qnx_sid_query              qsid_qu.gml         QXO
.fnc qnx_spawn                  qspawn.gml          QXO
.fnc qnx_spawn_options          qspawn_o.gml        QXO
.fnc qnx_strtonid               qstrton.gml         QXO
.fnc qnx_sync                   qsync.gml           QXO
.fnc qnx_trace_close            qtclose.gml         QXO
.fnc qnx_trace_info             qtinfo.gml          QXO
.fnc qnx_trace_open             qtopen.gml          QXO
.fnc qnx_trace_read             qtread.gml          QXO
.fnc qnx_trace_severity         qtsever.gml         QXO
.fnc qnx_trace_trigger          qttrigg.gml         QXO
.fnc qnx_umask                  qumask.gml          QXO
.fnc qnx_vc_attach              qvc_at.gml          QXO
.fnc qnx_vc_detach              qvc_de.gml          QXO
.fnc qnx_vc_name_attach         qvcn_at.gml         QXO
.fnc qnx_vc_poll_parm           qvc_pp.gml          QXO
.fnc qsort                      qsort.gml       DOS QNX W WIN32
.fnc qsort_s                    qsort_s.gml     DOS QNX W WIN32
.fnc raise                      raise.gml       DOS QNX W WIN32
.fnc rand                       rand.gml        DOS QNX W WIN32
.fnc read                       read.gml        DOS QNX W WIN32
.fnc _read                      read.gml        DOS       WIN32
.fnc readv                      read.gml            QXO
.fnc readdir                    readdir.gml     DOS QNX W WIN32
.fnw _wreaddir                  readdir.gml     DOS       WIN32
.fnc readlink                   readlink.gml        QXO
.fnc Readmsg                    readmsg.gml         QXO
.fnc Readmsgmx                  readmsgm.gml        QXO
.fnc realloc                    realloc.gml     DOS QNX W WIN32
.fnc _brealloc                  realloc.gml     DOS QNX W
.fnf _frealloc                  realloc.gml     DOS QNX W
.fnc _nrealloc                  realloc.gml     DOS QNX W WIN32
.fnc Receive                    receive.gml         QXO
.fnc Receivemx                  receivem.gml        QXO
.fnc _rectangle                 gr_rect.gml     DOS QNX W GRAPH
.fnc _rectangle_w               gr_rect.gml     DOS QNX W GRAPH
.fnc _rectangle_wxy             gr_rect.gml     DOS QNX W GRAPH
.fnc regcomp                    regcomp.gml         QXO
.fnc regerror                   regerror.gml        QXO
.fnc regexec                    regexec.gml         QXO
.fnc regfree                    regfree.gml         QXO
.fnc _registerfonts             gr_reg.gml      DOS QNX W GRAPH
.fnc Relay                      relay.gml           QXO
.fnc remainder                  rmnder.gml      DOS QNX W WIN32
.fnc _remapallpalette           gr_mappa.gml    DOS QNX W GRAPH
.fnc _remappalette              gr_mappc.gml    DOS QNX W GRAPH
.fnc remove                     remove.gml      DOS QNX W WIN32
.fnw _wremove                   remove.gml      DOS       WIN32
.fnc rename                     rename.gml      DOS QNX W WIN32
.fnw _wrename                   rename.gml      DOS       WIN32
.fnc Reply                      reply.gml           QXO
.fnc Replymx                    replymx.gml         QXO
.fnc rewind                     rewind.gml      DOS QNX W WIN32
.fnc rewinddir                  rewinddi.gml    DOS QNX W WIN32
.fnw _wrewinddir                rewinddi.gml    DOS       WIN32
.fnc rint                       rint.gml        DOS QNX W WIN32
.fnc rmdir                      rmdir.gml       DOS QNX W WIN32
.fn8 _rmdir                     rmdir.gml       DOS       WIN32
.fnw _wrmdir                    rmdir.gml       DOS       WIN32
.fnc _rotl                      _rotl.gml       DOS QNX W WIN32
.fnc _rotr                      _rotr.gml       DOS QNX W WIN32
.fnc round                      round.gml       DOS QNX W WIN32
.fnc sbrk                       sbrk.gml        DOS QNX W WIN32
.fnc scalbn                     scalbn.gml      DOS QNX W WIN32
.fnc scanf                      scanf.gml       DOS QNX W WIN32
.fnw wscanf                     scanf.gml       DOS QNX W WIN32
.fnc scanf_s                    scanf_s.gml     DOS QNX W WIN32
.fnw wscanf_s                   scanf_s.gml     DOS QNX W WIN32
.fnc sched_getparam             scgetpa.gml         QXO
.fnw sched_getparam             schgprm.gml     DOS     W WIN32
.fnc sched_get_priority_max     scgetmax.gml        QXO
.fnc sched_get_priority_min     scgetmin.gml        QXO
.fnc sched_get_priority_max     schprmax.gml    DOS     W WIN32
.fnc sched_get_priority_min     schprmin.gml    DOS     W WIN32
.fnc sched_getscheduler         scgetsc.gml         QXO
.fnc sched_getscheduler         schgsch.gml     DOS     W WIN32
.fnc sched_rr_get_interval      schrrint.gml    DOS     W WIN32
.fnc sched_setparam             scsetpa.gml         QXO
.fnw sched_setparam             schsprm.gml     DOS     W WIN32
.fnc sched_setscheduler         scsetsc.gml         QXO
.fnc sched_setscheduler         schssch.gml     DOS     W WIN32
.fnc sched_yield                scyield.gml         QXO
.fnc sched_yield                schyield.gml    DOS     W WIN32
.fnc _scrolltextwindow          gr_scrtw.gml    DOS QNX W GRAPH
.fnc _searchenv                 _searche.gml    DOS QNX W WIN32
.fnw _wsearchenv                _searche.gml    DOS       WIN32
.fnc searchenv                  srchenv.gml         QXO
.fnc segread                    segread.gml     DOS QNX W WIN32
.fnc select                     select.gml          QXO
.fnc _selectpalette             gr_selp.gml     DOS QNX W GRAPH
.fnc Send                       send.gml            QXO
.fnc Sendfd                     sendfd.gml          QXO
.fnc Sendfdmx                   sendfdmx.gml        QXO
.fnc Sendmx                     sendmx.gml          QXO
.fnc set_constraint_handler_s   set_chnd.gml    DOS QNX W WIN32
.fnc _setactivepage             gr_setap.gml    DOS QNX W GRAPH
.fnc _setbkcolor                gr_setbc.gml    DOS QNX W GRAPH
.fnc setbuf                     setbuf.gml      DOS QNX W WIN32
.fnc _setcharsize               gr_setcs.gml    DOS QNX W GRAPH
.fnc _setcharsize_w             gr_setcs.gml    DOS QNX W GRAPH
.fnc _setcharspacing            gr_setcx.gml    DOS QNX W GRAPH
.fnc _setcharspacing_w          gr_setcx.gml    DOS QNX W GRAPH
.fnc _setcliprgn                gr_setcr.gml    DOS QNX W GRAPH
.fnc _setcolor                  gr_setc.gml     DOS QNX W GRAPH
.fnc setegid                    setegid.gml         QXO
.fnc setenv                     setenv.gml      DOS QNX W WIN32
.fn7 _setenv                    setenv.gml      DOS QNX W WIN32
.fnw _wsetenv                   setenv.gml      DOS QNX W WIN32
.fnc seteuid                    seteuid.gml         QXO
.fnc _setfillmask               gr_setfm.gml    DOS QNX W GRAPH
.fnc _setfont                   gr_setf.gml     DOS QNX W GRAPH
.fnc setgid                     setgid.gml          QXO
.fnc _setgtextvector            gr_settv.gml    DOS QNX W GRAPH
.fnc setjmp                     setjmp.gml      DOS QNX W WIN32
.fnc _setlinestyle              gr_setls.gml    DOS QNX W GRAPH
.fnc setlocale                  setlocal.gml    DOS QNX W WIN32
.fnw _wsetlocale                setlocal.gml    DOS QNX W WIN32
.fnc _set_matherr               _setmath.gml    DOS QXO   WIN32
.fnm _setmbcp                   setmbcp.gml     DOS       WIN32
.fnc setmode                    setmode.gml     DOS QNX W WIN32
.fnc _setmode                   setmode.gml     DOS       WIN32
.fnc _setmx                     setmx.gml           QXO
.fnc _set_new_handler           setnew.gml      DOS QNX W WIN32
.fnc set_new_handler            setnew.gml      DOS QNX W WIN32
.fnc setpgid                    setpgid.gml         QXO
.fnc _setpixel                  gr_setpx.gml    DOS QNX W GRAPH
.fnc _setpixel_w                gr_setpx.gml    DOS QNX W GRAPH
.fnc _setplotaction             gr_setpa.gml    DOS QNX W GRAPH
.fnc setprio                    setprio.gml         QXO
.fnc setpwent                   setpwent.gml    DOS QNX W WIN32
.fnc setsid                     setsid.gml          QXO
.fnc _settextalign              gr_setta.gml    DOS QNX W GRAPH
.fnc _settextcolor              gr_settc.gml    DOS QNX W GRAPH
.fnc _settextcursor             gr_settu.gml    DOS QNX W GRAPH
.fnc _settextorient             gr_setto.gml    DOS QNX W GRAPH
.fnc _settextpath               gr_settt.gml    DOS QNX W GRAPH
.fnc _settextposition           gr_settp.gml    DOS QNX W GRAPH
.fnc _settextrows               gr_settr.gml    DOS QNX W GRAPH
.fnc _settextwindow             gr_settw.gml    DOS QNX W GRAPH
.fnc setuid                     setuid.gml          QXO
.fnc setvbuf                    setvbuf.gml     DOS QNX W WIN32
.fnc _setvideomode              gr_setvm.gml    DOS QNX W GRAPH
.fnc _setvideomoderows          gr_setvr.gml    DOS QNX W GRAPH
.fnc _setvieworg                gr_setlo.gml    DOS QNX W GRAPH
.fnc _setviewport               gr_setvw.gml    DOS QNX W GRAPH
.fnc _setvisualpage             gr_setvp.gml    DOS QNX W GRAPH
.fnc _setwindow                 gr_setw.gml     DOS QNX W GRAPH
.fnc shm_open                   shmopen.gml         QXO
.fnc shm_unlink                 shmulink.gml        QXO
.fnc sigaction                  sigact.gml          QXO
.fnc sigaddset                  sigadd.gml          QXO
.fnc sigdelset                  sigdel.gml          QXO
.fnc sigemptyset                sigempty.gml        QXO
.fnc sigfillset                 sigfill.gml         QXO
.fnc sigismember                sigis.gml           QXO
.fnc siglongjmp                 siglongj.gml        QXO
.fnc signal                     signal.gml      DOS QNX W WIN32
.fnc signbit                    signbit.gml     DOS QNX W WIN32
.fnc sigpending                 sigpend.gml         QXO
.fnc sigprocmask                sigpmask.gml        QXO
.fnc sigsetjmp                  sigsetj.gml         QXO
.fnc sigsuspend                 sigsusp.gml         QXO
.fnc sin                        sin.gml         DOS QNX W WIN32
.fnc sinh                       sinh.gml        DOS QNX W WIN32
.fnm mbsinit                    sisinit.gml     DOS       WIN32
.fnm sisinit                    sisinit.gml     DOS       WIN32
.fnc sleep                      sleep.gml       DOS QNX W WIN32
.fnc _snprintf                  _snprntf.gml    DOS QNX W WIN32
.fnc _snwprintf                 _snprntf.gml    DOS QNX W WIN32
.fnc snprintf                   snprintf.gml    DOS QNX W WIN32
.fnc snwprintf                  snprintf.gml    DOS QNX W WIN32
.fnc snprintf_s                 snprtf_s.gml    DOS QNX W WIN32
.fnc snwprintf_s                snprtf_s.gml    DOS QNX W WIN32
.fnc sopen                      sopen.gml       DOS QNX W WIN32
.fnc _sopen                     sopen.gml       DOS       WIN32
.fnw _wsopen                    sopen.gml       DOS       WIN32
.fnc sound                      sound.gml       DOS QNX W
.fnc spawn&grpsfx               spawn.gml       DOS QNX W WIN32
.fnc spawnl                     spawn.gml       DOS QNX W WIN32
.fnc _spawnl                    spawn.gml       DOS QNX W WIN32
.fnw _wspawnl                   spawn.gml       DOS       WIN32
.fnc spawnle                    spawn.gml       DOS QNX W WIN32
.fnc _spawnle                   spawn.gml       DOS QNX W WIN32
.fnw _wspawnle                  spawn.gml       DOS       WIN32
.fnc spawnlp                    spawn.gml       DOS QNX W WIN32
.fnc _spawnlp                   spawn.gml       DOS QNX W WIN32
.fnw _wspawnlp                  spawn.gml       DOS       WIN32
.fnc spawnlpe                   spawn.gml       DOS QNX W WIN32
.fnc _spawnlpe                  spawn.gml       DOS QNX W WIN32
.fnw _wspawnlpe                 spawn.gml       DOS       WIN32
.fnc spawnv                     spawn.gml       DOS QNX W WIN32
.fnc _spawnv                    spawn.gml       DOS QNX W WIN32
.fnw _wspawnv                   spawn.gml       DOS       WIN32
.fnc spawnve                    spawn.gml       DOS QNX W WIN32
.fnc _spawnve                   spawn.gml       DOS QNX W WIN32
.fnw _wspawnve                  spawn.gml       DOS       WIN32
.fnc spawnvp                    spawn.gml       DOS QNX W WIN32
.fnc _spawnvp                   spawn.gml       DOS QNX W WIN32
.fnw _wspawnvp                  spawn.gml       DOS       WIN32
.fnc spawnvpe                   spawn.gml       DOS QNX W WIN32
.fnc _spawnvpe                  spawn.gml       DOS QNX W WIN32
.fnw _wspawnvpe                 spawn.gml       DOS       WIN32
.fnc _splitpath                 _splitpa.gml    DOS QNX W WIN32
.fnw _wsplitpath                _splitpa.gml    DOS QNX W WIN32
.fnc _splitpath2                _splitp2.gml    DOS QNX W WIN32
.fnw _wsplitpath2               _splitp2.gml    DOS QNX W WIN32
.fnc sprintf                    sprintf.gml     DOS QNX W WIN32
.fnw swprintf                   sprintf.gml     DOS QNX W WIN32
.fnc sprintf_s                  sprntf_s.gml    DOS QNX W WIN32
.fnw swprintf_s                 sprntf_s.gml    DOS QNX W WIN32
.fnc sqrt                       sqrt.gml        DOS QNX W WIN32
.fnc srand                      srand.gml       DOS QNX W WIN32
.fnc sscanf                     sscanf.gml      DOS QNX W WIN32
.fnw swscanf                    sscanf.gml      DOS QNX W WIN32
.fnc sscanf_s                   sscanf_s.gml    DOS QNX W WIN32
.fnw swscanf_s                  sscanf_s.gml    DOS QNX W WIN32
.fnc stackavail                 stackava.gml    DOS QNX W WIN32
.fnc _stackavail                stackava.gml    DOS QNX W WIN32
.fnc stat                       stat.gml        DOS QNX W WIN32
.fnc _stat                      stat.gml        DOS       WIN32
.fn8 _stati64                   stat.gml        DOS       WIN32
.fnw _wstat                     stat.gml        DOS       WIN32
.fnw _wstati64                  stat.gml        DOS       WIN32
.fnc lstat                      stat.gml        DOS       WIN32
.fnc _status87                  _status8.gml    DOS QNX W WIN32
.fnc strcasecmp                 strcase.gml     DOS QNX W WIN32
.fnc strcat                     strcat.gml      DOS QNX W WIN32
.fnf _fstrcat                   strcat.gml      DOS QNX W WIN32
.fnw wcscat                     strcat.gml      DOS QNX W WIN32
.fnm _mbscat                    strcat.gml      DOS       WIN32
.fnn _fmbscat                   strcat.gml      DOS       WIN32
.fnc strcat_s                   strcat_s.gml    DOS       WIN32
.fnw wcscat_s                   strcat_s.gml    DOS       WIN32
.fnc strchr                     strchr.gml      DOS QNX W WIN32
.fnf _fstrchr                   strchr.gml      DOS QNX W WIN32
.fnw wcschr                     strchr.gml      DOS QNX W WIN32
.fnm _mbschr                    strchr.gml      DOS       WIN32
.fnn _fmbschr                   strchr.gml      DOS       WIN32
.fnc strcmp                     strcmp.gml      DOS QNX W WIN32
.fnf _fstrcmp                   strcmp.gml      DOS QNX W WIN32
.fnw wcscmp                     strcmp.gml      DOS QNX W WIN32
.fnm _mbscmp                    strcmp.gml      DOS       WIN32
.fnn _fmbscmp                   strcmp.gml      DOS       WIN32
.fnc strcmpi                    strcmpi.gml     DOS QNX W WIN32
.fnw wcscmpi                    strcmpi.gml     DOS QNX W WIN32
.fnc strcoll                    strcoll.gml     DOS QNX W WIN32
.fnw wcscoll                    strcoll.gml     DOS QNX W WIN32
.fnm _mbscoll                   strcoll.gml     DOS       WIN32
.fnc strcpy                     strcpy.gml      DOS QNX W WIN32
.fnf _fstrcpy                   strcpy.gml      DOS QNX W WIN32
.fnw wcscpy                     strcpy.gml      DOS QNX W WIN32
.fnm _mbscpy                    strcpy.gml      DOS       WIN32
.fnn _fmbscpy                   strcpy.gml      DOS       WIN32
.fnc strcpy_s                   strcpy_s.gml    DOS       WIN32
.fnw wcscpy_s                   strcpy_s.gml    DOS       WIN32
.fnc strcspn                    strcspn.gml     DOS QNX W WIN32
.fnf _fstrcspn                  strcspn.gml     DOS QNX W WIN32
.fnw wcscspn                    strcspn.gml     DOS QNX W WIN32
.fnm _mbscspn                   strcspn.gml     DOS       WIN32
.fnn _fmbscspn                  strcspn.gml     DOS       WIN32
.fnc _strdate                   strdate.gml     DOS QNX W WIN32
.fnw _wstrdate                  strdate.gml     DOS QNX W WIN32
.fnc _strdec                    strdec.gml      DOS QNX W WIN32
.fnw _wcsdec                    strdec.gml      DOS QNX W WIN32
.fnm _mbsdec                    strdec.gml      DOS       WIN32
.fnn _fmbsdec                   strdec.gml      DOS       WIN32
.fnc strdup                     strdup.gml      DOS QNX W WIN32
.fnc _strdup                    strdup.gml      DOS QNX W WIN32
.fnf _fstrdup                   strdup.gml      DOS QNX W WIN32
.fnw _wcsdup                    strdup.gml      DOS QNX W WIN32
.fnm _mbsdup                    strdup.gml      DOS       WIN32
.fnn _fmbsdup                   strdup.gml      DOS       WIN32
.fnc strerror                   strerror.gml    DOS QNX W WIN32
.fnw wcserror                   strerror.gml    DOS       WIN32
.fnc strerror_s                 strerr_s.gml    DOS       WIN32
.fnw wcserror_s                 strerr_s.gml    DOS       WIN32
.fnc strerrorlen_s              streln_s.gml    DOS       WIN32
.fnw wcserrorlen_s              streln_s.gml    DOS       WIN32
.fnc strftime                   strftime.gml    DOS QNX W WIN32
.fnw wcsftime                   strftime.gml    DOS QNX W WIN32
.fnw _wstrftime_ms              strftime.gml    DOS QNX W WIN32
.fnc stricmp                    stricmp.gml     DOS QNX W WIN32
.fnc _stricmp                   stricmp.gml     DOS QNX W WIN32
.fnf _fstricmp                  stricmp.gml     DOS QNX W WIN32
.fnw _wcsicmp                   stricmp.gml     DOS QNX W WIN32
.fnm _mbsicmp                   stricmp.gml     DOS       WIN32
.fnn _fmbsicmp                  stricmp.gml     DOS       WIN32
.fn7 _stricoll                  stricoll.gml    DOS QNX W WIN32
.fnw _wcsicoll                  stricoll.gml    DOS QNX W WIN32
.fnm _mbsicoll                  stricoll.gml    DOS       WIN32
.fnc _strinc                    strinc.gml      DOS QNX W WIN32
.fnw _wcsinc                    strinc.gml      DOS QNX W WIN32
.fnm _mbsinc                    strinc.gml      DOS       WIN32
.fnn _fmbsinc                   strinc.gml      DOS       WIN32
.fnc strlcat                    strlcat.gml     DOS QNX W WIN32
.fnw wcslcat                    strlcat.gml     DOS QNX W WIN32
.fnc strlcpy                    strlcpy.gml     DOS QNX W WIN32
.fnw wcslcpy                    strlcpy.gml     DOS QNX W WIN32
.fnc strlen                     strlen.gml      DOS QNX W WIN32
.fnf _fstrlen                   strlen.gml      DOS QNX W WIN32
.fnw wcslen                     strlen.gml      DOS QNX W WIN32
.fnm _mbslen                    strlen.gml      DOS       WIN32
.fnn _fmbslen                   strlen.gml      DOS       WIN32
.fnc strnlen_s                  strnln_s.gml    DOS       WIN32
.fnw wcsnlen_s                  strnln_s.gml    DOS       WIN32
.fnc strlwr                     strlwr.gml      DOS QNX W WIN32
.fnc _strlwr                    strlwr.gml      DOS QNX W WIN32
.fnf _fstrlwr                   strlwr.gml      DOS QNX W WIN32
.fnw _wcslwr                    strlwr.gml      DOS QNX W WIN32
.fnm _mbslwr                    strlwr.gml      DOS       WIN32
.fnn _fmbslwr                   strlwr.gml      DOS       WIN32
.fnc strncasecmp                strncase.gml    DOS QNX W WIN32
.fnc strncat                    strncat.gml     DOS QNX W WIN32
.fnf _fstrncat                  strncat.gml     DOS QNX W WIN32
.fnw wcsncat                    strncat.gml     DOS QNX W WIN32
.fnm _mbsncat                   strncat.gml     DOS       WIN32
.fnn _fmbsncat                  strncat.gml     DOS       WIN32
.fnc strncat_s                  stncat_s.gml    DOS       WIN32
.fnw wcsncat_s                  stncat_s.gml    DOS       WIN32
.fnc strncmp                    strncmp.gml     DOS QNX W WIN32
.fnf _fstrncmp                  strncmp.gml     DOS QNX W WIN32
.fnw wcsncmp                    strncmp.gml     DOS QNX W WIN32
.fnm _mbsncmp                   strncmp.gml     DOS       WIN32
.fnn _fmbsncmp                  strncmp.gml     DOS       WIN32
.fn7 _strncoll                  strncoll.gml    DOS QNX W WIN32
.fnw _wcsncoll                  strncoll.gml    DOS QNX W WIN32
.fnm _mbsncoll                  strncoll.gml    DOS       WIN32
.fnc strncpy                    strncpy.gml     DOS QNX W WIN32
.fnf _fstrncpy                  strncpy.gml     DOS QNX W WIN32
.fnw wcsncpy                    strncpy.gml     DOS QNX W WIN32
.fnm _mbsncpy                   strncpy.gml     DOS       WIN32
.fnn _fmbsncpy                  strncpy.gml     DOS       WIN32
.fnc strncpy_s                  stncpy_s.gml    DOS       WIN32
.fnw wcsncpy_s                  stncpy_s.gml    DOS       WIN32
.fnc strnicmp                   strnicmp.gml    DOS QNX W WIN32
.fnc _strnicmp                  strnicmp.gml    DOS QNX W WIN32
.fnf _fstrnicmp                 strnicmp.gml    DOS QNX W WIN32
.fnw _wcsnicmp                  strnicmp.gml    DOS QNX W WIN32
.fnm _mbsnicmp                  strnicmp.gml    DOS       WIN32
.fnn _fmbsnicmp                 strnicmp.gml    DOS       WIN32
.fn7 _strnicoll                 strnicol.gml    DOS QNX W WIN32
.fnw _wcsnicoll                 strnicol.gml    DOS QNX W WIN32
.fnm _mbsnicoll                 strnicol.gml    DOS       WIN32
.fnc _strninc                   strninc.gml     DOS QNX W WIN32
.fnw _wcsninc                   strninc.gml     DOS QNX W WIN32
.fnm _mbsninc                   strninc.gml     DOS       WIN32
.fnn _fmbsninc                  strninc.gml     DOS       WIN32
.fnc strnset                    strnset.gml     DOS QNX W WIN32
.fn7 _strnset                   strnset.gml     DOS QNX W WIN32
.fnf _fstrnset                  strnset.gml     DOS QNX W WIN32
.fnw _wcsnset                   strnset.gml     DOS QNX W WIN32
.fnm _mbsnset                   strnset.gml     DOS       WIN32
.fnn _fmbsnset                  strnset.gml     DOS       WIN32
.fnc strpbrk                    strpbrk.gml     DOS QNX W WIN32
.fnf _fstrpbrk                  strpbrk.gml     DOS QNX W WIN32
.fnw wcspbrk                    strpbrk.gml     DOS QNX W WIN32
.fnm _mbspbrk                   strpbrk.gml     DOS       WIN32
.fnn _fmbspbrk                  strpbrk.gml     DOS       WIN32
.fnc strrchr                    strrchr.gml     DOS QNX W WIN32
.fnf _fstrrchr                  strrchr.gml     DOS QNX W WIN32
.fnw wcsrchr                    strrchr.gml     DOS QNX W WIN32
.fnm _mbsrchr                   strrchr.gml     DOS       WIN32
.fnn _fmbsrchr                  strrchr.gml     DOS       WIN32
.fnc strrev                     strrev.gml      DOS QNX W WIN32
.fnc _strrev                    strrev.gml      DOS QNX W WIN32
.fnf _fstrrev                   strrev.gml      DOS QNX W WIN32
.fnw _wcsrev                    strrev.gml      DOS QNX W WIN32
.fnm _mbsrev                    strrev.gml      DOS       WIN32
.fnn _fmbsrev                   strrev.gml      DOS       WIN32
.fnc strset                     strset.gml      DOS QNX W WIN32
.fn7 _strset                    strset.gml      DOS QNX W WIN32
.fnf _fstrset                   strset.gml      DOS QNX W WIN32
.fnw _wcsset                    strset.gml      DOS QNX W WIN32
.fnm _mbsset                    strset.gml      DOS       WIN32
.fnn _fmbsset                   strset.gml      DOS       WIN32
.fnc strspn                     strspn.gml      DOS QNX W WIN32
.fnf _fstrspn                   strspn.gml      DOS QNX W WIN32
.fnw wcsspn                     strspn.gml      DOS QNX W WIN32
.fnm _mbsspn                    strspn.gml      DOS       WIN32
.fnn _fmbsspn                   strspn.gml      DOS       WIN32
.fn7 strspnp                    strspnp.gml     DOS QNX W WIN32
.fn7 _strspnp                   strspnp.gml     DOS QNX W WIN32
.fnf _fstrspnp                  strspnp.gml     DOS QNX W WIN32
.fnw _wcsspnp                   strspnp.gml     DOS QNX W WIN32
.fnm _mbsspnp                   strspnp.gml     DOS       WIN32
.fnn _fmbsspnp                  strspnp.gml     DOS       WIN32
.fnc strstr                     strstr.gml      DOS QNX W WIN32
.fnf _fstrstr                   strstr.gml      DOS QNX W WIN32
.fnw wcsstr                     strstr.gml      DOS QNX W WIN32
.fnm _mbsstr                    strstr.gml      DOS       WIN32
.fnn _fmbsstr                   strstr.gml      DOS       WIN32
.fnc _strtime                   strtime.gml     DOS QNX W WIN32
.fnw _wstrtime                  strtime.gml     DOS QNX W WIN32
.fnc strtod                     strtod.gml      DOS QNX W WIN32
.fnw wcstod                     strtod.gml      DOS QNX W WIN32
.fnc strtok                     strtok.gml      DOS QNX W WIN32
.fnf _fstrtok                   strtok.gml      DOS QNX W WIN32
.fnw wcstok                     strtok.gml      DOS QNX W WIN32
.fnm _mbstok                    strtok.gml      DOS       WIN32
.fnm _mbstok_r                  strtok.gml      DOS       WIN32
.fnn _fmbstok                   strtok.gml      DOS       WIN32
.fnn _fmbstok_r                 strtok.gml      DOS       WIN32
.fnc strtok_r                   strtok.gml      DOS       WIN32
.fnc strtok_s                   strtok_s.gml    DOS       WIN32
.fnf _fstrtok_r                 strtok.gml      DOS       WIN32
.fnw wcstok_s                   strtok_s.gml    DOS       WIN32
.fnc strtol                     strtol.gml      DOS QNX W WIN32
.fnw wcstol                     strtol.gml      DOS QNX W WIN32
.fnc strtoll                    strtoll.gml     DOS QNX W WIN32
.fnw wcstoll                    strtoll.gml     DOS QNX W WIN32
.fnc strtoimax                  strtoimx.gml    DOS QNX W WIN32
.fnc wcstoimax                  strtoimx.gml    DOS QNX W WIN32
.fnc strtoul                    strtoul.gml     DOS QNX W WIN32
.fnw wcstoul                    strtoul.gml     DOS QNX W WIN32
.fnc strtoull                   strtoull.gml    DOS QNX W WIN32
.fnw wcstoull                   strtoull.gml    DOS QNX W WIN32
.fnc strtoumax                  strtoumx.gml    DOS QNX W WIN32
.fnc wcstoumax                  strtoumx.gml    DOS QNX W WIN32
.fnc strupr                     strupr.gml      DOS QNX W WIN32
.fnc _strupr                    strupr.gml      DOS QNX W WIN32
.fnf _fstrupr                   strupr.gml      DOS QNX W WIN32
.fnw _wcsupr                    strupr.gml      DOS QNX W WIN32
.fnm _mbsupr                    strupr.gml      DOS       WIN32
.fnn _fmbsupr                   strupr.gml      DOS       WIN32
.fnc strxfrm                    strxfrm.gml     DOS QNX W WIN32
.fnw wcsxfrm                    strxfrm.gml     DOS QNX W WIN32
.fnc swab                       swab.gml        DOS QNX W WIN32
.fnc symlink                    symlink.gml         QXO
.fnc sync                       sync.gml            QXO
.fnc sysconf                    sysconf.gml         QXO
.fnc sysconf                    sysconf.gml     DOS     W WIN32
.fnc sysinfo                    sysinfo.gml     DOS     W WIN32
.fnc system                     system.gml      DOS QNX W WIN32
.fnw _wsystem                   system.gml      DOS       WIN32
.fnc tan                        tan.gml         DOS QNX W WIN32
.fnc tanh                       tanh.gml        DOS QNX W WIN32
.fnc tcdrain                    tcdrain.gml         QXO
.fnc tcdropline                 tcdrplin.gml        QXO
.fnc tcflow                     tcflow.gml          QXO
.fnc tcflush                    tcflush.gml         QXO
.fnc tcgetattr                  tcgetatr.gml        QXO
.fnc tcgetpgrp                  tcgetpgr.gml        QXO
.fnc tcsendbreak                tcsndbrk.gml        QXO
.fnc tcsetattr                  tcsetatr.gml        QXO
.fnc tcsetct                    tcsetct.gml         QXO
.fnc tcsetpgrp                  tcsetpgr.gml        QXO
.fnc tell                       tell.gml        DOS QNX W WIN32
.fnc _tell                      tell.gml        DOS       WIN32
.fn8 _telli64                   tell.gml        DOS       WIN32
.fn7 _tempnam                   tempnam.gml     DOS       WIN32
.fnw _wtempnam                  tempnam.gml     DOS       WIN32
.fnc term_attr_type             att_type.trm        QX40
.fnc term_axis                  axis.trm            QX40
.fnc term_bar                   bar.trm             QX40
.fnc term_box                   box.trm             QX40
.fnc term_box_fill              box_fill.trm        QX40
.fnc term_box_off               box_off.trm         QX40
.fnc term_box_on                box_on.trm          QX40
.fnc term_clear                 clear.trm           QX40
.fnc term_color                 color.trm           QX40
.fnc term_cur                   cur.trm             QX40
.fnc term_delete_char           del_char.trm        QX40
.fnc term_delete_line           del_line.trm        QX40
.fnc term_down                  down.trm            QX40
.fnc term_field                 field.trm           QX40
.fnc term_fill                  tmfill.gml          QX40
.fnc term_flush                 flush.trm           QX40
.fnc term_get_line              get_line.trm        QX40
.fnc term_home                  home.trm            QX40
.fnc term_init                  init.trm            QX40
.fnc term_insert_char           tminschr.gml        QX40
.fnc term_insert_line           ins_line.trm        QX40
.fnc term_insert_off            ins_off.trm         QX40
.fnc term_insert_on             ins_on.trm          QX40
.fnc term_key                   key.trm             QX40
.fnc term_left                  left.trm            QX40
.fnc term_lmenu                 lmenu.trm           QX40
.fnc term_load                  load.trm            QX40
.fnc term_menu                  menu.trm            QX40
.fnc term_mouse_flags           tmmsflgs.gml        QX40
.fnc term_mouse_hide            tmmshide.gml        QX40
.fnc term_mouse_move            tmmsmove.gml        QX40
.fnc term_mouse_off             tmmsoff.gml         QX40
.fnc term_mouse_on              tmmson.gml          QX40
.fnc term_printf                printf.trm          QX40
.fnc term_relearn_size          tmresize.gml        QX40
.fnc term_resize_off            tmrszoff.gml        QX40
.fnc term_resize_on             tmrszon.gml         QX40
.fnc term_restore               restore.trm         QX40
.fnc term_restore_image         rest_im.trm         QX40
.fnc term_right                 right.trm           QX40
.fnc term_save_image            save_im.trm         QX40
.fnc term_scroll_down           scl_down.trm        QX40
.fnc term_scroll_up             scl_up.trm          QX40
.fnc term_state                 state.trm           QX40
.fnc term_type                  type.trm            QX40
.fnc term_unkey                 unkey.trm           QX40
.fnc term_up                    up.trm              QX40
.fnc term_video_off             vid_off.trm         QX40
.fnc term_video_on              vid_on.trm          QX40
.fnc tgamma                     tgamma.gml      DOS QNX W WIN32
.fnc time                       time.gml        DOS QNX W WIN32
.fnc timer_create               timercre.gml        QXO
.fnc timer_delete               timerdel.gml        QXO
.fnc timer_gettime              timerget.gml        QXO
.fnc timer_settime              timerset.gml        QXO
.fnc timer_create               tmrcreat.gml    DOS     W WIN32
.fnc timer_delete               tmrdel.gml      DOS     W WIN32
.fnc timer_gettime              tmrget.gml      DOS     W WIN32
.fnc timer_settime              tmrset.gml      DOS     W WIN32
.fnc timer_getoverrun           tmrover.gml     DOS     W WIN32
.fnc times                      times.gml           QX0
.fnc tmpfile                    tmpfile.gml     DOS QNX W WIN32
.fnc tmpfile_s                  tmpfil_s.gml    DOS QNX W WIN32
.fnc tmpnam_s                   tmpnam_s.gml    DOS QNX W WIN32
.fnw _wtmpnam_s                 tmpnam_s.gml    DOS       WIN32
.fnc tmpnam                     tmpnam.gml      DOS QNX W WIN32
.fnw _wtmpnam                   tmpnam.gml      DOS       WIN32
.fnc tolower                    tolower.gml     DOS QNX W WIN32
.fnc _tolower                   tolower.gml     DOS QNX W WIN32
.fnw towlower                   tolower.gml     DOS QNX W WIN32
.fnc toupper                    toupper.gml     DOS QNX W WIN32
.fnc _toupper                   toupper.gml     DOS QNX W WIN32
.fnw towupper                   toupper.gml     DOS QNX W WIN32
.fnw towctrans                  towctrns.gml    DOS QNX W WIN32
.fnc Trace                      trace.gml           QXO
.fnc Trace0                     trace.gml           QXO
.fnc Trace0b                    trace.gml           QXO
.fnc Trace1                     trace.gml           QXO
.fnc Trace2                     trace.gml           QXO
.fnc Trace3                     trace.gml           QXO
.fnc Trace3b                    trace.gml           QXO
.fnc Trace4                     trace.gml           QXO
.fnc Trace5                     trace.gml           QXO
.fnc Trace6                     trace.gml           QXO
.fnc Trigger                    trigger.gml         QXO
.fnc trunc                      trunc.gml       DOS QNX W WIN32
.fnc ttyname                    ttyname.gml     DOS QNX W WIN32
.fnc ttyname_r                  ttyname.gml     DOS QNX W WIN32
.fnc tzset                      tzset.gml       DOS QNX W WIN32
.fnc ulltoa                     ulltoa.gml      DOS QNX W WIN32
.fnc _ulltoa                    ulltoa.gml      DOS QNX W WIN32
.fnw _ulltow                    ulltoa.gml      DOS QNX W WIN32
.fnc ultoa                      ultoa.gml       DOS QNX W WIN32
.fnc _ultoa                     ultoa.gml       DOS QNX W WIN32
.fnw _ultow                     ultoa.gml       DOS QNX W WIN32
.fnc umask                      umask.gml       DOS QNX W WIN32
.fnc _umask                     umask.gml       DOS       WIN32
.fnc umount                     umount.gml          QXO
.fnc uname                      uname.gml           QXO
.fnc ungetc                     ungetc.gml      DOS QNX W WIN32
.fnw ungetwc                    ungetc.gml      DOS QNX W WIN32
.fnc ungetch                    ungetch.gml     DOS QNX W WIN32
.fnc unlink                     unlink.gml      DOS QNX W WIN32
.fnc _unlink                    unlink.gml      DOS       WIN32
.fnw _wunlink                   unlink.gml      DOS       WIN32
.fnc unlock                     unlock.gml      DOS QNX W WIN32
.fnc _unregisterfonts           gr_unreg.gml    DOS QNX W GRAPH
.fnc utime                      utime.gml       DOS QNX W WIN32
.fn7 _utime                     utime.gml       DOS       WIN32
.fnw _wutime                    utime.gml       DOS       WIN32
.fnc utoa                       utoa.gml        DOS QNX W WIN32
.fnc _utoa                      utoa.gml        DOS QNX W WIN32
.fnw _utow                      utoa.gml        DOS QNX W WIN32
.fnc va_arg                     va_arg.gml      DOS QNX W WIN32
.fnc va_end                     va_end.gml      DOS QNX W WIN32
.fnc va_start                   va_start.gml    DOS QNX W WIN32
.fnc _vbprintf                  _vbprint.gml    DOS QNX W WIN32
.fnw _vbwprintf                 _vbprint.gml    DOS QNX W WIN32
.fnc vcprintf                   vcprintf.gml    DOS QNX W WIN32
.fnc vcscanf                    vcscanf.gml     DOS QNX W WIN32
.fnc vfprintf                   vfprintf.gml    DOS QNX W WIN32
.fnw vfwprintf                  vfprintf.gml    DOS QNX W WIN32
.fnc vfprintf_s                 vfprtf_s.gml    DOS QNX W WIN32
.fnw vfwprintf_s                vfprtf_s.gml    DOS QNX W WIN32
.fnc vfscanf                    vfscanf.gml     DOS QNX W WIN32
.fnw vfwscanf                   vfscanf.gml     DOS QNX W WIN32
.fnc vfscanf_s                  vfscnf_s.gml    DOS QNX W WIN32
.fnw vfwscanf_s                 vfscnf_s.gml    DOS QNX W WIN32
.fnc vprintf                    vprintf.gml     DOS QNX W WIN32
.fnw vwprintf                   vprintf.gml     DOS QNX W WIN32
.fnc vprintf_s                  vprntf_s.gml    DOS QNX W WIN32
.fnw vwprintf_s                 vprntf_s.gml    DOS QNX W WIN32
.fnc vscanf                     vscanf.gml      DOS QNX W WIN32
.fnw vwscanf                    vscanf.gml      DOS QNX W WIN32
.fnc vscanf_s                   vscanf_s.gml    DOS QNX W WIN32
.fnw vwscanf_s                  vscanf_s.gml    DOS QNX W WIN32
.fnc _vsnprintf                 _vsnprnt.gml    DOS QNX W WIN32
.fnc _vsnwprintf                _vsnprnt.gml    DOS QNX W WIN32
.fnc vsnprintf                  vsnprint.gml    DOS QNX W WIN32
.fnc vsnwprintf                 vsnprint.gml    DOS QNX W WIN32
.fnc vsnprintf_s                vsnprt_s.gml    DOS QNX W WIN32
.fnc vsnwprintf_s               vsnprt_s.gml    DOS QNX W WIN32
.fnc vsprintf                   vsprintf.gml    DOS QNX W WIN32
.fnw vswprintf                  vsprintf.gml    DOS QNX W WIN32
.fnc vsprintf_s                 vsprtf_s.gml    DOS QNX W WIN32
.fnw vswprintf_s                vsprtf_s.gml    DOS QNX W WIN32
.fnc vsscanf                    vsscanf.gml     DOS QNX W WIN32
.fnw vswscanf                   vsscanf.gml     DOS QNX W WIN32
.fnc vsscanf_s                  vsscnf_s.gml    DOS QNX W WIN32
.fnw vswscanf_s                 vsscnf_s.gml    DOS QNX W WIN32
.fnc wait                       wait.gml        DOS QNX W WIN32
.fnc waitpid                    waitpid.gml         QXO
.fnw wcrtomb                    wcrtomb.gml     DOS       WIN32
.fnn _fwcrtomb                  wcrtomb.gml     DOS       WIN32
.fnw wcrtomb_s                  wcrtom_s.gml    DOS       WIN32
.fnw _fwcrtomb_s                wcrtom_s.gml    DOS       WIN32
.fnw wcsrtombs                  wcsrtomb.gml    DOS       WIN32
.fnw _fwcsrtombs                wcsrtomb.gml    DOS       WIN32
.fnw wcsrtombs_s                wcsrto_s.gml    DOS       WIN32
.fnw _fwcsrtombs_s              wcsrto_s.gml    DOS       WIN32
.fnw wcstombs                   wcstombs.gml    DOS QNX W WIN32
.fnw _fwcstombs                 wcstombs.gml    DOS       WIN32
.fnw wcstombs_s                 wcstom_s.gml    DOS QNX W WIN32
.fnw _fwcstombs_s               wcstom_s.gml    DOS       WIN32
.fnw wctob                      wctob.gml       DOS       WIN32
.fnw wctomb                     wctomb.gml      DOS QNX W WIN32
.fnw _fwctomb                   wctomb.gml      DOS       WIN32
.fnw wctomb_s                   wctomb_s.gml    DOS QNX W WIN32 TEST
.fnw _fwctomb_s                 wctomb_s.gml    DOS       WIN32 TEST
.fnw wctrans                    wctrans.gml     DOS QNX W WIN32
.fnw wctype                     wctype.gml      DOS QNX W WIN32
.fnc _wrapon                    gr_wrpon.gml    DOS QNX W GRAPH
.fnc write                      write.gml       DOS QNX W WIN32
.fnc _write                     write.gml       DOS       WIN32
.fnc writev                     write.gml           QXO
.fnc Writemsg                   writemsg.gml        QXO
.fnc Writemsgmx                 writemsx.gml        QXO
.fnc Yield                      yield.gml           QXO
.fnc zentohan                   zentohan.gml    NEC

.*
.* .ty End of build pass - count = &__idx
.*
.if '&machsys' eq 'TEST' .do begin
:cmt.:set symbol="machsys" value="QXO".
.do end
.*
.ty ***NOTE*** Number of functions described by this document for &machsys is &__idx
