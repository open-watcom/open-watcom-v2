.chap WATCOM Forms Editor Application Interface
.*
.section Why Use the Forms Editor?
.*
.np
The Forms Editor can be used to edit a wide variety of objects in a Windows
environment.  The Forms Editor receives Windows mouse events and translates
them into calls to an application through the routines defined below.  The
Forms Editor notifies the application when the user wishes to create,
delete, move, or resize an object.  The Forms Editor will inform an
application object when it needs to draw itself or define itself.  The Forms
Editor takes care of such functions as scrolling and resizing of the edit
window and provides for banded select, multiple moves, relative alignment,
cut, copy, paste and delete.  The Forms Editor also allows the user to
select a current object or current objects, so that any operation that the
application implements can take advantage of the ability to ask the Forms
Editor what the current objects are.
.*
.section How To Use The Forms Editor
.*
.np
The forms Forms Editor is a DLL and library which defines a number of
interface routines.  There are five types of interface functions: high
level interface functions, painting interface functions, action interface
functions, information interface functions and application input functions.
To use the Forms Editor, follow these steps :
.np
1) Create the Forms Editor window.  It is the application's responsibility
to create the Forms Editor window before any of the Forms Editor routines
are called.  When creating the Forms Editor window, the application should
declare it to have the style CS_DBLCLKS in the InitApplication routine.
This will allows the Forms Editor to received double click messages, which
it then translates into a call the Define routine for the object being
double clicked.  Also in this routine, the application must set csWndExtra
to be sizeof(HANDLE).  In the InitInstance routine, the window must be
given attributes WS_HSCROLL and WS_VSCROLL if the application would like
the Forms Editor to take care of scrolling.
.np
2)  Call OpenFormEdit, passing it the HWND of the window just created, a
CREATE_TABLE and menu and scrolling configuration information.  The create
table is a table of addresses that the Forms Editor will call to create
application objects. Each element in this table must correspond to an OBJ_ID.
The first create routine in the create table must be the main object which
will contain all other objects.  One of the tasks of the OpenFormEdit routine
is to create an object of this type and set this object to be the main object.
.np
The Forms Editor expects to edit objects which know how to respond to certain
messages.  These objects are of type OBJPTR and are expected to have as their
first element a pointer to their dispatch routine, which must be a FARPROC.
That is, your must call MakeProcInstance with the address of the dispatch
routine and make the value returned from the call to MakeProcInstance the
first element in the object.  The Forms Editor will communicate with the
application by calling these objects (using this dispatch routine) and
passing it, in this order, an ACTION, an OBJPTR (pointer to the object being
called) and up to two additional parameters to be used by the implied
function.
.np
The ACTION_ID type is defined as follows :
.np
.millust begin
typedef enum {
    REGISTER,
    MOVE,
    RESIZE,
    LOCATE,
    FIND_SUBOBJECTS,
    DESTROY,
    DRAW,
    DEFINE,
    NOTIFY,
    FORWARD,
    VALIDATE_ACTION,
    ADD_SUBOBJECT,
    REMOVE_SUBOBJECT,
    RESIZE_INFO,
    CUT,
    COPY,
    PASTE,
    GET_PARENT,
    GET_PRIORITY
} ACTION_ID;
.millust end
.np
These actions correspond to the action interface functions described below.
Each of these routines returns a bool type value, and this is expected to be
returned as the return code from the call to the dispatch routine.
.np
The objects themselves can also use these interface routines to talk to each
other and can define their own actions and interface functions for this
purpose.  Any such actions must have a value no less than the defined
constant APP_ACTIONS.
.*
.section Inheritance
.*
.np
The Forms Editor is designed to edit self-contained objects that know all
about themselves but not much about the outside world. The most they know
is that they have a parent and can use its handle to communicate with it
and that, in some cases, they are also the parent of other objects and can
use the handle of those objects to communicate with them.  The Forms Editor
facilitates communication between objects by the provided routines and
allows the application to add to this list of actions and routines, as it
requires.  This setup allows the Forms Editor to exploit the use of
inheritance.  The idea of inheritance is that if you know a certain type
of object can handle certain actions, you can inherit that object and use
it for that functionality.  When you inherit it, it becomes part of you,
but you do not have to understand how it works. To encourage the use of
inheritance, the Forms Editor has provided an object which takes care of
some of the simplest actions.
.*
.section The Forms Editor Provides a Basic Object
.*
.np
The Forms Editor provides the basic object called 'object item'.  This
object can be created by calling Create with the OBJ_ID of OBJ_ITEM (a
defined constant).  The object item will handle the following actions :
REGISTER, MOVE, LOCATE, RESIZE, NOTIFY, DESTROY, VALIDATE_ACTION, CUT,
COPY, PASTE, GET_PARENT and GET_PRIORITY.  The Forms Editor provides this
object in the hopes that it will be useful to applications who want to
avoid implementing this basic functionality themselves.  They can create
an object of this type as an 'inherited' object of one of their objects,
and then forward the above messages to it.  In this case, the application
would probably want to create the object item with the third parameter to
Create being the address of the object who inherits the object item, so
that the object item will communicate with the world as if it were the
object that is inheriting it.
.*
.section High Level Interface
.*
.np
These functions are called by the application to start, restart and end an
editing session.
.begnote
.note extern void   FMEDITAPI OpenFormEdit( HWND, CREATE_TABLE, int, SCR_CONFIG );
.np
This function is to be called only once for each instance of the Forms
Editor, after the Forms Editor window is created.  This allows the Forms
Editor to do any required initialization.
.np
The HWND is the handle of the Forms Editor window.  The create routine must
be a Windows callback routine, and the address in the create table must
be the address returned from calling MakeProcInstance with the address of
the create routine.  The first create routine in the create table must be
the main object which will contain all other objects.  One of the tasks of
the OpenFormEdit routine is to create an object of this type and set this
object to be the main object.
.np
The CREATE_TABLE is an array of pointers to functions (create routines),
each of which returns an OBJPTR.  These create routines will be called
when Create is called with the corresponding OBJ_ID.
.np
The third parameter is menu information.  The integer passed should be an
ORed value using the following defined constants.  These constants indicate
which menus the Forms Editor should create for the application :
.np
.millust begin
Constant        Menu Name   Item Name   IDM Expected

