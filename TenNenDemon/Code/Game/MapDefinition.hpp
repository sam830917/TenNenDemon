#pragma once
#include "Game/EntityDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <vector>

class Texture;
class RenderContext;
class EnvironmentEntityDefinition;
class CutsceneDefinition;

class MapDefinition
{
public:
	static std::map< std::string, MapDefinition* >	s_definitionMap;

	static void LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath );
	static MapDefinition* GetDefinitions( const std::string& mapName );

	static Strings GetAllMapNames();
	static const std::vector<MapDefinition*> GetAllMapDefs();

public:
	MapDefinition( RenderContext* context, const XmlElement& mapDefElement, const std::map<char, std::string>& legendMap );
	~MapDefinition();

	static void InitialLegend( std::map<char, std::string>& out, const XmlElement& legendElement );

public:
	std::string						m_name = "UNNAMED";
	std::string						m_levelName = "UNNAMED";
	std::string						m_nextLevelName = "";
	IntVec2							m_dimensions = IntVec2( 10, 10 );
	IntVec2							m_playerStartIndex = IntVec2::ZERO;
	IntVec2							m_petStartIndex = IntVec2(2,2);
	Vec2							m_mapStartPoint = Vec2::ZERO;
	Texture*						m_backgroundImage = nullptr;
	int								m_startAbilityLimitNumber = 5;
	std::vector<EnvironmentEntityDefinition*>	m_environmentEntities;
	CutsceneDefinition*				m_beforeLevelCutscene = nullptr;
	CutsceneDefinition*				m_afterLevelCutscene = nullptr;

};