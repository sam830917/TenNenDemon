#include <string>
std::string errorShader = R"(
struct vs_input_t 
{
   // we are not defining our own input data; 
   float3 position      : POSITION; 
   float4 color         : COLOR; 
   float2 uv            : TEXCOORD; 

   float3 normal        : NORMAL;
   float3 tangent       : TANGENT;
   float3 bitangent     : BITANGENT;
}; 

static float SHIFT = 0.75f;

cbuffer time_constants : register(b0)
{
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTA_SECONDS;
};

cbuffer camera_constants : register(b1)
{
	float4x4 PROJECTION; // CAMERA_TO_CLIP_TRANSFORM
	float4x4 VIEW;	// WORLD_TO_CAMERA_TRANSFORM
	
	float3 CAMERA_POSITION;
	float pad00;
};

cbuffer model_matrix : register(b2)
{
	float4x4 MATRIX;
};

cbuffer model_tint : register(b3)
{
	float4 TINT_COLOR;
};

Texture2D <float4> tDiffuse : register(t0); // color of the surface
SamplerState sSampler : register(s0);		// sampler are rules on how to sample (read) from the texture.

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
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

   v2f.position = clip_pos; // we want to output the clip position to raster (a perspective point)
   v2f.color = input.color * TINT_COLOR; 
   v2f.uv = input.uv; 

   return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
	return float4( 1.0f, 0.0f, 1.0f, 1.0f );
}
)";