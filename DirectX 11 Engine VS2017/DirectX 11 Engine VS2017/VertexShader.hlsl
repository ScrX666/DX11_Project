struct VS_Input
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD0;
};
struct VS_Output
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD0;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.outPosition = float4(input.inPos, 1.0f);
    output.outTexCoord = input.inTexCoord;
   
    return output;
}