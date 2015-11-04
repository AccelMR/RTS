/********************************************************************
	Created:	2014/02/01
	Filename:	geLogger.cpp
	Author:		Samuel Prince

	Purpose:	Implementaci�n de la clase y objetos de logging
				y Debug
*********************************************************************/

/************************************************************************************************************************/
/* Incluimos los archivos necesarios para la compilaci�n                												*/
/************************************************************************************************************************/
#pragma region Includes y Namespaces
#include "geCoreStd.h"
#include "Debugging/geLogger.h"

#include "Multicore/CriticalSection.h"
#include "Utilities/String.h"

#pragma endregion

#pragma region Constantes, Est�ticos y Globales
/************************************************************************************************************************/
/* Definici�n de Constantes                                             												*/
/************************************************************************************************************************/
static const TCHAR* ERRORLOG_FILENAME = TEXT("error.log");		//Nombre del archivo de Log

/************************************************************************************************************************/
/* Banderas de visualizaci�n Default                                    												*/
/************************************************************************************************************************/
#ifdef _DEBUG	//Si estamos en modo de Debug
	const unsigned char ERRORFLAG_DEFAULT =		LOGFLAG_WRITE_TO_DEBUGGER;	//Mostramos los errores en el debugger
	const unsigned char WARNINGFLAG_DEFAULT =	LOGFLAG_WRITE_TO_DEBUGGER;	//Mostramos las advertencias en el debugger
	const unsigned char LOGFLAG_DEFAULT =		LOGFLAG_WRITE_TO_DEBUGGER;	//Mostramos los mensajes de log en el debugger
#elif NDEBUG	//Estamos en modo Release con funci�n de debug
	const unsigned char ERRORFLAG_DEFAULT =		(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);	//Mostramos errores en el debugger y el archivo de log
	const unsigned char WARNINGFLAG_DEFAULT =	(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);	//Mostramos advertencias en el debugger y el archivo de log
	const unsigned char LOGFLAG_DEFAULT =		(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);	//Mostramos mensajes de Log en el debugger y el archivo de log
#else	//Estamos en modo Release
	const unsigned char ERRORFLAG_DEFAULT =		0;	//No mostramos errores
	const unsigned char WARNINGFLAG_DEFAULT =	0;	//No mostramos advertencias
	const unsigned char LOGFLAG_DEFAULT =		0;	//No mostramos mensajes de Log
#endif

/************************************************************************************************************************/
/* Creamos un Singleton para el manejador de Log                        												*/
/************************************************************************************************************************/
class LogMgr;
static LogMgr* s_pLogMgr = NULL;

#pragma endregion

#pragma region Declaraci�n de la Clase LogMgr
/************************************************************************************************************************/
/* Clase LogMgr.- Esta clase es el manejador general de todo el sistema de logging										*/
/************************************************************************************************************************/
class LogMgr
{
	//Declaraci�n de variables y tipos de la clase
public:
	enum ErrorDialogResult
	{//Enumeraci�n constante de los posibles resultados del di�logo de errores
		LOGMGR_ERROR_ABORT,
		LOGMGR_ERROR_RETRY,
		LOGMGR_ERROR_IGNORE
	};

	//Tipos definidos
	typedef std::map<geString, uint8> Tags;
	typedef std::list<geLogger::ErrorMessenger*> ErrorMessengerList;

	Tags m_tags;							//Tags definidos para la clasificaci�n de mensajes al Log
	ErrorMessengerList m_errorMessengers;	//Lista de Mensajeros de error (se utiliza para dar capacidades de Multithread al Logger)

	//Seguridad de Thread
	CriticalSection m_tagCriticalSection;		//Secci�n cr�tica para el uso de Tags
	CriticalSection m_messengerCriticalSection;	//Secci�n cr�tica para la mensajer�a de errores

public:
	//Declaraci�n de funciones
	LogMgr(void);	//Constructor
	~LogMgr(void);	//Destructor
	
