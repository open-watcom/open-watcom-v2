/*
From: c++std=core
To: jww; steve; fb; gregor; jga; francis; clark_nelson; cpp-std; steve_dum
Subject: Derived to base conversions & copy ctors
Date: Wednesday, April 24, 1996 4:11PM

Return-Path: <c++std=core@research.att.com>
Received: from sandal.watcom.on.ca by msmail id <317EB704@msmail>; Wed, 24 Apr
    96 19:19:32 EDT
Received: from research.att.com by sandal.watcom.on.ca (8.6.12/1.38) id
    SAA24171; Wed, 24 Apr 1996 18:17:40 -0500
Received: from research.att.com by ns; Wed Apr 24 19:16:16 EDT 1996
To: steve@maths.warwick.ac.uk, fb@ddi.com, gregor@netcom.com, jga@unitech.com,
    francis@robinton.demon.co.uk, clark_nelson@ccm.jf.intel.com,
    cpp-std@tartan.com, jww@watcom.on.ca, steve_dum@mentorg.com
From: bensc@microsoft.com
Errors-To: c++std-postmaster@research.att.com
Reply-To: c++std-core@research.att.com
Precedence: list
Transport-Options: /ignore
Received: from research.att.com by cetus; Wed Apr 24 19:12:05 EDT 1996
Received: from tide19.microsoft.com by research; Wed Apr 24 19:11:54 EDT 1996
Received: by tide19.microsoft.com with Microsoft Exchange (IMC 4.0.838.5) id
    <01BB31F8.C2439500@tide19.microsoft.com>; Wed, 24 Apr 1996 16:11:56 -0700
Message-ID: <c=US%a=_%p=msft%l=RED-23-MSG-960424231153Z-7391@tide19.microsoft.
    com>
From: Ben Schreiber <bensc@microsoft.com>
Subject: Derived to base conversions & copy ctors
Date: Wed, 24 Apr 1996 16:11:53 -0700
X-Mailer:  Microsoft Exchange Server Internet Mail Connector Version 4.0.838.5
MIME-Version: 1.0
Content-Type: text/plain; charset="us-ascii"
Content-Transfer-Encoding: 7bit
------------------------------------------------------------------------------
To: C++ core language mailing list
Message c++std-core-6667

Is a derived-to-base conversion required to be implemented by a copy
constructor of the base class?  Or is it always the best constructor of
the base class that's used?

I.e., which constructor is called in the following example:

*/

#include "dump.h"

  class B;
  class D;

  class B {
   public:
      B( const B& ) {GOOD} ;    // #1 - a copy constructor
      B( const D& ) {GOOD};    // #2 - a different constructor
      B() {GOOD};              // **** added by jww
  };

  class D: public B {
  };

  class Q {
   public:
      operator D () {};
  };

  void func1(B){};

  void func2() {
      D d;
      Q q;

      B b( d );         // case 1: #1 or #2?
      B b2 = d;         // case 2: #1 or #2?
      B b3 = q;         // case 3: #1 or #2?

      func1( d );       // case 4: #1 or #2?

      func1( q );       // case 5: #1 or #2?
  }

/*
//=================================
        1 2 3 4 5
WATCOM  2 2 2 2 2
//=================================
Case 1 is direct initialization, so presumably all constructors are
considered, thus #2 is the one that is used.

The rest are not so clear.

For case 2, 8.5 [dcl.init] paragraph 12, 4th bullet, 2nd sub-bullet
would appear to apply, in which case both #1 & #2 are considered, so #2
is used.

Case 4 should be the same as case 2, but 13.3.3.1.2 [over.ics.user]
paragraph 4 says:
  "A conversion of an expression of class type to the same class type
or to  a  base  class of that type is a standard conversion rather than
a user-defined conversion in spite of the fact that a  copy  constructor
(i.e., a user-defined conversion function) is called."

This paragraph makes the assumption that the only way to perform such a
conversion is by copy constructor, but constructor #2 can also perform
this conversion.

Cases 3 and 5 run into a similar problem.  A user-defined conversion is
used, and then the result is initialized from the result of the UDC
operator.  The footnote to 8.5p12b4sb3 (as revised by 96-0021) states:
  "Because the type of the temporary is the same as the type of the
object being initialized, or a derived class thereof, this
direct-initialization, if well-formed, will use a copy constructor."
However, as case #1 shows, it is not the copy-constructor that gets used
for the direct initialization.


Options:

1)  Require that in all cases where a class is being initialized by a
derived class, the copy-constructors are the only ones considered, i.e.
in the example above, all cases would resolve to #1.

2)  In all places where a copy-constructor is called for, all
constructors of the target class are actually considered, ie change the
phrase "a copy-constructor is called" to "a constructor is called to
copy ...".  The one selected by overload resolution is the one used,
even if that use does not include calling it (eg. in cases of
elimination of temporaries).  In the above example, this would resolve
all cases to #2.  The special status of 'copy-constructor' then only
affects whether one is implicitly generated (and what its signature is).

3)  Try to fix on a case-by-case basis.  I'm not going to try to work
this out here.


I have a slight preference for option #2.  I would discourage option #3,
because it runs the risk of creating new inconsistancies.

        -- Ben Schreiber
           Microsoft Corp.
           bensc@microsoft.com
*/
