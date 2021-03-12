#include "Engine/Core/ParticleSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Update( float deltaSeconds )
{
	for( Emitter* e : m_emitters )
	{
		e->Update( deltaSeconds );
	}
}

void ParticleSystem::Render() const
{
	for( Emitter* e : m_emitters )
	{
		e->Render();
	}
}

Emitter* ParticleSystem::CreateEmitter()
{
	Emitter* emitter = new Emitter();
	emitter->m_clock = new Clock();
	emitter->m_parentSystem = this;
	m_emitters.push_back( emitter );

	return emitter;
}

void Emitter::Update( float deltaSeconds )
{
	if ( !m_isRender )
	{
		for( int i = 0; i < (int)m_particles.size(); ++i )
		{
			delete m_particles[i];

			uint lastIndex = (uint)m_particles.size() - 1;
			m_particles[i] = m_particles[lastIndex];
			m_particles.pop_back();
			--i;
		}
		return;
	}

	if ( !m_stopCreatingParticle )
	{
		m_age += deltaSeconds;
		if( m_age >= m_interval )
		{
			m_age = 0.f;
			Particle* p = CreateParticle();
			float offsetX = m_parentSystem->m_rng.RollRandomFloatInRange( m_spawnOffsetMin.x, m_spawnOffsetMax.x );
			float offsetY = m_parentSystem->m_rng.RollRandomFloatInRange( m_spawnOffsetMin.y, m_spawnOffsetMax.y );
			float offsetZ = m_parentSystem->m_rng.RollRandomFloatInRange( m_spawnOffsetMin.z, m_spawnOffsetMax.z );
			p->m_maxAge = m_maxAge;
			p->m_transform.m_position = m_position + Vec3( offsetX, offsetY, offsetZ );
			p->m_color = m_color;
			p->m_transform.m_scale = m_scale;
			Vec3 r = Vec3( 1.f, 0.f, 0.f );
			float orientation = m_parentSystem->m_rng.RollRandomFloatInRange( m_minOrientation, m_maxOrientation );
			r = r.GetRotatedAboutZDegrees( orientation ) * m_velocity;
			p->m_velocity = r;
		}
	}

	for( int i = 0; i < (int) m_particles.size(); ++i )
	{
		m_particles[i]->m_age += deltaSeconds;
		m_particles[i]->m_transform.m_position += m_particles[i]->m_velocity * deltaSeconds;
		if ( m_particles[i]->m_maxAge <= m_particles[i]->m_age )
		{
			delete m_particles[i];

			uint lastIndex = (uint)m_particles.size() - 1;
			m_particles[i] = m_particles[lastIndex];
			m_particles.pop_back();
			--i;
		}
	}
}

void Emitter::Render() const
{
	if( !m_isRender )
	{
		return;
	}

	g_theRenderer->BindShader( m_shader );
	g_theRenderer->BindTexture( m_texture );
	std::vector<Vertex_PCU> vertices;
	for ( Particle* p : m_particles )
	{
		Rgba8 color = p->m_color;
		if ( m_hasFadeOut )
		{
			float scale = 1.f - ( p->m_age / p->m_maxAge );
			color.a = (char)((float)color.a * scale);
		}
		AABB2 bounds = AABB2( Vec2::ZERO, Vec2( 1.f, 1.f ) * p->m_transform.m_scale.GetXY() );
		bounds.Translate( p->m_transform.m_position.GetXY() );
		AppendQuad( vertices, Vec3( bounds.mins, 0.f ), Vec3( bounds.maxs.x, bounds.mins.y, 0.f ), 
			Vec3( bounds.mins.x, bounds.maxs.y, 0.f ), Vec3( bounds.maxs, 0.f ), AABB2::ZERO_TO_ONE, color );
	}
	g_theRenderer->DrawVertexArray( vertices );
}

Particle* Emitter::CreateParticle()
{
	Particle* p = new Particle();
	m_particles.push_back( p );

	return p;
}
