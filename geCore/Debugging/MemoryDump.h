/********************************************************************
	Created:	2014/08/16
	Filename:	MemoryDump.h
	Author:		Samuel Prince

	Purpose:	Clase de debug encargada de hacer un Dump de memoria
				compatible con el compilador utilizado para detectar
				fallas del programa e informarlas a desarrollo
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Declaraci�n de constantes utilizadas por las clases de este archivo													*/
/************************************************************************************************************************/
#define USER_DATA_BUFFER_SIZE 4096												//Tama�o del tama�o del buffer de datos de usuario

/************************************************************************************************************************/
/* Declaraci�n de la clase geMemoryDump																					*/
/************************************************************************************************************************/
class geMemoryDump
{
	/************************************************************************************************************************/
	/* Declaraci�n de variables protegidas																					*/
	/************************************************************************************************************************/
protected:
	static geMemoryDump* g_pDumper;												//Usaremos esta variable est�tica para generar acceso como singleton
	static long WINAPI Handler(struct _EXCEPTION_POINTERS *pExceptionInfo);		//Funci�n de manejo de escepci�n de memoria 

	bool m_bHeadless;															//Esta bandera indica si debe mostrarse un mensaje o solo hacer el dump sin preguntar
	time_t m_lTime;																//Informaci�n de tiempo en el que se genera este Dump
	_EXCEPTION_POINTERS *m_pExceptionInfo;										//Informaci�n de punteros de excepci�n

	TCHAR m_szDumpPath[MAX_PATH];												//Ruta donde se guardar� el archivo de dump
	TCHAR m_szAppPath[MAX_PATH];												//Ruta de la aplicaci�n
	TCHAR m_szAppBaseName[MAX_PATH];											//Nombre de base de la aplicaci�n
	
	/************************************************************************************************************************/
	/* Declaraci�n de funciones protegidas																					*/
	/************************************************************************************************************************/
protected:
	long WriteMemoryDump(_EXCEPTION_POINTERS *pExceptionInfo);					//Escribe el archivo de Dump de la aplicaci�n

	virtual void SetDumpFileName(void);											//Establece el nombre del archivo de Dump
	virtual MINIDUMP_USER_STREAM_INFORMATION* GetUserStreamArray()				//Funci�n virtual que regresa un stream de datos del ususario (por el momento no se utiliza)
	{
		return NULL;
	}
	virtual const TCHAR* GetUserMessage(){return TEXT("");}						//Obtiene el mensaje del usuario (la funci�n de windows puede preguntar por comentarios del usaurio)

	/************************************************************************************************************************/
	/* Declaraci�n de funciones p�blicas																					*/
	/************************************************************************************************************************/
public:
	geMemoryDump(bool headless);												//Constructor (el par�metro indica si en el caso de una escepci�n debe preguntar al usuario para hacer el dump)
};

/************************************************************************************************************************/
/* Declaraci�n de la clase geModuleEntry usada para dar informaci�n de un m�dulo en particular							*/
/************************************************************************************************************************/
class geModuleEntry
{
	/************************************************************************************************************************/
	/* Declaraci�n de variables protegidas																					*/
	/************************************************************************************************************************/
protected:
	MODULEENTRY32 m_me32;														//Informaci�n del m�dulo
	TCHAR* m_pFileVersionInfo;													//Puntero a la informaci�n de la versi�n del archivo
	TCHAR* m_pProductVersion;													//Puntero a la informaci�n de la versi�n del producto

	/************************************************************************************************************************/
	/* Declaraci�n de funciones protegidas																					*/
	/************************************************************************************************************************/
protected:
	void ReadFileVersionInfo();													//Funci�n que lee la informaci�n de versi�n del archivo
	void FindProductVersion();													//Funci�n que lee la informaci�n de la versi�n del producto

	/************************************************************************************************************************/
	/* Declaraci�n de funciones p�blicas																					*/
	/************************************************************************************************************************/
public:
	geModuleEntry(const MODULEENTRY32 &me);										//Constructor de la clase
	virtual ~geModuleEntry();													//Destructor virtual

	/************************************************************************************************************************/
	/* Accesores constantes																									*/
	/************************************************************************************************************************/
	const MODULEENTRY32* Get() const {return &m_me32;}
	const TCHAR* GetProductVersion() const {return m_pProductVersion;}
};