	//Funcion de inicializaci�n
	void Init(const TCHAR* loggingConfigFilename);	

	//Funciones de Log
	void Log(const geString& tag, const geString& message, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum);	//Esta funci�n manda un mensaje de Log
	void SetDisplayFlags(const geString& tag, uint8 flags);																			//Establece las banderas de visualizaci�n para un Tag espec�fico

	//Funciones para el mensajero de errores
	void AddErrorMessenger(geLogger::ErrorMessenger* pMessenger);	//Agrega un mensajero de error a la lista
	LogMgr::ErrorDialogResult Error(const geString& errorMessage, bool isFatal, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum);	//Muestra un mensaje de error al usuario

private:
	//Funciones de ayuda y utilidades
	void OutputFinalBufferToLogs(const geString& finalBuffer, uint8 flags);
	void WriteToLogFile(const geString& data);
	void GetOutputBuffer(geString& outOutputBuffer, const geString& tag, const geString& message, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum);
};
#pragma endregion

#pragma region Definici�n de la Clase LogMgr

/************************************************************************************************************************/
/* Constructor                                                          												*/
/************************************************************************************************************************/
LogMgr::LogMgr(void)
{
	//Establecemos las banderas de visualizaci�n Default a los Tags m�nimos
	SetDisplayFlags(TEXT("ERROR"),		ERRORFLAG_DEFAULT);
	SetDisplayFlags(TEXT("WARNING"),	WARNINGFLAG_DEFAULT);
	SetDisplayFlags(TEXT("INFO"),		LOGFLAG_DEFAULT);
}

/************************************************************************************************************************/
/* Destructor                                                           												*/
/************************************************************************************************************************/
LogMgr::~LogMgr(void)
{
	//Bloqueamos la secci�n cr�tica de mensajer�a
	m_messengerCriticalSection.Lock();

	//Eliminamos de memoria todos los mensajeros y vaciamos la lista
	for(auto it=m_errorMessengers.begin(); it!=m_errorMessengers.end(); ++it)
	{
		geLogger::ErrorMessenger* pMessenger = (*it);
		delete pMessenger;
	}
	m_errorMessengers.clear();

	//Liveramos la secci�n cr�tica
	m_messengerCriticalSection.Unlock();
}

/************************************************************************************************************************/
/* Inicializamos el manejador de Logs                                   												*/
/*																														*/
/* Parametros:																											*/
/*     loggingConfigFilename: Nombre del archivo de configuraci�n. Si este par�metro es NULL, iniciar� en modo default	*/
/************************************************************************************************************************/
void LogMgr::Init(const TCHAR* loggingConfigFilename)
{
	if( loggingConfigFilename )
	{//Se envi� un par�metro indicando un archivo de configuraci�n
		//Cargamos este archivo como un XML
#if PLATFORM_TCHAR_IS_1_BYTE == 1
	TiXmlDocument loggingConfigFile(loggingConfigFilename);
#else
	TiXmlDocument loggingConfigFile(  ws2s(loggingConfigFilename).c_str() );
#endif // PLATFORM_TCHAR_IS_1_BYTE == 1
		if( loggingConfigFile.LoadFile() )
		{//El archivo se carg� con �xito
			//Leemos el elemento root
			TiXmlElement* pRoot = loggingConfigFile.RootElement();
			if( !pRoot )
			{//Fall� al cargar el elemento raiz
				return;	//Se ignora la siguiente parte y se utilizar�n las opciones Default
			}

			//Iteramos a trav�s de todos los elementos hijos
			for(TiXmlElement* pNode=pRoot->FirstChildElement(); pNode; pNode=pNode->NextSiblingElement())
			{
				//Creamo un tag con el nombre del elemento
				uint8 flags = 0;
				std::string tag( pNode->Attribute("tag") );

				//Leemos el atributo de debug de este Tag
				int debugger = 0;
				pNode->Attribute("debugger", &debugger);
				if( debugger )
				{//Si este tag requiere que se escriba a la consola de Debug, establecemos la bandera
					flags |= LOGFLAG_WRITE_TO_DEBUGGER;
				}

				//Leemos el atributo de file de este Tag
				int logfile = 0;
				pNode->Attribute("file", &logfile);
				if( logfile )
				{//Si este tag requiere que se escriba al archivo de log, establecemos la bandera
					flags |= LOGFLAG_WRITE_TO_LOG_FILE;
				}

				//Agregamos este tag y las banderas a la lista
#if PLATFORM_TCHAR_IS_1_BYTE == 1
				SetDisplayFlags(tag, flags);
#else
				SetDisplayFlags(s2ws(tag), flags);
#endif // PLATFORM_TCHAR_IS_1_BYTE == 1
			}
		}
	}
}

