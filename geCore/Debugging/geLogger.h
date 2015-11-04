/********************************************************************
	Created:	2014/01/31
	Filename:	geLogger.h
	Author:		Samuel Prince

	Purpose:	Definici�n de clases y objetos relacionados con
				el manejo de Loggeo y notificaci�n de Mensajes.
				En estas clases encontraremos lo necesario para
				hacer el log a un archivo o a la consola de debug
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Definimos seg�n la configuraci�n del sistema, el tipo de string que utilizaremos										*/
/************************************************************************************************************************/
#ifdef DEBUG

#endif // DEBUG


/************************************************************************************************************************/
/* Definici�n de banderas de visualizaci�n (donde se mostrar�n los logs)												*/
/************************************************************************************************************************/
const uint8 LOGFLAG_WRITE_TO_LOG_FILE =		1 << 0;		//Si esta bandera es establecida, el logger escribir� a un archivo
const uint8 LOGFLAG_WRITE_TO_DEBUGGER =		1 << 1;		//Si esta bandera es establecida, el logger escribir� a la consola de debug de Visual Studio

/************************************************************************************************************************/
/* Esta es la interface p�blica de geLogger. Debes llamar a Init() antes de que cualquier loggeo pueda llevarse a cabo	*/
/* y Destroy() cuando hayas terminado para hacer limpieza. Llama SetDisplayFlags() para establecer las banderas de		*/
/* visualizaci�n para un tag de logging en particular. Por default, todas est�n desactivadas. Aunque se puede,	no		*/
/* deber�a llamarse a Log directamente. Usa el macro GEE_LOG() en su lugar ya que puede ser eliminado por el			*/
/* compilador en construcciones de Release.																				*/
/************************************************************************************************************************/
namespace geLogger
{
	class ErrorMessenger
	{//NOTA.- Esta clase es utilizada por los macros de debug y no deber�a ser accedida externamente
		//Definici�n de variables miembro
	private:
		bool m_enabled;		//Indica que el objeto ya fue inicializado
		//Definici�n de Funciones
	public:
		ErrorMessenger(void);	//Constructor
		void Show(const geString& errorMessage, bool isFatal, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum);	//Muestra el error al usuario
	};

	//Funciones de contrucci�n / destrucci�n
	void Init(const TCHAR* loggingConfigFilename);	//Esta funci�n DEBE ser llamada al inicio del programa antes de utilizar el logger
	void Destroy(void);								//Esta funci�n debe llamarse cuando quiera destruirse el objeto, normalmente al salir del programa

	//Funciones de Loggeo
	void Log(const geString& tag, const geString& message, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum);	//Env�a un mensaje al log
	void SetDisplayFlags(const geString& tag, uint8 flags);	//Establece las banderas de visualizaci�n para un TAG espec�fico
}

/************************************************************************************************************************/
/* Macros de Debug																										*/
/************************************************************************************************************************/

/************************************************************************************************************************/
/* Los errores fatales obviamente son fatales y siempre se le presentan	al usuario										*/
/************************************************************************************************************************/
#define GEE_FATAL(str) \
	MULTI_LINE_MACRO_BEGIN \
		static geLogger::ErrorMessenger* pErrorMessenger = GEE_NEW geLogger::ErrorMessenger; \
		geString s((str)); \
		pErrorMessenger->Show(s, true, __FUNCTION__, __FILE__, __LINE__); \
	MULTI_LINE_MACRO_END

#if defined(_DEBUG)	//Si estamos construyendo en modo de DEBUG
/************************************************************************************************************************/
/* Los errores son malos y potencialmente fatales. Estos son presentados con un di�logo con opciones de Abort, Retry e	*/
/* Ignore. Abort har� un break en el debugger, Retry continuar� con la ejecuci�n e Ignore continuar� la ejecuci�n e		*/
/* ignorar� cualquier subsecuente llamada a ese espec�fico error. Este Macro es completamente ignorado en modo Release	*/
/************************************************************************************************************************/
#define GEE_ERROR(str) \
	MULTI_LINE_MACRO_BEGIN \
		static geLogger::ErrorMessenger* pErrorMessenger = GEE_NEW geLogger::ErrorMessenger; \
		geString s((str)); \
		pErrorMessenger->Show(s, false, __FUNCTION__, __FILE__, __LINE__); \
	MULTI_LINE_MACRO_END

