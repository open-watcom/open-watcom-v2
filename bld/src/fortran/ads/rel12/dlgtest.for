!               ADS Programmable Dialog Box Test Program
!
!    Programmable Dialog Box Test Program
!
!         This program is the ADS counterpart to the LISP test
!    program, dlgtest.lsp.  It provides a simple dimensioning
!    dialog invoked with the command "dimen" and a simple color
!    dialog invoked with the command "setcolor".
!
!         The purposes of providing this program:
!    1) Demonstrate Programmable Dialog Box use with minimum of code
!       to sort through
!    2) Demonstrate differences between LISP and ADS dialog
!        programming
!    3) Use as a starting point to try out new dialog code
!
!         Dlgtest uses the file dlgtest.dcl as the DCL (Dialog
!    Control Language) file.
!
!         ADS functions are associated with dialog buttons with
!    the ads_action_tile functions.  These functions are called
!    when the user presses buttons during the ads_start_dialog
!    function.
!
!    Special tile names (keys):
!        "accept" - Ok button
!        "cancel" - Cancel button

c$include adsapi.fi
c$include adsdlgap.fi

        program dlgtest

        include 'adslib.fi'

        integer*2 scode/RSRSLT/
        integer stat

        integer dofun, loadfuncs, funcinit
        external dofun, loadfuncs, funcinit

        call funcinit()

        call ads_init( 0, 0 )

        loop

            stat = ads_link( scode )
            if( stat .lt. 0 )then
                print *, 'TEST: bad status from ads_link() = ', stat
                return
            endif

            scode = RSRSLT              ! default return code

            ! Check for AT LEAST the following cases here
            select( stat )

                ! Load or define Lisp functions
            case( RQXLOAD )             ! Load request.  Send function defuns
                if( loadfuncs() .ne. 0 )then
                    scode = -RSRSLT
                else
                    scode = -RSERR
                endif

                ! Unload or undefine ALL functions previously defined.
            case( RQXUNLD )             ! Unload request. Do cleanup
                ! This defaults to sending an RSRSLT.  If you send an
                ! RSERR, you can refuse to unload the program, but Lisp
                ! will still ask you to terminate.

                ! Execute a "loaded" function that was defined via RQXLOAD
            case( RQSUBR )
                call dofun()

            endselect
        endloop
        end


