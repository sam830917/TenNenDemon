#include "Game/CutsceneDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/FileUtils.hpp"

STATIC std::map< std::string, CutsceneDefinition* > CutsceneDefinition::s_definitionMap;

STATIC void CutsceneDefinition::LoadDefinitions( const std::string& folderPath )
{
	Strings fileNames = GetFileNamesInFolder( folderPath, "*.xml" );

	for( int i = 0; i < (int)fileNames.size(); ++i )
	{
		const std::string& cutsceneFileName = fileNames[i];
		std::string cutsceneName = GetFileBaseName( cutsceneFileName );

		std::string cutsceneFilePath = folderPath + cutsceneFileName;
		XmlDocument cutsceneFileDoc;
		cutsceneFileDoc.LoadFile( &cutsceneFilePath[0] );

		CutsceneDefinition* newCutsceneDef = new CutsceneDefinition( *cutsceneFileDoc.RootElement(), cutsceneName );
		s_definitionMap[cutsceneName] = newCutsceneDef;
	}
}

CutsceneDefinition* CutsceneDefinition::GetDefinitions( const std::string& deinitionsName )
{
	if( s_definitionMap.count( deinitionsName ) > 0 )
	{
		return s_definitionMap.at( deinitionsName );
	}
	return nullptr;
}

CutsceneDefinition::CutsceneDefinition( const XmlElement& cutsceneDefElement, const std::string& name )
{
	m_name = name;
	const XmlElement* LinesElement = cutsceneDefElement.FirstChildElement( "Lines" );
	while( LinesElement )
	{
		std::string backgroundPath = ParseXmlAttribute( *LinesElement, "background", "" );
		Texture* bgTex = nullptr;
		if ( backgroundPath != "" )
		{
			bgTex = g_theRenderer->CreateOrGetTextureFromFile( backgroundPath.c_str() );
		}

		for( const XmlElement* line = LinesElement->FirstChildElement( "Line" ); line; line = line->NextSiblingElement( "Line" ) )
		{
			std::string characterImage = ParseXmlAttribute( *line, "characterImage", "" );
			Texture* characterTex = nullptr;
			if ( "" != characterImage )
			{
				characterTex = g_theRenderer->CreateOrGetTextureFromFile( characterImage.c_str() );
			}
			std::string lineStr = ParseXmlAttribute( *line, "line", "" );

			CutsceneLines* newLine = new CutsceneLines();
			newLine->m_background = bgTex;
			newLine->m_line = lineStr;
			newLine->m_characterImage = characterTex;
			newLine->m_characterName = ParseXmlAttribute( *line, "characterName", "" );
			m_lines.push_back( newLine );
		}
		LinesElement = LinesElement->NextSiblingElement( "Lines" );
	}
}

CutsceneDefinition::~CutsceneDefinition()
{
}
