#ifndef wobject_class
#define wobject_class

#define TRUE 1
#define FALSE 0
#define NIL 0

#define breakif(x)              if(x)break
#define whileptr(x)     while((x)!=NIL)
#define ifptr(x)        if((x)!=NIL)
#define isptr(x)        ((x)!=NIL)

#define whilenil(x)     while((x)==NIL)
#define ifnil(x)        if((x)==NIL)
#define isnil(x)        ((x)==NIL)

#define bool xbool
typedef char bool;
#define WFAR far
#define WEXPORT

#ifdef __DLL__
#define WCLASS class _export
#else
#ifdef DLL
#define WCLASS class huge
#else
#define WCLASS class
#endif
#endif

#pragma warning 849 10

WCLASS WObject
{
        public:
                WEXPORT WObject() {}
                virtual WEXPORT ~WObject() {}
                virtual bool WEXPORT operator==( WObject& obj ) { return isEqual( &obj ); }
                virtual bool WEXPORT operator==( char* ) { return FALSE; }
                virtual bool WEXPORT isEqual( WObject* ) { return FALSE; }
};

typedef void (WObject::*cb)();
typedef bool (WObject::*bcb)();
typedef void (WObject::*cbi)( int i );
typedef void (WObject::*cbo)( WObject* o );
typedef void (WObject::*cbc)( char* c );

#endif //wobject_class




