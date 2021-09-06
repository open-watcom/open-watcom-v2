:: MS onexit function
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
typedef void (_WCCALLBACK *_onexit_t)( void );
_WCRTLINK extern _onexit_t  _onexit( _onexit_t __func );
:elsesegment
:: MS deprecated
typedef void (_WCCALLBACK *onexit_t)( void );
_WCRTLINK extern onexit_t   onexit( onexit_t __func );
:endsegment
