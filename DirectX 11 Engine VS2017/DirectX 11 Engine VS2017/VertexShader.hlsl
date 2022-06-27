struct VS_Input
{
    float2 inPos : POSITION;
    float3 InColor : COLOR;
};
struct VS_Output
{
    float4 outPosition : SV_POSITION;
    float3 outColor : COLOR;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.outPosition = float4(input.inPos, 0.0f, 1.0f);
    output.outColor = input.InColor;
   
    return output;
}