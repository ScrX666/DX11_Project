struct VS_Input
{
    float3 inPos : POSITION;
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
    output.outPosition = float4(input.inPos, 1.0f);
    output.outColor = input.InColor;
   
    return output;
}