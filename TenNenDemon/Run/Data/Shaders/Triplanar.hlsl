#include "common/Common.hlsl"
#include "common/LightCommon.hlsl"

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

Texture2D <float4> tXColor 		: register(t0);
Texture2D <float4> tXNormal   	: register(t1);
Texture2D <float4> tYColor 		: register(t2);
Texture2D <float4> tYNormal   	: register(t3);
Texture2D <float4> tZColor 		: register(t4);
Texture2D <float4> tZNormal   	: register(t5);

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
   float2 z_uv = frac( input.world_position.xy );  // value - floor(value)

   float2 x_uv = frac( input.world_position.zy );
   x_uv.x = 1.0f - x_uv.x; 

   float2 y_uv = frac( input.world_position.xz ); 
   y_uv.y = 1.0f - y_uv.y; 
   
   // use the uv to sample the texture
   float4 x_color = tXColor.Sample( sSampler, x_uv );
   float4 y_color = tYColor.Sample( sSampler, y_uv );
   float4 z_color = tZColor.Sample( sSampler, z_uv );
   
   float3 x_normal = tXNormal.Sample( sSampler, x_uv ).xyz;
   float3 y_normal = tYNormal.Sample( sSampler, y_uv ).xyz;
   float3 z_normal = tZNormal.Sample( sSampler, z_uv ).xyz;
   
   float3 weights = normalize(input.world_normal);
   weights = pow( abs(weights), (4.0).xxx );  // x*x, y*y, z*z  (x^2 + y^2 + z^2 == 1)
   float sum = weights.x + weights.y + weights.z; 
   weights /= sum;

   float4 final_color = weights.x * x_color + weights.y * y_color + weights.z * z_color;
   
   x_normal = NormalColorToVector(x_normal); 
   y_normal = NormalColorToVector(y_normal); 
   z_normal = NormalColorToVector(z_normal); 
   
   // remap z-normal on flip side
   z_normal.z = sign(input.world_normal.z) * z_normal.z; 

   // x side
   float3x3 xtbn = float3x3( 
      float3(0, 0, -1), 
      float3(0, 1, 0), 
      float3(1, 0, 0)
   ); 
   x_normal = mul(x_normal, xtbn);  
   x_normal.x *= sign(input.world_normal.x); 

   float3x3 ytbn = float3x3( 
      float3( 1, 0, 0 ), 
      float3( 0, 0, -1 ), 
      float3( 0, 1, 0 )
   );
   y_normal = mul(y_normal, ytbn);  
   y_normal.y = sign(input.world_normal.y) * y_normal.y; 

   // y size
   float3 final_normal = weights.x * x_normal + weights.y * y_normal + weights.z * z_normal;
   final_normal = normalize(final_normal);
   
   // Light
   float3 surf_color = ComputeLightingAt( input.world_position, final_normal, final_color.xyz, SPECULAR_FACTOR );
   surf_color = ApplyFog( input.world_position, surf_color ); 
   
   return float4( surf_color, 1.0f );  
}