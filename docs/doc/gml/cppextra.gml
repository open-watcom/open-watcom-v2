.se cpp_co=&$co.
.se cpp_ju=&$ju.
.if &e'&dohelp eq 0 .do begin
:LAYOUT.
:TOC
        toc_levels=7
:TOCH1
        indent=2
:TOCH2
        indent=2
:TOCH3
        indent=0
:TOCH4
        indent=0
:TOCH5
        indent=0
:FIG
        left_adjust=0
:H5
        pre_top_skip=0
        pre_skip=0
        post_skip=0
        page_eject=no
        number_form=none
        display_heading=no
:H6
        pre_top_skip=0
        pre_skip=0
        post_skip=0
        page_eject=no
        number_form=none
        display_heading=no
        font=8
        page_position=left
:eLAYOUT.
.do end

:cmt.=======================================================================
:cmt. general definitions
:cmt.=======================================================================

.gt sepsect add @sepsect
.dm @sepsect begin
.if &e'&dohelp eq 0 .do begin
:ZH0.&*
.do end
:set symbol="headtext$" value=" ".
:set symbol="headtxt0$" value="&*".
.pa odd
.dm @sepsect end

.gt chapter add @chap att
.ga * id any
.dm @chap begin
.if '&*id.' ne '' .do begin
.chap *refid=&*id. &*
.do end
.el .do begin
.chap &*
.do end
:P.
.dm @chap end

.gt section add @section att
.ga * id any
.dm @section begin
.if '&*id.' ne '' .do begin
.section *refid=&*id. &*
.do end
.el .do begin
.section &*
.do end
:P.
.dm @section end

.gt italics add i
.dm i begin
:HP1.&*.:eHP1.
.dm i end

.gt hilite add hi
.dm hi begin
:SF font=2.&*.:eSF.
.dm hi end

.gt mono add mno
.dm mno begin
:SF font=4.&*:eSF.
.dm mno end

.gt cont add cont
.dm cont begin
.ct &*.
.dm cont end

.gt period add @period cont
.dm @period begin
.ct .li .;
.dm @period end

.gt blankline add blankline
.dm blankline begin
.sp
.dm blankline end

.gt break add break
.dm break begin
.br
.dm break end

.gt newpage add newpage
.dm newpage begin
.pa
.dm newpage end


:cmt.=======================================================================
:cmt. keywords
:cmt.=======================================================================
.'se eof=';.mno EOF;.ct '
.'se getarea=';.i get area;.ct '
.'se getptr=';.i get pointer;.ct '
.'se noteof=';.mno __NOT_EOF;.ct '
.'se null=';.mno NULL;.ct '
.'se putarea=';.i put area;.ct '
.'se putptr=';.i put pointer;.ct '
.'se rsvarea=';.i reserve area;.ct '
.'se fmtwidth=';.i format width;.ct '
.'se fmtprec=';.i format precision;.ct '
.'se fillchar=';.i fill character;.ct '
.'se fmtflags=';.mno ios::fmtflags;.ct '
.'se iostate=';.mno ios::iostate;.ct '
.'se errstate=';. error state in the inherited ;.mno ios;. object;.ct '
.'se goodbit=';.mno ios::goodbit;.ct '
.'se badbit=';.mno ios::badbit;.ct '
.'se failbit=';.mno ios::failbit;.ct '
.'se eofbit=';.mno ios::eofbit;.ct '
.'se cin=';.mno cin;.ct '
.'se cout=';.mno cout;.ct '
.'se cerr=';.mno cerr;.ct '
.'se clog=';.mno clog;.ct '

:cmt.=======================================================================
:cmt. macros
:cmt.=======================================================================

.dm topsect begin
.if &e'&dohelp eq 0 .do begin
.topsect1 &*
.do end
.el .do begin
.topsect2 &*
.do end
.dm topsect end

.dm topsect1 begin
.'if '&cl_type.' ne '' .th .do begin
.' .'se headtxt0$=&cl_type.::&*.
.' .'se headtxt1$=&cl_type.::&*.
.do end
.el .'if '&cl2_name.' ne '' .th .do begin
.  .'if '&cl_long.' eq 'yes' .th .do begin
.' .' .'se *tmp=&cl_name.,&cl2_name.::&*.
.  .do end
.  .el .do begin
.' .' .'se *tmp=&cl_name.::&*., &cl2_name.::&*.
.  .do end
.' .'se headtxt0$=&*tmp.
.' .'se headtxt1$=&*tmp.
.do end
.el .'if '&cl3_name.' ne '' .th .do begin
.' .'se headtxt0$=&*.
.' .'se headtxt1$=&*.
.do end
.el .do begin
.' .'se headtxt0$=&cl_name.::&*.
.' .'se headtxt1$=&cl_name.::&*.
.do end
:H6.&*
.dm topsect1 end

