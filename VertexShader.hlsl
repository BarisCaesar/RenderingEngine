struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

VSOut main( float2 pos : POSITION, float3 color : COLOR )
{
    VSOut output;
    output.pos = float4(pos, 0.f, 1.f);
    output.color = color;
    return output;
}