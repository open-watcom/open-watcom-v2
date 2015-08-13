in wdkeys.dbg
display all /close
display status
display toolbar /fixed 0
display stack /close 8000,4992,2000,4992
display watch /open 4000,5408,4000,4576
display modules /open 8000,5408,2000,4576
display register /open 0,5408,4000,4576
display assembly /open 0,0,10000,5408
display source /open 0,0,10000,5408
display locals /open 0,5408,4000,4576
display file /close 0,0,10000,10000
set source {.}
if _dbg@dbg$kanji {
    font all {-16 0 0 0 400 0 0 0 128 3 2 1 17 "‚l‚r –¾’©"}
} else {
    font all {-13 0 0 0 400 0 0 0 0 1 2 1 49 "Courier"}
}
font memory {-12 0 0 0 400 0 0 0 0 1 2 1 49 "Fixedsys"}
set autosave off
set assembly lower source hexadecimal
set variable partial nocode noinherit nocompiler members private protected
set functions all
set globals all
set register hexadecimal normal
set fpu decimal
set bell on
set dclick 300
set implicit on
set radix 10
set recursion on
set search /caseignore/norx {^$\.[(|?+*~@}
set symbol /ignore {*}
set symbol /add /ignore {*_}
set symbol /add /ignore {_*}
set tab 8
set supportroutine {__wcpp_*}{IF@*}{RT@*}{__CHK}{__CHP}{__EDA}{__EDC}{__EDD}
set supportroutine {__EDM}{__EDS}{__EPI}{__Far16Cdecl2}{__Far16Func2}
set supportroutine {__Far16Pascal2}{__Far16ToFlat}{__FDA}{__FDC}{__FDD}{__FDFS}
set supportroutine {__FDI4}{__FDI8}{__FDM}{__FDN}{__FDS}{__FDU4}{__FDU8}
set supportroutine {__FlatToFar16}{__FSA}{__FSC}{__FSD}{__FSFD}{__FSI4}{__FSI8}
set supportroutine {__FSM}{__FSN}{__FSS}{__FSU4}{__FSU8}{__GETDS}{__GRO}{__I4D}
set supportroutine {__I4FD}{__I4FS}{__I4M}{__I8DQE}{__I8DQ}{__I8DRE}{__I8DR}
set supportroutine {__I8D}{__I8FD}{__I8FS}{__I8M}{__PIA}{__PIS}{__PRO}{__PTC}
set supportroutine {__PTS}{__RDFS}{__RDI4}{__RDU4}{__RSI4}{__RSU4}{__SCN1}{__SCN2}
set supportroutine {__SCN4}{__STK}{__TNC}{__TNK}{__U4D}{__U4FD}{__U4FS}{__U4M}
set supportroutine {__U8DQE}{__U8DQ}{__U8DRE}{__U8DR}{__U8D}{__U8FD}{__U8FS}{__U8M}
flip on
hook progstart {
    if _dbg@dbg$loaded && !?_dbg@nogo {
        if ?@@fmain {
            if !?@@fwinmain {
                go/until/noflip @@fmain
            } else {
                go/until @@fmain
            }
        }
        elseif ?@@main {
            if !?@@winmain {
                go/until/noflip @@main
            } else {
                go/until @@main
            }
        }
        elseif ?@@fwinmain {
            go/until/noflip @@fwinmain
        }
        elseif ?@@winmain {
            go/until/noflip @@winmain
        }
        elseif _dbg@dbg$os == 12 && ?@main {
            go/until/noflip @main
        }
    };
}
hook progend {
}
hook dllstart {
}
hook dllend {
}
hook execstart {
}
hook execend {
}
hook sourceinfo {
    display locals; display source
}
hook assemblyinfo {
    display register; display assembly
}
hook modulechange {
}
