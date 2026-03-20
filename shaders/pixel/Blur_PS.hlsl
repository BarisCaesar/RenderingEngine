Texture2D tex;
SamplerState samplerState;

static const int r = 7;
static const float divisor = (r * 2 + 1) * (r * 2 + 1);

float4 main(float2 uv : Texcoord) : SV_Target
{
    uint width, height;
    tex.GetDimensions(width, height);
    const float dx = 1.f / width;
    const float dy = 1.f / height;
    float4 acc = float4(0.f, 0.f, 0.f, 0.f);
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x < r; x++)
        {
            const float2 tc = uv + float2(dx * x, dy * y);
            acc += tex.Sample(samplerState, tc).rgba;
        }

    }
        return acc / divisor;
}