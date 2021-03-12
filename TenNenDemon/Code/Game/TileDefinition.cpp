#include "TileDefinition.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/StringUtils.hpp"

SpriteSheet* g_tileSpriteSheet = nullptr;

STATIC std::map< std::string, TileDefinition* > TileDefinition::s_definitionMap;

STATIC void TileDefinition::LoadDefinitions( const std::string& deinitionsXmlFilePath )
{
	XmlDocument tileXmlDoc;
	tileXmlDoc.LoadFile( &deinitionsXmlFilePath[0] );
	XmlElement* tileDefsElement = tileXmlDoc.RootElement();
	if ( tileDefsElement == nullptr )
	{
		g_theConsole->PrintString( Rgba8::RED, "WARNING: Tile definitions file not found" );
		return;
	}
	InitialTileSpriteSheet( *tileDefsElement );
	XmlElement* tileDefElement = tileDefsElement->FirstChildElement();
	while ( tileDefElement )
	{
		TileDefinition* newTileDef = new TileDefinition( *tileDefElement );
		s_definitionMap[ newTileDef->m_name ] = newTileDef;
		tileDefElement = tileDefElement->NextSiblingElement();
	}
}

STATIC void TileDefinition::InitialTileSpriteSheet( const XmlElement& tileDefsElement )
{
	std::string spriteSheetFilePath = ParseXmlAttribute( tileDefsElement, "spriteSheet", "" );
	spriteSheetFilePath = Stringf( "%s%s", g_gameConfigBlackboard.GetValue( "imgFilePrefix", "Data/Images/" ).c_str(), spriteSheetFilePath.c_str() );
	IntVec2 spriteLayout = ParseXmlAttribute( tileDefsElement, "spriteLayout", IntVec2::ZERO );
	g_tileSpriteSheet = new SpriteSheet( *g_theRenderer->CreateOrGetTextureFromFile( &spriteSheetFilePath[0] ), spriteLayout );
}

TileDefinition::TileDefinition( const XmlElement& tileDefElement )
{
	m_name = ParseXmlAttribute( tileDefElement, "name", m_name );
	m_tint = ParseXmlAttribute( tileDefElement, "spriteTint", Rgba8::WHITE );
	m_colorInMapImage = ParseXmlAttribute( tileDefElement, "colorInMapImage", Rgba8( 0, 0, 0, 0 ) );
	m_allowsSight = ParseXmlAttribute( tileDefElement, "allowsSight", m_allowsSight );
	m_allowsWalk = ParseXmlAttribute( tileDefElement, "allowsWalk", m_allowsWalk );
	m_allowsSwim = ParseXmlAttribute( tileDefElement, "allowsSwim", m_allowsSwim );
	m_allowsFly = ParseXmlAttribute( tileDefElement, "allowsFly", m_allowsFly );

	//Parse spriteCoords and convert to UVs
	IntVec2 spriteCoords = ParseXmlAttribute( tileDefElement, "spriteCoords", IntVec2( 0, 0 ) );
	m_UVs = g_tileSpriteSheet->GetTextureCoordsForSpriteCoords( spriteCoords );
}

TileDefinition::~TileDefinition()
{
}

void TileDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins = m_UVs.mins;
	out_uvAtMaxs = m_UVs.maxs;
}