/************************************************************************************************************************/
/* Esta funci�n construye el string de log y lo saca a diversos lugares basado en las banderas de visualizaci�n			*/
/*																														*/
/* Parametros:																											*/
/*     tag: Nombre del Tag al que corresponder� este mensaje															*/
/*     message: Mensaje que se escribir� en el Log																		*/
/*     funcName: Nombre de la funci�n que llam� esta entrada en el Log													*/
/*     sourceFile: Archivo de c�digo fuente desde el que se envi� esta entrada en el Log								*/
/*     lineNum: N�mero de l�nea de c�digo desde la que se envi� esta entrada en el Log									*/
/************************************************************************************************************************/
void LogMgr::Log(const geString& tag, const geString& message, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum)
{
	//Bloqueamos la secci�n cr�tica de los Tags
	m_tagCriticalSection.Lock();

	//Buscamos el objeto de Tag en nuestro mapa
	Tags::iterator findIt = m_tags.find(tag);
	if( findIt != m_tags.end() )
	{//Encontramos el Tag
		//Liberamos la secci�n cr�tica
		m_tagCriticalSection.Unlock();

		//Generamos el mensaje final
		geString buffer;
		GetOutputBuffer(buffer, tag, message, funcName, sourceFile, lineNum);

		//Desplegamos el mensaje seg�n donde se requiera
		OutputFinalBufferToLogs(buffer, findIt->second);
	}
	else
	{//El tag pedido no existe
		//La secci�n cr�tica fue liberada en la declaraci�n anterior, as� que necesitamos liberarla aqu� ya que esa secci�n no fue ejecutada
		m_tagCriticalSection.Unlock();
	}
}

/************************************************************************************************************************/
/* Establece una o m�s banderas de visualizaci�n																		*/
/*																														*/
/* Parametros:																											*/
/*     tag: Nombre del Tag al que se le asignar�n las banderas (si no existe se crear� uno nuevo)						*/
/*     flags: Banderas que se asignar�n al tag																			*/
/************************************************************************************************************************/
void LogMgr::SetDisplayFlags(const geString& tag, uint8 flags)
{
	m_tagCriticalSection.Lock();
	if( flags != 0 )
	{
		Tags::iterator findIt = m_tags.find(tag);
		if (findIt == m_tags.end())
		{
			m_tags.insert(std::make_pair(tag, flags));
		}
		else
		{
			findIt->second = flags;
		}
	}
	else
	{
		m_tags.erase(tag);
	}
	m_tagCriticalSection.Unlock();
}

/************************************************************************************************************************/
/* Agrega un mensajero de error a la lista																				*/
/*																														*/
/* Parametros:																											*/
/*     pMessenger: Mensajero que ser� agregado																			*/
/************************************************************************************************************************/
void LogMgr::AddErrorMessenger(geLogger::ErrorMessenger* pMessenger)
{
	m_messengerCriticalSection.Lock();
	m_errorMessengers.push_back(pMessenger);
	m_messengerCriticalSection.Unlock();
}

