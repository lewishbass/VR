// Vertex shader input
struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

// Vertex shader output / Fragment shader input
struct VertexOutput
{
    float4 position : SV_Position;
    float3 color : COLOR;
};

// Vertex shader
[shader("vertex")]
VertexOutput vertex(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.color = input.color;
    return output;
}

// Fragment shader
[shader("fragment")]
float4 fragment(VertexOutput input) : SV_Target
{
    return float4(input.color, 1.0);
}
