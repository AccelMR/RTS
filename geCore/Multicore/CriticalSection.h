/********************************************************************
	Created:	2014/02/02
	Filename:	CriticalSection.h
	Author:		Samuel Prince

	Purpose:	Define clases de auda para programaci�n multithread
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Declaraci�n de la clase CriticalSection (este tipo de objetos no debe ser copiable)									*/
/************************************************************************************************************************/
class CriticalSection : public GEE_noncopyable
{
	/************************************************************************************************************************/
	/* Declaraci�n e implementaci�n de las funciones de la clase            												*/
	/************************************************************************************************************************/
public:
	CriticalSection()
	{//Constructor
		InitializeCriticalSection( &m_cs );
	}
 
	~CriticalSection()
	{//Destructor
		DeleteCriticalSection( &m_cs );
	}
 
 
	void Lock()
	{//Bloquea la secci�n cr�tica
		EnterCriticalSection( &m_cs );
	}

	void Unlock()
	{//Libera la secci�n cr�tica
		LeaveCriticalSection( &m_cs );
	}

	/************************************************************************************************************************/
	/* Declaraci�n de las variables de la clase                             												*/
	/************************************************************************************************************************/
protected:
	mutable CRITICAL_SECTION m_cs;
};

/************************************************************************************************************************/
/*	Descripcion
		Clase de ayuda que permite Locking / Unlocking autom�tico de un Recirso, protegido por una secci�n cr�tica
		- Hace Lock cuando este objeto es construido
		- Hace Unlock cuando el objeto es destruido (sale del scope)

	Uso
		Cuando necesites acceso protegido aun recurso, haz lo siguiente:
		1. Crea una secci�n cr�tica asociada con el recurso
		2. Envuelve el c�digo que accede al recurso en llaves {}
		3. Construye un objeto ScopedCriticalSection

	Ejemplo:
		//Asumimos que m_CriticalSection es una variable privada, y la usamos para proteger 'this' de ser accedido
		//mientras necesitamos acceso seguro a un recurso
		
		//C�digo que no requiere bloqueo de recursos
		{
			ScopedCriticalSection I_am_locked( m_cs);
			//C�digo que necesita de bloqueo de threads
		}
		//C�digo que no requiere bloqueo de recursos
*/
/****************************************************************************************************************************************************************/
class ScopedCriticalSection : public GEE_noncopyable
{
	/************************************************************************************************************************/
	/* Declaraci�n e implementaci�n de funciones de la clase                												*/
	/************************************************************************************************************************/
public:
	ScopedCriticalSection( CriticalSection & csResource) : m_csResource( csResource )
	{//Constructor
		m_csResource.Lock();	//Bloquea la secci�n cr�tica
	}

	~ScopedCriticalSection()
	{//Destructor
		m_csResource.Unlock();	//Libera la secci�n cr�tica
	}
 
	/************************************************************************************************************************/
	/* Declaraci�n de variables de la clase                                 												*/
	/************************************************************************************************************************/
private:
	CriticalSection & m_csResource;	//La secci�n crpitica para este recurso
};

/************************************************************************************************************************/
/* concurrent_queue fue sacada de 																						*/
/* http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html		*/
/* y fue escrita por Anthony Williams																					*/
/************************************************************************************************************************/
template<typename Data>
class concurrent_queue
{
	/************************************************************************************************************************/
	/* Declaraci�n de variables de la clase                                 												*/
	/************************************************************************************************************************/
private:
	std::queue<Data> the_queue;		//El objeto queue
	CriticalSection m_cs;			//Secci�n cr�tica dedicada a este objeto
	HANDLE m_dataPushed;			//Manejador a un evento disparado por la acci�n de agregar datos al queue

	/************************************************************************************************************************/
	/* Declaraci�n e implementaci�n de funciones de la clase                												*/
	/************************************************************************************************************************/
public:
	concurrent_queue()
	{//Constructor
		//Crea un nuevo objeto evento
		m_dataPushed = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	/************************************************************************************************************************/
	/* Esta funci�n agrega datos al queue de forma segura para multithread  												*/
	/************************************************************************************************************************/
	void push(Data const& data)
	{
		{
			ScopedCriticalSection locker(m_cs);
			the_queue.push(data);
		}
		PulseEvent(m_dataPushed);
	}

	/************************************************************************************************************************/
	/* Vacia el queue de forma segura para multithread                      												*/
	/************************************************************************************************************************/
	bool empty() const
	{
		ScopedCriticalSection locker(m_cs);
		return the_queue.empty();
	}

	/************************************************************************************************************************/
	/* Intenta hacer un pop al queue, si la lista est� vac�a regresar� false, si tiene un objeto lo copiar� en popped_value	*/
	/************************************************************************************************************************/
	bool try_pop(Data& popped_value)
	{
		ScopedCriticalSection locker(m_cs);
		if(the_queue.empty())
		{
			return false;
		}

		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	/************************************************************************************************************************/
	/* Esta funci�n hace un pop, esperando de ser necesario hasta que se agregue un objeto al queue							*/
	/************************************************************************************************************************/
	void wait_and_pop(Data& popped_value)
	{
		ScopedCriticalSection locker(m_cs);
		while(the_queue.empty())
		{
			WaitForSingleObject(m_dataPushed);
		}

		popped_value = the_queue.front();
		the_queue.pop();
	}
};