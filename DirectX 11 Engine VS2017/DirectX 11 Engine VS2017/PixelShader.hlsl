struct PS_Input
{
    float4 inPos : SV_POSITION;
    float3 inColor : COLOR;
};


float4 main(PS_Input input) : SV_TARGET
{
	return float4(input.inColor, 1.0f);
}