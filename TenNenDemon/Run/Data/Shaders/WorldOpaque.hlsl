#include "common/Common.hlsl"

float RangeMap( float val, float inMin, float inMax, float outMin, float outMax )
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ((val - inMin) / domain) * range + outMin;
};

struct v2f_t 
{
   float4 position : SV_POSITION; 
   float4 color : COLOR; 
   float2 uv : UV; 
}; 

Texture2D <float4> tDiffuse : register(t0); // color of the surface
SamplerState sSampler : register(s0);		// sampler are rules on how to sample (read) from the texture.

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t VertexFunction( vs_input_t input )
{
   v2f_t v2f = (v2f_t)0;

   // forward vertex input onto the next stage
   v2f.position = float4( input.position, 1.0f ); 
   v2f.color = input.color; 
   v2f.uv = input.uv; 
   
   float4 worldPos = float4( input.position, 1 );
   worldPos = mul( MATRIX, worldPos );
   float4 cameraPos = mul( VIEW, worldPos );
   float4 clipPos = mul(PROJECTION, cameraPos);
   
   v2f.position = clipPos;
    
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
	float4 color = tDiffuse.Sample( sSampler, input.uv );
	float4 shadedTexel = color * input.color;
	if( shadedTexel.x * shadedTexel.y * shadedTexel.z * shadedTexel.w > 0.9 )
	{
		float voob = 0.5 + 0.5 * sin( 2.0 * SYSTEM_TIME_SECONDS - 33.0 * input.uv.x  + 33.0 * input.uv.y );
		voob = 1.0 - 0.5 * (voob * voob * voob * voob * voob * voob * voob * voob);
		shadedTexel.x *= voob;
		shadedTexel.y *= voob;
	}
	clip(shadedTexel.a - 0.01f);
	
	return shadedTexel;
}