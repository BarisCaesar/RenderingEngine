Texture2D tex;
SamplerState samplerstate;

float4 main(float2 uv : Texcoord) : SV_Target
{
    return 1.f - tex.Sample(samplerstate, uv).rgba;
}