cbuffer TransformBuffer : register(b0)
{
    float4x4 worldViewProjection;
};

struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    // Transform the vertex into clip space.
    output.position = mul(
        float4(input.position, 1.0f),
        worldViewProjection
    );

    output.color = input.color;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}