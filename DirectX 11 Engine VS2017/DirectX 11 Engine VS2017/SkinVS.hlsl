cbuffer SkinningTransforms : register(b0)
{
    matrix WorldMatrix;
    matrix ViewProjMatrix;
    matrix boneTransform[6];
    matrix SkinNormalMatrices[6];
};

Texture2D ColorTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    int4 bone : BONEINDICES;
    float4 weights : BONEWEIGHTS;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORDS;
};
VS_OUTPUT VSMAIN(in VS_INPUT input)
{
    VS_OUTPUT output;
// Calculate the output position of the vertex
    output.position = (mul(float4(input.position, 1.0f), boneTransform[input.bone.x]) * input.weights.x);
    output.position += (mul(float4(input.position, 1.0f), boneTransform[input.bone.y]) * input.weights.y);
    output.position += (mul(float4(input.position, 1.0f), boneTransform[input.bone.z]) * input.weights.z);
    output.position += (mul(float4(input.position, 1.0f), boneTransform[input.bone.w]) * input.weights.w);
// Transform world position with viewprojection matrix
    output.position = mul(output.position, ViewProjMatrix);
// Calculate the world space normal vector
    output.normal = (mul(input.normal, (float3x3) SkinNormalMatrices[input.bone.x])* input.weights.x).xyz;
    output.normal += (mul(input.normal, (float3x3) SkinNormalMatrices[input.bone.y])* input.weights.y).xyz;
    output.normal += (mul(input.normal, (float3x3) SkinNormalMatrices[input.bone.z])* input.weights.z).xyz;
    output.normal += (mul(input.normal, (float3x3) SkinNormalMatrices[input.bone.w])* input.weights.w).xyz;
// Pass the texture coordinates through
    output.tex = input.tex;
    return output;
}