MENU_NONE       -           -           -
MENU_SETUP      Setup       Grid        IDM_GRID
MENU_DELETE     Edit        Delete      IDM_DELETEOBJECT
MENU_CUT        Edit        Cut         IDM_CUTOBJECT
MENU_PASTE      Edit        Paste       IDM_PASTEOBJECT
MENU_COPY       Edit        Copy        IDM_COPYOBJECT
MENU_ALIGN      Edit        Align       IDM_FMLEFT,
                                        IDM_FMHCENTRE,
                                        IDM_FMRIGHT,
                                        IDM_FMTOP,
                                        IDM_FMVCENTRE,
                                        IDM_FMBOTTOM
.millust end
.np
If the application decides that the Forms Editor should not create one of
the above menus, then the application will have to pass the indicated IDM
at the appropriate time if the corresponding functionality is desired.  The
Align menu has a popup submenu, which will generate the six indicated IDMs
to initiate alignment of the current objects relative to the primary object.
.np
The fourth parameter indicates what type of scrolling the application would
like the editor to provide.  The following enumeration is defined :
.np
.millust begin
typedef enum {
    SCROLL_NONE,
    SCROLL_HORZ,
    SCROLL_VERT,
    SCROLL_BOTH
} SCR_CONFIG.
.millust end
.np
.note extern void   FMEDITAPI CloseFormEdit( HWND );
.np
This function is called only once, when the Forms Editor is to be closed.
HWND is the handle of the Forms Editor window to be closed.  This function
must be called while that handle is still valid.
.np
.note extern BOOL   CALLBACK FMEditWndProc( HWND, UINT, WPARAM, LPARAM );
.np
This routine is to be called by the application with any windows messages
that the application does not understand.  The Forms Editor will call
DefWndProc with these messages if it does not recognize them either.  All
menu actions with IDMs of greater or equal to IDM_FMEDIT are expected to
be passed to the Forms Editor via FMEditWndProc.
.np
.note extern void   FMEDITAPI ResetFormEdit( HWND );
.np
This routine is to be called by the application whenever a new Forms Editor
session is desired.  Information about the previous editing session will be
lost.  HWND is the handle of the Forms Editor window.
.np
.note extern int   FMEDITAPI FMTranslateAccelerator( HWND, LPMSG );
.np
This routine should be called by the application in its WinMain procedure
before translating or dispatching each message.  This function returns a
nonzero value to indicate that a translation has occurred and that the
message should not be passed to TranslateMessage and then DispatchMessage,
or a zero value to indicate that translation has not occurred and the
message should be passed to TranslateMessage and then DispatchMessage.
The forms editor uses accelerators for cut, copy, paste and delete.
It also uses the escape key to get out of a paste state.  That is, if paste
is initiated by the user from the menus, but the user then decides not to
paste, escape can be used to return to a normal state.  If none of these
accelerators are required, then the application does not need to call
FMTranslateAccelerator.  If the application wishes to define its own
accelerator for or instead of escape, it should pass an IDM_ESCAPE to the
Forms Editor at the appropriate time.
.endnote
.*
.section Painting Interface Functions
.*
.np
These functions are provided for convenience so that the application can
use them in response to a Draw function call or to indicate that a region
needs to be redrawn.  If an HDC must be passed into one of these functions,
it must be the same HDC that is passed to the application in the Draw
message that is being responded to.  The rectangles passed to these
functions must be in device coordinates (screen pixels).
.begnote
.note extern void   FMEDITAPI MarkInvalid( LPRECT );
.np
The application can call this function to tell the Forms Editor that the
indicated rectangle needs to be redrawn.  For example, this might happen
if the attributes of the object has changed, thus changing the way it
will be drawn.
.np
.note extern void   FMEDITAPI DarkGreyRect( LPRECT, LPSTR, HDC );
.np
Fill in the indicated rectangle with a dark grey brush.
.np
.note extern void   FMEDITAPI OutlineDoubleRect( LPRECT, HDC );
.np
Outline the indicated rectangle with a double black line.
.np
.note extern void   FMEDITAPI OutlineSingleRect( LPRECT, HDC );
.np
Outline the indicated rectangle with a singe black line.
.np
.note extern void   FMEDITAPI DrawConstText( LPRECT, LPSTR, HDC );
.np
Draw the passed string as constant text in the indicated rectangle.
.np
.endnote
.*
.section Action Interface Functions
.*
.np
In order for an application to use to Forms Editor, each of the objects it
expects to edit (those that it gives Create routine addresses for in the
OpenFormEdit call ) must be able to respond to the following routines,
called via their dispatch routine.  An object can choose to return FALSE
when called with any of these actions, instead of implementing the routine,
if it knows that it does not require the routine.
.begnote
.note extern OBJPTR   FMEDITAPI Create( OBJ_ID, OBJPTR, RECT *, OBJPTR );
.np
Action : CREATE
.np
This function is called when the current base object type is the OBJ_ID
passed as the first parameter and the user has drawn a rectangle of the
screen at the location indicated by the passed rectangle.  The second
parameter indicates which object is to be the parent of the object to be
created.  When this routine is called by the Forms Editor, this parameter
will be the object that the initial mouse press down that started the
create occurred on.  The third parameter indicates the handle with which
that object is to communicate to the world with.  When the Forms Editor
calls Create, this parameter will be NULL, indicating that the object
should communicate to the world with a handle which is its own address.
The address of the newly created object (or the handle) is expected as the
return value from calling this function.
.np
The base object type is set by the application (usually in response to a
menu action by the user) and is communicated to the Forms Editor by calling
SetBaseObjectType (described below).  The user objects must have a value
of no less than USER_OBJ, a defined constant.
.np
.note extern bool   FMEDITAPI Register( OBJPTR );
.np
Action : REGISTER
.np
This routine will be called when an object has been successfully created
and initialized and should now register itself with its parent.  That is,
inform that parent that it now has a new child object.  In the normal
course of events, when the object is created as a respond to user actions,
the Register call will be made by the Forms Editor.  If an application is
initializing an editing session by creating its own objects, it will have
to also ensure that they register themselves with their parent.
.np
.note extern bool   FMEDITAPI Move( OBJPTR, POINT *, bool );
.np
Action : MOVE
.np
This routine will be called when the user moves the indicated object.  The
point passed in will indicate the amount to move in both the x and y
directions.  These values will be in device coordinates.  The third
parameter will always be set to TRUE when the Forms Editor calls this
routine, indicating that the action was initiated by the user and that the
object may wish to verify that the requested move action is valid for that
object.  When the application calls this routine to communicate between its
objects, it may wish to set this parameter to FALSE, indicating that the
object initiating the move (typically the parent) already knows that this
move is valid.  The Forms Editor will remove the object from its parent
before Move is called.  Even if the move fails, the object is expected to
have its location recorded as the new location.  The Forms Editor will
initiate the opposite move in order to undo the operation.  This must be
done separately to facilitate moving multiple objects at the same time.
.np
.note extern bool   FMEDITAPI Resize( OBJPTR, RECT * );
.np
Action : RESIZE
.np
This routine will be called when the user resizes the indicated object.
The passed rectangle represents the new location of the object, and is
in device coordinates.
.np
.note extern bool   FMEDITAPI Location( OBJPTR, RECT * );
.np
Action : LOCATION
.np
This routine will be called by the Forms Editor when it wants to know the
coordinates that the called object is located at.  The object must return
its location, in device coordinates, in the rectangle structure passed in
as the second parameter.
.np
.note extern bool   FMEDITAPI FindObjList( OBJPTR, SUBOBJ_REQUEST *, LIST ** );
.np
Action : FIND_SUBOBJECTS
.np
This routine will be called by the Forms Editor when it wants to know about
the subobjects of the called object.  It passes a request for information
in the first parameter and expects a list of the requested objects to be
returned, with the address of the list structure returned as the third
parameter.  The request for information has the following structure :
.np
.millust begin
typedef union {
    ALL_REQ    a;
    POINT_REQ  p;
} SUBOBJ_REQUEST;

