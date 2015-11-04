/********************************************************************
	Created:	2014/08/16
	Filename:	MemoryDump.cpp
	Author:		Samuel Prince

	Purpose:	Clase de debug que genera un Dump de memoria a un
				archivo para el futuro an�lisis de errores
*********************************************************************/

/************************************************************************************************************************/
/* Inclusi�n de archivos necesarios para la compilaci�n																	*/
/************************************************************************************************************************/
#pragma region Includes, Namespaces y librer�as necesarias
#include "geCoreStd.h"
#include "MemoryDump.h"

#pragma comment(lib, "version.lib")

#pragma endregion

#pragma region Constantes, Est�ticos y Globales
/************************************************************************************************************************/
/* Definici�n de Constantes, Est�ticos y Globales																		*/
/************************************************************************************************************************/

/************************************************************************************************************************/
/* Prototipo de funci�n basado en dbghelp.h																				*/
/************************************************************************************************************************/
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

geMemoryDump *geMemoryDump::g_pDumper = NULL;			//Inicializamos 


#pragma endregion

#pragma region Declaraci�n de la Clase LogMgr
/************************************************************************************************************************/
/* Clase geMemoryDump.- Clase de debug encargada de hacer un Dump de memoria a archivo									*/
/************************************************************************************************************************/
geMemoryDump::geMemoryDump(bool headless)
{
	//Detectamos si existe m�s de un objeto geMemoryDump
	GEE_ASSERT(!g_pDumper);

	if( !g_pDumper )
	{//Si a�n no existe un objeto Dumper en la aplicaci�n
		::SetUnhandledExceptionFilter(Handler);		//Establecemos el manejador local para ser utilizado como funci�n de filtro de excepciones
		g_pDumper = this;							//Establecemos este objeto de clase como el Dumper en uso
		m_bHeadless = headless;						//Establecemos la bandera que indica si debe mostrarse un di�logo preguntando al usuario por la creaci�n del archivo de Dump
	}
}

/************************************************************************************************************************/
/* Funci�n Handler() - Esta funci�n se encarga de manejar las excepciones de memoria									*/
/************************************************************************************************************************/
long geMemoryDump::Handler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	long retval = EXCEPTION_CONTINUE_SEARCH;

	if( !g_pDumper )
	{//Si no se ha establecido un dumper, simplemente regresa sin hacer nada
		return retval;	//TODO: Esto indica un error y deber�a ser revisado
	}

	//Mandamos a escribir el archivo del Dump
	return g_pDumper->WriteMemoryDump(pExceptionInfo);
}

