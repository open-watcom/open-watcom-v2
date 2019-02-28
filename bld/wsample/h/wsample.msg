pick( ERR_DISK_FULL
        , "Fatal Error: Disk full."
        , "d‘å‚ÈƒGƒ‰[:ƒfƒBƒXƒN‚ª‚¢‚Á‚Ï‚¢‚Å‚·"
)
pick( ERR_SAMPLES_LOST
        , "ƒGƒ‰[:ƒTƒ“ƒvƒŠƒ“ƒOEƒf[ƒ^‚Ìˆê•”‚ª¸‚í‚ê‚Ü‚µ‚½"
        , "Error: some samples have been lost"
)
pick( ERR_SAMPLE_TRUNCATE
        , "Error: Out of disk space. Sample file truncated"
        , "ƒGƒ‰[:ƒfƒBƒXƒN‚Ì‹ó‚«—Ìˆæ‚ª•s‘«‚Å‚·.ƒTƒ“ƒvƒ‹ƒtƒ@ƒCƒ‹‚ğØ‚è‹l‚ß‚Ü‚· "
)
pick( MSG_EXPECTING
        , "Expecting either a '=' or a '#' instead found: "
        , "'=' ‚Ü‚½‚Í '#' ‚ª‚ ‚é‚×‚«‰ÓŠ‚ÉŸ‚ÌƒR[ƒh‚ª‚ ‚è‚Ü‚µ‚½: "
)
pick( MSG_INVALID_CHAR
        , "Invalid character after number"
        , "”š‚ÌŒã‚É–³Œø‚È•¶š‚ª‚ ‚è‚Ü‚·"
)
pick( MSG_OUT_OF_RANGE
        , "number is outside of its allowable range"
        , "”š‚ª—LŒø”ÍˆÍ“à‚É‚ ‚è‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_BUFF
        , "Can't allocate sample buffer"
        , "ƒTƒ“ƒvƒŠƒ“ƒO—pƒoƒbƒtƒ@‚Ìƒƒ‚ƒŠ‚ªŠm•Û‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_CALLGRAPH_BUFF
        , "Can't allocate callgraph buffer"
        , "ƒR[ƒ‹ƒOƒ‰ƒt—pƒoƒbƒtƒ@‚Ìƒƒ‚ƒŠ‚ğŠm•Û‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_VERSION
        , "Can't work with this version of the operating system"
        , "‚±‚Ìƒo[ƒWƒ‡ƒ“‚ÌƒIƒyƒŒ[ƒeƒBƒ“ƒOEƒVƒXƒeƒ€‚Ìã‚Å‚Í“®ì‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_FILE
        , "Can't open sample file"
        , "ƒTƒ“ƒvƒ‹ƒtƒ@ƒCƒ‹‚ğƒI[ƒvƒ“‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_PROGRAM
        , "Can't find program: "
        , "ƒvƒƒOƒ‰ƒ€‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ: "
)
pick( MSG_BUFF_SMALL
        , "Buffer size too small for timer rate"
        , "ƒ^ƒCƒ}[ŠÔŠu‚É‘Î‚µ‚Äƒoƒbƒtƒ@‚ª¬‚³‚·‚¬‚Ü‚·"
)
pick( MSG_INVALID_OPTION
        , "invalid option: "
        , "–³Œø‚ÈƒIƒvƒVƒ‡ƒ“: "
)

