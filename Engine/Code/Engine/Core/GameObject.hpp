#pragma once
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Renderer/Transform.hpp"

class GPUMesh;
class SpriteDefinition;

class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	Rigidbody2D* m_rigidbody = nullptr;

	// 2D thing
	SpriteDefinition* definition = nullptr; // equivalent of this, 3d object
	Vec2 position;
	float rotation = 0.f;

	// 3D equivalent using transform
	GPUMesh* m_mesh = nullptr;
	Transform m_transform;
};