/************************************************************************************************************************/
/* Funci�n de ayuda para el mensajero de errores																		*/
/*																														*/
/* Parametros:																											*/
/*     errorMessage: Mensaje de error a mostrar																			*/
/*     isFatal: Indica si se cometi� un error fatal																		*/
/*     funcName: Nombre de la funci�n que llam� esta entrada en el Log													*/
/*     sourceFile: Archivo de c�digo fuente desde el que se envi� esta entrada en el Log								*/
/*     lineNum: N�mero de l�nea de c�digo desde la que se envi� esta entrada en el Log									*/
/************************************************************************************************************************/
LogMgr::ErrorDialogResult LogMgr::Error(const geString& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	//Creamos variables temporales
	geString tag = ((isFatal) ? TEXT("FATAL") : TEXT("ERROR"));	//Elige el tag al que se har� el Logging dependiendo de la severidad del mensaje
	geString buffer;											//Buffer para nuestro string final de salida

	//Obtenemos el mensaje final a mostrar
	GetOutputBuffer(buffer, tag, errorMessage, funcName, sourceFile, lineNum);

	//Escribimos el mensaje a los varios logs que tengamos asociados
	m_tagCriticalSection.Lock();	//Bloqueamos la secci�n cr�tica de tags
	Tags::iterator findIt = m_tags.find(tag);
	if( findIt != m_tags.end() )
	{//Si encontramos el tag pedido, escribimos a los logs
		OutputFinalBufferToLogs(buffer, findIt->second);
	}
	m_tagCriticalSection.Unlock();	//Liberamos la secci�n cr�tica

	//Mostramos el cuadro de di�logo del error al usuario para elegir una acci�n a tomar
	int result = ::MessageBox(NULL, buffer.c_str(), tag.c_str(), MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_DEFBUTTON3);

	//Actuamos seg�n la desici�n que se haya tomado
	switch( result )
	{
		case IDIGNORE :	return LogMgr::LOGMGR_ERROR_IGNORE;					//Ignoramos el error y cualquier futura referencia del mismo durante esta ejecuci�n
		case IDABORT  :	__debugbreak(); return LogMgr::LOGMGR_ERROR_RETRY;	//Instrucci�n en lenguaje ensamblador para hacer un break en el debugger
		case IDRETRY :	return LogMgr::LOGMGR_ERROR_RETRY;					//Reintentamos la ejecuci�n
		default :		return LogMgr::LOGMGR_ERROR_RETRY;					//Por default reintentamos la ejecuci�n
	}
}

/************************************************************************************************************************/
/* Esta es una funci�n de ayuda que revisa todas las banderas de visualizaci�n y despliega el finalBuffer pasado a los	*/
/* lugares apropiados																									*/
/*																														*/
/* Parametros:																											*/
/*     finalBuffer: String que ser� escrito en los posibles dispositivos de salida especificados por las banderas		*/
/*     flags: Banderas que indican cuales son los dispositivos a los que se debe escribir el mensaje del buffer			*/
/************************************************************************************************************************/
void LogMgr::OutputFinalBufferToLogs(const geString& finalBuffer, uint8 flags)
{
	//Revisamos si debemos escribir al archivo
	if( (flags & LOGFLAG_WRITE_TO_LOG_FILE) > 0 )
	{//Escribimos en el archivo
		WriteToLogFile(finalBuffer);
	}
	
	//Revisamos si debemos escribir a la ventana de salida de Debug
	if( (flags & LOGFLAG_WRITE_TO_DEBUGGER) > 0 )
	{//Escribimos a la consola de Debug
		::OutputDebugString(finalBuffer.c_str());
	}
}

