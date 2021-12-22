// Vertex Shader Input
struct VS_INPUT
{
    // parameters with matching semantic names
    float3 inPos : POSITION;
    float3 inColor : COLOR;
};

// Vertex Shader Output
struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;    // turn our position-semantic-variable to a SV (system variable)
    float3 outColor : COLOR;        // stays the same as it was
};

// Convert input to output
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.outPos = float4(input.inPos, 1.0f);
    output.outColor = input.inColor;

    return output;
}