pick( MSG_USAGE_LN_1
        , "Usage: wsample [options] program [program arguments]"
        , "g—p•û–@: wsample [options] program [program arguments]"
)
pick( MSG_USAGE_LN_2
        , "Options:"
        , "ƒIƒvƒVƒ‡ƒ“:"
)
#ifndef __WINDOWS__
pick( MSG_USAGE_LN_3
        , "    -b=<size> specify the buffer size for samples (default: 63KB)"
        , "    -b=<size> ƒTƒ“ƒvƒŠƒ“ƒO—pƒoƒbƒtƒ@ƒTƒCƒY (ÃŞÌ«ÙÄ: 63KB)"
)
#endif
pick( MSG_USAGE_LN_4
        , "    -c        callgraph information is to be included in the sample file"
        , "    -c        ƒR[ƒ‹ƒOƒ‰ƒtî•ñ‚ğƒTƒ“ƒvƒ‹ƒtƒ@ƒCƒ‹‚ÉŠÜ‚ß‚Ü‚·"
)
pick( MSG_USAGE_LN_5
        , "    -f=<file> specify the output filename for the sample file"
        , "    -f=<file> ƒTƒ“ƒvƒ‹ƒtƒ@ƒCƒ‹‚Ìƒtƒ@ƒCƒ‹–¼"
)
#if defined(__NETWARE__)
pick( MSG_USAGE_LN_6
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload"
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload"
)
pick( MSG_USAGE_LN_7
        , "    -e        Estimate the timer terminal count for -o= (see above)"
        , "    -e        Estimate the timer terminal count for -o= (see above)"
)
#endif

pick( MSG_OPTIONS_1
        , "    -r=<rate> specify the sampling rate"
        , "    -r=<rate> ƒTƒ“ƒvƒŠƒ“ƒOŠÔŠu"
)
#ifdef __DOS__      /* messages in sampdos */
pick( MSG_OPTIONS_2
        , "    -i=<intr> specify an interrupt number for assigning samples"
        , "    -i=<intr> ƒTƒ“ƒvƒŠƒ“ƒO‚ğs‚¤Š„‚İ”Ô†"
)
pick( MSG_OPTIONS_3
        , "    -d        disable assignment of DOS interrupts to application"
        , "    -d        DOSŠ„‚İ‚ÌŠÄ‹‚Ì‹Ö~"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_OPTIONS_2
        , "    -s        start the program in a new session"
        , "    -s        V‚µ‚¢ƒZƒbƒVƒ‡ƒ“‚Å‚ÌƒvƒƒOƒ‰ƒ€‚ÌŠJn"
)
#endif

#ifndef __WINDOWS__
pick( MSG_USAGE_LN_8
        , "\n    <size>    is a number in the range 1 to 63 (default: 63 KB)"
        , "\n    <size>    ‚Í 1 ‚©‚ç 63 ‚Ü‚Å‚Ì”ÍˆÍ‚Å‚· (ÃŞÌ«ÙÄ: 63 KB)"
)
#endif

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu(ĞØ•b’PˆÊ)‚ğ"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              w’è‚·‚é”’l(”ÍˆÍ:1‚©‚ç55‚Ü‚Å)‚Å‚· (ÃŞÌ«ÙÄ: 55)"
)
pick( MSG_EXPLAIN_3
        , "    <intr>    is a hex (base 16) number (range 20 to ff) specifying"
        , "    <intr>    ‚Í16i”(”ÍˆÍ:20‚©‚çff‚Ü‚Å)‚ÅAˆÈ‰º‚Ì“Á•Ê‚È•û–@‚Åˆ—‚·‚é"
)
pick( MSG_EXPLAIN_4
        , "              an interrupt number to treat in a special manner - "
        , "              Š„‚İ”Ô†‚Å‚· - "
)
pick( MSG_EXPLAIN_5
        , "              any samples during the execution of the software interrupt"
        , "              ƒ\ƒtƒgƒEƒFƒAŠ„‚İÀs’†‚ÌƒTƒ“ƒvƒŠƒ“ƒO‚ÍAŠ„‚İ‚ğ”­¶‚µ‚½–½—ß‚É"
)
pick( MSG_EXPLAIN_6
        , "              are assigned to the instruction that invoked the interrupt"
        , "              Š„‚è“–‚Ä‚Ü‚·"
)
pick( MSG_EXPLAIN_7
        , "              (default: the DOS interrupt 21 is monitored)"
        , "              (ÃŞÌ«ÙÄ: DOS Š„‚İ 21 ‚ªÓÆÀ°‚³‚ê‚Ä‚¢‚Ü‚·)"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              w’è‚·‚é”’l (”ÍˆÍ:27 ‚©‚ç 1000‚Ü‚Å) ‚Å‚· (ÃŞÌ«ÙÄ: 55)"
)
#endif

