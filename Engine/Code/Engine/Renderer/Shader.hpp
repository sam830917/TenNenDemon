#pragma once
#include <string>

class RenderContext;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct buffer_attribute_t;

enum eShaderType
{
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_FRAGMENT,
};

class ShaderStage
{
public:
	~ShaderStage();

	bool Complie( RenderContext* ctx,
		std::string const& filename,
		void const* source,
		size_t const sourceByteLen,
		eShaderType stage );
	bool IsValid() const { return (nullptr != m_handle); }

	void const* GetByteCode() const;
	size_t GetByteCodeLength() const;

public:
	eShaderType m_type;
	ID3D10Blob* m_byteCode = nullptr;
	union {
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_fs;
	};
};

class Shader
{
public:
	Shader( RenderContext* context );
	~Shader();
	bool CreateFromFile( std::string const& filename );
	const char* GetFilePath() const { return m_fileName.c_str(); }

	ID3D11InputLayout* GetOrCreateInputLayout();
	ID3D11InputLayout* GetOrCreateInputLayout( const buffer_attribute_t* layout );

public:
	RenderContext* m_owner = nullptr;
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;

	ID3D11InputLayout* m_inputLayout = nullptr;
	std::string m_fileName;
};