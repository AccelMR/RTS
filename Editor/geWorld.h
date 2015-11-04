/********************************************************************
	Created:	2014/02/03
	Filename:	geWorld.h
	Author:		Samuel Prince

	Purpose:	Esta clase contiene los datos de Mundo para el juego
				Aqu� se almacenan todos los datos de una escena
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Declaraci�n de la clase geWorld                                      												*/
/************************************************************************************************************************/
class geWorld
{
	/************************************************************************************************************************/
	/* Definici�n de variables miembro                                      												*/
	/************************************************************************************************************************/
private:
	//Variables de Render
	SDL_Renderer *m_pRenderer;						//Renderer asociado al mundo
	
	//Variables de Mapa
	geTiledMap *m_pTiledMap;						//Mapa asociado al mundo

	//Variables de Pathfinding
	std::vector<geMapGridWalker*> m_walkersList;	//Lista de Walkers capaces de hacer pathfinding (cada uno tiene un algoritmo diferente)
	geMapGridWalker *m_activeWalker;				//Puntero al Walker activo (que algoritmo estamos usando para pathfinding)
	int8 m_activeWalkerIndex;						//Indice en la lista perteneciente al algoritmo utilizado en este momento
	
	uint16 m_AppResolutionX;						//Resoluci�n de la aplicaci�n en X
	uint16 m_AppResolutionY;						//Resoluci�n de la aplicaci�n en Y

	/************************************************************************************************************************/
	/* Definici�n de Constructores y Destructores de la clase               												*/
	/************************************************************************************************************************/
public:
	geWorld(void);		//Constructor Standard
	~geWorld(void);		//Destructor

	/************************************************************************************************************************/
	/* Definici�n de funciones miembro                                      												*/
	/************************************************************************************************************************/
public:
	bool Init(SDL_Renderer* pRenderer);		//Inicializaci�n de la clase
	void Destroy();							//Destrucci�n de objetos de la clase
	void Update(float deltaTime);			//Actualizaci�n
	void Render();							//Impresi�n de objetos

	/************************************************************************************************************************/
	/* Accesores                                                            												*/
	/************************************************************************************************************************/
	geTiledMap* getTiledMap(){ return m_pTiledMap; }

	/************************************************************************************************************************/
	/* Funciones de control de resoluci�n de la aplicaci�n																	*/
	/************************************************************************************************************************/
	void UpdateResolutionData();			//Esta funci�n avisa de un cambio de resoluci�n a los objetos de mundo dependientes de ella (e.j. UI)
	void setResolution(const uint16 w, const uint16 h) { m_AppResolutionX = w; m_AppResolutionY = h; UpdateResolutionData(); }
	uint16 getResolutionWidth() { return m_AppResolutionX; }
	uint16 getResolutionHeight() { return m_AppResolutionY; }

	/************************************************************************************************************************/
	/* Funciones para pathfinding                                           												*/
	/************************************************************************************************************************/
	void setCurrentWalker(const int8 index);		//Establece el Walker (algoritmo de pathfinding) a utilizar
};

