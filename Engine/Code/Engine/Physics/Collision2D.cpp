#include "Engine/Physics/Collision2D.hpp"

Collision2D Collision2D::GetInverse() const
{
	Collision2D col;
	col.me = them;
	col.them = me;
	Manifold2 manifold2 = manifold;
	manifold2.normal = -manifold.normal;
	col.manifold = manifold2;

	return col;
}
