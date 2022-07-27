struct PS_Input
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_Input input) : SV_TARGET
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	return float4(pixelColor, 1.0f);
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
}