#ifdef __LINUX__      /* messages in samplinux */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 10)"
        , "              w’è‚·‚é”’l (”ÍˆÍ:27 ‚©‚ç 1000‚Ü‚Å) ‚Å‚· (ÃŞÌ«ÙÄ: 10)"
)
#endif

#if defined(__PLS__) || defined(__NETWARE__)      /* messages in samppls and sampnlm */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_2
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_3
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              w’è‚·‚é”’l (”ÍˆÍ:1 ‚©‚ç 55‚Ü‚Å) ‚Å‚· (ÃŞÌ«ÙÄ: 55)"
)
pick( MSG_EXPLAIN_4
        , "              NB: use this switch with caution. It will cause the time"
        , "              ’ˆÓ: ‚±‚ÌƒXƒCƒbƒ`‚Ìg—p‚É‚Í’ˆÓ‚µ‚Ä‚­‚¾‚³‚¢.ƒvƒƒOƒ‰ƒ€Às’†‚Ì"
)
pick( MSG_EXPLAIN_5
        , "                  to be incorrect during program execution. Also, rates"
        , "                  ŠÔ‚ª³‚µ‚­‚È‚­‚È‚è‚Ü‚·.‚Ü‚½A’l‚ª¬‚³‚·‚¬‚é‚ÆA’x‚¢ƒVƒXƒeƒ€"
)
pick( MSG_EXPLAIN_6
        , "                  which are too small, may crash slower systems."
        , "                  ‚Ìê‡AƒNƒ‰ƒbƒVƒ…‚·‚é‰Â”\«‚ª‚ ‚è‚Ü‚·."
)
#endif

#if defined(__RSI__)      /* messages in samprsi */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              w’è‚·‚é”’l (”ÍˆÍ:1 ‚©‚ç 55‚Ü‚Å) ‚Å‚· (ÃŞÌ«ÙÄ: 55)"
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    ‚ÍƒTƒ“ƒvƒŠƒ“ƒOŠÔ‚Ì‚¨‚æ‚»‚ÌŠÔŠÔŠu (’PˆÊ:ĞØ•b) ‚ğ"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              w’è‚·‚é”’l (”ÍˆÍ:1 ‚©‚ç 1000‚Ü‚Å) ‚Å‚· (ÃŞÌ«ÙÄ: 55)"
)
#endif

pick( MSG_USAGE_LN_9
        , "\n   '#' may be used instead of '=' in options (eg. -b#10)"
        , "\n   ƒIƒvƒVƒ‡ƒ“‚Ì’†‚Å‚Í '=' ‚Ì‘ã‚è‚É '#' ‚ğg‚Á‚Ä‚­‚¾‚³‚¢ (—á -b#10)"
)

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_SAMPLE_1
        , "Too many sections in program"
        , "ƒvƒƒOƒ‰ƒ€‚Ì’†‚ÌƒZƒNƒVƒ‡ƒ“‚ª‘½‚·‚¬‚Ü‚·"
)
pick( MSG_SAMPLE_2
        , "Interrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored"
        , "Š„‚İ 34 ‚©‚ç 3d ‚Í 80x87 ƒGƒ~ƒ…ƒŒ[ƒVƒ‡ƒ“‚Ì‚½‚ß‚É—\–ñ‚³‚ê‚Ä‚¨‚èAƒ‚ƒjƒ^[‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_3
        , "Cannot monitor more than eight interrupts"
        , "9ŒÂˆÈã‚ÌŠ„‚İ‚Íƒ‚ƒjƒ^[‚Å‚«‚Ü‚¹‚ñ"
)
#endif

#if defined( __OS2__ ) && defined( _M_I86 )      /* messages in sampos2 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "Às’†: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "“à•”ƒGƒ‰[: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ƒtƒ@ƒCƒ‹‚ğÀs‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "V‚µ‚¢ƒXƒŒƒbƒh‚ğì‚ê‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "ƒXƒŠ[ƒpEƒXƒŒƒbƒh‚Ì—Dæ‡ˆÊ‚ğã‚°‚ç‚ê‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "‹L–@‚ªŠÔˆá‚Á‚Ä‚¢‚Ü‚·"
)
pick( MSG_SAMPLE_7
        , "DosPTrace( GO ) failed"
        , "DosPTrace( GO ) ‚ª¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_8
        , "Cannot find OS/2 2.x sampler"
        , "OS/2 2.x —pƒTƒ“ƒvƒ‰‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_9
        , "Cannot start OS/2 2.x sampler"
        , "OS/2 2.x —pƒTƒ“ƒvƒ‰‚ğŠJn‚Å‚«‚Ü‚¹‚ñ"
)
#endif

