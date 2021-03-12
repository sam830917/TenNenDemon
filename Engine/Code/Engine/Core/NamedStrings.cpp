#include "NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <math.h>

NamedStrings::NamedStrings()
{
}

bool NamedStrings::PopulateFromXmlFile( const char* xmlFilePath )
{
	XmlDocument xmlDocument;
	xmlDocument.LoadFile( xmlFilePath );
	if( xmlDocument.ErrorID() != tinyxml2::XML_SUCCESS )
	{
		return false;
	}
	
	XmlElement* rootElement = xmlDocument.RootElement();
	if( !rootElement )
	{
		return false;
	}
	
	PopulateFromXmlElementAttributes( *rootElement );
	return true;
}

void NamedStrings::PopulateFromXmlElementAttributes( const XmlElement& element )
{
	const XmlAttribute* attribute = element.FirstAttribute();
	while( attribute )
	{
		m_keyValuePairs[attribute->Name()] = attribute->Value();
		attribute = attribute->Next();
	}
}

void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

bool NamedStrings::HasValue( const std::string& keyName )
{
	return m_keyValuePairs.find( keyName ) != m_keyValuePairs.end();
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	bool value = defaultValue;
	if ( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		if( str == "true" )
		{
			value = true;
		}
		else if( str == "false" )
		{
			value = false;
		}
	}

	return value;
}

int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	int value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value = atoi( &str[0] );
	}

	return value;
}

float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	float value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value = (float) atof( &str[0] );
	}

	return value;
}

std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::string value = defaultValue;
	if ( m_keyValuePairs.count( keyName ) > 0 )
	{
		value = m_keyValuePairs.at( keyName );
	}

	return value;
}

std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		value = m_keyValuePairs.at( keyName );
	}

	return value;
}

Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
	Rgba8 value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value.SetFromText( &str[0] );
	}

	return value;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	Vec2 value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value.SetFromText( &str[0] );
	}

	return value;
}

Vec3 NamedStrings::GetValue( const std::string& keyName, const Vec3& defaultValue ) const
{
	Vec3 value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value.SetFromText( &str[0] );
	}

	return value;
}

IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	IntVec2 value = defaultValue;
	if( m_keyValuePairs.count( keyName ) > 0 )
	{
		std::string str = m_keyValuePairs.at( keyName );
		value.SetFromText( &str[0] );
	}

	return value;
}

NamedStrings NamedStrings::PopulateForEventCommand( const std::string& commandInputValue )
{
	NamedStrings commandInputMap;
	if ( (int) commandInputValue.size() == 0 )
	{
		return commandInputMap;
	}
	Strings stringList = SplitStringOnDelimiter( commandInputValue, ',' );
	for ( int i = 0; i < (int) stringList.size(); i++ )
	{
		if ( (int) stringList[i].size() != 0 )
		{
			commandInputMap.m_keyValuePairs[stringList[i]] = "";
		}
	}

	return commandInputMap;
}
