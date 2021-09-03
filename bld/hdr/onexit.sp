:: MS specific onexit
::
typedef void (_WCCALLBACK *_onexit_t)( void );
_WCRTLINK extern _onexit_t  _onexit( _onexit_t __func );
:: deprecated version
typedef void (_WCCALLBACK *onexit_t)( void );
_WCRTLINK extern onexit_t   onexit( onexit_t __func );
