/********************************************************************
	Created:	2014/02/01
	Filename:	PlatformTypes.h
	Author:		Samuel Prince

	Purpose:	Define los tipos de variables para la plataforma.
				En este caso la plataforma es Windows, pero pueden
				tenerse diferentes plataformas y as� reducimos el 
				tiempo de porting
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Tipos unsigned b�sicos                                               												*/
/************************************************************************************************************************/
typedef unsigned char 		uint8;		//8-bit  unsigned.
typedef unsigned short int	uint16;		//16-bit unsigned.
typedef unsigned int		uint32;		//32-bit unsigned.
typedef unsigned long long	uint64;		//64-bit unsigned.

/************************************************************************************************************************/
/* Tipos signed b�sicos                                               													*/
/************************************************************************************************************************/
typedef	signed char			int8;		//8-bit  signed.
typedef signed short int	int16;		//16-bit signed.
typedef signed int	 		int32;		//32-bit signed.
typedef signed long long	int64;		//64-bit signed.

/************************************************************************************************************************/
/* Tipos de caracteres                                                  												*/
/************************************************************************************************************************/
typedef char				ANSICHAR;	//Un caracter ANSI character. Normalmente un tipo signed
typedef wchar_t				UNICHAR;	//Un caracter UNICODE. Normalmente un tipo unsigned

//Tipo de definici�n de NULL
typedef int32				TYPE_OF_NULL;

//Definimos el tama�o del tipo SIZE_T dependiendo de la arquitectura para la que estemos compilando
#ifdef _WIN64
	typedef unsigned __int64	SIZE_T;
#else
	typedef unsigned long		SIZE_T;
#endif

/************************************************************************************************************************/
/* Defines utilizados para indicar las caracter�sticas de la plataforma espec�fica										*/
/************************************************************************************************************************/
#define PLATFORM_DESKTOP							1
#if defined( _WIN64 )
	#define PLATFORM_64BITS							1
#else
	#define PLATFORM_64BITS							0
#endif

#define PLATFORM_LITTLE_ENDIAN						1
#define PLATFORM_EXCEPTIONS_DISABLED				!PLATFORM_DESKTOP
#define PLATFORM_SUPPORTS_PRAGMA_PACK				1
#define PLATFORM_ENABLE_VECTORINTRINSICS			1
#define PLATFORM_USE_LS_SPEC_FOR_UNICODE			1
#define PLATFORM_USE_SYSTEM_VSWPRINTF				1
#define PLATFORM_TCHAR_IS_4_BYTES					0
#define PLATFORM_HAS_vsnprintf						0
#define PLATFORM_HAS_BSD_TIME						0
#define PLATFORM_USE_PTHREADS						0
#define PLATFORM_MAX_FILEPATH_LENGTH				MAX_PATH
#define PLATFORM_HAS_BSD_SOCKET_FEATURE_WINSOCKETS	1
#define PLATFORM_USES_MICROSOFT_LIBC_FUNCTIONS		1
#define PLATFORM_SUPPORTS_TBB						1
#define PLATFORM_SUPPORTS_NAMED_PIPES				1

/************************************************************************************************************************/
/* Modificadores de m�todos                                             												*/
/************************************************************************************************************************/
#pragma warning(disable : 4481)	//Extensi�n no-standard utilizada: override specifier 'override'
#ifndef OVERRIDE						
	#define OVERRIDE	override
#endif
#ifndef FINAL						
	#define FINAL		sealed
#endif
#ifndef ABSTRACT						
	#define ABSTRACT	abstract
#endif
#ifndef CONSTEXPR
	#define CONSTEXPR	
#endif

/************************************************************************************************************************/
/* Macros de alineaci�n de memoria                                      												*/
/************************************************************************************************************************/
#define MS_ALIGN(n)	__declspec(align(n))		//Macro de alineaci�n compatible con el compilador de Microsoft
#ifndef GCC_PACK								//Pack en estilo GCC (no utilizado en compiladores Microsoft)
	#define GCC_PACK(n)
#endif
#ifndef GCC_ALIGN								//Align en estilo GCC (no utilizado en compiladores Microsoft)
	#define GCC_ALIGN(n)
#endif

#define MSVC_PRAGMA(Pragma) __pragma(Pragma)	//Pragma estilo microsoft, definido para que otros compiladores puedan quitarlo

/************************************************************************************************************************/
/* Macros de tipos de funci�n                                                                     						*/
/************************************************************************************************************************/
#ifndef VARARGS
	#define VARARGS			__cdecl					//Funciones con argumentos variables
#endif
#ifndef CDECL
	#define CDECL			__cdecl					//Funciones en Standard C
#endif
#ifndef STDCALL
	#define STDCALL			__stdcall				//Convenci�n de llamada Standard
#endif
#ifndef FORCEINLINE
	#define FORCEINLINE		__forceinline			//Forza a que el c�digo est� inline
#endif
#ifndef FORCENOINLINE
	#define FORCENOINLINE	__declspec(noinline)	//Forza a que el c�digo NO est� inline
#endif
#ifndef RESTRICT
	#define RESTRICT		__restrict				//Restrict indica al compilador que un simbolo no tiene alias en el ambito actual
#endif


