#include "Basic2DHeader.hlsli"

InPixel main( InVertex inVertex )
{
    InPixel out_pixel;
    out_pixel.pos = float4(inVertex.pos, 1.0f);
    out_pixel.color = inVertex.color;
    
    return out_pixel;
}