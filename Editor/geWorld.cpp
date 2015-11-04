/********************************************************************
	Created:	2014/02/03
	Filename:	geWorld.cpp
	Author:		Samuel Prince

	Purpose:	Implementaci�n de las funciones de la clase geWorld
*********************************************************************/

/************************************************************************************************************************/
/* Inclusi�n de cabeceras necesarias para la compilaci�n                												*/
/************************************************************************************************************************/
#include "stdafx.h"
#include "geWorld.h"

/************************************************************************************************************************/
/* Implementaci�n de funciones de la clase                              												*/
/************************************************************************************************************************/
geWorld::geWorld(void)
{//Constructor Standard
	//Limpiamos las variables miembro
	m_pTiledMap = NULL;
	m_pRenderer = NULL;
	m_activeWalkerIndex = -1;	//Lo inicializamos en -1 indicando que es un �ndice inv�lido
}

geWorld::~geWorld(void)
{//Destructor
	Destroy();
}

bool geWorld::Init(SDL_Renderer* pRenderer)
{//Inicializa los objetos de la clase para su uso
	if(m_pTiledMap != NULL)
	{//Si el mapa ya est� inicializado, quiere decir que ya creamos esta secci�n
		//Mandamos un mensaje indicando el error y destruimos los objetos para intentar reinicializar
		GEE_WARNING(TEXT("geWorld Ya hab�a sido inicializado anteriormente"));
		Destroy();
	}

	//Copiamos localmente el puntero al renderer
	GEE_ASSERT(pRenderer);
	m_pRenderer = pRenderer;

	//Primero inicializamos el mapa (Creamos de momento un mapa vacio)
	m_pTiledMap = GEE_NEW geTiledMap();
	GEE_ASSERT(m_pTiledMap);
	m_pTiledMap->Init(pRenderer, 256);

	//BORRAR: Asignamos puntos de prueba para ver que se est� imprimiendo todo el mapa
	for(int i=0; i<10; i++)
		m_pTiledMap->setType(i, 0, TT_WALKABLE);

	//Creamos los algoritmos de pathfinding y los metemos a la lista de Walkers
	m_walkersList.push_back( GEE_NEW geBreadthFirstSearchMapGridWalker(m_pTiledMap) );

	//Inicializamos los algoritmos
	for(SIZE_T it=0; it<m_walkersList.size(); it++)
	{
		m_walkersList[it]->Init();
	}

	//Establecemos el algoritmo que vamos a utilizar
	setCurrentWalker(0);

	return true;
}

void geWorld::Destroy()
{//
	//Destruimos los sistemas de pathfinding
	while(m_walkersList.size() > 0)
	{
		GEE_DELETE m_walkersList.back();
		m_walkersList.pop_back();
	}

	//Destruimos el mapa (Este debe ser el �ltimo paso ya que se requieren revisar variables en objetos anteriores)
	SAFE_DELETE(m_pTiledMap);
}

void geWorld::Update(float deltaTime)
{
	m_pTiledMap->Update(deltaTime);		//Actualizamos los datos del mapa
}

void geWorld::Render()
{
	//Imprimimos el mapa
	m_pTiledMap->Render();
}

void geWorld::UpdateResolutionData()
{
	//Con la resoluci�n establecida actualmente, enviamos informaci�n nueva a los objetos que la requieran
	if( m_pTiledMap != NULL )
	{
		m_pTiledMap->setStart(0, 32);	//De momento es la posici�n fija no dependiente de la resoluci�n donde se comienza a imprimir el mapa
		m_pTiledMap->setEnd(getResolutionWidth(), getResolutionHeight()-175);
		m_pTiledMap->moveCamera(0, 0);	//Hacemos esto para hacer un clamp a la posici�n si fuera necesario debido al cambio de resoluci�n
	}
}

void geWorld::setCurrentWalker(const int8 index)
{//Establece el Walker Actual
	//Revisamos que el walker exista (en modo de debug)
	GEE_ASSERT( m_walkersList.size() > (SIZE_T)index );

	m_activeWalker = m_walkersList[index];
	m_activeWalkerIndex = index;
}