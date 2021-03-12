#include "common/Common.hlsl"

Texture2D <float4> tDiffuse : register(t0); // color of the surface
SamplerState sSampler : register(s0);		// sampler are rules on how to sample (read) from the texture.
TextureCube <float4> tCube : register(t6); // color of the surface

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
	
	float3 local_position : LOCAL_POSITION; 
	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
	float3 world_tangent :WORLD_TANGENT;
	float3 world_bitangent : WORLD_BITANGENT;
}; 

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax )
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ((val - inMin) / domain) * range + outMin;
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
   v2f.local_position = local_pos.xyz; 
   v2f.world_position = world_pos.xyz; 
   v2f.world_normal = mul( MATRIX, float4( input.normal, 0.f ) ).xyz;
   v2f.world_tangent = mul( MATRIX, float4( input.tangent, 0.f ) ).xyz;
   v2f.world_bitangent = mul( MATRIX, float4( input.bitangent, 0.f ) ).xyz;

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
	float3 incident = input.world_position - CAMERA_POSITION; 
	float3 reflection = reflect( incident, input.world_normal ); 
	float4 color = tCube.Sample( sSampler, reflection );
	return color;
}