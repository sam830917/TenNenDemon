#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <map>
#include <vector>
#include <string>

class Texture;

struct CutsceneLines
{
	std::string m_line = "";
	std::string m_characterName = "";
	Texture* m_background = nullptr;
	Texture* m_characterImage = nullptr;
};

class CutsceneDefinition
{
public:
	static void LoadDefinitions( const std::string& folderPath );
	static CutsceneDefinition* GetDefinitions( const std::string& deinitionsName );

public:
	CutsceneDefinition( const XmlElement& cutsceneDefElement, const std::string& name );
	~CutsceneDefinition();

private:
	static std::map< std::string, CutsceneDefinition* >	s_definitionMap;

public:
	std::string m_name = "";
	std::vector<const CutsceneLines*> m_lines;

};