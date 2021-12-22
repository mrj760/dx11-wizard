// Pixel Shader Input
struct PS_INPUT
{
    float4 inPos : SV_POSITION; // turn our position-semantic-variable to a SV (system variable)
    float3 inColor : COLOR; // stays the same as it was
};

// convert input to SV (system variable) target
float4 main(PS_INPUT input) : SV_Target
{
    return float4(input.inColor, 1.0f);
}