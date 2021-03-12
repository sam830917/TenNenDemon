//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

// inputs are made up of internal names (ie: uv) and semantic names
// (ie: TEXCOORD).  "uv" would be used in the shader file, where
// "TEXCOORD" is used from the client-side (cpp code) to attach ot. 
// The semantic and internal names can be whatever you want, 
// but know that semantics starting with SV_* usually denote special 
// inputs/outputs, so probably best to avoid that naming.
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

struct light_t
{
   float3 world_position; 
   float pad00;  // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned

   float3 color; 
   float intensity; // rgb and an intensity
   
   float3 attenuation;
   float pad01;
};

/*
// cpu side
struct light_t
{
   Vec3 worldPosition; 
   float pad00; 
   Vec3 color; 
   float intesity; // RGBA will not work - is not the same size;  
};
*/

// buffer telling us information about our camera
cbuffer light_constants : register(b4)         // constant buffer slot 1
{
   float4 AMBIENT; 
   light_t LIGHT; 
   
   float SPECULAR_POWER;
   float SPECULAR_FACTOR;
};

cbuffer material_contants : register(b5)         // constant buffer slot 1
{
   float3 FRESNEL_COLOR; 
   float FRESNEL_POWER;
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

float2 ComputeLightFactor( light_t light, float3 world_pos, float3 world_normal, float3 dir_to_eye )
{
   float3 vec_to_light = light.world_position - world_pos; 

   float dist = length(vec_to_light);
   float3 dir_to_light = normalize(vec_to_light);
   float3 att_vec = float3( 1.0f, dist, dist * dist ); 

   float diff_att = light.intensity / dot( att_vec, light.attenuation ); 
   float spec_att = light.intensity / dot( att_vec, light.attenuation ); 

   // compute diffuse
   // max prevents lighting from the "back", which would subtract light
   float diffuse = max( 0.0f, dot( dir_to_light, world_normal ) );
   
   // phong
   float3 ref_dir_to_light = reflect( -dir_to_light, world_normal );  
   float specular = max( 0.0f, dot( ref_dir_to_light, dir_to_eye ) );

   // finalize specular
   specular = SPECULAR_FACTOR * pow( specular, SPECULAR_POWER ); 

   // return result
   return float2( diff_att * diffuse, spec_att * specular ); 
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

// raster step
// float3 

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 FragmentFunction( v2f_t input ) : SV_Target0
{
   float3 normal = normalize(input.world_normal);
   float3 tangent = normalize(input.world_tangent);
   float3 bitangent = normalize(input.world_bitangent);
   float3x3 tbn = float3x3( tangent, bitangent, normal );
   
   float3 dir_to_surface = normalize( input.world_position - CAMERA_POSITION );
   
   // get my world normal
   float4 norm_color = tNormal.Sample( sSampler, input.uv );
   float3 surface_normal = NormalColorToVector( norm_color.xyz );
   float3 world_normal = mul( surface_normal, tbn );
   
   
   float dp = length( cross( dir_to_surface, world_normal ) );
   
   float factor = pow( dp, FRESNEL_POWER * 16 + 15.0f * sin(SYSTEM_TIME_SECONDS * 4.f) );
   
   return float4( FRESNEL_COLOR, factor );
}