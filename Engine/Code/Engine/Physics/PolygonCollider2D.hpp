#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Polygon2.hpp"

class PolygonCollider2D : public Collider2D
{
public:
	PolygonCollider2D();
	~PolygonCollider2D();

	virtual void	UpdateWorldShape() override;
	virtual Vec2	GetClosestPoint( Vec2 pos ) const override;
	virtual Vec2	GetCenterPoint() const override;
	virtual bool	Contains( Vec2 pos ) const override;
	virtual void	DebugRender( RenderContext* ctx, Rgba8 const& borderColor, Rgba8 const& fillColor ) override;
	virtual void	Destroy() override;
	virtual Vec2	GetBottomPosition() override;
	virtual float	GetCosmeticRadius() override;
	virtual AABB2	GetWorldBounds() override;
	virtual float	CalculateMoment( float mass ) override;

	Polygon2 GetWorldPositionPolygon() const;
	Vec2 Support( const Vec2& direction ) const;

public:
	Vec2 m_worldPosition;
	Polygon2 m_polygon2;
};