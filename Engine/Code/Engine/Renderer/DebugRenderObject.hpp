#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/OBB2.hpp"

class Timer;
class RenderContext;
class GPUMesh;
class DebugRenderSystem;

class DebugRenderObject
{
	friend class DebugRenderSystem;

public:
	virtual void RenderObject() const = 0;

	bool IsOld() const;

protected:
	virtual ~DebugRenderObject();

	Rgba8 GetCurrentColor() const;
	Rgba8 GetCurrentColor( const Rgba8& startColor, const Rgba8& endColor ) const;

public:
	Rgba8 m_startColor;
	Rgba8 m_endColor;
	eDebugRenderMode m_mode;
	float m_duration;

	Timer* m_timer = nullptr;
};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Point : public DebugRenderObject
{
public:
	DebugRenderObject_Point( Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_Point();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;
	Vec3 m_pos;
	float m_size = 1.f;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Quad : public DebugRenderObject
{
public:
	DebugRenderObject_Quad( Vec3* points, AABB2 uvs, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_Quad();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;
	Vec3 m_point0;
	Vec3 m_point1;
	Vec3 m_point2;
	Vec3 m_point3;
	AABB2 m_uvs;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Line : public DebugRenderObject
{
public:
	DebugRenderObject_Line( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
		Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
		float duration, eDebugRenderMode mode );
	~DebugRenderObject_Line();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;

	Vec3 m_pos0;
	Vec3 m_pos1;
	Rgba8 m_pos1StartColor;
	Rgba8 m_pos1EndColor;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Arrow : public DebugRenderObject
{
public:
	DebugRenderObject_Arrow( Vec3 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
		Vec3 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
		float duration, eDebugRenderMode mode );
	~DebugRenderObject_Arrow();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;

	Vec3 m_startPoint;
	Vec3 m_endPoint;
	Vec3 m_coneCenterPoint;
	Rgba8 m_pos1StartColor;
	Rgba8 m_pos1EndColor;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_WireBounds : public DebugRenderObject
{
public:
	DebugRenderObject_WireBounds( AABB3 bounds, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_WireBounds();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;
	AABB3 m_bounds;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_WireSphere : public DebugRenderObject
{
public:
	DebugRenderObject_WireSphere( Vec3 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_WireSphere();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;
	Vec3 m_pos;
	float m_size = 1.f;

};
//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Basis : public DebugRenderObject
{
public:
	DebugRenderObject_Basis( Mat44 basis, Rgba8 start_color, Rgba8 end_color, float duration, float axesRadius, eDebugRenderMode mode );
	~DebugRenderObject_Basis();

	virtual void RenderObject() const override;

public:
	GPUMesh* m_mesh = nullptr;

	Mat44 m_basis;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_Text : public DebugRenderObject
{
public:
	DebugRenderObject_Text( std::string text, Mat44 basis, Vec3 position, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_Text();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;

	Mat44 m_basis;
	Vec3 m_position;

};

//------------------------------------------------------------------------------------------------------------------------
class DebugRenderObject_BillboardText : public DebugRenderObject
{
public:
	DebugRenderObject_BillboardText( std::string text, Vec3 position, Vec2 pivot, Rgba8 start_color, Rgba8 end_color, float duration, eDebugRenderMode mode );
	~DebugRenderObject_BillboardText();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;

	Camera* m_currentCamera = nullptr;
	Vec3 m_worldPosition;
	Vec2 m_pivot;

};

//------------------------------------------------------------------------------------------------------------------------
//														Screen Object
//------------------------------------------------------------------------------------------------------------------------
// Point
class DebugRenderObject_ScreenPoint : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenPoint( Vec2 center, float size, Rgba8 start_color, Rgba8 end_color, float duration );
	~DebugRenderObject_ScreenPoint();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;
	Vec2 m_center;
	float m_size = 1.f;

};

//------------------------------------------------------------------------------------------------------------------------
// Line
class DebugRenderObject_ScreenLine : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenLine( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
		Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
		float duration );
	~DebugRenderObject_ScreenLine();

	virtual void RenderObject() const override;

public:
	Vec2 m_pos0;
	Vec2 m_pos1;
	Rgba8 m_pos1StartColor;
	Rgba8 m_pos1EndColor;

	OBB2 m_line;

};

//------------------------------------------------------------------------------------------------------------------------
// Arrow
class DebugRenderObject_ScreenArrow : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenArrow( Vec2 p0, Rgba8 p0_start_color, Rgba8 p0_end_color,
		Vec2 p1, Rgba8 p1_start_color, Rgba8 p1_end_color,
		float duration );
	~DebugRenderObject_ScreenArrow();

	virtual void RenderObject() const override;

public:
	Vec2 m_startPos;
	Vec2 m_endPos;
	Rgba8 m_pos1StartColor;
	Rgba8 m_pos1EndColor;

	OBB2 m_line;
	Vec2 m_arrowPos0;
	Vec2 m_arrowPos1;

};

//------------------------------------------------------------------------------------------------------------------------
// Line
class DebugRenderObject_ScreenQuad : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenQuad( AABB2 bounds, Rgba8 start_color, Rgba8 end_color, float duration, AABB2 uvs = AABB2::ZERO_TO_ONE, const Texture* tex = nullptr );
	~DebugRenderObject_ScreenQuad();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;

	AABB2 m_bounds;
	AABB2 m_uvs;
	const Texture* m_tex = nullptr;

};

//------------------------------------------------------------------------------------------------------------------------
// Text
class DebugRenderObject_ScreenText : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenText( std::string text, Vec2 pos, float size, Rgba8 start_color, Rgba8 end_color, float duration );
	~DebugRenderObject_ScreenText();

	virtual void RenderObject() const override;

public:
	std::vector<Vertex_PCU> m_vertices;

	Vec2 m_position;
	float m_size;

};

//------------------------------------------------------------------------------------------------------------------------
// Text
class DebugRenderObject_ScreenMessage : public DebugRenderObject
{
public:
	DebugRenderObject_ScreenMessage( std::string text, Rgba8 start_color, Rgba8 end_color, float duration );
	~DebugRenderObject_ScreenMessage();

	virtual void RenderObject() const override;
	void RenderMessage( uint index ) const;

public:
	std::string m_text;

};