/************************************************************************************************************************/
/* Esta es una funci�n de ayuda que escribe el string data al archivo de log											*/
/*																														*/
/* Parametros:																											*/
/*     data: string que se escribir� en el archivo																		*/
/************************************************************************************************************************/
void LogMgr::WriteToLogFile(const geString& data)
{
	//Abrimos el archivo de Log
	FILE* pLogFile = NULL;
	_tfopen_s(&pLogFile, ERRORLOG_FILENAME, TEXT("a+"));
	
	if( !pLogFile )
	{//No se puede escribir en el archivo de log por alguna raz�n
		return;
	}

	//Escribimos la informaci�n
	_ftprintf_s(pLogFile, data.c_str());
	
	//Cerramos el archivo
	fclose(pLogFile);
}

/************************************************************************************************************************/
/* Rellena outOutputBuffer con un string de Log formateado que contiene toda la informaci�n requerida					*/
/*																														*/
/* Parametros:																											*/
/*     outOutputBuffer: buffer en el que se escribir�n los datos														*/
/*     tag: Nombre del Tag al que corresponder� este mensaje															*/
/*     message: Mensaje que se escribir� en el Log																		*/
/*     funcName: Nombre de la funci�n que llam� esta entrada en el Log													*/
/*     sourceFile: Archivo de c�digo fuente desde el que se envi� esta entrada en el Log								*/
/*     lineNum: N�mero de l�nea de c�digo desde la que se envi� esta entrada en el Log									*/
/************************************************************************************************************************/
void LogMgr::GetOutputBuffer(geString& outOutputBuffer, const geString& tag, const geString& message, const ANSICHAR* funcName, const ANSICHAR* sourceFile, uint32 lineNum)
{
	//Transformamos los nombres ANSI a versiones compatibles con el sistema
	TCHAR t_funcName[MAX_PATH];
	TCHAR t_sourceFile[MAX_PATH];
	if( AnsiToGenericCch(t_funcName, funcName, MAX_PATH) == E_INVALIDARG )
	{
		t_funcName[0] = '\0';
	}
	if( AnsiToGenericCch(t_sourceFile, sourceFile, MAX_PATH) == E_INVALIDARG )
	{
		t_sourceFile[0] = '\0';
	}

	//Si estamos utilizando el sistema de tags
	if( !tag.empty() )
	{//Agregamos el tag al inicio y despues el mensaje
		outOutputBuffer = TEXT("[")+tag+TEXT("] ")+message;
	}
	else
	{//No estamos utilizando tags, as� que solo agregamos el mensaje
		outOutputBuffer = message;
	}

	//Revisamos si se envi� el nombre de una funci�n
	if( funcName != NULL )
	{//Se envi� el nombre de funci�n, agregamos en una nueva l�nea el dato
		outOutputBuffer += LINE_TERMINATOR;
		outOutputBuffer += TEXT("Function: ");
		outOutputBuffer += t_funcName;
	}

	//Revisamos si se envi� el nombre del archivo de c�digo fuente
	if( sourceFile != NULL )
	{//Si se envi�, as� que agregamos una nueva l�nea que contenga esta informaci�n
		outOutputBuffer += LINE_TERMINATOR;
		outOutputBuffer += t_sourceFile;
	}

	//Revisamos si se envi� el n�mero de l�nea de c�digo
	if( lineNum != 0 )
	{//Se envi� la l�nea, agregamos la informaci�n en el buffer de datos
		outOutputBuffer += LINE_TERMINATOR;
		outOutputBuffer += TEXT("Line: ");
		outOutputBuffer += ToStr(lineNum);
	}

	//Al final agregamos un break de l�nea
	outOutputBuffer += LINE_TERMINATOR;
}

#pragma endregion

#pragma region Definici�n de la Clase ErrorMessenger
/************************************************************************************************************************/
/* Constructor		                                                    												*/
/************************************************************************************************************************/
geLogger::ErrorMessenger::ErrorMessenger(void)
{
	//Agregamos este mensajero al manager de log
	s_pLogMgr->AddErrorMessenger(this);
	m_enabled = true;	//Marcamos nuestra bandera de activo
}