.dm topsect2 begin
.'se *cl_tmp=&cl_name.
.'if '&cl_type.' ne '' .th .do begin
.' .'se headtxt0$=&cl_type.::&*.
.' .'se headtxt1$=&cl_type.::&*.
.' .'se *cl_tmp=&cl_type.
.do end
.el .'if '&cl2_name.' ne '' .th .do begin
.  .'if '&cl_long.' eq 'yes' .th .do begin
.' .' .'se *tmp=&cl_name.,&cl2_name.::&*.
.  .do end
.  .el .do begin
.' .' .'se *tmp=&cl_name.::&*., &cl2_name.::&*.
.  .do end
.' .'se headtxt0$=&*tmp.
.' .'se headtxt1$=&*tmp.
.' .'se *cl_tmp=&cl_name.,&cl2_name.
.do end
.el .'if '&cl3_name.' ne '' .th .do begin
.' .'se headtxt0$=&*.
.' .'se headtxt1$=&*.
.do end
.el .do begin
.' .'se headtxt0$=&cl_name.::&*.
.' .'se headtxt1$=&cl_name.::&*.
.do end
.'se *tag=&'translate(&headtxt0$, &trto, &trfrom )
.'if '&prvtag.' eq '&*tag.' .do begin
:HDG.Member function
.do end
.el .do begin
:ZH2 ctx='&*tag.'.&* [&*cl_tmp.]
.' .'if &e'&dohelp eq 1 .do begin
.' .' .'pu 1 .ixsect &* [&*cl_tmp.]
.' .' .'pu 1 .ixsectid &*tag.
.' .' .'se SCTlvl=2
.' .' .cntents &headtxt0$.
.' .do end
.' .'se prvtag=&*tag.
.do end
.dm topsect2 end

.dm libsect begin
.'se *tag=&*. Class Description
.'se *tag=&'translate(&*tag, &trto, &trfrom )
.'if &e'&dohelp eq 0 .do begin
:H5.&* Class Description
.do end
.el .do begin
:ZH2 ctx='&*tag.'.&* Class Description
.pu 1 .ixsect &* Class Description
.pu 1 .ixsectid &*tag.
.se SCTlvl=1
.cntents &* Class Description
.do end
.se headtxt0$=&*.
.se headtxt1$=&*.
.dm libsect end

.gt ARG add arg
.dm arg begin
:ITALICS.&*.
.dm arg end

.gt CLFNM add clfnm att nocont
.ga * cl2 any
.ga * * value '' default
.dm clfnm begin
.'se cl_name='&*.'
.'se cl2_name='&*cl2.'
.'if '&*cl2.' ne '' .th .do begin
.' .'se obj=';. ;.mno &*.;. and ;.mno &*cl2.;. objects;.ct '
.' .'se cls=';. ;.mno &*.;. and ;.mno &*cl2.;. classes;.ct '
.do end
.el .do begin
.' .'se obj=';. ;.mno &*.;. object;.ct '
.' .'se cls=';. ;.mno &*.;. class;.ct '
.do end
.dm clfnm end

.gt HFILE add hfile
.dm hfile begin
.sk 1
:ZDT.Declared:
:ZDD.
:MONO.&*.
.sk 1
.dm hfile end

.gt HDG add hdg
.dm hdg begin
.sk 1
.if &e'&dohelp eq 1 .do begin
.cp &WDWlvl
.do end
:HILITE.&*.
.sk 1
.dm hdg end

.gt SMTICS add smtics
.dm smtics begin
.sk 1
:ZDT.Semantics:
:ZDD.
.dm smtics end

.gt PRTCL add prtcl
.dm prtcl begin
.sk 1
:ZDT.Derived Implementation Protocol:
:ZDD.
.dm prtcl end

.gt RSLTS add rslts
.dm rslts begin
.sk 1
:ZDT.Results:
:ZDD.
.dm rslts end

.gt DEFIMPL add defimpl
.dm defimpl begin
.sk 1
:ZDT.Default Implementation:
:ZDD.
.dm defimpl end

