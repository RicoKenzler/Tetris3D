cbuffer TransformBuffer : register(b0)
{
    float4x4 worldViewProjection;
    float4x4 normalMatrix;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    // Transform the vertex into clip space.
    output.position = mul(
        float4(input.position, 1.0f),
        worldViewProjection
    );
    
    output.normal = normalize(mul(
        input.normal,
        (float3x3) normalMatrix));
    
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 lightDirection = normalize(float3(-0.5f, 1.0f, -0.5f));
    float3 normal = normalize(input.normal);
    float diffuse = saturate(dot(normal, lightDirection));
    float colorIntensity = 0.25f + 0.75f * diffuse;
    
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
    float3 color = textureColor * colorIntensity;
    
    return float4(color, 1.0f);
}