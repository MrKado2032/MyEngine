#include "Basic2DHeader.hlsli"

float4 main( InPixel in_pixel) : SV_TARGET
{
	return in_pixel.color;
}