! FUNCINIT  --  Initialize function definition structure

        integer function funcinit()

        include 'adslib.fi'

        integer loadfuncs

        integer id, rb, i

        integer  setcolor
        external setcolor, setdimen

        structure /func_entry/
            character*32        func_name
            integer             func
        end structure

        ! To add another function, change the value of "NUM_FUNS" and
        ! add the new function(s) to the function table ("func_table").

        integer NUM_FUNS
        parameter (NUM_FUNS = 2)
        record /func_entry/ func_table(NUM_FUNS)

        ! Subroutines names to be registered with AutoLisp and provided
        ! in this application.
        func_table(1).func_name = 'C:dimen'c
        func_table(1).func = loc( setdimen )
        func_table(2).func_name = 'C:setcolor'c
        func_table(2).func = loc( setcolor )

        funcinit = 1

        return

        entry loadfuncs()

        do i = 1, NUM_FUNS
            if( ads_defun( func_table(i).func_name, i - 1 ) .eq. 0 )then
                loadfuncs = 0
                return
            endif
        end do

        call ads_printf( 'Functions: 1-dimen 2-setcolor.'//
     &                   char(10)//char(0) )
        loadfuncs = 1

        return


! Execute a defined function.

        entry dofun()

        ! Get the function code from the combuf
        id = ads_getfuncode()
        if( id .lt. 0 )then
            dofun = 0
            return
        endif

        rb = ads_getargs()
        if( rb .ne. NULL )then
            call ads_printf( 'No arguments expected'c )
            call ads_relrb( rb )
        endif

        select( id )    ! Which function is called?

        case( 0 )
            ! dimen -- AutoCAD dimensioning dialog
            call setdimen()

        case( 1 )
            ! setcolor -- AutoCAD color dialog
            call setcolor()

        endselect

        dofun = 1

        end


! DIMEN -- Dimensioning Dialog

        block data dims_init

        integer dimx, dimy
        common /dims/ dimx, dimy
        ! Position of dialog, centered to start
        data dimx/-1/, dimy/-1/

        character*7 dimbools(10)
        common /dimbools/ dimbools
        data dimbools/ 'dimse1'c, 'dimse2'c, 'dimtih'c, 'dimtoh'c,
     &                 'dimtad'c, 'dimtol'c, 'dimlim'c, 'dimalt'c,
     &                 'dimaso'c, 'dimsho'c /

        character*7 dimreals(7)
        common /dimreals/ dimreals
        data dimreals/ 'dimasz'c, 'dimtsz'c, 'dimtxt'c, 'dimcen'c,
     &                 'dimexo'c, 'dimexe'c, 'dimdle'c /

        end


        subroutine setdimen()

        include 'adslib.fi'
        include 'adsdlg.fi'

        integer hdlg, dlg_status, dcl_id

        integer dimx, dimy
        common /dims/ dimx, dimy

        external dimen_ok

        call ads_load_dialog( 'dlgtest.dcl'c, dcl_id )
        if( dcl_id .lt. 0 )then
            call ads_printf( 'Error loading "dlgtest.dcl"'//
     &                       char(10)//char(0) )
            return
        endif
        ! Display the "dimensions" dialog
        call ads_new_positioned_dialog( 'dimensions'c, dcl_id, NULL_PTR,
     &                                  dimx, dimy, hdlg )
        if( hdlg .eq. NULL )then
            call ads_printf( 'The ads_new_dialog function failed',
     &                       char(10)//char(0) )
            call ads_unload_dialog( dcl_id )
            return
        endif

        ! Register dimen_ok function with the OK button
        call ads_action_tile( hdlg, 'accept'c, dimen_ok )

        ! show current values in dialog
        call get_dimvars( hdlg )

        ! run the dialog
        call ads_start_dialog( hdlg, dlg_status )

        ! free all memory for dialog
        call ads_unload_dialog( dcl_id )

        end


! DIMEN_OK -- callback function for OK button of dimension dialog.

        subroutine dimen_ok( cpkt )
        record /ads_callback_packet/ cpkt

        include 'adslib.fi'
        include 'adsdlg.fi'

        integer dimx, dimy
        common /dims/ dimx, dimy

        ! User pressed OK button to end dialog.  Check modified data
        ! and send to AutoCAD.
        call set_dimvars( cpkt.dialog )
        call ads_done_positioned_dialog( cpkt.dialog, 1, dimx, dimy )
        end


! Show current values in dialog

        subroutine get_dimvars( hdlg )
        integer hdlg

        include 'adslib.fi'
        include 'adsdlg.fi'
        include 'malloc.fi'

        integer i
        character*80 value

        character*7 dimbools(10)
        common /dimbools/ dimbools

        character*7 dimreals(7)
        common /dimreals/ dimreals

        record /resbuf/ rb

        do i = 1, 10
            call ads_getvar( dimbools(i), rb )
            if( rb.restype .ne. RTSHORT )then
                call ads_printf( 'No such AutoCAD variable: %s'//
     &                           char(10)//char(0), dimbools(i) )
                if( rb.restype .eq. RTSTR )then
                    call free( rb.resval.rstring )
                endif
                cycle
            endif
            write( value, '(i10,a)' ) rb.resval.rint, char(0)
            call ads_set_tile( hdlg, dimbools(i), value )
        enddo

        do i = 1, 7
            call ads_getvar( dimreals(i), rb )
            if( rb.restype .ne. RTREAL )then
                call ads_printf( 'No such AutoCAD variable: %s'//
     &                           char(10)//char(0), dimreals(i) )
                if( rb.restype .eq. RTSTR )then
                    call free( rb.resval.rstring )
                endif
                cycle
            endif
            call ads_rtos( rb.resval.rreal, -1, -1, value )
            call ads_set_tile( hdlg, dimreals(i), value )
        enddo
        call ads_set_tile( hdlg, 'test_item'c, 'test_value'c )

        end


! set modified dimension variables in AutoCAD

        subroutine set_dimvars( hdlg )
        integer hdlg

        include 'adslib.fi'
        include 'adsdlg.fi'
        include 'stdlib.fi'

        integer i
        character*(MAX_TILE_STR+1) val

        character*7 dimbools(10)
        common /dimbools/ dimbools

        character*7 dimreals(7)
        common /dimreals/ dimreals

        record /resbuf/ rb

        ! Check all the checkbox tiles for new values
        do i = 1, 10
            ! Get the new value of tiles
            call ads_get_tile( hdlg, dimbools(i), val, MAX_TILE_STR )
            rb.restype = RTSHORT
            rb.resval.rint = atoi( val )
            call ads_setvar( dimbools(i), rb )
        enddo

        ! Check all the edit box tiles for new values
        do i = 1, 7
            ! Get the new value of tiles
            call ads_get_tile( hdlg, dimreals(i), val, MAX_TILE_STR )
            rb.restype = RTREAL
            call ads_distof( val, -1, rb.resval.rreal )
            call ads_setvar( dimreals(i), rb )
        enddo
        call ads_get_tile( hdlg, 'test_item'c, 'test_value'c, 50 )

        end


! SETCOLOR -- Color Dialog

        integer function setcolor()

        include 'adslib.fi'
        include 'adsdlg.fi'
        include 'stdlib.fi'
        include 'string.fi'

        integer idx, dlg_status, dcl_id, hdlg, cptr, len, i
        character ptr(:)
        integer*2 colorsave
        record /resbuf/ rb
        character*10 cname

        character*8 colorlist(8)/ 'black'c, 'red'c, 'yellow'c,
     &                            'green'c, 'cyan'c, 'blue'c,
     &                            'magenta'c, 'white'c/

        integer CLEN
        parameter (CLEN=32)
        character*(CLEN+1) colorstr

        integer*2 color
        common /color/ color

        external editcol_cb, listcol_cb

        ! Load the dialog file
        call ads_load_dialog( 'dlgtest.dcl'c, dcl_id )
        if( dcl_id .lt. 0 )then
            call ads_printf( 'Error loading "dlgtest.dcl"'//
     &                       char(10)//char(0) )
            setcolor = -1
            return
        endif
        ! initialize the setcolor dialog, no default callback function
        call ads_new_dialog( 'setcolor'c, dcl_id, NULL_PTR, hdlg )
        if( hdlg .eq. NULL )then
            call ads_printf( 'new_dialog for setcolor failed'//
     &                       char(10)//char(0) )
            call ads_unload_dialog( dcl_id )
            setcolor = -1
            return
        endif
        ! Get the current color from AutoCAD
        call ads_getvar( 'CECOLOR'c, rb )
        ! AutoCAD  currently returns  "human readable" colour strings
        ! like "1 (red)" for the standard colours.  Trim  the  string
        ! at  the  first space to guarantee we have a valid string to
        ! restore the colour later.
        cptr = rb.resval.rstring
        len = strlen( cptr )
        allocate( ptr(1+len), location=cptr )
        do i = 1, len
            if( ptr(i) .eq. ' ' ) exit
            colorstr(i:i) = ptr(i)
        enddo
        colorstr(i:i) = char(0)
        color = colorsave = atoi( colorstr )

        ! Update other tiles when one is changed by using callback
        ! functions
        call ads_action_tile( hdlg, 'edit_col'c, editcol_cb )
        call ads_action_tile( hdlg, 'list_col'c, listcol_cb )

        ! Use the client data pointer to store the key of each tile,
        ! for convenient access during callbacks.   We could use
        ! get_attr_string during the callbacks instead.
        call ads_client_data_tile( hdlg, 'edit_col'c, 'edit_col' )
        call ads_client_data_tile( hdlg, 'list_col'c, 'list_col' )

        ! Fill list box
        call ads_start_list( hdlg, 'list_col'c, LIST_NEW, 0 )
        do idx = 1, 8
            call ads_add_list( colorlist(idx) )
        enddo
        do idx = 9, 256
            write( cname, '(i3,a)' ) idx-1, char(0)
            call ads_add_list( cname )
        enddo
        call ads_end_list()

        ! Show initial values in edit box, list box, and image tile
        call ads_set_tile( hdlg, 'edit_col'c, colorstr )
        call ads_set_tile( hdlg, 'list_col'c, colorstr )
        call dlg_colortile( hdlg, 'show_image'c, color, 1 )

        ! Hand control over to the dialog until OK or CANCEL is pressed
        call ads_start_dialog( hdlg, dlg_status )

        ! Dialog ended with OK button, "accept"?
        if( dlg_status .eq. DLGOK )then
            if( color .eq. 0 )then
                call ads_command( RTSTR, 'COLOUR'c, RTSTR, 'BYLAYER'c,
     &                            RTNONE )
            else
                call ads_command( RTSTR, 'COLOUR'c, RTSHORT, color,
     &                            RTNONE )
            endif
        endif
        ! free all memory for dialog
        call ads_unload_dialog( dcl_id )

        if( dlg_status .eq. DLGOK )then
            setcolor = color
        else
            setcolor = colorsave
        endif

        end


! EDITCOL_CB -- ADS callback for color edit box.

        subroutine editcol_cb( cpkt )
        record /ads_callback_packet/ cpkt

        include 'adslib.fi'
        include 'adsdlg.fi'
        include 'stdlib.fi'
        include 'string.fi'

        integer*2 color
        common /color/ color

        character ptr(:)

        if( cpkt.value .eq. NULL ) return
        allocate( ptr(strlen(cpkt.value)), location=cpkt.value )
        if( ptr(1) .eq. char(0) ) return

        call ads_set_tile( cpkt.dialog, 'list_col'c, cpkt.value )
        color = atoi( cpkt.value )
        call dlg_colortile( cpkt.dialog, 'show_image'c, color, 1 )

        end


! LISTCOL_CB -- ADS callback for color edit box.

        subroutine listcol_cb( cpkt )
        record /ads_callback_packet/ cpkt

        include 'adslib.fi'
        include 'adsdlg.fi'
        include 'stdlib.fi'
        include 'string.fi'

        integer MAXKEYLEN
        parameter (MAXKEYLEN=32)
        character*(MAXKEYLEN) akey

        integer*2 color
        common /color/ color

        character ptr(:)

        if( cpkt.value .eq. NULL ) return
        allocate( ptr(strlen(cpkt.value)), location=cpkt.value )
        if( ptr(1) .eq. char(0) ) return

        ! Get key
        call ads_get_attr_string( cpkt.tile, 'key'c, akey, MAXKEYLEN )

        call ads_set_tile( cpkt.dialog, 'edit_col'c, cpkt.value )
        color = atoi( cpkt.value )
        call dlg_colortile( cpkt.dialog, 'show_image'c, color, 1 )
        end


! DLG_COLORTILE -- Color a tile.  "dialog" can be NULL to use the
!                  current dialog.  Values for "color" may be 0 to
!                  255.  "color" may be one of the defines in
!                  colours.h, such as RED.   Draws border if
!                  borderflag is TRUE. */

        subroutine dlg_colortile( hdlg, key, color, borderflag )
        integer hdlg, borderflag
        integer*2 color
        character*(*) key

        include 'adslib.fi'
        include 'adsdlg.fi'

        integer*2 width, height

        integer WHITE
        parameter (WHITE = 7)

        call ads_dimensions_tile( hdlg, key, width, height )
        call ads_start_image( hdlg, key )
        call ads_fill_image( 0, 0, width, height, color )

        if( borderflag )then
            ! Put border around color
            call dlg_rect( 0, 0, width, height, WHITE )
        endif
        call ads_end_image()

        end


! DLG_RECT -- Draw a rectangle in an image tile.  Use tile dimensions
!             to draw border around tile.  Assumes start_image
!             has been called. */

        subroutine dlg_rect( x, y, width, height, color )
        integer*2 x, y, width, height, color

        include 'adslib.fi'
        include 'adsdlg.fi'

        integer*2 x2, y2

        x2 = x + width - 1
        y2 = y + height - 1
        call ads_vector_image( x,  y,  x,  y2, color )
        call ads_vector_image( x,  y2, x2, y2, color )
        call ads_vector_image( x2, y2, x2, y,  color )
        call ads_vector_image( x2, y,  x,  y,  color )

        end
