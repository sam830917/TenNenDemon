#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

static std::map< std::string, Material* > s_definitionMap;

STATIC void Material::LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath )
{
	XmlDocument mapXmlDoc;
	mapXmlDoc.LoadFile( &deinitionsXmlFilePath[0] );
	XmlElement* mapDefsElement = mapXmlDoc.RootElement();
	if( mapDefsElement == nullptr )
	{
		g_theConsole->PrintString( Rgba8::RED, "WARNING: Material definitions file not found" );
		return;
	}
	XmlElement* shaderStateDefElement = mapDefsElement->FirstChildElement();
	while( shaderStateDefElement )
	{
		Material* newShaderStateDef = new Material( context, *shaderStateDefElement );
		if( s_definitionMap.count( newShaderStateDef->m_name ) > 0 )
		{
			ERROR_AND_DIE( Stringf( "ShaderState Definitions include the same name \"%s\"", newShaderStateDef->m_name.c_str() ) );
		}
		s_definitionMap[newShaderStateDef->m_name] = newShaderStateDef;
		shaderStateDefElement = shaderStateDefElement->NextSiblingElement();
	}
}

STATIC Material* Material::GetDefinitions( const std::string& deinitionsName )
{
	if( s_definitionMap.count( deinitionsName ) > 0 )
	{
		return s_definitionMap.at( deinitionsName );
	}
	return nullptr;
}

Material::Material( RenderContext* context, const XmlElement& definitionXmlElement )
{
	m_context = context;
	m_ubo = new RenderBuffer( m_context, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_name = ParseXmlAttribute( definitionXmlElement, "name", m_name );

	// Shader State
	const XmlElement* shaderStateElement = definitionXmlElement.FirstChildElement( "ShaderState" );
	std::string name = ParseXmlAttribute( *shaderStateElement, "name", "" );
	m_shaderState = m_context->GetShaderStateFromName( name );

	// Textures
	const XmlElement* texturesElement = definitionXmlElement.FirstChildElement( "Textures" );
	const XmlElement* diffuseTexElement = texturesElement->FirstChildElement( "Diffuse" );
	if ( diffuseTexElement )
	{
		std::string diffusePath = ParseXmlAttribute( *diffuseTexElement, "path", "" );
		SetDiffuseTexture( diffusePath );

		Rgba8 tint = ParseXmlAttribute( *diffuseTexElement, "tint", m_tint );
		m_tint = tint;
	}
	const XmlElement* normalTexElement = texturesElement->FirstChildElement( "Normal" );
	if( normalTexElement )
	{
		std::string normalPath = ParseXmlAttribute( *normalTexElement, "path", "" );
		SetNormalTexture( normalPath );

	}
	const XmlElement* specluarElement = texturesElement->FirstChildElement( "Specular" );
	if( specluarElement )
	{
		std::string specluarPath = ParseXmlAttribute( *specluarElement, "path", "" );
		if( "" != specluarPath )
		{
			SetTexture( 2, specluarPath );
		}
		float power = ParseXmlAttribute( *specluarElement, "power", m_specularPower );
		m_specularPower = power;
		float factor = ParseXmlAttribute( *specluarElement, "factor", m_specularFactor );
		m_specularFactor = factor;
	}
	const XmlElement* texElement = texturesElement->FirstChildElement( "Texture" );
	while ( texElement )
	{
		uint slot = ParseXmlAttribute( *texElement, "slot", 2 );
		std::string texPath = ParseXmlAttribute( *texElement, "path", "" );
		SetTexture( slot, texPath );
		texElement = texElement->NextSiblingElement( "Texture" );
	}
}

Material::Material( RenderContext* context, const std::string& shaderStateName )
{
	m_context = context;
	m_shaderState = m_context->GetShaderStateFromName( shaderStateName );
	m_ubo = new RenderBuffer( m_context, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
}

Material::~Material()
{
	delete m_ubo;
	m_ubo = nullptr;
}

void Material::UpdateUBOIfDirty()
{
	if ( !m_uboIsDirty )
	{
		return;
	}

	size_t size = (size_t) m_uboCPUData.size();
	m_ubo->Update( &m_uboCPUData[0], size, size );
	m_uboIsDirty = false;
}

void Material::SetTexture( uint slot, Texture* tex )
{
	m_textureMap[slot] = tex;
}

void Material::SetDiffuseTexture( Texture* tex )
{
	if ( tex == nullptr )
	{
		tex = m_context->m_texDefultColor;
	}
	m_textureMap[0] = tex;
}

void Material::SetDiffuseTexture( const std::string& fileName )
{
	Texture* tex = m_context->CreateOrGetTextureFromFile( &fileName[0] );
	SetDiffuseTexture(tex);
}

void Material::SetNormalTexture( Texture* tex )
{
	if( tex == nullptr )
	{
		tex = m_context->m_texDefultNormal;
	}
	m_textureMap[1] = tex;
}

void Material::SetNormalTexture( const std::string& fileName )
{
	Texture* tex = m_context->CreateOrGetTextureFromFile( &fileName[0] );
	SetNormalTexture( tex );
}

void Material::SetTexture( uint slot, const std::string& fileName )
{
	m_textureMap[slot] = m_context->CreateOrGetTextureFromFile( &fileName[0] );
}

void Material::SetSampler( uint slot, Sampler* sampler )
{
	m_samplerMap[slot] = sampler;
}

void Material::SetShaderStateByName( const std::string& name )
{
	m_shaderState = m_context->GetShaderStateFromName( name );
}
