#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>

class Clock;

struct Particle
{
	Transform m_transform;
	Vec3 m_velocity = Vec3( 1.f, 0.f, 0.f );
	float m_age = 0.f;
	float m_maxAge = 1.f;
	Rgba8 m_color = Rgba8::WHITE;
};

class Emitter
{
	friend class ParticleSystem;
public:
	// Base
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_scale = Vec3::ONE;
	float m_velocity = 5.f; // angle relative to system and float speed
	float m_maxOrientation = 90.f;
	float m_minOrientation = 0.f;

	// Options
	Shader* m_shader = nullptr;
	Texture* m_texture = nullptr;
	Rgba8 m_color = Rgba8::WHITE;
	float m_maxAge = 1.f;
	float m_interval = 0.5f;
	bool m_hasFadeOut = false;
	bool m_isRender = true;
	bool m_stopCreatingParticle = false;
	Vec3 m_spawnOffsetMin = Vec3::ZERO;
	Vec3 m_spawnOffsetMax = Vec3::ZERO;

public:
	void Render() const;

private:
	void Update( float deltaSeconds );

	Particle* CreateParticle();
	
private:
	ParticleSystem* m_parentSystem = nullptr;
	Clock* m_clock = nullptr;
	float m_age = 0.f;
	std::vector<Particle*> m_particles;

};

class ParticleSystem
{
	friend class Emitter;
public:
	ParticleSystem();
	~ParticleSystem();

	void Update( float deltaSeconds );
	void Render() const;

	Emitter* CreateEmitter();

private:
	std::vector<Emitter*> m_emitters;
	RandomNumberGenerator m_rng;

};