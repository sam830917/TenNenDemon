#pragma once
#include <string>
#include <vector>

typedef std::vector< std::string > Strings;

//void* FileRendToNewBuffer( std::string const& filename, size_t *out_size = nullptr );
std::string FileRend( std::string const& filename);
Strings GetFileNamesInFolder( const std::string& folderPath, const char* filePattern );
std::string GetFileBaseName( std::string const& filename );