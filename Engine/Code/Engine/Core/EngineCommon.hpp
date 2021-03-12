#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Todo.hpp"
#include "Engine/Core/Delegate.hpp"

#define UNUSED(x) (void)(x);
#define STATIC  ;

typedef unsigned int uint;

extern DevConsole* g_theConsole;
extern EventSystem* g_theEventSystem;
extern NamedStrings g_gameConfigBlackboard;

const Vec2 ALIGN_CENTERED		= Vec2( 0.5f, 0.5f );
const Vec2 ALIGN_BOTTOM_LEFT	= Vec2( 0.f, 0.f );
const Vec2 ALIGN_BOTTOM_MIDDLE	= Vec2( 0.5f, 0.f );
const Vec2 ALIGN_BOTTOM_RIGHT	= Vec2( 1.f, 0.f );
const Vec2 ALIGN_MIDDLE_RIGHT	= Vec2( 1.f, 0.5f );
const Vec2 ALIGN_MIDDLE_LEFT	= Vec2( 0.f, 0.5f );
const Vec2 ALIGN_TOP_RIGHT		= Vec2( 1.f, 1.f );
const Vec2 ALIGN_TOP_MIDDLE		= Vec2( 0.5f, 1.f );
const Vec2 ALIGN_TOP_LEFT		= Vec2( 0.f, 1.f );

constexpr float GRAVITY =  9.8f;
constexpr int LIGHT_NUMBER = 8;
constexpr int USER_TEXTURE_SLOT_START = 5;