/********************************************************************
	Created:	2014/07/06
	Filename:	Initialization.h
	Author:		Samuel Prince

	Purpose:	Funciones de inicializaci�n y detecci�n de hardware
				Tambien nos encargamos de algunas funciones para la
				obtenci�n de opciones a trav�s de archivos de
				configuraci�n
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Declaraci�n de funciones de chequeo																					*/
/************************************************************************************************************************/
extern bool			CheckStorage(const uint64 diskSpaceNeeded);
extern uint32		ReadCPUSpeed();
extern bool			CheckMemory(const uint64 physicalRAMNeeded, const uint64 virtualRAMNeeded);

/************************************************************************************************************************/
/* Declaraci�n de funciones de uso para la aplicaci�n																	*/
/************************************************************************************************************************/
extern bool			IsOnlyInstance(LPCTSTR gameTitle);
extern const TCHAR*	GetSaveGameDirectory(HWND hWnd, const TCHAR *gameAppDirectory);
extern bool			CheckForJoystick(HWND hWnd);
