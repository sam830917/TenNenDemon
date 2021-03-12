#include "Engine/Platform/WindowUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const void SetClipboard( const std::string& str )
{
	const size_t len = str.size() + 1;
	if( len <= 1 )
	{
		return;
	}

	HGLOBAL hMem =  GlobalAlloc( GMEM_MOVEABLE, len );
	memcpy( GlobalLock( hMem ), str.c_str(), len );
	GlobalUnlock( hMem );
	OpenClipboard( 0 );
	EmptyClipboard();
	SetClipboardData( CF_TEXT, hMem );
	CloseClipboard();
}

const std::string GetClipboard()
{
	std::string str;
	OpenClipboard( 0 );
	HANDLE hData = GetClipboardData( CF_TEXT );
	if( hData != nullptr )
	{
		LPCSTR lpData = (LPCSTR)GlobalLock( hData );
		if( lpData != nullptr )
		{
			str = lpData;
		}
		GlobalUnlock( hData );
	}
	CloseClipboard();

	return str;
}

