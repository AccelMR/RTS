/********************************************************************
	created:	2014/01/23
	created:	23:1:2014   2:11
	filename: 	geVector2D.h
	author:		Samuel Prince
	
	purpose:	Definici�n de una estructura para manejo de vectores 2D
*********************************************************************/
#pragma once

/************************************************************************/
/* Incluimos archivos necesarios para la compilaci�n                    */
/************************************************************************/

/************************************************************************************************************************/
/* Estructura para el control de vectores 2D                            												*/
/************************************************************************************************************************/
struct geVector2D
{
	/************************************************************************************************************************/
	/* Declaraci�n de variables (simplemente son dos flotantes)             												*/
	/************************************************************************************************************************/
	float	X,
			Y;

	/************************************************************************************************************************/
	/* Constructores                                                        												*/
	/************************************************************************************************************************/
	FORCEINLINE geVector2D();
	FORCEINLINE geVector2D(float InX,float InY);
	
	/************************************************************************************************************************/
	/* Declaraci�n de operadores aritm�ticos                                 												*/
	/************************************************************************************************************************/
	FORCEINLINE geVector2D operator+( const geVector2D& V ) const;		//Adici�n
	FORCEINLINE geVector2D operator-( const geVector2D& V ) const;		//Substracci�n
	FORCEINLINE geVector2D operator*( float Scale ) const;				//Multiplicaci�n (escalar)
	FORCEINLINE geVector2D operator*( const geVector2D& V ) const;		//Multiplicaci�n (vector)
	geVector2D operator/( float Scale ) const;							//Divisi�n (escalar)
	geVector2D operator/( const geVector2D& V ) const;					//Divisi�n (vector)
	FORCEINLINE geVector2D operator-() const;							//Inverso aditivo

	FORCEINLINE float operator|( const geVector2D& V) const;			//Dot Product
	FORCEINLINE float operator^( const geVector2D& V) const;			//Cross Product

	FORCEINLINE static float DotProduct( const geVector2D& A, const geVector2D& B );	//Calcula el DotProduct de dos vectores

	/************************************************************************************************************************/
	/* Declaraci�n de operadores l�gicos                                     												*/
	/************************************************************************************************************************/
	bool operator==( const geVector2D& V ) const;						//Igual a
	bool operator!=( const geVector2D& V ) const;						//Diferente a
	bool operator<( const geVector2D& Other ) const;					//Menor que
	bool operator>( const geVector2D& Other ) const;					//Mayor que
	bool operator<=( const geVector2D& Other ) const;					//Menor o igual a
	bool operator>=( const geVector2D& Other ) const;					//Mayor o igual a
	bool Equals(const geVector2D& V, float Tolerance) const;			//Compara si son "iguales" manejando una toleracia

	/************************************************************************************************************************/
	/* Declaraci�n de operadores de asignaci�n compuesta                    												*/
	/************************************************************************************************************************/
	FORCEINLINE geVector2D operator+=( const geVector2D& V );			
	FORCEINLINE geVector2D operator-=( const geVector2D& V );
	FORCEINLINE geVector2D operator*=( float Scale );
	geVector2D operator/=( float V );
	geVector2D operator*=( const geVector2D& V );
	geVector2D operator/=( const geVector2D& V );
	
	/************************************************************************************************************************/
	/* Operadores de acceso a componentes por �ndice                        												*/
	/************************************************************************************************************************/
	float& operator[]( int32 Index );
	float operator[]( int32 Index ) const;

	/************************************************************************************************************************/
	/* Declaraci�n de funciones                                             												*/
	/************************************************************************************************************************/
	void Set( float InX, float InY );								//Establece valores a los componentes
	float GetMax() const;											//Regresa la m�xima de las coordenadas del vector
	float GetAbsMax() const;										//Regresa la m�xima absoluta de las coordenadas del vector
	float GetMin() const;											//Regresa la m�nima de las coordenadas del vector
	float Size() const;												//Regresa la longitud de este vector
	float SizeSquared() const;										//Regresa la longitud cuadr�tica de este vector
	void Normalize(float Tolerance=SMALL_NUMBER);					//Normaliza el vector (lo convierte en un vector de tama�o 1)
	bool IsNearlyZero(float Tolerance=KINDA_SMALL_NUMBER) const;	//Revisa si el vector es cercano a Zero dentro de una tolerancia especificada
	bool IsZero() const;											//Regresa true si el vector es exactamente zero, false en cualquier otro caso
	geVector2D SafeNormal(float Tolerance=SMALL_NUMBER) const;		//Regresa una copia normalizada del vector si es seguro, (0,0) en cualquier otro caso

