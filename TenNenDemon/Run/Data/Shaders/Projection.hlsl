#include "common/Common.hlsl"
#include "common/LightCommon.hlsl"

cbuffer material_constants : register(b5)
{
   float4x4 PROJECTION_MATRIX;
   
   float3 PROJECTOR_POSITION; 
   float PROJECTOR_STRENGTH; 
};

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax )
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ((val - inMin) / domain) * range + outMin;
};

float3 NormalColorToVector( float3 col ) 
{
   return normalize(col * float3(2.0f, 2.0f, 1.0f) - float3( 1.0f, 1.0f, 0.0f )); 
}

Texture2D <float4> tProjectionImg : register(t0); // color of the surface
SamplerState sSampler : register(s0);		// sampler are rules on how to sample (read) from the texture.

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;

	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // move the vertex through the spaces
   float4 local_pos  = float4( input.position, 1.0f );               // passed in position is usually inferred to be "local position", ie, local to the object
   float4 world_pos  =  mul( MATRIX, local_pos );               // world pos is the object moved to its place int he world by the model, not used yet
   float4 camera_pos = mul( VIEW, world_pos ); 
   float4 clip_pos   = mul( PROJECTION, camera_pos ); 

   // tangent & bitangent

   v2f.position = clip_pos; // we want to output the clip position to raster (a perspective point)
   v2f.color = input.color * TINT_COLOR; 
   v2f.uv = input.uv; 
   v2f.world_position = world_pos.xyz; 
   v2f.world_normal = mul( MATRIX, float4( input.normal, 0.f ) ).xyz;

   return v2f;
}

// raster step
// float3 

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   // for determining if it is facing the camera
   // and then blending out out around corners (artistic choice)
   float3 dir_to_eye = normalize( PROJECTOR_POSITION - input.world_position ); 
   float3 normal = normalize( input.world_normal );
   
   // convert world position into a texture coordinate
   float4 clip_pos = mul( PROJECTION_MATRIX, float4( input.world_position, 1.0f ) ); 
   float local_z = clip_pos.w; 
   float3 ndc = clip_pos.xyz / local_z; 

   float2 uv = (ndc.xy + float2(1.0f, 1.0f)) * 0.5f; // move an ndc coordinate to a uv coordinate
   float4 projection = tProjectionImg.Sample( sSampler, uv ); 
   
   // 0 if behind, 1 if in front - going t use this to "clip" to the camera
   float ublend = step(0, uv.x) * (1.0f - step(1, uv.x));
   float vblend = step(0, uv.y) * (1.0f - step(1, uv.y));
   float final_blend = ublend * vblend;
   float dot3 = dot(normal, dir_to_eye); 
   
   final_blend *= max( 0.0f, dot3 ) * PROJECTOR_STRENGTH; 
   
   return lerp( float4(0.0f, 0.0f, 0.0f, 0.0f), projection, final_blend ); // don't output anything for things behind the projector
}