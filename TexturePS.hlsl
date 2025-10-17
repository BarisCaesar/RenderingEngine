Texture2D tex : register(t0);

SamplerState samplr;

float4 main(float2 tc : TexCoord) : SV_TARGET
{
    return tex.Sample(samplr, tc);
}