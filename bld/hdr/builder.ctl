# HDR Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
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
    <CPCMD> dos/*.h*           <RELROOT>/h/
    <CPCMD> dos/*.cnv          <RELROOT>/h/
    <CPCMD> dos/sys/*.h        <RELROOT>/h/sys/
    <CPCMD> dos/algorith       <RELROOT>/h/
    <CPCMD> dos/bitset         <RELROOT>/h/
    <CPCMD> dos/cassert        <RELROOT>/h/
    <CPCMD> dos/cctype         <RELROOT>/h/
    <CPCMD> dos/cerrno         <RELROOT>/h/
    <CPCMD> dos/cfloat         <RELROOT>/h/
    <CPCMD> dos/ciso646        <RELROOT>/h/
    <CPCMD> dos/climits        <RELROOT>/h/
    <CPCMD> dos/clocale        <RELROOT>/h/
    <CPCMD> dos/cmath          <RELROOT>/h/
    <CPCMD> dos/complex        <RELROOT>/h/
    <CPCMD> dos/csetjmp        <RELROOT>/h/
    <CPCMD> dos/csignal        <RELROOT>/h/
    <CPCMD> dos/cstdarg        <RELROOT>/h/
    <CPCMD> dos/cstddef        <RELROOT>/h/
    <CPCMD> dos/cstdint        <RELROOT>/h/
    <CPCMD> dos/cstdio         <RELROOT>/h/
    <CPCMD> dos/cstdlib        <RELROOT>/h/
    <CPCMD> dos/cstring        <RELROOT>/h/
    <CPCMD> dos/ctime          <RELROOT>/h/
    <CPCMD> dos/cwchar         <RELROOT>/h/
    <CPCMD> dos/cwctype        <RELROOT>/h/
    <CPCMD> dos/deque          <RELROOT>/h/
    <CPCMD> dos/exceptio       <RELROOT>/h/
    <CPCMD> dos/fstream        <RELROOT>/h/
    <CPCMD> dos/function       <RELROOT>/h/
    <CPCMD> dos/iomanip        <RELROOT>/h/
    <CPCMD> dos/ios            <RELROOT>/h/
    <CPCMD> dos/iosfwd         <RELROOT>/h/
    <CPCMD> dos/iostream       <RELROOT>/h/
    <CPCMD> dos/istream        <RELROOT>/h/
    <CPCMD> dos/iterator       <RELROOT>/h/
    <CPCMD> dos/limits         <RELROOT>/h/
    <CPCMD> dos/list           <RELROOT>/h/
    <CPCMD> dos/locale         <RELROOT>/h/
    <CPCMD> dos/map            <RELROOT>/h/
    <CPCMD> dos/memory         <RELROOT>/h/
    <CPCMD> dos/new            <RELROOT>/h/
    <CPCMD> dos/numeric        <RELROOT>/h/
    <CPCMD> dos/ostream        <RELROOT>/h/
    <CPCMD> dos/queue          <RELROOT>/h/
    <CPCMD> dos/random         <RELROOT>/h/
    <CPCMD> dos/set            <RELROOT>/h/
    <CPCMD> dos/slist          <RELROOT>/h/
    <CPCMD> dos/sstream        <RELROOT>/h/
    <CPCMD> dos/stack          <RELROOT>/h/
    <CPCMD> dos/stdexcep       <RELROOT>/h/
    <CPCMD> dos/streambu       <RELROOT>/h/
    <CPCMD> dos/string         <RELROOT>/h/
    <CPCMD> dos/strstrea       <RELROOT>/h/
    <CPCMD> dos/typeinfo       <RELROOT>/h/
    <CPCMD> dos/type_tra       <RELROOT>/h/
    <CPCMD> dos/unordere       <RELROOT>/h/
    <CPCMD> dos/utility        <RELROOT>/h/
    <CPCMD> dos/valarray       <RELROOT>/h/
    <CPCMD> dos/vector         <RELROOT>/h/

    <CPCMD> linux/*.h*         <RELROOT>/lh/
    <CPCMD> linux/arpa/*.h     <RELROOT>/lh/arpa/
    <CPCMD> linux/sys/*.h      <RELROOT>/lh/sys/
    <CPCMD> linux/netinet/*.h  <RELROOT>/lh/netinet/
    <CPCMD> linux/algorith     <RELROOT>/lh/
    <CPCMD> linux/bitset       <RELROOT>/lh/
    <CPCMD> linux/cassert      <RELROOT>/lh/
    <CPCMD> linux/cctype       <RELROOT>/lh/
    <CPCMD> linux/cerrno       <RELROOT>/lh/
    <CPCMD> linux/cfloat       <RELROOT>/lh/
    <CPCMD> linux/ciso646      <RELROOT>/lh/
    <CPCMD> linux/climits      <RELROOT>/lh/
    <CPCMD> linux/clocale      <RELROOT>/lh/
    <CPCMD> linux/cmath        <RELROOT>/lh/
    <CPCMD> linux/complex      <RELROOT>/lh/
    <CPCMD> linux/csetjmp      <RELROOT>/lh/
    <CPCMD> linux/csignal      <RELROOT>/lh/
    <CPCMD> linux/cstdarg      <RELROOT>/lh/
    <CPCMD> linux/cstddef      <RELROOT>/lh/
    <CPCMD> linux/cstdint      <RELROOT>/lh/
    <CPCMD> linux/cstdio       <RELROOT>/lh/
    <CPCMD> linux/cstdlib      <RELROOT>/lh/
    <CPCMD> linux/cstring      <RELROOT>/lh/
    <CPCMD> linux/ctime        <RELROOT>/lh/
    <CPCMD> linux/cwchar       <RELROOT>/lh/
    <CPCMD> linux/cwctype      <RELROOT>/lh/
    <CPCMD> linux/deque        <RELROOT>/lh/
    <CPCMD> linux/exceptio     <RELROOT>/lh/
    <CPCMD> linux/fstream      <RELROOT>/lh/
    <CPCMD> linux/function     <RELROOT>/lh/
    <CPCMD> linux/iomanip      <RELROOT>/lh/
    <CPCMD> linux/ios          <RELROOT>/lh/
    <CPCMD> linux/iosfwd       <RELROOT>/lh/
    <CPCMD> linux/iostream     <RELROOT>/lh/
    <CPCMD> linux/istream      <RELROOT>/lh/
    <CPCMD> linux/iterator     <RELROOT>/lh/
    <CPCMD> linux/limits       <RELROOT>/lh/
    <CPCMD> linux/list         <RELROOT>/lh/
    <CPCMD> linux/locale       <RELROOT>/lh/
    <CPCMD> linux/map          <RELROOT>/lh/
    <CPCMD> linux/memory       <RELROOT>/lh/
    <CPCMD> linux/new          <RELROOT>/lh/
    <CPCMD> linux/numeric      <RELROOT>/lh/
    <CPCMD> linux/ostream      <RELROOT>/lh/
    <CPCMD> linux/queue        <RELROOT>/lh/
    <CPCMD> linux/random       <RELROOT>/lh/
    <CPCMD> linux/set          <RELROOT>/lh/
    <CPCMD> linux/slist        <RELROOT>/lh/
    <CPCMD> linux/sstream      <RELROOT>/lh/
    <CPCMD> linux/stack        <RELROOT>/lh/
    <CPCMD> linux/stdexcep     <RELROOT>/lh/
    <CPCMD> linux/streambu     <RELROOT>/lh/
    <CPCMD> linux/string       <RELROOT>/lh/
    <CPCMD> linux/strstrea     <RELROOT>/lh/
    <CPCMD> linux/typeinfo     <RELROOT>/lh/
    <CPCMD> linux/type_tra     <RELROOT>/lh/
    <CPCMD> linux/unordere     <RELROOT>/lh/
    <CPCMD> linux/utility      <RELROOT>/lh/
    <CPCMD> linux/valarray     <RELROOT>/lh/
    <CPCMD> linux/vector       <RELROOT>/lh/

    <CPCMD> linux/arch/i386/*.h       <RELROOT>/lh/arch/i386/
    <CPCMD> linux/arch/i386/sys/*.h   <RELROOT>/lh/arch/i386/sys/
    <CPCMD> linux/arch/mips/*.h       <RELROOT>/lh/arch/mips/
    <CPCMD> linux/arch/mips/sys/*.h   <RELROOT>/lh/arch/mips/sys/

[ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx/*.h*           <RELROOT>/qh/
    <CPCMD> qnx/sys/*.h        <RELROOT>/qh/sys/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
