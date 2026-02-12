cbuffer LightCBuf
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState samplerState;

float3 MapNormal(
    const in float3 tan, 
    const in float3 bitan, 
    const in float3 normal, 
    const in float2 tc, 
    uniform Texture2D nmap, 
    uniform SamplerState samplerState)
{
    // build the tranform (rotation) into same space as tan/bitan/normal (target space)
    const float3x3 tanToTarget = float3x3(tan, bitan, normal);
    // sample and unpack normal data
    const float3 normalSample = nmap.Sample(samplerState, tc).xyz;
    const float3 tanNormal = normalSample * 2.f - 1.f;
    // bring normal from tanspace into target space
    return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(uniform float attConst, uniform float attLin,  uniform float attQuad, const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

float3 Diffuse(
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirFragToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToL, viewNormal));
}

float3 Speculate(
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewFragToL,
    const in float3 viewPos,
    const in float att,
    const in float specularPower)
{
    // calculate reflected light vector
    const float3 w = viewNormal * dot(viewFragToL, viewNormal);
    const float3 r = normalize(w * 2.0f - viewFragToL);
    // vector from camera to fragment (in view space)
    const float3 viewCamToFrag = normalize(viewPos);
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(-r, viewCamToFrag)), specularPower);
}

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, samplerState);
    }
	// fragment to light vector data
    const float3 viewFragToL = viewLightPos - viewPos;
    const float distFragToL = length(viewFragToL);
    const float3 viewDirFragToL = viewFragToL / distFragToL;
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if(specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(samplerState, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        
        if(hasGloss)
        {
            specularPower = pow(2.f, specularSample.a * 13.f);
        }  
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, distFragToL);
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, viewDirFragToL, viewNormal);
    
    // specular reflected
    const float3 specularReflected = Speculate(specularColor, 1.f, viewNormal, viewFragToL, viewPos, att, specularPower);
	// final color attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(samplerState, tc).rgb + specularReflected * specularReflectionColor), 1.f);
}