#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <vector>

#define UNUSED(x) (void)(x);
typedef std::vector< std::string > Strings;
typedef unsigned int uint;
//-----------------------------------------------------------------------------------------------
//                                     String Combination
//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string Stringv( char const* format, va_list args );

Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn );
Strings SplitStringOnDelimiterWithoutEmpty( const std::string& originalString, char delimiterToSplitOn );

//-----------------------------------------------------------------------------------------------
//                                          Char Utils
//-----------------------------------------------------------------------------------------------
inline bool IsUppercase( char const c ) { return ((c >= 'A') && (c <= 'Z')); }
inline bool IsLowercase( char const c ) { return ((c >= 'a') && (c <= 'z')); }
inline bool IsWhitespace( char const c ) { return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r')); }
inline bool IsNewLine( char const c ) { return ((c == '\t') || (c == '\n') || (c == '\r')); }
inline bool IsBracket( char const c ) { return ((c == '(') || (c == '{') || (c == '[') || (c == '<')); }
inline bool IsQuote( char const c ) { return ((c == '\'') || (c == '\"')); }
inline bool IsAlpha( char const c ) { return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')); }
inline bool IsNumeric( char const c ) { return ((c >= '0') && (c <= '9')); }
inline bool IsAlphaNumeric( char const c ) { return IsAlpha( c ) || IsNumeric( c ); }
inline bool IsHex( char const c ) { return IsNumeric( c ) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')); }

inline char ToUpper( char const c ) { return ((c >= 'a') && (c <= 'z')) ? (c & ~(1 << 5)) : c; }
inline char ToLower( char const c ) { return ((c >= 'A') && (c <= 'Z')) ? (c | (1 << 5)) : c; }

inline uint DecimalValue( char const c ) { return c - '0'; }
inline uint HexValue( char const c ) { return (c >= 'a') ? (c - 'a' + 10U) : ((c >= 'A') ? (c - 'A' + 10U) : (c - '0')); }

bool IsCharacterAny( char const c, char const* str );
uint StringCountCharacter( char const* str, char const c );
int GetIntFromString( char const* str, int defualt = 0 );

//-----------------------------------------------------------------------------------------------
//                                         CStr Untility
//-----------------------------------------------------------------------------------------------
size_t StringGetLength( char const* cstr );
size_t StringGetSize( char const* cstr );

void ToLower( char* buffer );
void ToUpper( char* buffer );

uint Unescape( char* buffer );

//-----------------------------------------------------------------------------------------------
//                              returns number of bytes copied, including NULL
//-----------------------------------------------------------------------------------------------
size_t StringCopy( char* dst, size_t dst_size, char const* src );

//-----------------------------------------------------------------------------------------------
//                                          String Checks
//-----------------------------------------------------------------------------------------------
int StringCompare( char const* a, char const* b );
int StringCompare( char const* a, char const* b, size_t len );
int StringCompareI( char const* a, char const* b );
int StringCompareI( char const* a, char const* b, size_t len );

bool StringIsWhitespace( char const* s );
inline bool StringIsNullOrEmpty( char const* s ) { return ((s == nullptr) || (*s == NULL)); }
inline bool StringEquals( char const* a, char const* b ) { return 0 == StringCompare( a, b ); }
inline bool StringEquals( char const* a, char const* b, size_t len ) { return 0 == StringCompare( a, b, len ); }
inline bool StringEqualsI( char const* a, char const* b ) { return 0 == StringCompareI( a, b ); }
inline bool StringEqualsI( char const* a, char const* b, size_t len ) { return 0 == StringCompareI( a, b, len ); }

//-----------------------------------------------------------------------------------------------
//                                             Search 
//-----------------------------------------------------------------------------------------------
char const* StringGetEnd( char const* str );    // returns the pointer at the ending NULL
uint StringFindFirstIndex( char const* str, char const search );
char const* StringFindFirst( char const* str, char const search );
char const* StringFindFirst( char const* str, char const* search );
char const* StringFindFirstI( char const* str, char const* search );
char const* StringFindLast( char const* str, char const search );
char const* StringFindLast( char const* str, char const* search );
char const* StringFindLastI( char const* str, char const* search );
uint StringFindFirstWhitespaceIndex( char const* str );

char const* FindNextWord( char const* iter );
char const* FindPreviousWord( char const* iter, char const* start );

//-----------------------------------------------------------------------------------------------
//                                           String Convert 
//-----------------------------------------------------------------------------------------------
char const* Parse( float* out, char const* str );
char const* Parse( bool* out, char const* str );
char const* Parse( int* out, char const* str );
char const* Parse( char* out, char const* str );
char const* Parse( std::string* out, char const* str );
char const* Parse( Rgba8* out, char const* str );
char const* Parse( Vec2* out, char const* str );
char const* Parse( Vec3* out, char const* str );
char const* Parse( IntVec2* out, char const* str );

template <typename T>
inline char const* Parse( T* out, char const* str ) { return nullptr; }

template <typename T>
T StringConvert( char const* str, T const& defValue )
{
	T out;
	char const* adv = Parse( &out, str );
	if( adv == nullptr )
	{
		return defValue;
	}
	else
	{
		return out;
	}
}

std::string ToString( const float& val );
std::string ToString( const bool& val );
std::string ToString( const int& val );
std::string ToString( const char& val );
std::string ToString( const std::string& val );
std::string ToString( const Rgba8& val );
std::string ToString( const Vec2& val );
std::string ToString( const Vec3& val );
std::string ToString( const IntVec2& val );

template <typename T>
std::string ToString( const T& val ) { UNUSED(val); return ""; }