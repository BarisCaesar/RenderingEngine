float4 main( float2 pos : POSITION ) : SV_POSITION
{
    return float4(pos, 0.f, 1.f);
}