#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <string>
#include <map>

class ShaderState;
class Shader;
class Texture;
class Sampler;
class RenderBuffer;
class RenderContext;

class Material
{
public:
	static void LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath );
	static Material* GetDefinitions( const std::string& deinitionsName );

public:
	Material( RenderContext* context, const std::string& shaderStateName );
	Material( RenderContext* context, const XmlElement& definitionXmlElement );
	~Material();

	void UpdateUBOIfDirty();

	void SetTexture( uint slot, Texture* tex );
	void SetDiffuseTexture( Texture* tex );
	void SetDiffuseTexture( const std::string& fileName );
	void SetNormalTexture( Texture* tex );
	void SetNormalTexture( const std::string& fileName );
	void SetTexture( uint slot, const std::string& fileName );
	void SetSampler( uint slot, Sampler* sampler );
	void SetShaderStateByName( const std::string& name );

	void SetData( void const* data, size_t dataSize )
	{
		m_uboCPUData.resize( dataSize );
		memcpy( &m_uboCPUData[0], data, dataSize );
		m_uboIsDirty = true;
	}

	template <typename UBO_STRUCT_TYPE>
	void SetData( UBO_STRUCT_TYPE const& data )
	{
		SetData( &data, sizeof( UBO_STRUCT_TYPE ) );
	}

	template <typename UBO_STRUCT_TYPE>
	UBO_STRUCT_TYPE* GetDataAs()
	{
		m_uboIsDirty = true;
		if( m_uboCPUData.size() == sizeof( UBO_STRUCT_TYPE ) ) {
			return (UBO_STRUCT_TYPE*)&m_uboCPUData[0];
		}
		else {
			m_uboCPUData.resize( sizeof( UBO_STRUCT_TYPE ) );
			UBO_STRUCT_TYPE* retPtr = (UBO_STRUCT_TYPE*)&m_uboCPUData[0];
			new (retPtr) UBO_STRUCT_TYPE();

			return retPtr;
		}
	}

public:
	RenderContext* m_context = nullptr;
	ShaderState* m_shaderState = nullptr;

	std::string	m_name;
	bool m_uboIsDirty = false;

	// renderer specific
	Rgba8 m_tint = Rgba8::WHITE;
	float m_specularFactor = 1.f;
	float m_specularPower = 50.f;

	// general data (and maybe diffuse)
	std::map<uint, Texture*> m_textureMap;
	std::map<uint, Sampler*> m_samplerMap;

	std::vector<unsigned char> m_uboCPUData;
	RenderBuffer* m_ubo = nullptr;
};