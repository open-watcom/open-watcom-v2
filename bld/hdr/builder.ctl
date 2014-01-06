# HDR Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=Standard header files

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> dos/*.h*           <OWRELROOT>/h/
    <CPCMD> dos/*.cnv          <OWRELROOT>/h/
    <CPCMD> dos/sys/*          <OWRELROOT>/h/sys/
    <CPCMD> dos/algorith       <OWRELROOT>/h/
    <CPCMD> dos/bitset         <OWRELROOT>/h/
    <CPCMD> dos/cassert        <OWRELROOT>/h/
    <CPCMD> dos/cctype         <OWRELROOT>/h/
    <CPCMD> dos/cerrno         <OWRELROOT>/h/
    <CPCMD> dos/cfloat         <OWRELROOT>/h/
    <CPCMD> dos/cinttype       <OWRELROOT>/h/
    <CPCMD> dos/ciso646        <OWRELROOT>/h/
    <CPCMD> dos/climits        <OWRELROOT>/h/
    <CPCMD> dos/clocale        <OWRELROOT>/h/
    <CPCMD> dos/cmath          <OWRELROOT>/h/
    <CPCMD> dos/complex        <OWRELROOT>/h/
    <CPCMD> dos/csetjmp        <OWRELROOT>/h/
    <CPCMD> dos/csignal        <OWRELROOT>/h/
    <CPCMD> dos/cstdarg        <OWRELROOT>/h/
    <CPCMD> dos/cstdbool       <OWRELROOT>/h/
    <CPCMD> dos/cstddef        <OWRELROOT>/h/
    <CPCMD> dos/cstdint        <OWRELROOT>/h/
    <CPCMD> dos/cstdio         <OWRELROOT>/h/
    <CPCMD> dos/cstdlib        <OWRELROOT>/h/
    <CPCMD> dos/cstring        <OWRELROOT>/h/
    <CPCMD> dos/ctime          <OWRELROOT>/h/
    <CPCMD> dos/cwchar         <OWRELROOT>/h/
    <CPCMD> dos/cwctype        <OWRELROOT>/h/
    <CPCMD> dos/deque          <OWRELROOT>/h/
    <CPCMD> dos/exceptio       <OWRELROOT>/h/
    <CPCMD> dos/fstream        <OWRELROOT>/h/
    <CPCMD> dos/function       <OWRELROOT>/h/
    <CPCMD> dos/iomanip        <OWRELROOT>/h/
    <CPCMD> dos/ios            <OWRELROOT>/h/
    <CPCMD> dos/iosfwd         <OWRELROOT>/h/
    <CPCMD> dos/iostream       <OWRELROOT>/h/
    <CPCMD> dos/istream        <OWRELROOT>/h/
    <CPCMD> dos/iterator       <OWRELROOT>/h/
    <CPCMD> dos/limits         <OWRELROOT>/h/
    <CPCMD> dos/list           <OWRELROOT>/h/
    <CPCMD> dos/locale         <OWRELROOT>/h/
    <CPCMD> dos/map            <OWRELROOT>/h/
    <CPCMD> dos/memory         <OWRELROOT>/h/
    <CPCMD> dos/new            <OWRELROOT>/h/
    <CPCMD> dos/numeric        <OWRELROOT>/h/
    <CPCMD> dos/ostream        <OWRELROOT>/h/
    <CPCMD> dos/queue          <OWRELROOT>/h/
    <CPCMD> dos/random         <OWRELROOT>/h/
    <CPCMD> dos/set            <OWRELROOT>/h/
    <CPCMD> dos/slist          <OWRELROOT>/h/
    <CPCMD> dos/sstream        <OWRELROOT>/h/
    <CPCMD> dos/stack          <OWRELROOT>/h/
    <CPCMD> dos/stdexcep       <OWRELROOT>/h/
    <CPCMD> dos/streambu       <OWRELROOT>/h/
    <CPCMD> dos/string         <OWRELROOT>/h/
    <CPCMD> dos/strstrea       <OWRELROOT>/h/
    <CPCMD> dos/typeinfo       <OWRELROOT>/h/
    <CPCMD> dos/type_tra       <OWRELROOT>/h/
    <CPCMD> dos/unordere       <OWRELROOT>/h/
    <CPCMD> dos/utility        <OWRELROOT>/h/
    <CPCMD> dos/valarray       <OWRELROOT>/h/
    <CPCMD> dos/vector         <OWRELROOT>/h/

    <CPCMD> linux/*.h*         <OWRELROOT>/lh/
    <CPCMD> linux/arpa/*       <OWRELROOT>/lh/arpa/
    <CPCMD> linux/sys/*        <OWRELROOT>/lh/sys/
    <CPCMD> linux/netinet/*    <OWRELROOT>/lh/netinet/
    <CPCMD> linux/algorith     <OWRELROOT>/lh/
    <CPCMD> linux/bitset       <OWRELROOT>/lh/
    <CPCMD> linux/cassert      <OWRELROOT>/lh/
    <CPCMD> linux/cctype       <OWRELROOT>/lh/
    <CPCMD> linux/cerrno       <OWRELROOT>/lh/
    <CPCMD> linux/cfloat       <OWRELROOT>/lh/
    <CPCMD> linux/cinttype     <OWRELROOT>/lh/
    <CPCMD> linux/ciso646      <OWRELROOT>/lh/
    <CPCMD> linux/climits      <OWRELROOT>/lh/
    <CPCMD> linux/clocale      <OWRELROOT>/lh/
    <CPCMD> linux/cmath        <OWRELROOT>/lh/
    <CPCMD> linux/complex      <OWRELROOT>/lh/
    <CPCMD> linux/csetjmp      <OWRELROOT>/lh/
    <CPCMD> linux/csignal      <OWRELROOT>/lh/
    <CPCMD> linux/cstdarg      <OWRELROOT>/lh/
    <CPCMD> linux/cstdbool     <OWRELROOT>/lh/
    <CPCMD> linux/cstddef      <OWRELROOT>/lh/
    <CPCMD> linux/cstdint      <OWRELROOT>/lh/
    <CPCMD> linux/cstdio       <OWRELROOT>/lh/
    <CPCMD> linux/cstdlib      <OWRELROOT>/lh/
    <CPCMD> linux/cstring      <OWRELROOT>/lh/
    <CPCMD> linux/ctime        <OWRELROOT>/lh/
    <CPCMD> linux/cwchar       <OWRELROOT>/lh/
    <CPCMD> linux/cwctype      <OWRELROOT>/lh/
    <CPCMD> linux/deque        <OWRELROOT>/lh/
    <CPCMD> linux/exceptio     <OWRELROOT>/lh/
    <CPCMD> linux/fstream      <OWRELROOT>/lh/
    <CPCMD> linux/function     <OWRELROOT>/lh/
    <CPCMD> linux/iomanip      <OWRELROOT>/lh/
    <CPCMD> linux/ios          <OWRELROOT>/lh/
    <CPCMD> linux/iosfwd       <OWRELROOT>/lh/
    <CPCMD> linux/iostream     <OWRELROOT>/lh/
    <CPCMD> linux/istream      <OWRELROOT>/lh/
    <CPCMD> linux/iterator     <OWRELROOT>/lh/
    <CPCMD> linux/limits       <OWRELROOT>/lh/
    <CPCMD> linux/list         <OWRELROOT>/lh/
    <CPCMD> linux/locale       <OWRELROOT>/lh/
    <CPCMD> linux/map          <OWRELROOT>/lh/
    <CPCMD> linux/memory       <OWRELROOT>/lh/
    <CPCMD> linux/new          <OWRELROOT>/lh/
    <CPCMD> linux/numeric      <OWRELROOT>/lh/
    <CPCMD> linux/ostream      <OWRELROOT>/lh/
    <CPCMD> linux/queue        <OWRELROOT>/lh/
    <CPCMD> linux/random       <OWRELROOT>/lh/
    <CPCMD> linux/set          <OWRELROOT>/lh/
    <CPCMD> linux/slist        <OWRELROOT>/lh/
    <CPCMD> linux/sstream      <OWRELROOT>/lh/
    <CPCMD> linux/stack        <OWRELROOT>/lh/
    <CPCMD> linux/stdexcep     <OWRELROOT>/lh/
    <CPCMD> linux/streambu     <OWRELROOT>/lh/
    <CPCMD> linux/string       <OWRELROOT>/lh/
    <CPCMD> linux/strstrea     <OWRELROOT>/lh/
    <CPCMD> linux/typeinfo     <OWRELROOT>/lh/
    <CPCMD> linux/type_tra     <OWRELROOT>/lh/
    <CPCMD> linux/unordere     <OWRELROOT>/lh/
    <CPCMD> linux/utility      <OWRELROOT>/lh/
    <CPCMD> linux/valarray     <OWRELROOT>/lh/
    <CPCMD> linux/vector       <OWRELROOT>/lh/

    <CPCMD> linux/arch/i386/*       <OWRELROOT>/lh/arch/i386/
    <CPCMD> linux/arch/i386/sys/*   <OWRELROOT>/lh/arch/i386/sys/
    <CPCMD> linux/arch/mips/*       <OWRELROOT>/lh/arch/mips/
    <CPCMD> linux/arch/mips/sys/*   <OWRELROOT>/lh/arch/mips/sys/

    <CCCMD> qnx/*.h*           <OWRELROOT>/qh/
    <CCCMD> qnx/sys/*          <OWRELROOT>/qh/sys/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