.gt LIBF add libf att nocont
.ga * prot any
.ga * * value '' default
.ga * fmt any
.ga * * value '' default
.ga * cltype any
.ga * * value '' default
.ga * cllong any
.ga * * value '' default
.dm libf begin
:NEWPAGE.
.if &e'&dohelp eq 0 .do begin
.fk begin
.do end
.'se cl_type=&*cltype.
.'se cl3_name=''
.'se cl_long=&*cllong.
.if '&*fmt' eq 'mfun' .do begin
.'  .topsect &*.()
.'  .'se fn=';.mno &*.;. &*prot. member function;.ct '
.do end
.el .if '&*fmt' eq 'ctor' .do begin
.'  .topsect &*.()
.'  .'se fn=';. &*prot ;.mno &cl_name.;. constructor;.ct '
.do end
.el .if '&*fmt' eq 'dtor' .do begin
.'  .topsect &*.()
.'  .'se fn=';. &*prot ;.mno ~~&cl_name.;. destructor;.ct '
.do end
.el .if '&*fmt' eq 'mdata' .do begin
.'  .topsect &*.
.'  .'se fn=';.mno &*.;. &*prot. member data;.ct '
.do end
.el .if '&*fmt' eq 'mtyp' .do begin
.'  .topsect &*.
.'  .'se fn=';.mno &cl_name.::&*.;. member typedef;.ct '
.do end
.el .if '&*fmt' eq 'fun' .do begin
.'  .'se cl3_name='nope'
.'  .topsect &cl_name. &*.()
.'  .'se fn=';.mno &*.;. function;.ct '
.do end
.el .if '&*fmt' eq 'mnp' .do begin
.'  .'se cl3_name='nope'
.'  .topsect &cl_name. &*.()
.'  .'se fn=';.mno &*.;. manipulator;.ct '
.do end
.el .if '&*fmt' eq 'hdrmnp' .do begin
.'  .'se cl3_name='nope'
.'  .topsect &*.
.'  .'se fn=';.mno &*.;.ct '
.do end
.el .if '&*fmt' eq 'hdr' .do begin
.'  .libsect &*.
.'  .'se fn=';.mno &*.;.ct '
.do end
.in 0
:ZDL termhi=2 break.
.dm libf end

.gt eLIBF add elibf
.dm elibf begin
.se fn = ''
:ZeDL.
.if &e'&dohelp eq 0 .do begin
.fk end
.fk dump
.do end
.dm elibf end

.gt CLSS add clss
.dm clss begin
.sk 1
.dm clss end

.gt eCLSS add eclss
.dm eclss begin
.dm eclss end

.gt XMPL add xmpl
.dm xmpl begin
:CMT. here be magic for excluding examples
.cs 99 ON
.fo off
:SF font=5.
.sk
.dm xmpl end

:CMT. here be magic for excluding examples
.gt eXMPL add exmpl CSOFF
.dm exmpl begin
.sk
:eSF.
.co &cpp_co.
.ju &cpp_ju.
.dm exmpl end

.gt DVFML add dvfml
.dm dvfml begin
:ZDT.Derived from:
.se dvfmc = 0
:ZDD.
.dm dvfml end

.gt DVFM add dvfm
.dm dvfm begin
.if '&dvfmc.' eq '1' .do begin
:CONT.,
.do end
.se dvfmc = 1
:I1.&*.
:MONO.&*.
.dm dvfm end

.gt eDVFML add edvfml
.dm edvfml begin
.sk 1
.dm edvfml end

.gt DVBYL add dvbyl
.dm dvbyl begin
:ZDT.Derived by:
.se dvbyc = 0
:ZDD.
.dm dvbyl end

.gt DVBY add dvby
.dm dvby begin
.if '&dvbyc.' eq '1' .do begin
:CONT.,
.do end
.se dvbyc = 1
:I1.&*.
:MONO.&*.
.dm dvby end

.gt eDVBYL add edvbyl
.dm edvbyl begin
.sk 1
.dm edvbyl end

.gt MFNL add mfnl
.dm mfnl begin
.ju off
.sk 1
.dm mfnl end

.gt MFN add mfn att nocont
.ga * index any
.ga * * value '' default
.dm mfn begin
:MONO.&*.
.br
.mfidx &*index.
.dm mfn end

.gt MFCD add mfcd att nocont
.ga * cd_idx any
.ga * * value '' default
.dm mfcd begin
:MONO.&*.
.br
:CDIDX index=&*cd_idx.
.dm mfcd end

