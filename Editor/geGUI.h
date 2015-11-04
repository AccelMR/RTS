/********************************************************************
	Created:	2014/02/05
	Filename:	geGUI.h
	Author:		Samuel Prince

	Purpose:	Declaración de la clase y objetos para el manejo de
				interface gráfica de la aplicación
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Inclusión de cabeceras necesarias para la compilación              												*/
/************************************************************************************************************************/

/************************************************************************************************************************/
/* Definición de constantes                                             												*/
/************************************************************************************************************************/

/************************************************************************************************************************/
/* Declaración de la clase geGUI para el manejo de la interface gráfica de la aplicación								*/
/************************************************************************************************************************/
class geGUI
{
	/************************************************************************************************************************/
	/* Declaración de constructores y destructor                            												*/
	/************************************************************************************************************************/
public:
	geGUI(void);
	geGUI(SDL_Renderer* pRenderer);
	~geGUI(void);

	/************************************************************************************************************************/
	/* Declaración de funciones miembro de la clase                         												*/
	/************************************************************************************************************************/
	bool Init(SDL_Renderer* pRenderer);
	void Destroy();

	void Update(float deltaTime);
	void Render();

	/************************************************************************************************************************/
	/* Declaración de variables de la clase                                 												*/
	/************************************************************************************************************************/
public:
	SDL_Renderer *m_pRenderer;			//Renderer asociado con este objeto
	geTexture* m_pMainGUITexture;		//Textura principal del GUI
};
