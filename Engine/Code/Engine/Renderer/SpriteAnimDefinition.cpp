#include "SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, 
	float durationSeconds, SpriteAnimPlaybackType playbackType )
	:m_spriteSheet( sheet ),
	m_durationSeconds( durationSeconds ),
	m_playbackType( playbackType )
{
	if ( startSpriteIndex == endSpriteIndex )
	{
		m_spriteIndexes.push_back( startSpriteIndex );
		m_spriteIndexes.push_back( startSpriteIndex ); // Push this frame TWICE
	}
	else if ( startSpriteIndex < endSpriteIndex )
	{
		for( int spriteIndex = startSpriteIndex; spriteIndex <= endSpriteIndex; spriteIndex++ )
		{
			m_spriteIndexes.push_back( spriteIndex );
		}
	}
	else
	{
		for( int spriteIndex = endSpriteIndex; spriteIndex <= startSpriteIndex; spriteIndex++ )
		{
			m_spriteIndexes.push_back( spriteIndex );
		}
	}
}

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, const std::vector<int>& spriteIndexs, 
	float durationSeconds, SpriteAnimPlaybackType playbackType )
	:m_spriteSheet( sheet ),
	m_durationSeconds( durationSeconds ),
	m_playbackType( playbackType )
{
	m_spriteIndexes = spriteIndexs;
}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	if ( SpriteAnimPlaybackType::ONCE == m_playbackType )
	{
		int numFrames = (int) m_spriteIndexes.size();
		float oneFrameTakeTime = m_durationSeconds / (float) numFrames;
		int frameNum = (int) floorf( seconds / oneFrameTakeTime );
		frameNum = Clamp( frameNum, 0, numFrames - 1 );
		int spriteIndex = m_spriteIndexes[ frameNum ];

		return m_spriteSheet.GetSpriteDefinition( spriteIndex );
	}
	else if ( SpriteAnimPlaybackType::LOOP == m_playbackType )
	{
		int numFrames = (int) m_spriteIndexes.size();
		float oneFrameTakeTime = m_durationSeconds / (float) numFrames;
		int frameNum = (int) floorf( seconds / oneFrameTakeTime );
		int spriteIndex = m_spriteIndexes[ frameNum % numFrames ];

		return m_spriteSheet.GetSpriteDefinition( spriteIndex );
	}
	else if( SpriteAnimPlaybackType::PINGPONG == m_playbackType )
	{
		int numFrames = (int) m_spriteIndexes.size() * 2 - 1;
		float oneFrameTakeTime = m_durationSeconds / (float) numFrames;
		int frameNum = (int) floorf( seconds / oneFrameTakeTime );
		frameNum = frameNum % numFrames;

		if ( frameNum <= ( numFrames * 0.5f ) )
		{
			int spriteIndex = m_spriteIndexes[frameNum];
			return m_spriteSheet.GetSpriteDefinition( spriteIndex );
		}
		else
		{
			int spriteIndex = m_spriteIndexes[numFrames - frameNum];
			return m_spriteSheet.GetSpriteDefinition( spriteIndex );
		}
	}
	return m_spriteSheet.GetSpriteDefinition( 0 );
}
