// Vertex Shader Input
cbuffer constbuffer : register(b0) // use first buffer slot
{
    // sort data so it comes out into chunks of 16 bytes if possible
    // https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
    float4x4 mat; // 4x4 matrix
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

    // Multiply the 1x4 matrix of our current position by another 4x4 matrix for translation/rotation/scaling/etc. operations
    // [x, y, z, 1] x 4x4 matrix
    output.outPos = mul(float4(input.inPos, 1.0f), mat);    

    output.outTexCoord = input.inTexCoord;

    return output;
}