#include "MapDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/EnvironmentEntityDefinition.hpp"
#include "Game/CutsceneDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

STATIC std::map< std::string, MapDefinition* > MapDefinition::s_definitionMap;

STATIC void MapDefinition::LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath )
{
	XmlDocument mapXmlDoc;
	mapXmlDoc.LoadFile( &deinitionsXmlFilePath[0] );
	XmlElement* mapDefsElement = mapXmlDoc.RootElement();
	if( mapDefsElement == nullptr )
	{
		g_theConsole->PrintString( Rgba8::RED, "WARNING: Map definitions file not found" );
		return;
	}

	XmlElement* legendElement = mapDefsElement->FirstChildElement( "Legend" );
	std::map<char, std::string> legendMap;
	InitialLegend( legendMap, *legendElement );

	XmlElement* mapDefElement = mapDefsElement->FirstChildElement("MapDefinition");
	while( mapDefElement )
	{
		MapDefinition* newMapDef = new MapDefinition( context, *mapDefElement, legendMap );
		s_definitionMap[newMapDef->m_name] = newMapDef;
		mapDefElement = mapDefElement->NextSiblingElement("MapDefinition");
	}
}

STATIC MapDefinition* MapDefinition::GetDefinitions( const std::string& mapName )
{
	if( s_definitionMap.count( mapName ) > 0 )
	{
		return s_definitionMap.at( mapName );
	}
	return nullptr;
}

STATIC Strings MapDefinition::GetAllMapNames()
{
	Strings mapNames;
	for( std::map< std::string, MapDefinition* >::iterator it = s_definitionMap.begin(); it != s_definitionMap.end(); ++it )
	{
		mapNames.push_back( it->first );
	}
	return mapNames;
}

STATIC const std::vector<MapDefinition*> MapDefinition::GetAllMapDefs()
{
	std::vector<MapDefinition*> mapDefs;

	for( std::map< std::string, MapDefinition* >::iterator it = s_definitionMap.begin(); it != s_definitionMap.end(); ++it )
	{
		mapDefs.push_back( it->second );
	}
	return mapDefs;
}

MapDefinition::MapDefinition( RenderContext* context, const XmlElement& mapDefElement, const std::map<char, std::string>& legendMap )
{
	m_name = ParseXmlAttribute( mapDefElement, "name", m_name );
	m_levelName = ParseXmlAttribute( mapDefElement, "level", m_levelName );
	m_dimensions = ParseXmlAttribute( mapDefElement, "dimensions", m_dimensions );
	m_mapStartPoint = ParseXmlAttribute( mapDefElement, "mapStartPoint", Vec2::ZERO );
	std::string bgImageText = ParseXmlAttribute( mapDefElement, "backgroundImage", "" );
	m_backgroundImage = context->CreateOrGetTextureFromFile( Stringf( "Data/Images/%s.png", bgImageText.c_str() ).c_str() );

	// Map Setup
	const XmlElement* mapSetUpElement = mapDefElement.FirstChildElement( "MapSetUp" );
	if ( mapSetUpElement )
	{
		const XmlElement* playerStartElement = mapSetUpElement->FirstChildElement( "PlayerStart" );
		m_playerStartIndex = ParseXmlAttribute( *playerStartElement, "pos", m_playerStartIndex );

		const XmlElement* petStartElement = mapSetUpElement->FirstChildElement( "PetStart" );
		m_petStartIndex = ParseXmlAttribute( *petStartElement, "pos", m_petStartIndex );

		const XmlElement* abilityLimitNumberElement = mapSetUpElement->FirstChildElement( "AbilityLimitNumber" );
		m_startAbilityLimitNumber = ParseXmlAttribute( *abilityLimitNumberElement, "number", m_startAbilityLimitNumber );

		const XmlElement* nextLevelElement = mapSetUpElement->FirstChildElement( "NextLevel" );
		if ( nextLevelElement )
		{
			m_nextLevelName = ParseXmlAttribute( *nextLevelElement, "name", m_nextLevelName );
		}
	}

	// Cutscenes
	const XmlElement* cutscenesElement = mapDefElement.FirstChildElement( "Cutscenes" );
	if( cutscenesElement )
	{
		const XmlElement* beforeLevelElement = cutscenesElement->FirstChildElement( "BeforeLevel" );
		if ( beforeLevelElement )
		{
			std::string beforeLevelStr = ParseXmlAttribute( *beforeLevelElement, "name", "" );
			m_beforeLevelCutscene = CutsceneDefinition::GetDefinitions( beforeLevelStr );
		}

		const XmlElement* afterLevelElement = cutscenesElement->FirstChildElement( "AfterLevel" );
		if ( afterLevelElement )
		{
			std::string afterLevelStr = ParseXmlAttribute( *afterLevelElement, "name", "" );
			m_afterLevelCutscene = CutsceneDefinition::GetDefinitions( afterLevelStr );
		}
	}

	// Map Rows
	const XmlElement* mapRowsElement = mapDefElement.FirstChildElement( "MapRows" );
	int tileY = m_dimensions.y - 1;
	for ( const XmlElement* mapRowElement = mapRowsElement->FirstChildElement("MapRow"); mapRowElement; mapRowElement = mapRowElement->NextSiblingElement("MapRow") )
	{
		std::string tiles = ParseXmlAttribute( *mapRowElement, "tiles", "" );
		if( tiles.length() != m_dimensions.x )
		{
			ERROR_AND_DIE( "MapRow length is not equal to map dimension!" );
		}
		for( int tileX = 0; tileX < m_dimensions.x; ++tileX )
		{
			char glyph = tiles[tileX];
			auto found = legendMap.find( glyph );
			if( found == legendMap.end() )
			{
				continue;
			}

			std::string name = found->second;
			eEntityType type = ENTITY_TYPE_BREAKABLE_OBJECT;
			if ( name == "Lava" )
			{
				type = ENTITY_TYPE_LAVA;
			}
			else if ( name == "UnbreakableRock" )
			{
				type = ENTITY_TYPE_UNBREAKABLE_OBJECT;
			}
			EnvironmentEntityDefinition* newEntityDef = new EnvironmentEntityDefinition( name, IntVec2( tileX, tileY ), type );
			m_environmentEntities.push_back( newEntityDef );
		}

		--tileY;
	}
}

MapDefinition::~MapDefinition()
{
}

STATIC void MapDefinition::InitialLegend( std::map<char, std::string>& out, const XmlElement& legendElement )
{
	for ( const XmlElement* entityElement = legendElement.FirstChildElement("Entity"); entityElement; entityElement = entityElement->NextSiblingElement("Entity") )
	{
		char glyph = ParseXmlAttribute( *entityElement, "glyph", '\0' );
		std::string name = ParseXmlAttribute( *entityElement, "name", "" );
		if ( name == "BreakableRock" )
		{
			out[glyph] = name;
		}
		else if( name == "Lava" )
		{
			out[glyph] = name;
		}
		else if( name == "UnbreakableRock" )
		{
			out[glyph] = name;
		}
	}
}