/************************************************************************************************************************/
/* Esta funci�n pide al manager que muestre el mensaje de error al usuario												*/
/*																														*/
/* Parametros:																											*/
/*     errorMessage: Mensaje de error a mostrar																			*/
/*     isFatal: Indica si se cometi� un error fatal																		*/
/*     funcName: Nombre de la funci�n que llam� esta entrada en el Log													*/
/*     sourceFile: Archivo de c�digo fuente desde el que se envi� esta entrada en el Log								*/
/*     lineNum: N�mero de l�nea de c�digo desde la que se envi� esta entrada en el Log									*/
/************************************************************************************************************************/
void geLogger::ErrorMessenger::Show(const geString& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	//Si este mensajero est� activo
	if (m_enabled)
	{//Muestra el mensaje de error
		if( s_pLogMgr->Error(errorMessage, isFatal, funcName, sourceFile, lineNum) == LogMgr::LOGMGR_ERROR_IGNORE )
		{//Si el usuario pidi� que se ignorara este error
			m_enabled = false;	//Desactivamos el mensaje en futuras instancias
		}
	}
}
#pragma endregion

/************************************************************************************************************************/
/* Interface C                                                             												*/
/************************************************************************************************************************/
#pragma region Interface C
namespace geLogger
{
	/************************************************************************************************************************/
	/* Inicializa el Singleton y el Manager con el archivo de configuraci�n													*/
	/*																														*/
	/* Parametros:																											*/
	/*     loggingConfigFilename: Nombre del archivo de configuraci�n. Si este par�metro es NULL, iniciar� en modo default	*/
	/************************************************************************************************************************/
	void Init(const TCHAR* loggingConfigFilename)
	{
		if( !s_pLogMgr )
		{
			s_pLogMgr = GEE_NEW LogMgr;
			s_pLogMgr->Init(loggingConfigFilename);
		}
	}

	/************************************************************************************************************************/
	/* Destruye el manager y limpia su memoria																				*/
	/*																														*/
	/* Parametros:																											*/
	/*     nameParam: DescParam																								*/
	/************************************************************************************************************************/
	void Destroy(void)
	{
		SAFE_DELETE(s_pLogMgr);
	}

	/************************************************************************************************************************/
	/* Esta funci�n construye el string de log y lo saca a diversos lugares basado en las banderas de visualizaci�n			*/
	/*																														*/
	/* Parametros:																											*/
	/*     tag: Nombre del Tag al que corresponder� este mensaje															*/
	/*     message: Mensaje que se escribir� en el Log																		*/
	/*     funcName: Nombre de la funci�n que llam� esta entrada en el Log													*/
	/*     sourceFile: Archivo de c�digo fuente desde el que se envi� esta entrada en el Log								*/
	/*     lineNum: N�mero de l�nea de c�digo desde la que se envi� esta entrada en el Log									*/
	/************************************************************************************************************************/
	void Log(const geString& tag, const geString& message, const char* funcName, const char* sourceFile, unsigned int lineNum)
	{
		GEE_ASSERT(s_pLogMgr);
		s_pLogMgr->Log(tag, message, funcName, sourceFile, lineNum);
	}

	/************************************************************************************************************************/
	/* Establece una o m�s banderas de visualizaci�n																		*/
	/*																														*/
	/* Parametros:																											*/
	/*     tag: Nombre del Tag al que se le asignar�n las banderas (si no existe se crear� uno nuevo)						*/
	/*     flags: Banderas que se asignar�n al tag																			*/
	/************************************************************************************************************************/
	void SetDisplayFlags(const geString& tag, uint8 flags)
	{
		GEE_ASSERT(s_pLogMgr);
		s_pLogMgr->SetDisplayFlags(tag, flags);
	}
}//end namespace geLogger
#pragma endregion
