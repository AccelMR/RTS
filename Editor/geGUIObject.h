/********************************************************************
	Created:	2014/02/18
	Filename:	geGUIObject.h
	Author:		Samuel Prince

	Purpose:	Declaraci�n de la clase geGUIObject como clase base
				de objetos de interface (botones, fondos, marcos, etc)
				Declaraci�n de objetos m�nimos y funciones compartidas
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Declaraci�n de la clase base geGUIObject																				*/
/************************************************************************************************************************/
class geGUIObject
{
	/************************************************************************************************************************/
	/* Constructor y destructor virtual                                     												*/
	/************************************************************************************************************************/
public:
	geGUIObject(void);
	virtual ~geGUIObject(void);
};