.gt MFNFLF add mfnflf att nocont
.ga * ind any
.ga * * value 0 default
.dm mfnflf begin
:MONO.&*.
.br
.dm mfnflf end

.gt eMFNL add emfnl
.dm emfnl begin
.ju &cpp_ju.
.sk 1
.dm emfnl end

.gt RFNL add rfnl
.dm rfnl begin
.sk 1
.fo off
.dm rfnl end

.gt RFN add rfn att nocont
.ga * index any
.ga * * value '' default
.dm rfn begin
:MONO.&*.
.br
.rfidx &*index.
.dm rfn end

.gt RFNFLF add rfnflf att nocont
.ga * ind any
.ga * * value 0 default
.dm rfnflf begin
:MONO.&*.
.br
.dm rfnflf end

.gt eRFNL add erfnl
.dm erfnl begin
:MONO.&*.
.co &cpp_co.
.ju &cpp_ju.
.sk 1
.dm erfnl end

.gt CDIDX add cdidx att nocont
.ga * index any
.ga * * value '' default
.dm cdidx begin
.if '&*index' eq 'c' .do begin
:ZI1.&cl_name.::&cl_name.
:IH1.constructor
:I2.&cl_name.
.pu 1 .ixline 'constructor' '&cl_name.'
.do end
.el .if '&*index' eq 'd' .do begin
:ZI1.&cl_name.::~~&cl_name.
:IH1.destructor
:I2.&cl_name.
.pu 1 .ixline 'destructor' '&cl_name.'
.do end
.dm cdidx end

.gt TYPL add typl
.dm typl begin
.sk 1
.fo off
.dm typl end

.gt TYP add typ att nocont
.ga * index any
.ga * * value '' default
.dm typ begin
:MONO.&*.
.br
:ZI1.&*index
.dm typ end

.gt eTYPL add etypl
.dm etypl begin
.co &cpp_co.
.ju &cpp_ju.
.sk 1
.dm etypl end

.gt MTYPL add mtypl
.dm mtypl begin
.sk 1
.fo off
.dm mtypl end

.gt MTYPFLF add mtypflf att nocont
.ga * ind any
.ga * * value 0 default
.dm mtypflf begin
:MONO.&*.
.br
.dm mtypflf end

.gt MTYP add mtyp att nocont
.ga * index any
.ga * * value '' default
.dm mtyp begin
:MONO.&*.
.br
.mtidx &*index.
.dm mtyp end

.gt eMTYPL add emtypl
.dm emtypl begin
.co &cpp_co.
.ju &cpp_ju.
.sk 1
.dm emtypl end

.gt SNPL add snpl
.dm snpl begin
:ZDT.Synopsis:
:ZDD.
.sk -1
.dm snpl end

.gt SNPF add snpf att nocont
.ga * index any
.dm snpf begin
:MONO.&*.
.br
.mfidx &*index.
.dm snpf end

.gt SNPD add snpd att nocont
.ga * index any
.dm snpd begin
:MONO.&*.
.br
.mdidx &*index.
.dm snpd end

.gt SNPT add snpt att nocont
.ga * index any
.dm snpt begin
:MONO.&*.
.br
.mtidx &*index.
.dm snpt end

.gt SNPCD add snpcd att nocont
.ga * cd_idx any
.dm snpcd begin
:MONO.&*.
.br
:CDIDX index=&*cd_idx.
.dm snpcd end

.gt SNPR add snpr att nocont
.ga * index any
.dm snpr begin
:MONO.&*.
.br
.rfidx &*index.
.dm snpr end

.gt SNPM add snpm att nocont
.ga * index any
.dm snpm begin
:MONO.&*.
.br
.mnpidx &*index.
.dm snpm end

.gt SNPFLF add snpflf att nocont
.ga * ind any
.ga * * value 0 default
.dm snpflf begin
:MONO.&*.
.br
.dm snpflf end

.gt eSNPL add esnpl
.dm esnpl begin
.sk 1
.dm esnpl end

.dm mtidx begin
:ZI1.&cl_name.::&*.
:IH1.&*., member enumeration
:I2.&cl_name.
.pu 1 .ixline 'member enumeration' '&*'
.dm mtidx end

.dm otidx begin
:ZI1.&ocl_name.::&*.
:IH1.&*., member enumeration
:I2.&ocl_name.
.pu 1 .ixline 'member enumeration' '&*'
.dm otidx end

