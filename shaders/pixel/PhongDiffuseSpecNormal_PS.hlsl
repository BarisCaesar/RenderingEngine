#include "ShaderOperations.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

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



float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample diffuse texture
    float4 dtex = tex.Sample(samplerState, tc);
    
    #ifdef NORM_MASK
    // clip the pixel if it is highly translucent
    clip(dtex.a < 0.1 ? -1 : 1); // aborts the pixel shader if the given value is below zero
    
    // flip normal when backface
    if(dot(viewNormal, viewFragPos) >= 0.f)
    {
        viewNormal = -viewNormal;
    }
    #endif
    
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, samplerState);
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
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
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    
    // specular reflected
    const float3 specularReflected = Specular(
        specularColor, 1.f, viewNormal, 
        lv.vToL, viewFragPos, att, specularPower
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected * specularReflectionColor), dtex.a);
}