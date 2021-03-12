#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

static std::map< std::string, ShaderState* > s_definitionMap;

STATIC void ShaderState::LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath )
{
	XmlDocument mapXmlDoc;
	mapXmlDoc.LoadFile( &deinitionsXmlFilePath[0] );
	XmlElement* mapDefsElement = mapXmlDoc.RootElement();
	if( mapDefsElement == nullptr )
	{
		g_theConsole->PrintString( Rgba8::RED, "WARNING: ShaderState definitions file not found" );
		return;
	}
	XmlElement* shaderStateDefElement = mapDefsElement->FirstChildElement();
	while( shaderStateDefElement )
	{
		ShaderState* newShaderStateDef = new ShaderState( context, *shaderStateDefElement );
		if( s_definitionMap.count( newShaderStateDef->m_name ) > 0 )
		{
			ERROR_AND_DIE( Stringf( "ShaderState Definitions include the same name \"%s\"", newShaderStateDef->m_name.c_str() ) );
		}
		s_definitionMap[newShaderStateDef->m_name] = newShaderStateDef;
		shaderStateDefElement = shaderStateDefElement->NextSiblingElement();
	}
}

STATIC ShaderState* ShaderState::GetDefinitions( const std::string& deinitionsName )
{
	if( s_definitionMap.count( deinitionsName ) > 0 )
	{
		return s_definitionMap.at( deinitionsName );
	}
	return nullptr;
}

ShaderState::ShaderState( RenderContext* context, const XmlElement& definitionXmlElement )
{
	m_name = ParseXmlAttribute( definitionXmlElement, "name", m_name );

	// Shader
	const XmlElement* shaderElement = definitionXmlElement.FirstChildElement( "Shader" );
	std::string path = ParseXmlAttribute( *shaderElement, "path", "" );
	m_shader = context->CreateOrGetShader( &path[0] );

	// BlendMode
	const XmlElement* blendElement = definitionXmlElement.FirstChildElement( "BlendMode" );
	if ( blendElement )
	{
		std::string blendMode = ParseXmlAttribute( *blendElement, "mode", "" );
		if( "Alpha" == blendMode )
		{
			m_blendMode = eBlendMode::BLEND_ALPHA;
		}
		else if( "Additive" == blendMode )
		{
			m_blendMode = eBlendMode::BLEND_ADDITIVE;
		}
		else if( "Opaque" == blendMode )
		{
			m_blendMode = eBlendMode::BLEND_OPAQUE;
		}
	}

	// Depth
	const XmlElement* depthElement = definitionXmlElement.FirstChildElement( "Depth" );
	if ( depthElement )
	{
		std::string depth = ParseXmlAttribute( *depthElement, "compare", "" );
		if( "Always" == depth )
		{
			m_depth = eCompareOp::COMPARISON_ALWAYS;
		}
		else if( "Equal" == depth )
		{
			m_depth = eCompareOp::COMPARISON_EQUAL;
		}
		else if( "Greater" == depth )
		{
			m_depth = eCompareOp::COMPARISON_GREATER;
		}
		else if( "GreaterEqual" == depth )
		{
			m_depth = eCompareOp::COMPARISON_GREATER_EQUAL;
		}
		else if( "Less" == depth )
		{
			m_depth = eCompareOp::COMPARISON_LESS;
		}
		else if( "LessEqual" == depth )
		{
			m_depth = eCompareOp::COMPARISON_LESS_EQUAL;
		}
		else if( "Never" == depth )
		{
			m_depth = eCompareOp::COMPARISON_NEVER;
		}
		else if( "NotEqual" == depth )
		{
			m_depth = eCompareOp::COMPARISON_NOT_EQUAL;
		}
	}

	// Depth Write
	const XmlElement* depthWriteElement = definitionXmlElement.FirstChildElement( "DepthWrite" );
	if ( depthWriteElement )
	{
		m_writeDepth = ParseXmlAttribute( *depthWriteElement, "isWriteDepth", true );
	}

	// Winding
	const XmlElement* windingElement = definitionXmlElement.FirstChildElement( "Winding" );
	if ( windingElement )
	{
		std::string winding = ParseXmlAttribute( *windingElement, "winding", "" );
		if( "CounterClockwise" == winding )
		{
			m_windingOrder = eWindingOrder::WIND_COUNTERCLOCKWISE;
		}
		else if( "Clockwise" == winding )
		{
			m_windingOrder = eWindingOrder::WIND_CLOCKWISE;
		}
	}

	// Cull Mode
	const XmlElement* cullElement = definitionXmlElement.FirstChildElement( "CullMode" );
	if ( cullElement )
	{
		std::string cullMode = ParseXmlAttribute( *cullElement, "mode", "" );
		if( "Back" == cullMode )
		{
			m_cullMode = eCullMode::CULL_BACK;
		}
		else if( "Front" == cullMode )
		{
			m_cullMode = eCullMode::CULL_FRONT;
		}
		else if( "None" == cullMode )
		{
			m_cullMode = eCullMode::CULL_NONE;
		}
	}

	// Fill Mode
	const XmlElement* fillElement = definitionXmlElement.FirstChildElement( "FillMode" );
	if ( fillElement )
	{
		std::string fillMode = ParseXmlAttribute( *fillElement, "mode", "" );
		if( "Solid" == fillMode )
		{
			m_fillMode = eFillMode::FILL_SOLID;
		}
		else if( "Wireframe" == fillMode )
		{
			m_fillMode = eFillMode::FILL_WIREFRAME;
		}
	}
}

ShaderState::~ShaderState()
{
}
