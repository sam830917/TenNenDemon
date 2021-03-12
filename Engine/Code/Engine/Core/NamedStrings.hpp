#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include <map>
#include <string>

class NamedStrings
{
public:
	NamedStrings();

	bool			PopulateFromXmlFile( const char* xmlFilePath );
	void			PopulateFromXmlElementAttributes( const XmlElement& element );
	void			SetValue( const std::string& keyName, const std::string& newValue );
	bool			HasValue( const std::string& keyName );

	bool			GetValue( const std::string& keyName, bool defaultValue ) const;
	int				GetValue( const std::string& keyName, int defaultValue ) const;
	float			GetValue( const std::string& keyName, float defaultValue ) const;
	std::string		GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string		GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba8			GetValue( const std::string& keyName, const Rgba8& defaultValue ) const;
	Vec2			GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
	Vec3			GetValue( const std::string& keyName, const Vec3& defaultValue ) const;
	IntVec2			GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;
	//FloatRange	GetValue( const std::string& keyName, const FloatRange& defaultValue ) const;
	//IntRange		GetValue( const std::string& keyName, const IntRange& defaultValue ) const;

	static NamedStrings	PopulateForEventCommand( const std::string& commandInputValue );

private:
	std::map< std::string, std::string >	m_keyValuePairs;

};