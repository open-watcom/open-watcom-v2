#define pick( a, b ) \
	extern JNINativeMethod b ## _Natives[]; \
	extern VMuint Number_ ## b ## _Natives; \

pick( "java/lang/Class", java_lang_Class )
pick( "java/lang/ClassLoader", java_lang_ClassLoader )
pick( "java/lang/Compiler", java_lang_Compiler )
pick( "java/lang/Double", java_lang_Double )
pick( "java/lang/Float", java_lang_Float )
pick( "java/lang/Math", java_lang_Math )
pick( "java/lang/Object", java_lang_Object )
pick( "java/lang/Runtime", java_lang_Runtime )
pick( "java/lang/SecurityManager", java_lang_SecurityManager )
pick( "java/lang/String", java_lang_String )
pick( "java/lang/System", java_lang_System )
pick( "java/lang/Thread", java_lang_Thread )
pick( "java/lang/Throwable", java_lang_Throwable )
pick( "java/lang/reflect/Array", java_lang_reflect_Array )
pick( "java/lang/reflect/Constructor", java_lang_reflect_Constructor )
pick( "java/lang/reflect/Field", java_lang_reflect_Field )
pick( "java/lang/reflect/Method", java_lang_reflect_Method )
#undef pick
