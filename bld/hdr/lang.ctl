# HDR Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> <PROJDIR>/dos/*.h*           <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/*.cnv          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/sys/*.h        <RELROOT>/h/sys/
    <CPCMD> <PROJDIR>/dos/algorith       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/bitset         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cassert        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cctype         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cerrno         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cfloat         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/ciso646        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/climits        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/clocale        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cmath          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/complex        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/csetjmp        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/csignal        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstdarg        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstddef        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstdint        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstdio         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstdlib        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cstring        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/ctime          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cwchar         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/cwctype        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/deque          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/exceptio       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/fstream        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/function       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/iomanip        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/ios            <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/iosfwd         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/iostream       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/istream        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/iterator       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/limits         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/list           <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/locale         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/map            <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/memory         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/new            <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/numeric        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/ostream        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/queue          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/random         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/set            <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/slist          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/sstream        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/stack          <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/stdexcep       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/streambu       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/string         <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/strstrea       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/typeinfo       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/type_tra       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/unordere       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/utility        <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/valarray       <RELROOT>/h/
    <CPCMD> <PROJDIR>/dos/vector         <RELROOT>/h/

    <CPCMD> <PROJDIR>/linux/*.h*         <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/arpa/*.h     <RELROOT>/lh/arpa/
    <CPCMD> <PROJDIR>/linux/sys/*.h      <RELROOT>/lh/sys/
    <CPCMD> <PROJDIR>/linux/netinet/*.h  <RELROOT>/lh/netinet/
    <CPCMD> <PROJDIR>/linux/algorith     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/bitset       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cassert      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cctype       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cerrno       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cfloat       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/ciso646      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/climits      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/clocale      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cmath        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/complex      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/csetjmp      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/csignal      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstdarg      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstddef      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstdint      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstdio       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstdlib      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cstring      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/ctime        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cwchar       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/cwctype      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/deque        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/exceptio     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/fstream      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/function     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/iomanip      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/ios          <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/iosfwd       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/iostream     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/istream      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/iterator     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/limits       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/list         <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/locale       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/map          <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/memory       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/new          <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/numeric      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/ostream      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/queue        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/random       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/set          <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/slist        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/sstream      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/stack        <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/stdexcep     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/streambu     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/string       <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/strstrea     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/typeinfo     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/type_tra     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/unordere     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/utility      <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/valarray     <RELROOT>/lh/
    <CPCMD> <PROJDIR>/linux/vector       <RELROOT>/lh/

    <CPCMD> <PROJDIR>/linux/arch/i386/*.h       <RELROOT>/lh/arch/i386/
    <CPCMD> <PROJDIR>/linux/arch/i386/sys/*.h   <RELROOT>/lh/arch/i386/sys/
    <CPCMD> <PROJDIR>/linux/arch/mips/*.h       <RELROOT>/lh/arch/mips/
    <CPCMD> <PROJDIR>/linux/arch/mips/sys/*.h   <RELROOT>/lh/arch/mips/sys/

[ IFDEF (os_qnx) <2*> ]
    <CPCMD> <PROJDIR>/qnx/*.h*           <RELROOT>/qh/
    <CPCMD> <PROJDIR>/qnx/sys/*.h        <RELROOT>/qh/sys/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
