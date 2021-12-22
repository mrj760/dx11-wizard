// Vertex Shader Input
cbuffer constbuffer : register(b0) // use first buffer slot
{
    float xOffset;
    float yOffset;
};

struct VS_INPUT
{
    // parameters with matching semantic names
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
};

// Vertex Shader Output
struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;    // turn our position-semantic-variable to a SV (system variable)
    float2 outTexCoord : TEXCOORD;        // stays the same as it was
};

// Convert input to output
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    input.inPos.x += xOffset;
    input.inPos.y += yOffset;

    output.outPos = float4(input.inPos, 1.0f);

    output.outTexCoord = input.inTexCoord;

    return output;
}