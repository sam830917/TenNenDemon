#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <io.h>

std::string FileRend( std::string const& filename )
{
	std::string name = std::string( filename );
	FILE* fp = nullptr;
	fopen_s( &fp, name.c_str(), "r" );
	if( fp == nullptr )
	{
		return "";
	}

	// get the size of the file
	fseek( fp, 0, SEEK_END );
	long file_size = ftell( fp );

	char* buffer = new char[file_size + 1];
	if( nullptr != buffer )
	{
		fseek( fp, 0, SEEK_SET );
		size_t bytes_read = fread( buffer, 1, (size_t)file_size, fp );
		buffer[bytes_read] = NULL;
	}
	fclose( fp );

	return buffer;
}

Strings GetFileNamesInFolder( const std::string& folderPath, const char* filePattern )
{
	Strings fileNamesInFolder;

#ifdef _WIN32
	std::string fileNamePattern = filePattern ? filePattern : "*";
	std::string filePath = folderPath + "/" + fileNamePattern;
	_finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( filePath.c_str(), &fileInfo );
	while ( searchHandle != -1 )
	{
		fileNamesInFolder.push_back( fileInfo.name );
		int errorCode = _findnext( searchHandle, &fileInfo );
		if ( errorCode != 0 )
		{
			break;
		}
	}
#else
	ERROR_AND_DIE( Stringf( "Not yet implemented for platform!" ) );
#endif

	return fileNamesInFolder;
}

std::string GetFileBaseName( std::string const& filename )
{
	Strings fullName = SplitStringOnDelimiter( filename, '.' );
	return fullName[0];
}
