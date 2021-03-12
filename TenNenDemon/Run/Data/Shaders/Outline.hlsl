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
   
   float2 taps[4] : TAPS;
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
	
	float4 worldPos = float4( input.position, 1 );
	worldPos = mul( MATRIX, worldPos );
	float4 cameraPos = mul( VIEW, worldPos );
	float4 clipPos = mul(PROJECTION, cameraPos);
	
	uint width;
	uint height;
	tDiffuse.GetDimensions(width, height);
	float2 texelSize = float2(1.f / width, 1.f / height);
	
	// forward vertex input onto the next stage
	v2f.color = input.color; 
	v2f.uv = input.uv; 
	v2f.position = clipPos;
	v2f.taps[0] = input.uv + texelSize;
	v2f.taps[1] = input.uv - texelSize;
	v2f.taps[2] = input.uv + texelSize * float2(1, -1);
	v2f.taps[3] = input.uv - texelSize * float2(1, -1);
		
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
	float4 t = tDiffuse.Sample( sSampler, input.taps[0]);   // top
	float4 b = tDiffuse.Sample( sSampler, input.taps[1]);   // bottom
	float4 l = tDiffuse.Sample( sSampler, input.taps[2]);   // left
	float4 r = tDiffuse.Sample( sSampler, input.taps[3]);   // right
	
	//float total = t + b + l + r;
	//float weight = abs(color.x * 4 - total);
	//color = clamp(color, float4(1,1,1,1), weight);
	//clip( distance(color.xyz,0) );
	if (color.w == 0 && (b.w != 0 || l.w != 0 || r.w != 0))
	{
		return float4(1,1,1,1);
	}
	
	return color * input.color * TINT_COLOR;
}