/************************************************************************************************************************/
/* Funci�n WriteMemoryDump() - Escribe el archivo con la informaci�n del Memory Dump									*/
/************************************************************************************************************************/
long geMemoryDump::WriteMemoryDump(_EXCEPTION_POINTERS *pExceptionInfo)
{
	//Creamos variables temporales
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[MAX_PATH];
	
	//Guardamos la informaci�n de la excepci�n en los miembros de la clase
	m_pExceptionInfo = pExceptionInfo;
	
	//Obtenemos la informaci�n de tiempo y fecha actual
	time( &m_lTime );

	//Debemos buscar la librer�a dbghelp.dll correcta
	//Buscamos primero en la misma ruta del EXE ya que el de System32 puede ser una versi�n antigua (Win2k)
	if( GetModuleFileName(NULL, m_szAppPath, MAX_PATH) )
	{
		//Buscamos la �ltima ocurrencia de la diagonal en la ruta de la aplicaci�n
		TCHAR* pSlash = _tcsrchr( m_szAppPath, '\\' );
		if( pSlash )
		{//Si la encontramos, copiamos la ruta limpia sin el nombre del archivo
#ifdef _VS2005_
			_tcscpy_s( m_szAppBaseName, pSlash + 1);
#else
			_tcscpy( m_szAppBaseName, pSlash + 1 );
#endif
			*(pSlash+1) = 0;	//Terminamos el string
		}

		//Creamos la ruta de b�squeda de la librer�a usando la ruta de la aplicaci�n
#ifdef _VS2005_
		_tcscpy_s( szDbgHelpPath, m_szAppPath );
		_tcscat_s( szDbgHelpPath, TEXT("DBGHELP.DLL") );
#else
		_tcscpy( szDbgHelpPath, m_szAppPath );
		_tcscat( szDbgHelpPath, TEXT("DBGHELP.DLL") );
#endif

		//Cargamos la librer�a DLL
		hDll = ::LoadLibrary(szDbgHelpPath);
	}


	if( hDll == NULL )
	{//Si la librer�a no se encontr�
		//Intentamos una vez m�s utilizando las rutas preestablecidas por las variables de entorno
		hDll = ::LoadLibrary( TEXT("DBGHELP.DLL") );
	}

	LPCTSTR szResult = NULL;
	if( hDll )
	{//Si encontramos la librer�a
		//Obtenemos un puntero al procedimiento "MiniDumpWriteDump" de la DLL
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump");
		if( pMiniDumpWriteDump )
		{//Si obtuvimos el puntero al procedimiento correctamente
			//Creamos un array para contener la informaci�n de usuario
			TCHAR szScratch[USER_DATA_BUFFER_SIZE];

			//Establece el nombre final del archivo de Dump
			SetDumpFileName();

			//Le pregunta al usuario si quiere guardar un archivo Dump (esto solo si )
			_tcssprintf(szScratch, TEXT("Ocurri� un error inesperado:\n\n%s\nQuieres guardar un archivo de diagn�stico?\n\nNombre del Archivo: %s"), GetUserMessage(), m_szDumpPath);
			if( m_bHeadless || (::MessageBox( NULL, szScratch, NULL, MB_YESNO ) == IDYES) )
			{//Se accedi� a la creaci�n del archivo de Dump
				//Creamos el archivo vac�o con los atributos necesarios
				HANDLE hFile = ::CreateFile(m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if( hFile != INVALID_HANDLE_VALUE)
				{//El archivo se cre� exitosamente
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;				//Esta estructura contenrdr� la informaci�n de la excepci�n

					ExInfo.ThreadId = ::GetCurrentThreadId();			//Obtenemos el identificador del thread de la aplicaci�n
					ExInfo.ExceptionPointers = pExceptionInfo;			//Guardamos los punteros de excepci�n
					ExInfo.ClientPointers = NULL;						//Limpiamos la informaci�n de punteros a clientes

					//Escribimos el Dump
					int32 bOK = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, GetUserStreamArray(), NULL);
					if( bOK )
					{//Si se escribi� el archivo correctamente
						szResult = NULL;					//No enviaremos mensajes de error
						retval = EXCEPTION_EXECUTE_HANDLER;	//Establecemos el valor de regreso
					}
					else
					{//Fall� al escribir el archivo
						//Establecemos un mensaje de error
						_tcssprintf(szScratch, TEXT("Fallo al escribir el archivo Dump a '%s' (error: %d)"), m_szDumpPath, GetLastError());
						szResult = szScratch;
					}

					//Cerramos el manejador al archivo
					::CloseHandle(hFile);
				}
				else
				{//Fallo al crear el archivo
					_tcssprintf( szScratch, TEXT("Fallo al crear el archivo Dump en '%s' (error: %d)"), m_szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{//La versi�n de la librer�a es muy vieja
			szResult = TEXT("DBGHELP.DLL es una versi�n muy vieja");
		}
	}
	else
	{//No se encontr� ninguna versi�n de la librer�a de debug
		szResult = TEXT("No se encontr� - DBGHELP.DLL");
	}

	if( szResult && !m_bHeadless )
	{//Si hubo alg�n mensaje de error, y debe mostrarse
		//Lanzamos un d�alogo con el mensaje
		::MessageBox(NULL, szResult, NULL, MB_OK);
	}

	//Terminamos el proceso de la aplicaci�n
	TerminateProcess( GetCurrentProcess(), 0);

	/************************************************************************************************************************/
	/* NOTA: ExitThread funcionar�a, y permite al Dumper matar un thread crasheado sin afectar al resto de la aplicaci�n.	*/
	/* Ahora bien... la pregunta del d�a: Es esa una buena idea??? Respuesta: DEFINITIVAMENTE NO!!!!!!!!					*/
	/************************************************************************************************************************/
	//ExitThread(0);

	return retval;
}

/************************************************************************************************************************/
/* SetDumpFileName - Establece el nombre que llevar� el archivo de Dump en esta instancia								*/
/************************************************************************************************************************/
void geMemoryDump::SetDumpFileName(void)
{
	//Creamos el nombre a partir del nombre de la aplicaci�n, y la fecha y hora de ejecuci�n
	_tcssprintf(m_szDumpPath, _T("%s%s.%ld.dmp"), m_szAppPath, m_szAppBaseName, m_lTime);
}

#pragma endregion
