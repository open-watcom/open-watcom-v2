:P.
.ix 'functions and types'
Each of the classes and functions in the Class Library is described in
this chapter.  Each description consists of a number of subsections:
.in 0
:DL termhi=2 break.
:DT.Declared:
:DD.This optional subsection specifies which header file contains the
declaration for a class.
It is only found in sections describing class declarations.
:DT.Derived From:
:DD.This optional subsection shows the inheritance for a class.
It is only found in sections describing class declarations.
:DT.Derived By:
:DD.This optional subsection shows which classes inherit from this class.
It is only found in sections describing class declarations.
:DT.Synopsis:
:DD.This subsection gives the name of the header file that contains the
declaration of the function.  This header file must be included in order to
reference the function.
:P.
For class member functions, the protection associated with the function is
indicated via the presence of one of the
:MONO.private
:CONT.,
:MONO.protected
:CONT.,
or
:MONO.public
keywords.
:P.
The full function prototype is specified.
Virtual class member functions are indicated via the presence of the
:MONO.virtual
keyword in the function prototype.
:DT.Semantics:
:DD.This subsection is a description of the function.
:DT.Derived Implementation Protocol:
:DD.This optional subsection is present for virtual member functions.
It describes how derived implementations of the virtual member function
should behave.
:DT.Default Implementation:
:DD.This optional subsection is present for virtual member functions.
It describes how the default implementation provided with the base class
definition behaves.
:DT.Results:
:DD.This optional subsection describes the function's return value, if any, and the
impact of a member function on its object's state.
:DT.See Also:
:DD.This optional subsection provides a list of related functions or classes.
:eDL.
