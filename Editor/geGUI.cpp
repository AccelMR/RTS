/********************************************************************
	Created:	2014/02/07
	Filename:	geGUI.cpp
	Author:		Samuel Prince

	Purpose:	Implementaci�n de la clase geGUI
*********************************************************************/

/************************************************************************************************************************/
/* Inclusi�n de cabeceras necesarias para la compilaci�n               													*/
/************************************************************************************************************************/
#include "stdafx.h"
#include "geGUI.h"

/************************************************************************************************************************/
/* Implementaci�n de contructores y destructor                          												*/
/************************************************************************************************************************/
geGUI::geGUI(void)
{//Constructor
	m_pRenderer = NULL;
	m_pMainGUITexture = NULL;
}

geGUI::~geGUI(void)
{//Destructor
	Destroy();
}

/************************************************************************************************************************/
/* Implementaci�n de las funciones miembro de la clase                  												*/
/************************************************************************************************************************/
bool geGUI::Init(SDL_Renderer* pRenderer)
{
	//Copiamos localmente la referencia al renderer al que estaremos asociado
	m_pRenderer = pRenderer;

	//Cargamos las texturas requeridas por este objeto
	m_pMainGUITexture = GEE_NEW geTexture();
	if( !m_pMainGUITexture->LoadFromFile(m_pRenderer, TEXT("Textures\\GUI\\GUI.png")) )
	{
		GEE_ERROR(TEXT("Fall� al cargar la imagen del GUI"));
		return false;
	}

	return true;
}

void geGUI::Destroy()
{
	m_pRenderer = NULL;
	SAFE_DELETE(m_pMainGUITexture);
}

void geGUI::Update(float)
{
	//De momento no necesitamos hacer nada para actualizar este objeto
}

void geGUI::Render()
{
	//Rendereamos la imagen del GUI
	GEE_ASSERT(m_pMainGUITexture);
	m_pMainGUITexture->Render(0,0);
}
