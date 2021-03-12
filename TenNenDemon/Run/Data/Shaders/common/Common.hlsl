//------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------
static const uint MAX_LIGHTS = 8; 

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
   float dot_inner_angle;  // cone light inner angle (default -1.0f) - angle at which cone lights begin to stop affecting an object
   
   float3 direction;         // direction light is point, default (0,0,1)
   float direction_factor; // do we consider this a point light (0.0f) or a directional light (1.0f).  Determine how we calcualte the incident light vector, default (0.0f)

   float3 spec_attenuation;  // attenuation for specular lighting (constant,linear,quadratic), default (0,0,1)
   float dot_outer_angle;  // cone light outer angle (default -1.0f) - angle at which cone lights stop affecting the object completely
};

cbuffer light_constants : register(b4)
{
   float4 AMBIENT; 
   
   light_t LIGHTS[MAX_LIGHTS];
   
   float SPECULAR_POWER;
   float SPECULAR_FACTOR;
   
   // fog
   float FOG_NEAR_DISTANCE; 
   float FOG_FAR_DISTANCE; 

   float3 FOG_NEAR_COLOR;
   float _fogpad0; 
   float3 FOG_FAR_COLOR;
   float FOG_INTENSITY;
};

//------------------------------------------------------------------------
float3 ApplyFog( float3 world_pos, float3 color ) 
{
   float dist = length( world_pos - CAMERA_POSITION );
   float t = smoothstep( FOG_NEAR_DISTANCE, FOG_FAR_DISTANCE, dist ); 
   return lerp( color, FOG_FAR_COLOR, t.xxx * FOG_INTENSITY );  
}