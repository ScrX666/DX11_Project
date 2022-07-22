cbuffer SkinningTransforms : register(b0)
{
    matrix WorldMatrix;
    matrix ViewProjMatrix;
    matrix transfomMat[256];
};

Texture2D ColorTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    uint4 bone : BONEINDICES;
    float4 weights : BONEWEIGHTS;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORDS;
};
VS_OUTPUT VSMAIN(in VS_INPUT input)
{
    VS_OUTPUT output;
// Calculate the output position of the vertex
    //output.position = float4(input.position, 1.0f);
    //output.position += float4(input.position, 1.0f);

    
    output.position = (mul(float4(input.position, 1.0f), transfomMat[input.bone.x]) * input.weights.x);
    output.position += (mul(float4(input.position, 1.0f), transfomMat[input.bone.y]) * input.weights.y);
    output.position += (mul(float4(input.position, 1.0f), transfomMat[input.bone.z]) * input.weights.z);
    output.position += (mul(float4(input.position, 1.0f), transfomMat[input.bone.w]) * input.weights.w);
// Transform world position with viewprojection matrix
    output.position = mul(output.position, WorldMatrix);
    output.position = mul(output.position, ViewProjMatrix);
    //output.position = float4(input.position, 1.0f);
// Pass the texture coordinates through
    output.tex = input.tex;
    return output;
}