#if defined( __OS2__ ) && !defined( _M_I86 )     /* messages in sampos22 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "Às’†: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "“à•”ƒGƒ‰[: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ƒtƒ@ƒCƒ‹‚ğÀs‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "V‚µ‚¢ƒXƒŒƒbƒh‚ğì‚ê‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "ƒXƒŠ[ƒpEƒXƒŒƒbƒh‚Ì—Dæ‡ˆÊ‚ğã‚°‚ç‚ê‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "‹L–@‚ªŠÔˆá‚Á‚Ä‚¢‚Ü‚·"
)
pick( MSG_SAMPLE_7
        , "DosDebug call failed"
        , "DosDebug ƒR[ƒ‹‚ª¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_8
        , "DosDebug( STOP ) failed"
        , "DosDebug( STOP ) ‚ª¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_9
        , "Unable to attach to process"
        , "ƒvƒƒZƒX‚ÉÚG‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½"
)
pick( MSG_SAMPLE_10
        , "CPU exception reported"
        , "CPU —áŠO‚ª•ñ‚³‚ê‚Ä‚¢‚Ü‚·"
)
#endif

#ifdef __LINUX__      /* messages in samplnx */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "Às’†: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "“à•”ƒGƒ‰[: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ƒtƒ@ƒCƒ‹‚ğÀs‚Å‚«‚Ü‚¹‚ñ"
)
pick( MSG_SAMPLE_4
        , "ptrace() failed"
        , "ptrace() ‚ª¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_5
        , "Unable to attach to process"
        , "ƒvƒƒZƒX‚ÉÚG‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½"
)
pick( MSG_SAMPLE_6
        , "setitimer() failed"
        , "setitimer() ‚ª¸”s‚µ‚Ü‚µ‚½"
)
#endif

