/********************************************************************
	Created:	2014/02/01
	Filename:	String.h
	Author:		Samuel Prince

	Purpose:	Definici�n y Clases para manejo de Strings
*********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Definici�n de Constantes                                             												*/
/************************************************************************************************************************/
#define MAX_DIGITS_IN_INT 12					//Numero m�ximo de d�gitos en un int (-2147483647 = 11 d�gitos, +1 por el '\0')
typedef std::vector<geString> StringVec;		//Tipo definido para listas de strings

/************************************************************************************************************************/
/* Funciones utilitarias                                                												*/
/************************************************************************************************************************/
extern void RemoveFirstLine(geString &src, geString &result);						//Elimina caracteres hasta que encuentra el primer corte de l�nea ('\n')
extern void TrimLeft(geString &s);													//Elimina los espacios blancos al inicio del string
extern int CountLines(const geString &s);											//Cuenta el n�mero de l�neas en un bloque de texto
extern bool WildcardMatch(const TCHAR *pat, const TCHAR *str);						//Hace el cl�sico matching de patrones * & ? en un nombre de archivo: Esto case sensitive!
extern void StringToWideString(const std::string& source, std::wstring& outDest);	//Convierte un string regular a un wide string
extern void ReplaceAll(geString &str, const geString& from, const geString& to);	//Reemplaza todas las instancias de un string con otro

/************************************************************************************************************************/
/* Funciones �tiles durante el manejo de informaci�n de archivos														*/
/************************************************************************************************************************/
extern void CheckFolderSlashes(geString &inString, const geString &charDiagonal);				//Esta funci�n revisa las diagonales en una ruta de archivo y la regresa con la forma pedida

/************************************************************************************************************************/
/* Funciones de conversi�n de tipos de string                           												*/
/************************************************************************************************************************/
extern HRESULT AnsiToWideCch( UNICHAR* dest, const ANSICHAR* src, int32 charCount);	//ANSI a WIDE
extern HRESULT WideToAnsiCch( ANSICHAR* dest, const UNICHAR* src, int32 charCount);	//WIDE a ANSI
extern HRESULT GenericToAnsiCch( ANSICHAR* dest, const TCHAR* src, int32 charCount);	//Gen�rico a ANSI
extern HRESULT GenericToWideCch( UNICHAR* dest, const TCHAR* src, int32 charCount);	//Gen�rico a WIDE
extern HRESULT AnsiToGenericCch( TCHAR* dest, const ANSICHAR* src, int32 charCount);	//ANSI a Gen�rico
extern HRESULT WideToGenericCch( TCHAR* dest, const UNICHAR* src, int32 charCount);	//WIDE a Gen�rico

/************************************************************************************************************************/
/* Funciones de conversi�n de N�mero a String                           												*/
/************************************************************************************************************************/
extern geString ToStr(int32 num, int32 base = 10);			//Conversi�n de Int a String
extern geString ToStr(uint32 num, int32 base = 10);			//Conversi�n de Unsigned Int a String
extern geString ToStr(unsigned long num, int32 base = 10);	//Conversi�n de Unsigned Long a String
extern geString ToStr(float num);							//Conversi�n de Float a String
extern geString ToStr(double num);							//Conversi�n de Double a String
extern geString ToStr(bool val);							//Conversi�n de Boolean a String

/************************************************************************************************************************/
/* Funciones de conversi�n de WIDE a String y de String a WIDE          												*/
/************************************************************************************************************************/
extern std::string ws2s(const std::wstring& s);
extern std::wstring s2ws(const std::string &s);

/************************************************************************************************************************/
/* Funciones de conversi�n de WIDE y ANSI a Gen�rico          															*/
/************************************************************************************************************************/
extern geString s2gs(const std::string &s);
extern geString ws2gs(const std::wstring &s);

/************************************************************************************************************************/
/* Divide un String por el delimitador y lo convierte en un vector de string. Por ejemplo, si tenemos el string:		*/
/* std::string test("uno,dos,tres");																					*/
/* Puedes llamar a Split() as�:																							*/
/* Split(test, outVec, ',');																							*/
/* outVec contendr� al salir los siguientes valores:																	*/
/* "uno", "dos", "tres"																									*/
/* Parametros:																											*/
/*     str: String que ser� dividido																					*/
/*     vec: lista que contendr� los valores de salida																	*/
/*     delimeter: caracter que se utilizar� como delimitador															*/
/************************************************************************************************************************/
void Split(const geString& str, StringVec& vec, TCHAR delimiter);

#pragma warning(push)				//Guardamos la configuraci�n actual del compilador
#pragma warning(disable : 4311)		//Deshabilitamos la advertencia 4311 (intentar poner un puntero en un int de 32 bits)

/************************************************************************************************************************/
/* Definici�n de la clase HashedString, la cual retiene el string (ANSI) inicial, adem�s de un hash para facil			*/
/* referencia																											*/
/************************************************************************************************************************/
class HashedString
{
public:
	//Constructor
	explicit HashedString( const TCHAR *pIdentString ) : m_ident( hash_name(pIdentString) ), m_identStr( pIdentString )
	{
	}

	//Regresa el valor Hash de este objeto
	uint32 getHashValue( void ) const
	{
		return reinterpret_cast<uint32>( m_ident );
	}

	//Regresa el string original
	const geString & getStr() const
	{
		return m_identStr;
	}

	//Regresa el Hash de el String
	static void * hash_name( const TCHAR *pIdentStr );

	//Declaraci�n de operador Menor-Que
	bool operator< ( const HashedString &o ) const
	{
		bool r = ( getHashValue() < o.getHashValue() );
		return r;
	}

	//Declaraci�n de operador de comparaci�n
	bool operator== ( const HashedString &o ) const
	{
		bool r = ( getHashValue() == o.getHashValue() );
		return r;
	}

private:
	/************************************************************************************************************************/
	/* NOTA: m_ident es almacenado como un void* no como un int, para que en el debugger se muestre con valor hexadecimal	*/
	/* en lugar de valores enteros. Esto es un poco m�s representativo de lo que estamos haciendo aqu� y hace m�s facil	el	*/
	/* permitir que el c�digo externo asigne tipos de eventos como se desea													*/
	/************************************************************************************************************************/
	void*	 m_ident;
	geString m_identStr;
};

#pragma warning(pop)