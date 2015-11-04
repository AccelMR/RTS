/********************************************************************
	Created:	2014/02/02
	Filename:	stdafx.h
	Author:		Samuel Prince

	Purpose:	Cabecera Precompilada de la aplicaci�n Editor
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Incluimos los archivos de las librer�as del Engine	                												*/
/************************************************************************************************************************/
#include "geCoreStd.h"		//Incluimos la librer�a de geCore

/************************************************************************************************************************/
/* Incluimos las cabeceras necesarias para utilizar LUA																	*/
/************************************************************************************************************************/
#include "lua.hpp"

/************************************************************************************************************************/
/* Incluimos las cabeceras necesarias para utilizar ToLua																*/
/************************************************************************************************************************/
#include "tolua.h"

/************************************************************************************************************************/
/* Incluimos los archivos de cabecera requeridos por la aplicaci�n      												*/
/************************************************************************************************************************/
#include <SDL.h>			//Incluimos las cabeceras de SDL
#include <SDL_opengl.h>		//Incluimos los objetos de SDL (TODO: En este momento no se est�n utilizando)
#include <SDL_image.h>		//Incluimos las cabeceras de la librer�a de im�genes de SDL

/************************************************************************************************************************/
/* Incluimos cabeceras de los archivos espec�ficos de la aplicaci�n		  												*/
/************************************************************************************************************************/
#include "geTexture.h"							//Clase de texturas SDL

#include "geGUI.h"								//Manejo de la inteface gr�fica de usuario
#include "geTiledMap.h"							//Manejo del mapa
#include "geMapTileNode.h"						//Objetos de nodos del mapa para Pathfinding
#include "geMapGridWalker.h"					//Clase base para algoritmos de Pathfinding
#include "geBreadthFirstSearchMapGridWalker.h"	//Implementaci�n de Breadth First Search para Pathfinding
#include "geWorld.h"							//Al final incluimos mundo ya que depende de las cabeceras anteriores

/************************************************************************************************************************/
/* Incluimos las librer�as que deben ligarse                            												*/
/************************************************************************************************************************/
#pragma comment(lib, "AntTweakBar.lib")
#pragma comment(lib, "TinyXml.lib")
#pragma comment(lib, "geCore.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "liblua.lib")
#pragma comment(lib, "tolua.lib")
//#pragma comment(lib, "pthread.lib")