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

Texture2D <float4> tDiffuse : register(t0); // color of the surface
Texture2D <float4> tNormal   : register(t1);    // normals of the surface
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
	float3 world_tangent :WORLD_TANGENT;
	float3 world_bitangent : WORLD_BITANGENT;
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
   v2f.world_tangent = mul( MATRIX, float4( input.tangent, 0.f ) ).xyz;
   v2f.world_bitangent = mul( MATRIX, float4( input.bitangent, 0.f ) ).xyz;

   return v2f;
}

struct fragment_output_t
{
	float4 color : SV_Target0;
	float4 bloom : SV_Target1;
	float4 normal : SV_Target2;
	float4 albedo : SV_Target3;
	float4 tangent : SV_Target4;
};

// raster step
// float3 

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
fragment_output_t FragmentFunction( v2f_t input )
{
	float3 normal = normalize(input.world_normal);
	float3 tangent = normalize(input.world_tangent);
	float3 bitangent = normalize(input.world_bitangent);
	float3x3 tbn = float3x3( tangent, bitangent, normal );
	
	// use the uv to sample the texture
	float4 diffuse_color = tDiffuse.Sample( sSampler, input.uv );
	float4 normal_color = tNormal.Sample( sSampler, input.uv );
	
	float3 surface_color = (input.color * diffuse_color).xyz; // multiply our tint with our texture color to get our final color; 
	float surface_alpha = (input.color.a * diffuse_color.a); 
	
	// get my surface normal - this comes from the vertex format
	// We now have a NEW vertex format
	float3 surface_normal = NormalColorToVector( normal_color.xyz );
	float3 world_normal = mul( surface_normal, tbn );
	
	float spec_factor = SPECULAR_FACTOR; 
	// Light
	
	float3 dir_to_eye = normalize(CAMERA_POSITION - input.world_position); 
	float3 diffuse = AMBIENT.xyz * AMBIENT.w; // assumes ambient is set from a user - so sRGB space
	float3 spec = float3(0.0f, 0.f, 0.f); 
	
	// add up contribution of all lights
	for (uint i = 0; i < MAX_LIGHTS; ++i)
	{
		float3 light_color = LIGHTS[i].color.xyz;
		float2 light_factors = ComputeLightFactor( LIGHTS[i], input.world_position, world_normal, dir_to_eye );
		
		diffuse += light_factors.x * light_color; 
		spec += light_factors.y * light_color; 
	}
	
	// limit it
	diffuse = min( diffuse, float3(1,1,1) );
	spec *= spec_factor;  // scale back specular based on spec factor
	
	float3 final_color = diffuse * surface_color + spec;
	final_color = ApplyFog( input.world_position, final_color );
	
	float3 bloom = max( float3(0,0,0), spec - float3(1,1,1) );
	
	fragment_output_t output;
	output.color = float4( final_color.xyz, surface_alpha );
	output.bloom = float4( bloom, 1 );
	output.normal = float4( (world_normal + float3(1,1,1)) * 0.5f, 1 );
	output.tangent = float4( (tangent + float3(1,1,1)) * 0.5f, 1 );
	output.albedo = diffuse_color;
	
	//return float4( final_color, surface_alpha );  
	return output;  
}