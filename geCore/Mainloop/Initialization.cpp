/********************************************************************
	Created:	2014/07/06
	Filename:	Initialization.cpp
	Author:		Samuel Prince

	Purpose:	Funciones de inicializaci�n y detecci�n de hardware
				Tambien nos encargamos de algunas funciones para la
				obtenci�n de opciones a trav�s de archivos de
				configuraci�n
*********************************************************************/

/************************************************************************************************************************/
/* Incluimos los archivos necesarios																					*/
/************************************************************************************************************************/
#include "geCoreStd.h"
#include <shlobj.h>
#include <direct.h>
#include "Initialization.h"

/************************************************************************************************************************/
/* Esta funci�n revisa si hay suficiente espacio libre en el disco duro actual											*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
bool CheckStorage(const uint64 diskSpaceNeeded)
{
	//Creamos variables temporales
	const int32 drive = _getdrive();	//Indice del disco actual
	_diskfree_t diskfree;				//Estructura de informaci�n de espacio libre

	//Obtenemos la informaci�n de espacio libre en el disco actual
	if( _getdiskfree(drive, &diskfree) != 0 )
	{//Si fall� al obtener el espacio libre del disco
		//Mandamos un error
		GEE_ERROR(TEXT("CheckStorage() - Fallo al obtener el tama�o de espacio libre del disco duro"));
	}

	//Calculamos el n�mero de clusters requeridos de acuerdo a la cantidad de espacio pedido
	const uint64 neededClusters = diskSpaceNeeded / (diskfree.sectors_per_cluster * diskfree.bytes_per_sector);

	//Revisamos si el espacio libre es suficiente de acuerdo al espacio necesitado
	if(diskfree.avail_clusters < neededClusters)
	{//Si no hay suficiente espacio libre en el disco duro
		//Informamos el error
		GEE_ERROR(TEXT("CheckStorage: No hay suficiente espacio libre en el disco duro"));
		return false;
	}
	return true;
}
#else
bool CheckStorage(const uint64 diskSpaceNeeded)
{
	GEE_LOG(TEXT("WARNING"),TEXT("CheckStorage: Funci�n no soportada"));
	return true;	
}
#endif

/************************************************************************************************************************/
/* Esta funci�n revisa si hay suficiente memoria f�sica y virtual														*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
bool CheckMemory(const uint64 physicalRAMNeeded, const uint64 virtualRAMNeeded)
{
	//Creamos variables temporales
	MEMORYSTATUSEX status;			//Estructura que contendr� la informaci�n sobre la memoria
	
	//Obtenemos la informaci�n de la memoria
	GlobalMemoryStatusEx(&status);

	//Revisamos la cantidad total de memoria f�sica
	if( status.ullTotalPhys < physicalRAMNeeded )
	{//Si no hay suficiente memoria f�sica
		//Le indicamos al usuario que vaya a conseguir una computadora real y le deje esta a su madre
		GEE_ERROR(TEXT("CheckMemory: No hay suficiente memoria f�sica"));
		return false;
	}

	//Revisamos la cantidad total de memoria virtual
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{//Si no hay suficiente memoria virtual
		//Le indicamos al usuario que cierre su copia de Unity o
		//whatever the fuck que est� chupando memoria como si no hubiera un ma�ana
		GEE_ERROR(TEXT("CheckMemory: No hay suficiente memoria virtual"));
		return false;
	}

	char *buff = GEE_NEW char[(uint32)virtualRAMNeeded];
	if( buff )
	{//Si el buffer pudo alojarse sin problemas, indica que hubo suficiente memoria para crearlo
		GEE_DELETE_ARRAY buff;	//Liberamos el buffer de memoria
	}
	else
	{//No se pudo alojar la memoria
		//Aun cuando hay suficiente memoria, no est� disponible en un solo bloque
		GEE_ERROR(TEXT("CheckMemory: No hay suficiente memoria contigua disponible"));
		return false;
	}
	return true;
}
#else
bool CheckMemory(const uint64 physicalRAMNeeded, const uint64 virtualRAMNeeded)
{
	GEE_LOG(TEXT("WARNING"),TEXT("CheckMemory: Funci�n no soportada"));
	return true;
}
#endif

/************************************************************************************************************************/
/* Esta funci�n lee la velocidad del CPU desde el registro del sistema (Windows only)									*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
uint32 ReadCPUSpeed()
{
	//Creamos variables temporales
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;

	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey);
	if( lError == ERROR_SUCCESS )
	{//Si logr� abrirse el registro
		//Leemos el valor que nos interesa
		RegQueryValueEx(hKey, TEXT("~MHz"), NULL, &(DWORD)type, (LPBYTE)&dwMHz, &BufSize);
	}
	return (uint32)dwMHz;
}
#else
uint32 ReadCPUSpeed()
{
	GEE_LOG(TEXT("WARNING"),TEXT("ReadCPUSpeed: Funci�n no soportada"));
	return 0;
}
#endif // _WINDOWS_

/************************************************************************************************************************/
/* Esta funci�n revisa al abrir la aplicaci�n, que no exista ya otra instancia corriendo (Windows only)					*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
bool IsOnlyInstance(LPCTSTR gameTitle)
{
	//Buscamos la ventana. Si est� activa, la establece como ventana activa y regresa false
	
	//Solo una instancia puede tener este Mutex en este momento
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);

	//Alguien m�s piensa que 'ERROR_SUCCESS' es un oxymoron?
	if( GetLastError() != ERROR_SUCCESS || handle == NULL )
	{//Si no pudimos obtener el Mutex, eso significa que otra instancia est� corriendo
		//Buscamos la ventana que se llame como nuestro juego
		HWND hWnd = FindWindow(gameTitle, NULL);
		if( hWnd ) 
		{//Si ecnontramos la ventana
			//Una instanciade este juego ya est� corriendo
			ShowWindow(hWnd, SW_SHOWNORMAL);	//Mostramos la ventana
			SetFocus(hWnd);						//Le establecemos el foco de eventos
			SetForegroundWindow(hWnd);			//La establecemos como la ventana frontal
			SetActiveWindow(hWnd);				//La establecemos como la ventana activa
			return false;						//Indicamos que no era la �nica instancia
		}
	}
	return true;								//Indicamos que esta es la �nica instancia
}
#else
bool IsOnlyInstance(LPCTSTR gameTitle)
{
	GEE_LOG(TEXT("WARNING"),TEXT("IsOnlyInstance: Funci�n no soportada"));
	return true;
}
#endif

/************************************************************************************************************************/
/* Esta funci�n regresa la ruta del directorio del programa																*/
/* Parametros:																											*/
/*     hWnd: Manejador de la ventana de aplicaci�n																		*/
/*     gameAppDirectory: Nombre que se le asignar� a la carpeta de aplicaci�n											*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory)
{
	//Creamos variables temporales
	TCHAR userDataPath[MAX_PATH];

	//Establecemos esta variable est�tica ya que la memoria debe mantenerse en todo momento
	static TCHAR m_SaveGameDirectory[MAX_PATH];
	
	//Limpiamos el contenido de m_SaveGameDirectory (con esto aseguramos que ser� un objeto NULL-Terminated)
	memset(m_SaveGameDirectory, 0, sizeof(m_SaveGameDirectory));

	//Obtenemos la ruta a la carpeta especial <user name>\Application Data y la almacenamos en userDataPath
	if( !SHGetSpecialFolderPath(hWnd, userDataPath, CSIDL_APPDATA, true) )
	{
		GEE_ERROR(TEXT("GetSaveGameDirectory() - No se pudo obtener la ruta a AppData"));
		return NULL;
	}

	//Copiamos la ruta especial en nuestro buffer est�tico
	_tcscpy_s(m_SaveGameDirectory, userDataPath);

	//Agreamos un slash seguido del nombre de folder que se nos pidi�
	_tcscat_s(m_SaveGameDirectory, FOLDER_SLASH);
	_tcscat_s(m_SaveGameDirectory, gameAppDirectory);

	//Revisamos si el directorio existe
	if( GetFileAttributes(m_SaveGameDirectory) == 0xffffffff )
	{//La carpeta pedida no existe todav�a
		//Creamos la carpeta
		if( SHCreateDirectoryEx(hWnd, m_SaveGameDirectory, NULL) != ERROR_SUCCESS )
		{//Si hubo alg�n error en la creaci�n
			return NULL;	//Regresamos el error
		}
	}
		
	//Agregamos un �ltimo slash para asegurarnos que cualquier ruta agregada a esta no requiera hacerlo cada vez
	_tcscat_s(m_SaveGameDirectory, FOLDER_SLASH);

	//Regresamos la direcci�n de la variable est�tica (as� siempre se conserva en el mismo espacio de memoria)
	return m_SaveGameDirectory;
}
#else
const TCHAR* GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory)
{
	GEE_LOG(TEXT("WARNING"),TEXT("GetSaveGameDirectory: Funci�n no soportada"));
	return NULL;
}
#endif

/************************************************************************************************************************/
/* Esta funci�n Indica si hay un Joystick conectado (Windows only)														*/
/* Parametros:																											*/
/*     hWnd: Manejador de la ventana de aplicaci�n																		*/
/************************************************************************************************************************/
#ifdef _WINDOWS_
bool CheckForJoystick(HWND hWnd)
{
	//Creamos variables temporales
	JOYINFO joyinfo; 
	UINT wNumDevs; 
	BOOL bDev1Attached, bDev2Attached; 
 
	//Obtenemos el n�mero de joysticks configurados
	if((wNumDevs = joyGetNumDevs()) == 0)
	{
		return false;
	}
	
	//Revisamos si hay uno o dos joysticks conectados
	bDev1Attached = joyGetPos(JOYSTICKID1, &joyinfo) != JOYERR_UNPLUGGED; 
	bDev2Attached = joyGetPos(JOYSTICKID2, &joyinfo) != JOYERR_UNPLUGGED; 
	
	//Si hay un primer dispositivo conectado
	if( bDev1Attached )
	{//Establecemos la velocidad de captura de datos para este joystick
		joySetCapture(hWnd, JOYSTICKID1, 1000/30, true);
	}

	//Si hay un segundo dispositivo conectado
	if( bDev2Attached )
	{//Establecemos la velocidad de captura de datos para este joystick
		joySetCapture(hWnd, JOYSTICKID2, 1000/30, true);
	}

	return true;	//Indicamos que si hay joysticks conectados
}
#else
bool CheckForJoystick(HWND hWnd)
{
	GEE_LOG(TEXT("WARNING"),TEXT("CheckForJoystick: Funci�n no soportada"));
	return false;
}
#endif