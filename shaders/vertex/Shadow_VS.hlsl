#include "Transform.hlsl"

struct Output
{
    float4 pos : SV_Position;
    float4 viewPos : Position;
};

Output main(float3 pos : Position)
{
    Output output;
    output.pos = mul(float4(pos, 1.0f), modelViewProj);
    output.viewPos.xyz = mul(float4(pos, 1.f), modelView).xyz;
    output.viewPos.w = output.pos.z;

    return output;
}

