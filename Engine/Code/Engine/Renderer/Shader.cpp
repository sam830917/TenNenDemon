#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/ErrorShader.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <stdio.h>
#include <d3dcompiler.h>
#include "..\Core\FileUtils.hpp"

void* FileRendToNewBuffer( std::string const& filename, size_t *out_size = nullptr )
{
	FILE* fp = nullptr;
	fopen_s( &fp, filename.c_str(), "r" );
	if ( fp == nullptr )
	{
		return nullptr;
	}

	// get the size of the file
	fseek( fp, 0, SEEK_END );
	long file_size = ftell(fp);

	unsigned char* buffer = new unsigned char[file_size + 1];
	if ( nullptr != buffer )
	{
		fseek( fp, 0, SEEK_SET );
		size_t bytes_read = fread( buffer, 1, (size_t)file_size, fp );
		buffer[bytes_read] = NULL;

		if( out_size != nullptr )
		{
			*out_size = (size_t)file_size;
		}
	}

	fclose( fp );

	return buffer;
}

Shader::Shader( RenderContext* context )
	:m_owner(context)
{
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
}

bool Shader::CreateFromFile( std::string const& filename )
{
	size_t file_size = 0;
	void* source = FileRendToNewBuffer( filename, &file_size );
	if (source == nullptr)
	{
		return false;
	}
	m_vertexStage.Complie( m_owner, filename, source, file_size, SHADER_STAGE_VERTEX );
	m_fragmentStage.Complie( m_owner, filename, source, file_size, SHADER_STAGE_FRAGMENT );

	delete[] source;
	m_fileName = filename;

	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout()
{
	if (nullptr != m_inputLayout)
	{
		return m_inputLayout;
	}

	return GetOrCreateInputLayout( Vertex_PCU::LAYOUT );
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout( const buffer_attribute_t* layout )
{
	DX_SAFE_RELEASE( m_inputLayout );

	int length = 0;
	while ( layout[++length].type != BUFFER_FORMAT_EMPTY ) {}

	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDescriptions;
	vertexDescriptions.reserve( length );

	for( int i = 0; i < length; i++ )
	{
		const buffer_attribute_t& bufferAttribute = layout[i];
		D3D11_INPUT_ELEMENT_DESC vertexDesc;

		vertexDesc.SemanticName				= bufferAttribute.name.c_str();
		vertexDesc.SemanticIndex			= 0;
		vertexDesc.InputSlot				= 0;
		vertexDesc.AlignedByteOffset		= bufferAttribute.offset;
		vertexDesc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		vertexDesc.InstanceDataStepRate		= 0;

		switch( bufferAttribute.type )
		{
		case BUFFER_FORMAT_VEC2:			vertexDesc.Format = DXGI_FORMAT_R32G32_FLOAT;		break;
		case BUFFER_FORMAT_VEC3:			vertexDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;	break;
		case BUFFER_FORMAT_R8G8B8A8_UNORM:	vertexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	break;
		default:	GUARANTEE_OR_DIE( false, "Unknown buffer format!" );	break;
		}
		vertexDescriptions.push_back(vertexDesc);
	}

	ID3D11Device* device = m_owner->m_device;
	device->CreateInputLayout(
		&vertexDescriptions[0], length,
		m_vertexStage.GetByteCode(), m_vertexStage.GetByteCodeLength(),
		&m_inputLayout );

	return m_inputLayout;
}

static char const* GetDefaultEntryPointForStage( eShaderType type )
{
	switch( type )
	{
	case SHADER_STAGE_VERTEX:	return "VertexFunction";
	case SHADER_STAGE_FRAGMENT: return "FragmentFunction";
	default: GUARANTEE_OR_DIE( false, "Bad Stage" );
	}
}

//------------------------------------------------------------------------
// what version of the shader language do we want to use - similar to say, C++11 vs C++14
static char const* GetShaderModelForStage( eShaderType type )
{
	switch( type ) {
	case SHADER_STAGE_VERTEX: return "vs_5_0";
	case SHADER_STAGE_FRAGMENT: return "ps_5_0";
	default: GUARANTEE_OR_DIE( false, "Unknown shader stage" );
	}
}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_byteCode);
	DX_SAFE_RELEASE(m_handle);
}

bool ShaderStage::Complie( RenderContext* ctx, std::string const& filename, void const* source, size_t const sourceByteLen, eShaderType stage )
{
	char const* entrypoint = GetDefaultEntryPointForStage( stage );
	char const* shaderModel = GetShaderModelForStage( stage );

	DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
	compileFlags |= D3D11_CREATE_DEVICE_DEBUG;
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;

	HRESULT hr = ::D3DCompile( source,
		sourceByteLen,						// plain text source code
		filename.c_str(),                   // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		shaderModel,                        // Compile Target (MSDN - "Specifying Compiler Targets")
		compileFlags,                       // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&byteCode,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors );                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr))
	{
		if( errors != nullptr ) {
			char* error_string = (char*)errors->GetBufferPointer();
			std::string errorMessage = std::string(error_string);
			g_theConsole->PrintString( Rgba8::RED, errorMessage );
			//DebuggerPrintf( "Failed to compile [%s].  Compiler gave the following output;\n%s",
			//	filename.c_str(),
			//	error_string );
		}
		//DEBUGBREAK();
		byteCode = nullptr;
		errors = nullptr;
		hr = ::D3DCompile( &errorShader[0],
			errorShader.size(),
			nullptr,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint,
			shaderModel,
			compileFlags,
			0,
			&byteCode,
			&errors );
	}
	ID3D11Device* device = ctx->m_device;
	void const* byteCodePtr = byteCode->GetBufferPointer();
	size_t byteCodeSize = byteCode->GetBufferSize();
	switch( stage )
	{
	case SHADER_STAGE_VERTEX:
		hr = device->CreateVertexShader( byteCodePtr, byteCodeSize, nullptr, &m_vs );
		GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
		break;
	case SHADER_STAGE_FRAGMENT:
		hr = device->CreatePixelShader( byteCodePtr, byteCodeSize, nullptr, &m_fs );
		GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
		break;
	default: GUARANTEE_OR_DIE( false, "Unimplemented stage." ); break;
	}

	DX_SAFE_RELEASE(errors);

	if (stage == SHADER_STAGE_VERTEX)
	{
		m_byteCode = byteCode;
	}
	else
	{
		DX_SAFE_RELEASE(byteCode);
		m_byteCode = nullptr;
	}

	m_type = stage;

	return IsValid();
}

void const* ShaderStage::GetByteCode() const
{
	return m_byteCode->GetBufferPointer();
}

size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}
