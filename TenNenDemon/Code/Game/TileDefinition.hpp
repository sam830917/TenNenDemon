#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <map>

class TileDefinition
{
public:
	static std::map< std::string, TileDefinition* >	s_definitionMap;
	static void LoadDefinitions( const std::string& deinitionsXmlFilePath );
	static void InitialTileSpriteSheet( const XmlElement& tileDefsElement );

public:
	TileDefinition( const XmlElement& tileDefElement );
	~TileDefinition();	

	void		GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const;

public:
	std::string		m_name = "UNNAMED";
	Rgba8			m_tint;
	Rgba8			m_colorInMapImage;
	bool			m_allowsSight = true;
	bool			m_allowsWalk = true;
	bool			m_allowsSwim = false;
	bool			m_allowsFly = false;
	AABB2			m_UVs = AABB2::ZERO_TO_ONE;
};