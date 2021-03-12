#include "SpriteAnimSetDefinition.hpp"
#include "GameCommon.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition( const XmlElement* spriteAnimSetElement )
{
	IntVec2 spriteLayout = ParseXmlAttribute( *spriteAnimSetElement, "spriteLayout", IntVec2::ONE );
	float defaultFPS = ParseXmlAttribute( *spriteAnimSetElement, "fps", 10.f );
	std::string filePath = Stringf( "%s%s", g_gameConfigBlackboard.GetValue( "imgFilePrefix", "Data/Images/" ).c_str(), ParseXmlAttribute( *spriteAnimSetElement, "spriteSheet", "" ).c_str() );
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile( &filePath[0] );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture, spriteLayout );

	const XmlElement* spriteAnimElement = spriteAnimSetElement->FirstChildElement();
	while( spriteAnimElement )
	{
		std::string name = ParseXmlAttribute( *spriteAnimElement, "name", "UNNAMED" );
		Strings spriteIndexes = SplitStringOnDelimiter( ParseXmlAttribute( *spriteAnimElement, "spriteIndexes", "0" ), ',' );
		std::vector<int> spriteIndexList;
		for (std::string numStr : spriteIndexes)
		{
			spriteIndexList.push_back( atoi( &numStr[0] ) );
		}
		float fps = ParseXmlAttribute( *spriteAnimElement, "fps", defaultFPS );
		float frameSec = 1.f / fps;
		SpriteAnimDefinition* animDef = new SpriteAnimDefinition( *spriteSheet, spriteIndexList, (int) spriteIndexList.size() * frameSec, SpriteAnimPlaybackType::LOOP );
		m_spriteAnims[name] = animDef;
		spriteAnimElement = spriteAnimElement->NextSiblingElement();
	}
}

SpriteAnimSetDefinition::~SpriteAnimSetDefinition()
{
}
