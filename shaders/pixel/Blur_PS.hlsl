Texture2D tex;
SamplerState samplerState;

cbuffer Kernel
{
    uint nTaps;
    float coefficients[15];
}

cbuffer Control
{
    bool horizontal;
}

float4 main(float2 uv : Texcoord) : SV_Target
{
    uint width, height;
    tex.GetDimensions(width, height);
    float dx, dy;
    if(horizontal)
    {
        dx = 1 / width;
        dy = 0.f;
    }
    else
    {
        dx = 0.f;
        dy = 1 / height;
    }
    const int r = nTaps / 2;

    float4 acc = { 0.f, 0.f, 0.f, 0.f};
    for (int i = -r; i <= r; i++)
    {
        const float2 tc = uv + float2(dx * i, dy * i);
        const float4 fullSample = tex.Sample(samplerState, tc).rgba;
        const float coef = coefficients[i + r];
        acc += fullSample * coef;
    }
    return acc;
}