	float& Component( int32 Index );								//Regresa una referencia al componente especificado
	float Component( int32 Index ) const;							//Regresa una copia del componente especificado

	geString ToString() const;										//Regresa los valores de la estructura formateados en texto

	FORCEINLINE bool ContainsNaN() const							//Regresa true si alguno de los par�metros del vector no han sido inicializados
	{
		return (IsNaN(X) || !IsFinite(X) || IsNaN(Y) || !IsFinite(Y));
	}

	static const geVector2D ZeroVector;								//Constante global de un vector 2D con valor zero
	static const geVector2D UnitVector;								//Constante global de un vector 2D con valor unitario
};

/************************************************************************************************************************/
/* Implementaci�n de funciones del vector                               												*/
/************************************************************************************************************************/
FORCEINLINE geVector2D::geVector2D()
{//Constructor standard

}

FORCEINLINE geVector2D::geVector2D(float InX,float InY) : X(InX), Y(InY)
{//Constructor con par�metros

}

/************************************************************************************************************************/
/* Implementaci�n de operadores aritm�ticos                             												*/
/************************************************************************************************************************/
FORCEINLINE geVector2D operator*( float Scale, const geVector2D& V )
{//Operador global de multiplicaci�n por vector ( en caso de que se ejecute en orden de float*geVector2D )
	return V.operator*( Scale );
}

FORCEINLINE geVector2D geVector2D::operator+( const geVector2D& V ) const
{//Suma este vector con el indicado como par�metro y regresa un nuevo vector convinado
	return geVector2D( X + V.X, Y + V.Y );
}

FORCEINLINE geVector2D geVector2D::operator-( const geVector2D& V ) const
{//Resta este vector con el indicado como par�metro y regresa un nuevo vector convinado
	return geVector2D( X - V.X, Y - V.Y );
}

FORCEINLINE geVector2D geVector2D::operator*( float Scale ) const
{//Multiplica este vector con la escala indicada como par�metro y regresa un nuevo vector convinado
	return geVector2D( X * Scale, Y * Scale );
}

FORCEINLINE geVector2D geVector2D::operator/( float Scale ) const
{//Divide este vector con la escala indicada como par�metro y regresa un nuevo vector convinado
	const float RScale = 1.f/Scale;
	return geVector2D( X * RScale, Y * RScale );
}

FORCEINLINE geVector2D geVector2D::operator*( const geVector2D& V ) const
{//Multiplica este vector con otro indicado como par�metro y regresa un nuevo vector convinado
	return geVector2D( X * V.X, Y * V.Y );
}

FORCEINLINE geVector2D geVector2D::operator/( const geVector2D& V ) const
{//Divide este vector con otro indicado como par�metro y regresa un nuevo vector convinado
	return geVector2D( X / V.X, Y / V.Y );
}

FORCEINLINE geVector2D geVector2D::operator-() const
{//Regresa un nuevo vector con el inverso aditivo de este
	return geVector2D( -X, -Y );
}

FORCEINLINE float geVector2D::operator|( const geVector2D& V) const
{//Dot Product
	return X*V.X + Y*V.Y;
}

FORCEINLINE float geVector2D::operator^( const geVector2D& V) const
{//Cross Product
	return X*V.Y - Y*V.X;
}

FORCEINLINE float geVector2D::DotProduct( const geVector2D& A, const geVector2D& B )
{//Regresa el Dot Product de dos diferentes vectores
	return A | B;
}

/************************************************************************************************************************/
/* Implementaci�n de operadores l�gicos                                 												*/
/************************************************************************************************************************/
FORCEINLINE bool geVector2D::operator==( const geVector2D& V ) const
{
	return X==V.X && Y==V.Y;
}

FORCEINLINE bool geVector2D::operator!=( const geVector2D& V ) const
{
	return X!=V.X || Y!=V.Y;
}

