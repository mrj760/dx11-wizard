// constant buffer registered to first slot which stores alpha of a pixel
cbuffer alphaBuffer : register(b0)
{
    float alpha;
}

// Pixel Shader Input
struct PS_INPUT
{
    float4 inPos : SV_POSITION; // turn our position-semantic-variable to a SV (system variable)
    float2 inTexCoord : TEXCOORD; // stays the same as it was
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

// convert input to SV (system variable) target
float4 main(PS_INPUT input) : SV_Target
{
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(pixelColor, alpha);
}