.dm mfidx begin
:ZI1.&cl_name.::&*.
:IH1.&*., member function
:I2.&cl_name.
.pu 1 .ixline 'member function' '&*'
.if '&cl2_name' ne '' .do begin
:ZI1.&cl2_name.::&*.
:IH1.&*., member function
:I2.&cl2_name.
.do end
.dm mfidx end

.dm ofidx begin
:ZI1.&ocl_name.::&*.
:IH1.&*., member function
:I2.&ocl_name.
.pu 1 .ixline 'member function' '&*'
.dm ofidx end

.dm mdidx begin
:ZI1.&cl_name.::&*.
:IH1.&*., member data
:I2.&cl_name.
.pu 1 .ixline 'member data' '&*'
.dm mdidx end

.dm rfidx begin
:IH1.&cl_name. related functions
:I2.&*.
:IH1.&*., related function
:I2.&cl_name.
.pu 1 .ixline 'related function' '&*'
.dm rfidx end

.dm mnpidx begin
:ZI1.&*., manipulator
:IH1.manipulators
:I2.&*.
:IH1.&cl_name. manipulators
:I2.&*.
.pu 1 .ixline 'manipulator' '&*'
.dm mnpidx end

.gt MANIPL add manipl
.dm manipl begin
.sk 1
.fo off
.dm manipl end

.gt MANIP add manip att nocont
.ga * index any
.ga * * value '' default
.dm manip begin
.mnpidx &*index.
:MONO.&*.
.br
.dm manip end

.gt eMANIPL add emanipl
.dm emanipl begin
.co &cpp_co.
.ju &cpp_ju.
.sk 1
.dm emanipl end

.*
.dm fi begin
:SF font=4.
..li 1;&*.
:eSF.
.dm fi end
.*
.dm fname delete
.dm fname begin
.fi &*.
.dm fname end
.*
.dm efname delete
.dm efname begin
.if &l'&*. eq 0
.   .th ..ct
.'   .'el ..'ct ..li 1;&*.
.dm efname end
.*
.gt fname del
.gt efname del
.gt fname add fname cont
.gt efname add efname
.*
.dm csect delete
.dm csect begin
.se *tmp=&sysin.
.in 0
.br
:SF font=10.
..ce &*.
:eSF.
.in &*tmp.
.dm csect end

.gt SALSO add salso
.dm salso begin
.se see = 0
.se msee = 0
.ju off
:ZDT.See Also:
:ZDD.
.dm salso end

.gt SAL add sal att nocont
.ga * typ any
.ga * * value '' default
.ga * ocls any
.ga * * value '' default
.dm sal begin
.if '&msee.' eq '0' .do begin
.  .if '&*typ' ne 'fun' and '&*typ' ne 'cls' .do begin
.  .  .if '&*typ' ne 'ofun' and '&*typ' ne 'mnp' .do begin
.  .  .  .if '&*typ' ne 'omtyp' .do begin
.  .  .  .  .se *cl_tmp=&cl_name.::
.  .  .  .  .se msee=1
.  .  .  .  .if '&see.' eq '1' .do begin
.  .  .  .  .  .se see=0
.  .  .  .  .  .br
.  .  .  .  .do end
.  .  .  .do end
.  .  .do end
.  .do end
.do end
.if '&see.' eq '1' .do begin
.ct ,
.do end
.se see = 1
.*
.if '&*typ' eq 'fun' .do begin
:MONO.&*.
.do end
.el .if '&*typ' eq 'cls' .do begin
:MONO.&*.
.do end
.el .if '&*typ' eq 'ofun' .do begin
.'se ocl_name='&*ocls.'
:MONO.&ocl_name.::&*.
.do end
.el .if '&*typ' eq 'mnp' .do begin
manipulator
:MONO.&*.
.do end
.el .if '&*typ' eq 'mfun' .do begin
:MONO.&*cl_tmp.&*.
.do end
.el .if '&*typ' eq 'mdata' .do begin
:MONO.&*cl_tmp.&*.
.do end
.el .if '&*typ' eq 'mtyp' .do begin
:MONO.&*cl_tmp.&*.
.do end
.el .if '&*typ' eq 'omtyp' .do begin
.'se ocl_name='&*ocls.'
:MONO.&ocl_name.::&*.
.do end
.el .if '&*typ' eq 'ctor' .do begin
:MONO.&cl_name.
.do end
.el .if '&*typ' eq 'dtor' .do begin
:MONO.~~&cl_name.
.do end
.dm sal end

.gt eSALSO add esalso
.dm esalso begin
.ju &cpp_ju.
.dm esalso end