typedef struct {
    REQ_ID  ty;
} ALL_REQ;

typedef struct {
    REQ_ID   ty;
    POINT    pt;
} POINT_REQ;

typedef enum {
    ALL,
    BY_POINT
} REQ_ID;
.millust end
.np
If the ty is ALL, a list of all of the subobjects is requested.  If the ty
is BY_POINT, a list of subobjects at the point pt is requested.  There will
only be one subobject at this point so the list is expected to have only
one object.  There are a set of list routines defined by the Forms Editor
that can be used by the application.  They are listed below.
.np
.note extern bool   FMEDITAPI Draw( OBJPTR, RECT *, HDC );
.np
Action : DRAW
.np
The Forms Editor will call this routine when it wants the called object to
draw any portion of itself that falls within the passed rectangle.  This
object should also instruct any of its children who fall within this
rectangle to do the same.  The object should use the passed HDC to do this,
and may use any of the painting routines described above, or use it own
method of drawing.
.np
.note extern bool   FMEDITAPI Destroy( OBJPTR, bool );
.np
Action : DESTROY
.np
This routine will be called by the Forms Editor when the user has selected
the indicated object and the delete menu item.  The boolean value passed
indicates whether the object is being destroyed because of a menu action
by the user.  Whenever the Forms Editor calls destroy, this value will
be TRUE.
.np
.note extern bool   FMEDITAPI Define( OBJPTR, POINT *, void * );
.np
Action : DEFINE
.np
This routine is called by the Forms Editor when the user double clicks on
the indicated object.  The second parameter is the location of the double
click, in case this information is required to define the object.  The third
parameter will always be NULL when Define is called by the Forms Editor,
but may be used by the application, if required.
.np
.note extern bool   FMEDITAPI Notify( OBJPTR, NOTE_ID, void * );
.np
Action : NOTIFY
.np
The Forms Editor will call this routine when it has some information to
communicate to the called object.  The second parameter communicates the
type of information.
.np
.millust begin
typedef enum {
    NEW_PARENT,
    PARENT_RESIZE,
    TERMINATE_EDIT,
    CURRENT_OBJECT
} NOTE_ID;
.millust end
.np
If the second parameter is NEW_PARENT, then the third parameter will be an
OBJPTR, which is the new parent of the called object.
.np
If the second parameter is PARENT_RESIZE, then the third parameter will be
NULL.  This indicates that the called object's parent has changed size.
.np
If the second parameter is TERMINATE_EDIT, then the third parameter will be
NULL.  This indicates that there has been a mouse action that indicates
that the user is finished editing the current object.
.np
The second parameter is CURRENT_OBJECT, then the third parameter will be
NULL and the Forms Editor is telling the object that it has become the
current object.
.np
The application may expand this enumeration to facilitate notification
between its objects.  The constant APP_NOTE_IDS is defined as the starting
value for extension of this enumeration.
.np
.note extern bool   FMEDITAPI Forward( OBJPTR, ACTION_ID, void *, void * );
.np
This function can be called by either the Forms Editor or the application
to call the dispatch routine of the object passed as the first parameter
with the action passed as the second parameter.  The third and fourth
parameters are passed to the object as parameters to the routine implied
by the action.  This is useful in the case of inheritance.  If an inherited
object is used to take care of certain functionality, the inheriting object
can simply forward the required messages to the inherited object, or even
just forward all of the messages it does not understand itself to the
inherited object.
.np
.note extern bool   FMEDITAPI ValidateAction( OBJPTR, ACTION_ID, void * );
.np
Action : VALIDATE_ACTION
.np
This function is called by either the Forms Editor or the application to
find out if the passed action is recognized by the called object.  This
is a way of telling if the action is valid for that object.  If the action
is MOVE, the third parameter will be a POINT *, and the object is expected
to indicate whether or not move is a valid operation for it, when the mouse
is pressed down at the indicated point.  This is useful for parent objects
who much be grabbed at their borders to be move.
.np
.note extern OBJPTR   FMEDITAPI FindObject( SUBOBJ_REQUEST * );
.np
This routine can be called by either the Forms Editor or the application
when one object needs to be found.  This routine will pass the subobject
request to the main object, and then, in turn, call the object that it
returns with the same request until the object called returns FALSE from
the subobject request.  This will be the object that is returned from the
call to FindObject.  For example, this routine can be used with a BY_POINT
request, so that it will find the object that is located at the point but
has no subobjects located at that point.  If move is not valid for an object,
then a mouse press on the object will result in the beginning of a banded
select operation.
.np
.note extern bool   FMEDITAPI AddObject( OBJPTR, OBJPTR );
.np
ACTOIN :  ADD_SUBOBJECT
.np
This routine is called by the Forms Editor or the application to add the
object passed as parameter two as a subobject of the called object
(the object passed in parameter one).
.np
.note extern bool   FMEDITAPI RemoveObject( OBJPTR, OBJPTR );
.np
Action : REMOVE_SUBOBJECT
.np
This routine is called by the Forms Editor or the application to remove the
object passed as parameter two from the list of subobjects of the called
object (the object passed as parameter one).
.np
.note extern bool   FMEDITAPI ExecuteCurrObject( ACTION_ID, void *, void * );
.np
This routine is called by the Forms Editor or the application when it wants
all of the current objects to have their dispatch routines called with the
passed action, with parameters two and three passed to the dispatch routine
as parameters to the routine implied by the action.
.np
.note extern bool   FMEDITAPI GetResizeInfo( OBJPTR, RESIZE_ID * );
.np
Action : RESIZE_INFO
.np
This routine is called by the forms editor when an object becomes the
primary object.  Primary objects may be resized by the user.  This function
asks the object in which dimensions it is resizeable.  The char whose
address is passed to the application should be set to an ORed value,
which is a combination of the following defined constants :
.np
.millust begin
typedef enum {
    R_NONE   = 0,
    R_TOP    = 1,
    R_BOTTOM = 2,
    R_LEFT   = 4,
    R_RIGHT  = 8,
    R_ALL    = R_TOP | R_BOTTOM | R_LEFT | R_RIGHT
} RESIZE_ID;
.millust end
.np
.note extern bool   FMEDITAPI CutObject( OBJPTR, OBJPTR * );
.np
Action : CUT
.np
This routine is called when the object passed in the first parameter is
current and the user initializes the cut menu action.  The object is
expected to remove itself from its parent and return the address of the
cut object in the address indicated by the second parameter.
.np
.note extern bool   FMEDITAPI CopyObject( OBJPTR, OBJPTR *, OBJPTR );
.np
Action : COPY
.np
This routine is called when the object passed in the first parameter is
current and the user initiates the copy menu action.  The object is
expected to make a copy of itself and return the address of the copied
object in the address indicated by the second parameter.  The third
parameter is for use by the application.  When the Forms Editor initiates
a call to CopyObject, the third parameter will be NULL.  The application
may wish to use the third parameter to pass the handle for the copied
object to use, in the case of inherited objects.
.np
.note extern bool   FMEDITAPI PasteObject( OBJPTR, OBJPTR, POINT );
.np
Action : PASTE
.np
This routine will be called when the object passed as the first parameter
has been cut or copied and then the user initiates a paste operation.
The second parameter contains the OBJPTR of the object that is to be the
parent of the object, when it is pasted.  The third parameter contains the
point that is to be the location of the object's upper left corner.
.np
.note extern bool   FMEDITAPI GetObjectParent( OBJPTR, OBJPTR *);
.np
Action : GET_PARENT
.np
The Forms Editor calls this function when it wants the object passed as
the first parameter to return the OBJPTR of its parent in the address
passed as the second parameter.
.np
.note extern bool   FMEDITAPI GetPriority( OBJPTR, int * );
.np
Action : GET_PRIORITY
.np
This function is called when the Forms Editor wants the object passed as
the first parameter to return its priority in the address passed as
parameter.  The priority is to be an integer value that reflects the
nesting or parent/child relationship of objects.  If the object does not
respond to this message, it will be assume that the object is a top level
object.  That is, it has no parent.  Each object should return the same
priority value for siblings and this value should be one greater than the
priority value for their parent.  This information is used by the Forms
Editor to perform certain operations on parents before their children.
.endnote
.*
.section Information Interface Functions
.*
.np
The following functions are used to communicate information between the
Forms Editor and the application.  To ensure that the correct information
is returned, call InitState before using any of the following functions.
Specifically, the state is different for each version of the Forms Editor,
if there is more than one copy of the Forms Editor running at the same time.
InitState will pass the editor the HWND parameter of the Forms Editor, from
which it derives its state information.  This function only needs to be
called when it is possible that the previously active window is not the
same as the currently active window.  This can happen if the Forms Editor
window has lost focus, as will happen during the use of a dialog box.
.begnote
.note extern OBJ_ID   FMEDITAPI GetBaseObjType();
.np
This function will return the base object type, as defined by the last call
to SetBaseObjectType.  The base object type indicates the type of object
that will be created when the user draws a new object in the Forms Editor.
.np
.note extern void   FMEDITAPI SetBaseObjType( OBJ_ID );
.np
Calling this function will set the base object type.  The base object type
indicates the type of object that will be created when the user draws a new
object in the Forms Editor.  This is typically indicated when the user
issues a menu action.  If the base object type is set to EDIT_SELECT (a
defined constant), then it is assumed that the user has indicated that a
select mode is desired instead of a create mode.  When this is the base
object type, the user will not be able to create objects but will, instead,
be able to select an object by single clicking on it, move it by press and
move,  resize it, or select multiple objects using banded select.
.np
.note extern void   FMEDITAPI InitState( HWND );
.np
This routine is called by an application when it wants to ensure that the
state is correctly set.  This must be done any time there is a possibility
that the application has lost focus to another application (for example,
when a dialog box is being used or multiple copies of the Forms Editor
are being used).  Typically, this routine is called immediately after
entering a dialog box that calls any of the information interface functions.
.np
.note extern OBJPTR   FMEDITAPI GetMainObject();
.np
The function is called by the Forms Editor or the application to get the
main object.  The main object is the one that contains all other objects.
The object is automatically created by then Forms Editor when OpenFormEdit
is called and is the parent of the highest level objects that are visible
in the Forms Editor.  The Forms Editor expects the main object to be the
first object in the list of object create routines passed to OpenFormEdit.
.np
.note extern CURROBJPTR   FMEDITAPI GetCurrObject();
.np
This function is called by the Forms Editor or the application to get the
current object.  That is, the object that the user has single clicked on
to make it current.  The value returned is a CURROBJPTR.  This is not a
pointer that the application can dereference.  It is a handle to the
current object.  If the application must know the OBJPTR of the object that
is associated with that current object, the application should call GetObjptr.
.np
.note extern CURROBJPTR   FMEDITAPI GetNextCurrObject( CURROBJPTR );
.np
If there is more than one current object, then this routine, when passed one
current object, will return the next current object in the list of current
objects.  This routine, in conjunction with GetCurrObject, can be used to
find out all of the current objects.  It will return NULL when there are
no more current objects.
.np
.note extern CURROBJPTR   FMEDITAPI GetPrimaryObject();
.np
This routine returns the current object which is primary. That is, the
current object within the list of current objects that was most recently
selected by the user.  The function returns a handle to that object (a
CURROBJPTR) not an OBJPTR (see GetCurrObject).
.np
.note extern OBJPTR   FMEDITAPI GetObjptr( CURROBJPTR );
.np
This routine takes a CURROBJPTR (a handle to a current object) and returns
the OBJPTR of the object associated with that current object.
.np
.note extern void   FMEDITAPI GetOffset( POINT * );
.np
This routine can be called by the Forms Editor or the application to get
the point which defines the scrolling offset of the client area.  That is,
if the user has scrolled down and to the right, the point returned will
have positive x and positive y values.  This value will indicate the offset
of the top left point that is visible in the Forms Editor from the top left
point that could be visible if the user scrolled to the top and left as far
as possible.  The point is given in device coordinates.
.np
.note extern void   FMEDITAPI SetHorizontalInc( unsigned );
.np
Set the horizontal grid value.  This is the increment used as the
horizontal granularity of move, resize and create operations.
.np
.note extern void   FMEDITAPI SetVerticalInc( unsigned );
.np
Set the vertical grid value.  This is the increment used as the
vertical granularity of move, resize and create operations.
.np
.note extern unsigned   FMEDITAPI GetHorizontalInc();
.np
Get the horizontal grid value.  This is the increment used as the
horizontal granularity of move, resize and create operations.
.np
.note extern unsigned   FMEDITAPI GetVerticalInc();
.np
Get the vertical grid value.  This is the increment used as the
vertical granularity of move, resize and create operations.
.np
.note extern void   FMEDITAPI AddCopyObject( OBJPTR, OBJPTR );
.np
Each time an object is called by CopyObject, it should call AddCopyObject
with the first parameter being the OBJPTR of the object being copied and
the second parameter being the OBJPTR of the copy of the object.  This is
required because, when a copy operation is taking place, an object who has
children may want to ensure that the children are also copied, even though
they are not current objects.  This presents a problem if they are also
current objects.  The child object should not be copied twice.  By keeping
a list of the copied objects, the Forms Editor will ensure that no object
is in the list twice.
.np
.note extern bool   FMEDITAPI CopyObjExists( OBJPTR );
.np
This function can be called by the application to see whether or not an
object has already been added to the copy list.  For example, if an object
is being copied and it wants to ensure that its children are also copied,
it can call CopyObjExists with the OBJPTR of the child.  If TRUE is
returned, then the child has already been copied and should not be copied
again.  If FALSE is returned, the child should be copied.
.np
.note extern bool   FMEDITAPI PasteValid();
.np
This function can be called by the application to ask if paste would
currently be a valid operation.  This can be used to grey the Paste menu
item if paste is not a valid action.  Paste would not be a valid action
if no cut or copy had taken place.
.np
.note extern void   FMEDITAPI DisplayError( char * );
.np
This function should be called by the application to display any error
messages that come up as the result of Forms Editor initiated activities.
The Forms Editor will display the message when it has completed its activity.
For example, if multiple objects are being moved, the Forms Editor does
not want any error message displayed until after the move is complete.
.endnote
.*
.section Application Input Functions
.*
.np
The following functions can be used by an application to provide
information to the Forms Editor.  These would normally be useful when an
application is editing Windows and the user can initiate actions the editor
needs to know about by interacting directly with those windows.
.begnote
.note extern bool   FMEDITAPI ObjectPress( OBJPTR, POINT *, WORD, HWND );
.np
By calling this function, the application is telling the Forms Editor that
the user has pressed on the object passed in the first parameter at the
point indicated by the second parameter.  The third parameter is the
information that would normally be received in the wparam of a mouse press
message received from Windows. The last parameter is the handle of the
Forms Editor window which contains the relevant object.
.np
.note extern void   FMEDITAPI ObjectDestroyed( OBJPTR );
.np
Buy calling this function, and application is telling the Forms Editor
that the object passed as the first parameter has been destroyed and the
OBJPTR is no longer valid.  Any references to this OBJPTR will be removed
from the Forms Editor.
.endnote
.*
.section Menus
.*
.np
The Forms Editor requires menu input.  An application can either create its
own menus or have the Forms Editor create them.  The third parameter of the
call to OpenFormEdit allows the user to specify which option it prefers for
each of the menus.  If the application asks the Forms Editor to supply a cut,
copy, paste, delete or align menu item, the Forms Editor will look at the
menus of its window.  If the application has created a menu with the name
'Edit', the Forms Editor will append the required menu items to that menu,
with a separator between the bottom of the existing menu and the new menu
items.  A separator will also be placed between the first four of these menu
items and the align menu item.  If there is no Edit menu, the Forms Editor
will create one.  If it is requested, the Forms Edit will add a 'Setup' menu
as the second last menu (assuming that the last one is the help menu), to
which it adds a 'Grid' menu item.  This menu item allows the user set the
granularity of movements, in pixels.  This is useful to help the user align
objects correctly by eye.  When these menu items are selected by the user,
it is expected that the application will pass the menu actions to the Forms
Editor via the FMEditWndProc function call.  The application must have a
'Select' menu item which, when active, will prompt the application to set
the Base Object Type to EDIT_SELECT (see SetBaseObjectType).
.*
.section Type Definitions
.*
.np
The above function and type definitions, as well as the ones below, will
all be available to the application by including the file fmedit.def and
linking with the library fmedit.lib. The file fmedit.dll must be somewhere
in the user's path in order to use the Forms Editor.
.np
.millust begin
#define EDIT_SELECT     0
#define USER_OBJ        16
#define OBJ_ITEM        2
#define IDM_FMEDIT      500

