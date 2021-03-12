#include "Engine/Core/NamedProperties.hpp"

bool NamedProperties::HasValue( const std::string& keyName )
{
	return m_keyValuePairs.find( keyName ) != m_keyValuePairs.end();
}

void NamedProperties::PopulateForEventCommand( const std::string& commandInputValue )
{
	if( (int)commandInputValue.size() == 0 )
	{
		return;
	}
	Strings stringList = SplitStringOnDelimiter( commandInputValue, ',' );
	for( int i = 0; i < (int)stringList.size(); i++ )
	{
		if( (int)stringList[i].size() != 0 )
		{
			std::string key = stringList[i];
			SetValue( key, "" );
		}
	}
}
