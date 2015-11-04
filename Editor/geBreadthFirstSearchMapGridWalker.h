/********************************************************************
	Created:	2014/02/14
	Filename:	geBreadthFirstSearchMapGridWalker.h
	Author:		Samuel Prince

	Purpose:	Declaraci�n de la clase utilizada para hacer
				pathfinding a partir del algorimo Breadth First
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Incluimos cabeceras de los archivos necesarios                       												*/
/************************************************************************************************************************/
#include "geMapGridWalker.h"
#include "geMapTileNode.h"
#include <queue>

class geBreadthFirstSearchMapGridWalker : public geMapGridWalker
{
	/************************************************************************************************************************/
	/* Declaraci�n de constructores y destructor virtual                    												*/
	/************************************************************************************************************************/
public:
	geBreadthFirstSearchMapGridWalker(void);				//Constructor standard
	geBreadthFirstSearchMapGridWalker(geTiledMap *pMap);	//Constructor con par�metro del mapa que utilizaremos para calcular
	virtual ~geBreadthFirstSearchMapGridWalker(void);		//Destructor virtual

	/************************************************************************************************************************/
	/* Funciones de ayuda de la clase                                      													*/
	/************************************************************************************************************************/
public:
	virtual bool Init();										//Funci�n de inicializaci�n de la clase (crea los objetos para su uso posterior)
	virtual void Destroy();										//Destruye los objetos de la clase (despues de llamarlo, las llamadas a otras funciones son inv�lidas)
	virtual WALKSTATETYPE Update();								//Actualiza el estado del pathfinding (puede hacerse por pasos o recursivamente)
	virtual void Render();										//Renderea la informaci�n necesaria para su uso en pathfinding
	virtual void Reset();										//Reinicializa las variables de esta clase para su uso en un nuevo c�lculo

	virtual bool weightedGraphSupported(){ return false; }		//Indica si este Walker soporta la asignaci�n y el uso de pesos a los nodos del graph

protected:
	virtual void visitGridNode(int32 x, int32 y);				//Marca un nodo de mapa como visitado (esto lo procesa seg�n el algoritmo utilizado)

	/************************************************************************************************************************/
	/* Declaraci�n de variables miembro exclusivas de esta clase            												*/
	/************************************************************************************************************************/
private:
	std::queue<geMapTileNode*> m_open;			//Nuestra lista abierta utilizando un queue ordinario
	geMapTileNode *m_start, *m_n, *m_end;		//Punteros a los nodos de inicio, uso y final
	geMapTileNode **m_nodegrid;					//Matriz para almacenamiento de los nodos del mapa
};
