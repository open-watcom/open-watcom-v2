:H1.Introduction
:H2.Overview
:P.
The Open Watcom Standard Template Library (OWSTL) is an implimentation of 
the C++ standard library defined in ISO/IEC 14882.
This document describes the design of the various parts of OWSTL.
Each section will describe an element of the library and will typically include an
overview of the design, design desisions made and reasoning behind them, and
problems encountered and explanations of the solution.
It is hoped a peer review of the code and design documentation will be
undertaken at some stage and questions raised, answers, or resulting changes
made will be documented here.

:H2.Philosophy
:P.
OWSTL is written entirely from scratch.
It does not, for example, assimilate an old HP/SGI code base.
When a new element is added to the library the topic should be researched
by the author before they commence coding so they can make use of any new
research and theories.
The commercial compiler &product is based on made its name for producing high
quality fast code.
The intention is to produce a high performance library to complement that.
This means choosing and experimenting with the best algorithms possible.
It does not mean the source code should be messy.
It is important the code is easy to read and understand as that will encorage
new developers to maintain and improve the library and will give the greatest
advantage in the long term.
This document is part of that, giving an overview and logging historical 
improvements and pitfalls discovered, the source files are the other half.
If improvements can be made by crafting complicated, difficult to understand
constructs in the source it probably suggests more effort should be spent
on the compiler and codegenerator.

:H2.Status
:P.
OWSTL is currently in very early stages of developement.
No parts are completely implemented, and many elements of the library
have not been started yet.
Having said that what does exsist will tend to be the most useful key parts.
The code is mainly templates and currently resides in under the 
:PATH.hdr
project.
In the future non-template classes or functions may be factored out of the 
templated code and be built into the static and dynamic libraries.
The exisiting library code is in :PATH.bld\plusplus\cpplib.
For example it should be possible to seperate the rebalancing algorithms
from the red-black tree code as these just manipulate pointers - they
don't really need to know the contained type.
Reasonably thorough regression tests can be found in :PATH.plustest/regress/owstl. 
These should be updated in parallel with new fuctionallity or fixes made to the 
library itself.
Some Benchmarks can be found in 
:PATH.bench/owstl.

:H1.Implementors Notes
:P.
When updating OWSTL remember to do all the following steps:
:UL.
:LI.check out latest source
:LI.run regresion tests, if broken fix or complain on contributors.news.openwatcom.org
:LI.update source
:LI.update regresion tests
:LI.update this document
:LI.update user documents (when/if they exist)
:LI.check regresion tests one last time
:LI.submit changelist
:eUL.