/************************************************************************************************************************/
/* Los Warnings son recuperables. Solo son logs con el tag "WARNING" que muestran la informaci�n de la llamada.			*/
/* Las banderas son inicialmente establecidas a WARNINGFLAG_DEFAULT (definido en geLogger.cpp), pero pueden ser			*/
/* sobreescritas normalmente.																							*/
/************************************************************************************************************************/
#define GEE_WARNING(str) \
	MULTI_LINE_MACRO_BEGIN \
		geString s((str)); \
		geLogger::Log(TEXT("WARNING"), s, __FUNCTION__, __FILE__, __LINE__); \
	MULTI_LINE_MACRO_END

/************************************************************************************************************************/
/* Este es solo un macro conveniente para hacer logging si no sientes que quieras lidiar con Tags. Este llama Log()	con	*/
/* un Tag "INFO". Las banderas son inicialmente establecidas a LOGFLAG_DEFAULT (definido en geLogger.cpp), pero pueden	*/
/* ser sobreescritas normalmente																						*/
/************************************************************************************************************************/
#define GEE_INFO(str) \
	MULTI_LINE_MACRO_BEGIN \
		geString s((str)); \
		geLogger::Log(TEXT("INFO"), s, NULL, NULL, 0); \
	MULTI_LINE_MACRO_END

/************************************************************************************************************************/
/* Este macro es utilizado para hacer logging y debe ser el m�todo preferido de "printf debugging". Puedes usar			*/
/* cualquier Tag que quieras, solo asegurate de activarlos en alg�n lado en tu inicializaci�n							*/
/************************************************************************************************************************/
#define GEE_LOG(tag, str) \
	MULTI_LINE_MACRO_BEGIN \
		geString s((str)); \
		geLogger::Log(tag, s, NULL, NULL, 0); \
	MULTI_LINE_MACRO_END

/************************************************************************************************************************/
/* Este macro reemplaza ASSERT()																						*/
/************************************************************************************************************************/
#define GEE_ASSERT(expr) \
	MULTI_LINE_MACRO_BEGIN \
		if (!(expr)) \
		{ \
			static geLogger::ErrorMessenger* pErrorMessenger = GEE_NEW geLogger::ErrorMessenger; \
			pErrorMessenger->Show(TEXT(#expr), false, __FUNCTION__, __FILE__, __LINE__); \
		} \
	MULTI_LINE_MACRO_END

#else	//Estamos construyendo en modo release

/************************************************************************************************************************/
/* Estas son las definiciones para el modo Release de los macros anteriores. Todos est�n definidos de tal manera que el	*/
/* compilador los ignorar�																								*/
/************************************************************************************************************************/
#define GEE_ERROR(str)		MULTI_LINE_MACRO_BEGIN (void)sizeof(str);MULTI_LINE_MACRO_END
#define GEE_WARNING(str)	MULTI_LINE_MACRO_BEGIN (void)sizeof(str);MULTI_LINE_MACRO_END
#define GEE_INFO(str)		MULTI_LINE_MACRO_BEGIN (void)sizeof(str);MULTI_LINE_MACRO_END
#define GEE_LOG(tag, str)	MULTI_LINE_MACRO_BEGIN (void)sizeof(tag); (void)sizeof(str);MULTI_LINE_MACRO_END
#define GEE_ASSERT(expr)	MULTI_LINE_MACRO_BEGIN (void)sizeof(expr);MULTI_LINE_MACRO_END

#endif  //#if defined(_DEBUG)