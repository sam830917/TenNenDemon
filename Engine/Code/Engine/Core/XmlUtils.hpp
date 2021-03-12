#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <string>
#include "StringUtils.hpp"

typedef tinyxml2::XMLElement XmlElement;
typedef tinyxml2::XMLDocument XmlDocument;
typedef tinyxml2::XMLAttribute XmlAttribute;

int			ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue );
char		ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue );
bool		ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue );
float		ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue );
Rgba8		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue );
Vec2		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue );
Vec3		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue );
IntVec2		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue );
IntRange	ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange	ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue );
AABB2		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue );
Strings		ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValues );