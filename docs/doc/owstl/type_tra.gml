.chap Type Traits
.*
.sect Introduction
.*
The header type_traits is based on the metaprogramming section of n1836 :Q.Draft
Technical Report on C++ Library Extensions:eQ.. It contains a set of templates
that allow compile type testing and modification of types.
.*
.section Status
.np
:AUTHOR date='18 Feb 2006'.D J F Cletheroe
.np
Reviewer: Not reviewed
.np
About half of the required functionality has been implemented so far. There are
currently a few compiler bugs stopping some parts being implemented.
.np
The missing templates are:
:UL.
:LI.is_member_object_pointer
:LI.is_member_function_pointer
:LI.is_enum
:LI.is_union
:LI.is_class
:LI.is_function
:LI.struct is_object
:LI.is_scalar
:LI.is_compound
:LI.is_member_pointer
:LI.is_pod
:LI.is_empty
:LI.is_polymorphic
:LI.is_abstract
:LI.has_trivial_constructor
:LI.has_trivial_copy
:LI.has_trivial_assign
:LI.has_trivial_destructor
:LI.has_nothrow_constructor
:LI.has_nothrow_copy
:LI.has_nothrow_assign
:LI.has_virtual_destructor
:LI.is_signed
:LI.is_unsigned
:LI.alignment_of
:LI.rank
:LI.extent
:LI.is_same
:LI.is_base_of
:LI.is_convertible
:LI.remove_extent
:LI.remove_all_extents
:LI.add_pointer
:LI.aligned_storage
:eUL.
.*
.section Design Details
.*
.sect Querying types
.*
This is implemented by specializing templates for the types that the test holds
true. The class derives from a helper class that contains a static const value.
The important cases are when this static const is a bool and is true or false.
The user can then access is_void<type>::value to see if the test is positive.
A set of macros are used to help make the definitions look a bit less cluttered.
There is a default case which declares the main template and is usually false.
There are then other macros that define the specializations. There are also
macros that define 4 specializations for the const volatile qualified variations
of the type.
.*
.sect Modifying types
.*
This works in a similar way. The template is specialized for the type with the
modifier and the class contains a typedef :Q.type:eQ. that refers to the
modified type. Macros aren't used for the modifiers as they tend to have subtle
differences for each template and in many cases there isn't the need for 4
different CV variations.
.*
.sect Use in main library
.*
This header should be a help for writing the constructors and member
functions of the standard containers that are required to have different
behavior for iterators and integral types.