FORCEINLINE bool geVector2D::operator<( const geVector2D& Other ) const
{
	return X < Other.X && Y < Other.Y;
}

FORCEINLINE bool geVector2D::operator>( const geVector2D& Other ) const
{
	return X > Other.X && Y > Other.Y;
}

FORCEINLINE bool geVector2D::operator<=( const geVector2D& Other ) const
{
	return X <= Other.X && Y <= Other.Y;
}

FORCEINLINE bool geVector2D::operator>=( const geVector2D& Other ) const
{
	return X >= Other.X && Y >= Other.Y;
}

FORCEINLINE bool geVector2D::Equals(const geVector2D& V, float Tolerance) const
{//Revisa si la diferencia entre este y otro vector es menor que la Tolerancia, si es as�, los considera "iguales"
	return Abs(X-V.X) < Tolerance && Abs(Y-V.Y) < Tolerance;
}

/************************************************************************************************************************/
/* Implementaci�n de operadores de asignaci�n compuesta                 												*/
/************************************************************************************************************************/
FORCEINLINE geVector2D geVector2D::operator+=( const geVector2D& V )
{
	X += V.X; Y += V.Y;
	return *this;
}

FORCEINLINE geVector2D geVector2D::operator-=( const geVector2D& V )
{
	X -= V.X; Y -= V.Y;
	return *this;
}

FORCEINLINE geVector2D geVector2D::operator*=( float Scale )
{
	X *= Scale; Y *= Scale;
	return *this;
}

FORCEINLINE geVector2D geVector2D::operator/=( float V )
{
	const float RV = 1.f/V;
	X *= RV; Y *= RV;
	return *this;
}

FORCEINLINE geVector2D geVector2D::operator*=( const geVector2D& V )
{
	X *= V.X; Y *= V.Y;
	return *this;
}

FORCEINLINE geVector2D geVector2D::operator/=( const geVector2D& V )
{
	X /= V.X; Y /= V.Y;
	return *this;
}

/************************************************************************************************************************/
/* Implementaci�n de operadores de acceso a componentes                 												*/
/************************************************************************************************************************/
FORCEINLINE float& geVector2D::operator[]( int32 Index )
{
	GEE_ASSERT(Index>=0 && Index<2);
	return ((Index == 0) ? X : Y);
}

FORCEINLINE float geVector2D::operator[]( int32 Index ) const
{
	GEE_ASSERT(Index>=0 && Index<2);
	return ((Index == 0) ? X : Y);
}

FORCEINLINE void geVector2D::Set( float InX, float InY )
{
	X = InX;
	Y = InY;
}

FORCEINLINE float geVector2D::GetMax() const
{
	return Max(X, Y);
}

FORCEINLINE float geVector2D::GetAbsMax() const
{
	float A = Abs(X);
	float B = Abs(Y);
	return Max(A, B);
}

FORCEINLINE float geVector2D::GetMin() const
{
	return Min(X, Y);
}

FORCEINLINE float geVector2D::Size() const
{
	return Sqrt( X*X + Y*Y);
}

FORCEINLINE float geVector2D::SizeSquared() const
{
	return X*X + Y*Y;
}

FORCEINLINE void geVector2D::Normalize(float Tolerance)
{
	const float SquareSum = X*X + Y*Y;
	if( SquareSum > Tolerance )
	{
		const float Scale = InvSqrt(SquareSum);
		X *= Scale;
		Y *= Scale;
		return;
	}

	X = 0.0f;
	Y = 0.0f;
}

FORCEINLINE bool geVector2D::IsNearlyZero(float Tolerance) const
{
	return Abs(X)<Tolerance && Abs(Y)<Tolerance;
}

FORCEINLINE bool geVector2D::IsZero() const
{
	return X==0.f && Y==0.f;
}

FORCEINLINE float& geVector2D::Component( int32 Index )
{
	return (&X)[Index];
}

FORCEINLINE float geVector2D::Component( int32 Index ) const
{
	return (&X)[Index];
}

FORCEINLINE geString geVector2D::ToString() const
{
	geString vectorString;
	vectorString = TEXT("X=") + ToStr(X) + TEXT(" Y=0") + ToStr(Y);

	return vectorString;
}
