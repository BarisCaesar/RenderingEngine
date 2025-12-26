cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : POSITION, float3 n : NORMAL, float2 tc : TEXCOORD )
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    return vso;
}