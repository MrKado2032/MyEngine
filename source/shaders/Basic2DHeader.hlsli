struct InVertex
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct InPixel
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};