/************************************************************************************************************************/
/* Macros de optimizaci�n                                                 												*/
/************************************************************************************************************************/
#define ASSUME(expr) __assume(expr)				//Le indica al compilador que la expresi�n es true; generalmente restringido a comparaci�n contra constantes
#define DECLARE_UINT64(x)	x					//En sistemas windows podemos definir directamente una variables de 64 bits
#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL __pragma(optimize("",off))	//Usa __pragma para activar dentro de un #define
#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL  __pragma(optimize("",on))	//Usa __pragma para activar dentro de un #define
#define TYPENAME_OUTSIDE_TEMPLATE				//Todos los compiladores soportan esto excepto microsoft, pero lo har�n pronto

/************************************************************************************************************************/
/* Constantes de strings                                                												*/
/************************************************************************************************************************/
#ifdef UNICODE		//Si estamos utilizando UNICODE
	#define PLATFORM_TCHAR_IS_1_BYTE 0			//Indicamos que los caracteres en esta plataforma son de m�s de 1 byte
	typedef wchar_t	TCHAR;						//Definimos el tipo TCHAR como un widechar
	typedef std::wstring geString;				//Definimos el tipo geString
#else	//Estamos compilando en Multi-byte
	#define PLATFORM_TCHAR_IS_1_BYTE 1			//Indicamos que los caracteres en esta plataforma son de 1 byte
	typedef char TCHAR;							//Definimos el tipo TCHAR como un char
	typedef std::string geString;				//Definimos el tipo geString
#endif	// UNICODE

/************************************************************************************************************************/
/* Macro para el manejo de texto                                        												*/
/************************************************************************************************************************/
#if !defined(TEXT)
	#if PLATFORM_TCHAR_IS_1_BYTE
		#define TEXT(s) s
	#else
		#define TEXT(s) L##s
	#endif
#endif

/************************************************************************************************************************/
/* Constantes de texto                                                  												*/
/************************************************************************************************************************/
#ifndef LINE_TERMINATOR						
	#define LINE_TERMINATOR TEXT("\r\n")
#endif

#ifndef FOLDER_SLASH
	#define FOLDER_SLASH TEXT("\\")
#endif

/************************************************************************************************************************/
/* Define de ayuda para declaraci�n de macros con multiples l�neas de c�digo											*/
/************************************************************************************************************************/
#define MULTI_LINE_MACRO_BEGIN do{
#define MULTI_LINE_MACRO_END \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	} while(0) \
	__pragma(warning(pop))

//Definimos este macro para revisar en tiempo de compilaci�n algunos detalles
#define checkAtCompileTime(expr, msg) static_assert( expr, #msg )

/************************************************************************************************************************/
/* Revisamos que los tipos sean del tama�o esperado en tiempo de compilaci�n											*/
/************************************************************************************************************************/
checkAtCompileTime(char(-1) < char(0),TypeTests_char_unsigned);

checkAtCompileTime(sizeof(uint8) == 1,TypeTests_BYTE_size);
checkAtCompileTime(int32(uint8(-1)) == 0xFF,TypeTests_BYTE_sign);

checkAtCompileTime(sizeof(uint16) == 2,TypeTests_WORD_size);
checkAtCompileTime(int32(uint16(-1)) == 0xFFFF,TypeTests_WORD_sign);

checkAtCompileTime(sizeof(uint32) == 4,TypeTests_DWORD_size);
checkAtCompileTime(int64(uint32(-1)) == int64(0xFFFFFFFF),TypeTests_DWORD_sign);

checkAtCompileTime(sizeof(uint64) == 8,TypeTests_QWORD_size);
checkAtCompileTime(uint64(-1) > uint64(0),TypeTests_QWORD_sign);

checkAtCompileTime(sizeof(int8) == 1,TypeTests_SBYTE_size);
checkAtCompileTime(int32(int8(-1)) == -1,TypeTests_SBYTE_sign);

checkAtCompileTime(sizeof(int16) == 2,TypeTests_SWORD_size);
checkAtCompileTime(int32(int16(-1)) == -1,TypeTests_SWORD_sign);

checkAtCompileTime(sizeof(int32) == 4,TypeTests_INT_size);
checkAtCompileTime(int64(int32(-1)) == int64(-1),TypeTests_INT_sign);

checkAtCompileTime(sizeof(int64) == 8,TypeTests_SQWORD_size);
checkAtCompileTime(int64(-1) < int64(0),TypeTests_SQWORD_sign);

checkAtCompileTime(sizeof(ANSICHAR) == 1,TypeTests_ANSICHAR_size);
checkAtCompileTime(int32(ANSICHAR(-1)) == -1,TypeTests_ANSICHAR_sign);

checkAtCompileTime(sizeof(UNICHAR) == 2,TypeTests_UNICHAR_size);
checkAtCompileTime(int32(UNICHAR(-1)) > 0,TypeTests_UNICHAR_sign);

checkAtCompileTime(sizeof(uint32) == 4,TypeTests_BITFIELD_size);
checkAtCompileTime(int64(uint32(-1)) == int64(0xFFFFFFFF),TypeTests_BITFIELD_sign);

checkAtCompileTime(sizeof(SIZE_T) == sizeof(void *),TypeTests_SIZE_T_size);
checkAtCompileTime(SIZE_T(-1) > SIZE_T(0),TypeTests_SIZE_T_sign);