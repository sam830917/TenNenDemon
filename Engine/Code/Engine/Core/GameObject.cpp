#include "Engine/Core/GameObject.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	if ( m_rigidbody )
	{
		m_rigidbody->Destroy(); // destroys through the system;  
		m_rigidbody = nullptr;
	}
	if( m_mesh )
	{
		delete m_mesh;
		m_mesh = nullptr;
	}
}