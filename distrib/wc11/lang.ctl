set relroot=c:\

[ BLOCK .<2> . ]
    set 2=c
    
[ BLOCK <2> c ]    
    set destdir=q:\wc11.b

[ BLOCK <2> ac ]    
    set destdir=e:\publish\wc11.axp

[ BLOCK <2> f77 ]    
    set destdir=q:\wf11.b
    
[ BLOCK <2> jc ]    
    set destdir=e:\publish\wc11.j
    
[ BLOCK <2> jf77 ]    
    set destdir=e:\publish\wf11.j
    
[ BLOCK .<1> . ]    
    @echo off
    echo Languages INSTALL builder 
    echo usage: builder [command] [product] 
    echo .
    echo where [product] is one of [ c f77 ac jc jf77 ]
    echo commands:
    echo 	INF	-	make a new setup.inf file
    echo 	SETUP	-	copy new setup executables
    echo 	LIST	-	regen. the lists of files for the below 
    echo 	MKDIR	-	create the directory structure
    echo 	COPY	-	copy all of the files
    echo 	CMP	-	compare the files in the install with rel2
    echo 	TOUCH	-	wtouch the dates of all appropriate files
    echo 	UPDATE	-	copy AND touch the files
    echo 
    echo 	INSTALL - 	**** DO EVERYTHING YOU NEED FOR A NEW INSTALL *****
    echo 			This is the equivalent of MKDIR + UPDATE + SETUP + INF
    
[ BLOCK <1> install ]
    [ LOG master_<2>.log ]
    
[ BLOCK <1> mkdir install ]
    echo MAKING DIRECTORIES ... Please Wait ....
    [ LOG md_<2>.log ]
    [ INCLUDE md_<2>.ctl ]
    
[ BLOCK <1> update install ]
    [ LOG up_<2>.log ]
    set docmd=acopy
    [ INCLUDE up_<2>.ctl ]
    if not exist <destdir>\vp 		md <destdir>\vp
    if not exist <destdir>\vp\disk1 	md <destdir>\vp\disk1
    if not exist <destdir>\vp\disk2 	md <destdir>\vp\disk2
    if not exist <destdir>\vp\disk3 	md <destdir>\vp\disk3
    xcopy <devdir2>\wvp\cd_setup\disk1\*.* <destdir>\vp\disk1 /s
    xcopy <devdir2>\wvp\cd_setup\disk2\*.* <destdir>\vp\disk2 /s
    xcopy <devdir2>\wvp\cd_setup\disk3\*.* <destdir>\vp\disk3 /s

[ BLOCK <1> list ]
    [ LOG list_<2>.log ]
    watfile/bye (t:200)..\supp\mklist.pgm ^[lang<ver>.dat^] ^[<2>^]
    vi cp_<2>.ctl -s demacro.vi
    vi md_<2>.ctl -s demacro.vi
    vi tch_<2>.ctl -s demacro.vi
    vi up_<2>.ctl -s demacro.vi

[ BLOCK <1> setup install ]
    [ LOG set_<2>.log ]
    copy <2>setup\*.exe <destdir>\
    copy <2>setup\*.sym <destdir>\
    wtouch /f <destdir>\setup.inf <destdir>\*.exe
    
[ BLOCK <1> inf install ]
    [ LOG inf_<2>.log ]
    [ INCLUDE startit.ctl ]
    set DDATE=02-24-98
    set TDATE=02/24/98
    set DTIME=11:00:00
    wtouch timestmp.fil -d <TDATE> -t <DTIME>
    watfile/bye (t:200)..\supp\mkdisk.pgm ^[<DCTRL>^] ^[<2>^]
    mkdisk <DMKOPT>        -i<DINCL> <2> 1.4 mkdisk.lst .\pack <DROOT>
    copy setup.inf <destdir>\setup.inf
    wtouch /f timestmp.fil <destdir>\setup.inf 
    wtouch /f timestmp.fil <destdir>\*.exe 
    
[ BLOCK <1> copy ]
    [ log cp_<2>.log ]
    set docmd=copy
    [ INCLUDE cp_<2>.ctl ]
    
[ BLOCK <1> cmp ]
    [ log cmp_<2>.log ]
    set docmd=cmp
    [ INCLUDE cp_<2>.ctl ]
    
[ BLOCK <1> touch ]
    [ log tch_<2>.log ]
    [ INCLUDE tch_<2>.ctl ]
    
