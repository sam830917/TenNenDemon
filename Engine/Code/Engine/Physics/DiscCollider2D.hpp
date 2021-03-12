#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"

class DiscCollider2D : public Collider2D
{
public: // implement the interface of Collider2D
		// A02 TODO
		// ...
	DiscCollider2D();
	~DiscCollider2D();

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

public:
	Vec2 m_localPosition; // my local offset from my parent
	Vec2 m_worldPosition; // calculated from local position and owning rigidbody if present
	float m_radius = 0.f;
};