:segment ENGLISH
Usage: %s {options} input-filename {options} [output-filename] {options}
Options: (-option is also accepted)
/?	       - print this help message
/q	       - operate quietly
/30	       - stamp file as requiring Windows 3.0
/31	       - stamp file as requiring Windows 3.1 (default)
/dNAME=value   - behave as if #define NAME value was at top of file
/ad	       - generate auto dependency information for use by WMAKE
:segment osi
/bt=windows    - build a WIN16 resource file
/bt=nt	       - build a WIN32 resource file
:elsesegment
:segment nt
/bt=windows    - build a WIN16 resource file
/bt=nt	       - build a WIN32 resource file (default)
:elsesegment
/bt=windows    - build a WIN16 resource file (default)
/bt=nt	       - build a WIN32 resource file
:endsegment
:endsegment
/c=name        - set code page conversion file
/e	       - for a DLL, global memory above EMS line
/fo=name       - set the output RES file to name
/fe=name       - set the output EXE file to name
/i=path        - look in path for include files
/k	       - don't sort segments (same as /s0)
/l	       - program uses LIM 3.2 EMS directly
/m	       - each instance of program has its own EMS bank
/o	       - preprocess only
/p	       - private DLL
/r	       - only build the RES file
/s{0,1,2}      - segment and resource sorting method:
		 0: no sorting, leave segments in the linker order
		 1: move preload segments to front and mark for fast load
		    if possible
		 2: (default) move preload, data, and non-discardable segments
		    to front and mark for fast load if possible
/t	       - protected mode only
:segment DEBUG
/v	       - verbose: print tokens as they are scanned
/v1	       - verbose: print grammar rules as they are reduced
/v2	       - verbose: print both tokens and grammar rules
/v3	       - verbose: print tokens as they are scanned
:endsegment
/x	       - ignore the INCLUDE environment variable
/zk{0,1,2,3}   - double-byte char support:
		 0: (default) Japanese (Kanji, CP 932)
		 1: Chinese (Traditional, CP 950)
		 2: Korean (Wansung, CP 949)
		 3: Chinese (Simplified, CP 936)
/zm	       - output Microsoft format .RES files
/zn	       - don't preprocess the file
:endsegment
:segment JAPANESE
JUsage: %s {options} input-filename {options} [output-filename] {options}
Options: (-option is also accepted)
/?	       - print this help message
/q	       - operate quietly
/30	       - stamp file as requiring Windows 3.0
/31	       - stamp file as requiring Windows 3.1 (default)
/dNAME=value   - behave as if #define NAME value was at top of file
/ad	       - generate auto dependency information for use by WMAKE
:segment osi
/bt=windows    - build a WIN16 resource file
/bt=nt	       - build a WIN32 resource file
:elsesegment
:segment nt
/bt=windows    - build a WIN16 resource file
/bt=nt	       - build a WIN32 resource file (default)
:elsesegment
/bt=windows    - build a WIN16 resource file (default)
/bt=nt	       - build a WIN32 resource file
:endsegment
:endsegment
/e	       - for a DLL, global memory above EMS line
/fo=name       - set the output RES file to name
/fe=name       - set the output EXE file to name
/i=path        - look in path for include files
/k	       - don't sort segments (same as /s0)
/l	       - program uses LIM 3.2 EMS directly
/m	       - each instance of program has its own EMS bank
/o	       - preprocess only
/p	       - private DLL
/r	       - only build the RES file
/s{0,1,2}      - segment and resource sorting method:
		 0: no sorting, leave segments in the linker order
		 1: move preload segments to front and mark for fast load
		    if possible
		 2: (default) move preload, data, and non-discardable segments
		    to front and mark for fast load if possible
/t	       - protected mode only
/v	       - verbose: print tokens as they are scanned
/v1	       - verbose: print grammar rules as they are reduced
/v2	       - verbose: print both tokens and grammar rules
/v3	       - verbose: print tokens as they are scanned
/x	       - ignore the INCLUDE environment variable
/zk{0,1,2,3}   - double-byte char support:
		 0: (default) Japanese (Kanji, CP 932)
		 1: Chinese (Traditional, CP 950)
		 2: Korean (Wansung, CP 949)
		 3: Chinese (Simplified, CP 936)
/zm	       - output Microsoft format .RES files
/zn	       - don't preprocess the file
:endsegment
.
