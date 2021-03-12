#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <math.h>

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

const std::string Stringv( char const* format, va_list args )
{
	char buffer[1024];
	vsnprintf_s( buffer, 1024, format, args );
	return buffer;
}

Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn )
{
	Strings stringList;
	std::string str = "";
	for( int textIndex = 0;textIndex < originalString.size(); textIndex++ )
	{
		const char& text = originalString[textIndex];
		if ( text == delimiterToSplitOn )
		{
			stringList.push_back( str );
			str = "";
		}
		else
		{
			str = Stringf( "%s%c", str.c_str(), text );
		}
	}
	stringList.push_back( str );

	return stringList;
}

Strings SplitStringOnDelimiterWithoutEmpty( const std::string& originalString, char delimiterToSplitOn )
{
	Strings stringList;
	std::string str = "";
	for( int textIndex = 0; textIndex < originalString.size(); textIndex++ )
	{
		const char& text = originalString[textIndex];
		if( text == delimiterToSplitOn )
		{
			if ( "" != str )
			{
				stringList.push_back( str );
				str = "";
			}
		}
		else
		{
			str = Stringf( "%s%c", str.c_str(), text );
		}
	}
	if( "" != str )
	{
		stringList.push_back( str );
	}

	return stringList;
}

int GetIntFromString( char const* str, int defualt )
{
	int value = defualt;
	value = atoi( str );
	return value;
}

size_t StringGetLength( char const* cstr )
{
	size_t length = 0;
	while( cstr[length] != '\0' ) length++;
	return length;
}

size_t StringGetSize( char const* cstr )
{
	size_t size = 0;
	while( cstr[size] != '\0' ) size++;
	return size;
}

void ToLower( char* buffer )
{
	size_t length = StringGetLength(buffer);
	for ( int i = 0; i < length; ++i )
	{
		buffer[i] = ToLower( buffer[i] );
	}
}

void ToUpper( char* buffer )
{
	size_t length = StringGetLength( buffer );
	for( int i = 0; i < length; ++i )
	{
		buffer[i] = ToUpper( buffer[i] );
	}
}

char const* Parse( float* out, char const* str )
{
	try
	{
		*out = std::stof( str );
		return str;
	}
	catch (...)
	{
		return nullptr;
	}
}

char const* Parse( int* out, char const* str )
{
	try
	{
		*out = std::stoi( str );
		return str;
	}
	catch( ... )
	{
		return nullptr;
	}
}

char const* Parse( char* out, char const* str )
{
	*out = *str;
	return str;
}

char const* Parse( std::string* out, char const* str )
{
	*out = std::string(str);
	return str;
}

char const* Parse( Rgba8* out, char const* str )
{
	Strings stringList = SplitStringOnDelimiter( str, ',' );
	if( stringList.size() >= 3 )
	{
		int rNum = 255;
		int gNum = 255;
		int bNum = 255;
		int aNum = 255;
		Parse( &rNum, stringList[0].c_str() );
		Parse( &gNum, stringList[1].c_str() );
		Parse( &bNum, stringList[2].c_str() );
		if( stringList.size() == 4 )
		{
			Parse( &aNum, stringList[3].c_str() );
		}

		out->r = (unsigned char)rNum;
		out->g = (unsigned char)gNum;
		out->b = (unsigned char)bNum;
		out->a = (unsigned char)aNum;
		return str;
	}
	return nullptr;
}

char const* Parse( Vec3* out, char const* str )
{
	Strings stringList = SplitStringOnDelimiter( str, ',' );
	if( stringList.size() == 3 )
	{
		float xNum = 0.f;
		float yNum = 0.f;
		float zNum = 0.f;
		Parse( &xNum, stringList[0].c_str() );
		Parse( &yNum, stringList[1].c_str() );
		Parse( &zNum, stringList[2].c_str() );

		out->x = xNum;
		out->y = yNum;
		out->z = zNum;
		return str;
	}
	return nullptr;
}

char const* Parse( IntVec2* out, char const* str )
{
	Strings stringList = SplitStringOnDelimiter( str, ',' );
	if( stringList.size() == 2 )
	{
		int xNum = 0;
		int yNum = 0;
		Parse( &xNum, stringList[0].c_str() );
		Parse( &yNum, stringList[1].c_str() );

		out->x = xNum;
		out->y = yNum;
		return str;
	}
	return nullptr;
}

char const* Parse( bool* out, char const* str )
{
	std::string string = std::string(str);
	ToLower( &string[0] );
	if ( "false" == string )
	{
		*out = false;
		return str;
	}
	else if ( "true" == string )
	{
		*out = true;
		return str;
	}
	return nullptr;
}

char const* Parse( Vec2* out, char const* str )
{
	Strings stringList = SplitStringOnDelimiter( str, ',' );
	if( stringList.size() == 2 )
	{
		float xNum = 0.f;
		float yNum = 0.f;
		Parse( &xNum, stringList[0].c_str() );
		Parse( &yNum, stringList[1].c_str() );

		out->x = xNum;
		out->y = yNum;
		return str;
	}
	return nullptr;
}

std::string ToString( const float& val )
{
	return std::to_string(val);
}

std::string ToString( const bool& val )
{
	if ( val )
	{
		return std::string("true");
	}
	else
	{
		return std::string("false");
	}
}

std::string ToString( const int& val )
{
	return std::to_string(val);
}

std::string ToString( const std::string& val )
{
	return val;
}

std::string ToString( const Rgba8& val )
{
	return Stringf( "%f,%f,%f,%f", val.r, val.g, val.b, val.a );
}

std::string ToString( const Vec2& val )
{
	return Stringf( "%f,%f", val.x, val.y );
}

std::string ToString( const Vec3& val )
{
	return Stringf( "%f,%f,%f", val.x, val.y, val.z );
}

std::string ToString( const IntVec2& val )
{
	return Stringf( "%i,%i", val.x, val.y );
}

std::string ToString( const char& val )
{
	return std::string(&val);
}