typedef int                 OBJ_ID;
typedef struct object       OBJECT;
typedef OBJECT *            OBJPTR;
typedef OBJPTR CALLBACK     CREATE_RTN( OBJPTR, RECT *, OBJPTR );
typedef CREATE_RTN **       CREATE_TABLE;
typedef struct list         LIST;
.millust end
.np
.*
.section List Functions
.*
.np
The following list functions are provided to the user.  Although they deal
with the type OBJPTR, this type is defined to be void far *, so these
routines can be used a generic routines for list of any type.
.np
.millust begin
extern OBJPTR   FMEDITAPI ListElement( LIST * );
extern void     FMEDITAPI ListFree( LIST * );
extern LIST *   FMEDITAPI ListNext( LIST * );
extern LIST *   FMEDITAPI ListPrev( LIST * );
extern void     FMEDITAPI ListAddElt( LIST **, OBJPTR );
extern void     FMEDITAPI ListRemoveElt( LIST **, OBJPTR );
extern LIST *   FMEDITAPI ListConsume( LIST * );
extern LIST *   FMEDITAPI ListFindElt( LIST *, OBJPTR );
extern LIST *   FMEDITAPI ListCopy( LIST * );
extern int      FMEDITAPI ListCount( LIST * );
extern void     FMEDITAPI ListInsertElt( LIST *, OBJPTR );
extern void     FMEDITAPI ListMerge( LIST **, LIST * );
.millust end
.np
.chap WATCOM Report Editor Design Notes
.np
This file contains notes on the report editor internal design. Let's try to keep it up to date.
.*
.section History ( 03/23/1991 )
.*
.np
The report editor started as just the message processing loop for the parent
window.  The menu items supported were : About, New, Open, Save, SaveAs and
Exit.  These menus simply displayed and allowed modification of files using
an edit window.
.np
The DataSource menu item was then implemented to get datasource information
using dialog boxes.
.np
The Generate menu item was next to be implemented.  It would take the text
in the edit window and the datasource information already specified and
create a report specification, representing both, which could be used as
input to the report generator. The text from the edit window was
interpreted as a series of constant value items.
.np
Soon the edit window was changed.  Its previous class was Edit, a predefined
control class.  A new class, PaintWClass, now gets registered on
initialization and this is the type of the edit window - now called
the Paint Window.
.np
The Paint Window allows panels, fields and text to be created, represented
on the screen and edited.  The internal data structures now represent
arbitrary levels of panels and this information can be used to generate
input for the report generator.
.*
.section OverView
.*
.np
The Report Editor consists of the following 8 managers :
.autonote
.note
the Main Window Manager
.note
the Panel Window Message Loop
.note
the Panel Manager
.note
the DataSource Manager
.note
the Mouse Manager
.note
the Screen Painter
.note
the Edit Window Manager
.note
the Output Manager
.endnote
which are descriped below.
.*
.section The Main Window Manager
.*
.np
The Main Window Manager has three main functions :
.np
.autonote
.note
it contains WinMain which Windows calls to initialize the application
and subsequent instances of the application.
.note
it contains MainWndProc which is the main message processing loop.
This message loop :
.begbull
.bull
receives messages from Windows when menu items are selected.  It then
calls the correct function to carry out the required action.
.bull
sends painting information to the Paint Window when the Paint Window has
lost focus and so will not get paint messages directly from Windows.
.endbull
.note
it contains functions called in response to menus being selected if
those functions are not the responsibility of any other manager.
.endnote
.*
.section The Panel Window Message Loop
.*
.np
The Panel Window Message Loop receives messages from Windows and passes
this informatio on to the correct manager.
.np
Specifically :
.begbull
.bull
if mouse input is received, the correct routine of the Mouse Manager is
called with the information
.bull
if a WM_PAINT message is received, the Paint Manager is called with the
information
.endbull
.*
.section The Panel Manager
.*
.np
The Panel Manager contains all of the subroutines that access the PANEL data
structure.  It alone knows what this structure looks like and only the Panel
Manager is allowed to create, modify and delete objects contained within
this structure.  The Panel Manager gets its information from both the user
( using dialog boxes ) and other managers ( such as the Mouse Manager when
an object gets moved ).
.*
.section The DataSource Manager
.*
.np
The DataSource Manager is called by the Main Message Loop when the DataSource
menu item is selected.  The DataSource Manager uses a series of dialog boxes
to allow the user to input, view and edit information about datasources.
Only the DataSource Manager has access to the information in the DATASOURCE
data structure.
.*
.section The Mouse Manager
.*
.np
The Mouse Manager receives input from the Panel Window Message Loop when it
gets mouse input.  The Mouse Manager is responsible for allowing the user to :
.begbull
.bull
create rectangles
.bull
select and rectangle and use it to create a field or panel
.bull
select a field or panel and change its attributes
.bull
select and move an object ( in the above three cases, the Panel Manager is
called to collect and update information as required )
.bull
enter text ( the Edit Window Manager is called )
.endbull
.*
.section The Screen Painter
.*
.np
The Screen Painter is called by the Panel Window Message Loop whenever it
receives a WM_PAINT message.  The Screen Painter is responsible for
repainting the screen so that it is an accurate reflection of the current
state of the data without repainting anything that does not need to be
repainted.
.*
.section The Edit Window Manager
.*
.np
The Edit Window Manager is called by the Mouse Manager whenever the user
indicates that he wants to enter or edit text.  The Edit Window Manager is
responsible for allowing the user to enter and edit text, storing the text
information, and ensuring that the Paint Manager receives the information
which it requires to keep the screen current.
.*
.section The Output Manager
.*
.np
The Output Manager is called by the Main Window Manager when the Generate
menu item is selected.  The Output Manager is responsible for :
.begbull
.bull
getting the datasource information from the DataSource Manager
.bull
getting the panel information from the Panel Manager
.bull
using this information to generate a report specification ( in the specified
file ) that can be used as input to the report generator.
.endbull
.*
.section Data Structure Change ( 10/04/1991 )
.*
.np
There are now three types of data structures :
.np
.* .beglevel
objects ( OBJECT )
.np
fields  ( FIELDINFO )
.np
panels  ( PANEL ).
.* .endlevel
.np
The first field of both the FIELDINFO and PANEL datastructures is of type
OBJECT so that field and panel pointers can be cast as object pointers so
that only the Panel Manager actually accesses the data fields specific to
the FIELDINFO and PANEL data structures.  All other functions only access
the fields in the OBJECT data structure.
.*
.section Outstanding Issues ( 10/04/1991 )
.*
.autonote
.note
Occasionally a ButtonDown message will result in the wrong object being
selected.  The reason for this is unknown.
.note
The Main Window Manager currently intercepts some paint messages and relays
them to the Panel Window Manager.  This should not really be necessary.
.note
Since the local heap should be used for all memory allocations of less than
64K, the entire application should be using LocalAlloc.  Any memory that is
permanent ( ie field labels ) should be allocated as moveable and then
locked when they are referenced.  Using LocalAlloc for allocation of space
for data structures should also be considered.
.endnote
.*
.section Outstanding Issues( 24/04/1991 )
.*
.autonote
.note
Outstanding issue 1) from 10/04/1991 has been resolved.
.note
Outstanding issuue 3) from 10/04/1991 has be partially resolved. All memory
is now allocated from the local heap using LocalAlloc. It is currently all
fixed.  There are still a few places where memory should be dynamically
allocated by it is being statically allocated ( some dialog boxes ).
.endnote
.*
.section Issues Developers Should Be Aware Of ( 24/04/1991 )
.*
.np
The function InsertObj() will insert any object it is asked to. Before
calling InsertObj( objectptr ), either ValidObj( objectptr ),
HorizontalAlign( objectptr ) or VerticalAlign( objectptr ) must be
called and return TRUE.
.np
It is important to understand what the various states mean.  To the best
of my knowledge they mean the following :
.np
.begnote
.note SELECT
.np
an object is selected.  The cursor is a cross and the selected object can
be moved.  The mouse has been depressed to select the object but not yet
released.
.np
.note NOSELECT
.np
although there is a current object ( that has the resizing squares around
it ) no object is currently 'selected' to be moved.  This is that state
that the system will be in by default if nothing else is happening.
.np
.note DRAWRECT
.np
a new object is being drawn.  The mouse has been depressed to start drawing
the object but has not yet be released to signify that the object currently
drawn should be created.
.np
.note EDITTEXT
.np
a textedit window currently exists and text is being edited.
.np
.note OVERBOX
.np
the mouse has not been depressed but it is currently placed above one of
the sizing squares at the corners or sides of the current object and so
the cursor is currently in the shape of a filled in black square
.np
.note SIZE
.np
while in state overbox, the mouse was depressed so that the current object
is being resized.  The mouse has not yet been released.
.endnote
.np
When ValidObj() or InsertObj() are called, they do different things
depending on the current state.  As a result, states are occasionally
changed before a call to one of these functions and then restored to what
is was after the call in order to achieve the desired result. This is
probably not a good idea but it works!
.np
For example, when ValidObj() is called and the state is DRAWRECT, the object
