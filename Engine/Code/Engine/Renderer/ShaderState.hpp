#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include <string>
#include <map>

class Shader;
class RenderContext;

enum class eWindingOrder
{
	WIND_COUNTERCLOCKWISE,
	WIND_CLOCKWISE
};

enum class eBlendMode
{
	BLEND_ALPHA,
	BLEND_ADDITIVE,
	BLEND_OPAQUE
};

enum class eCompareOp
{
	COMPARISON_NEVER,
	COMPARISON_LESS,
	COMPARISON_EQUAL,
	COMPARISON_LESS_EQUAL,
	COMPARISON_GREATER,
	COMPARISON_NOT_EQUAL,
	COMPARISON_GREATER_EQUAL,
	COMPARISON_ALWAYS
};

enum class eCullMode
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK
};

enum class eFillMode
{
	FILL_WIREFRAME,
	FILL_SOLID
};

class ShaderState
{
public:
	static void LoadDefinitions( RenderContext* context, const std::string& deinitionsXmlFilePath );
	static ShaderState* GetDefinitions( const std::string& deinitionsName );

public:
	explicit ShaderState( RenderContext* context, const XmlElement& definitionXmlElement );
	~ShaderState();

public:
	Shader* m_shader;

	std::string	m_name;
	bool m_writeDepth                = true;
	eBlendMode m_blendMode           = eBlendMode::BLEND_ALPHA;
	eCompareOp m_depth				 = eCompareOp::COMPARISON_LESS_EQUAL;
	eWindingOrder m_windingOrder     = eWindingOrder::WIND_COUNTERCLOCKWISE;
	eCullMode m_cullMode             = eCullMode::CULL_BACK;
	eFillMode m_fillMode             = eFillMode::FILL_SOLID;
};