#ifdef __PLS__      /* messages in samppls */
pick( MSG_SAMPLE_1
        , "Internal error"
        , "“à•”ƒGƒ‰["
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ƒ[ƒh‚Å‚«‚Ü‚¹‚ñ: "
)
pick( MSG_SAMPLE_3
        , "Task Exception: "
        , "ƒ^ƒXƒN—áŠO: "
)
pick( MSG_SAMPLE_4
        , "Unexpected Exception: "
        , "—\Šú‚µ‚È‚¢—áŠO: "
)
#endif

#ifdef __NETWARE__      /* messages in sampnlm */
pick( MSG_SAMPLE_1
        , "Error! Cannot execute: "
        , "ƒGƒ‰[! Às‚Å‚«‚Ü‚¹‚ñ: "
)
#endif

#ifdef __RSI__      /* messages in samprsi */
pick( MSG_SAMPLE_1
        , "Task Exception: "
        , "ƒ^ƒXƒN—áŠO: "
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ƒ[ƒh‚Å‚«‚Ü‚¹‚ñ: "
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_SAMPLE_1
        , "Couldn't find any modules!"
        , "ƒ‚ƒWƒ…[ƒ‹‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ!"
)
pick( MSG_SAMPLE_2
        , "Failed to hook interrupts"
        , "Š„‚İ‚ÌƒtƒbƒN‚É¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_3
        , "Failed to get notify hook"
        , "ƒtƒbƒN‚Ì’Ê’m‚ğó‚¯‚é‚Ì‚É¸”s‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_4
        , "Could not start sampling!"
        , "ƒTƒ“ƒvƒŠƒ“ƒO‚ğŠJn‚Å‚«‚Ü‚¹‚ñ!"
)
pick( MSG_SAMPLE_5
        , "Sample buffer flushed"
        , "ƒTƒ“ƒvƒ‹ƒoƒbƒtƒ@‚ğƒtƒ‰ƒbƒVƒ…‚µ‚Ü‚µ‚½"
)
pick( MSG_SAMPLE_6
        , "internal error: "
        , "“à•”ƒGƒ‰[: "
)
pick( MSG_SAMPLE_7
        , "Timer rate set to "
        , "ƒ^ƒCƒ}ŠÔŠu‚ğ "
)
pick( MSG_SAMPLE_8
        , " ms"
        , "ƒ ms"
)
pick( MSG_SAMPLE_9
        , "Determining what other modules are present..."
        , "‘¼‚Ì‚Ç‚Ì‚æ‚¤‚Èƒ‚ƒWƒ…[ƒ‹‚ª‚ ‚é‚©‚ğ’Tõ’†..."
)
pick( MSG_SAMPLE_10
        , "Found "
        , "‚Ù‚©‚É "
)
pick( MSG_SAMPLE_11
        , " other modules"
        , " ŒÂ‚Ìƒ‚ƒWƒ…[ƒ‹‚ª‚ ‚è‚Ü‚·"
)
pick( MSG_SAMPLE_12
        , "Will use WDEBUG.386"
        , "WDEBUG.386‚ğg—p‚µ‚Ü‚·"
)
pick( MSG_SAMPLE_13
        , "Execution finished, total time="
        , "ÀsI—¹, ‘SŠÔ="
)
pick( MSG_SAMPLE_14
        , " seconds"
        , " •b"
)
pick( MSG_SAMPLE_15
        , "Error loading sampler!"
        , "ƒTƒ“ƒvƒ‰Eƒ[ƒhEƒGƒ‰[!"
)
pick( MSG_SAMPLE_16
        , "Cannot close sampler!"
        , "ƒTƒ“ƒvƒ‰‚ğƒNƒ[ƒY‚Å‚«‚Ü‚¹‚ñ!"
)
#endif

#if defined(__RSI__) || defined(__PLS__)
pick( MSG_EXCEPT_0
        , ""
        , ""
)
pick( MSG_EXCEPT_1
        , "divide overflow"
        , "œZƒI[ƒo[ƒtƒ["
)
pick( MSG_EXCEPT_2
        , "non-maskable interrupt"
        , "ƒ}ƒXƒN•s‰Â”\Š„‚İ(ÉİÏ½¶ÌŞÙŠ„‚İ,NMI)"
)
pick( MSG_EXCEPT_3
        , "integer overflow"
        , "®”ƒI[ƒo[ƒtƒ["
)
pick( MSG_EXCEPT_4
        , "bounds check"
        , "‹«ŠEŒŸ¸"
)
pick( MSG_EXCEPT_5
        , "invalid opcode"
        , "–³Œø‚ÈƒIƒyƒR[ƒh"
)
pick( MSG_EXCEPT_6
        , "coprocessor not available"
        , "ƒRƒvƒƒZƒbƒT‚Í‚ ‚è‚Ü‚¹‚ñ"
)
pick( MSG_EXCEPT_7
        , "double fault"
        , "“ñdƒtƒH[ƒ‹ƒg"
)
pick( MSG_EXCEPT_8
        , "coprocessor segment overrun"
        , "ƒRƒvƒƒZƒbƒTEƒZƒOƒƒ“ƒgEƒI[ƒo[ƒ‰ƒ“"
)
pick( MSG_EXCEPT_9
        , "invalid TSS"
        , "–³Œø‚ÈTSS"
)
pick( MSG_EXCEPT_10
        , "segment not present"
        , "ƒZƒOƒƒ“ƒg‚ª(ƒƒ‚ƒŠã‚É)‚ ‚è‚Ü‚¹‚ñ"
)
pick( MSG_EXCEPT_11
        , "stack exception"
        , "ƒXƒ^ƒbƒN—áŠO"
)
pick( MSG_EXCEPT_12
        , "general protection fault"
        , "ˆê”Ê•ÛŒìƒtƒH[ƒ‹ƒg"
)
pick( MSG_EXCEPT_13
        , "page fault"
        , "ƒy[ƒWEƒtƒH[ƒ‹ƒg"
)
pick( MSG_EXCEPT_14
        , "coprocessor error"
        , "ƒRƒvƒƒZƒbƒTEƒGƒ‰["
)
#endif
